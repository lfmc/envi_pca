#include<stdio.h>
#include<stdlib.h>
#include<gsl/gsl_vector.h>
#include<gsl/gsl_eigen.h>
#include<gsl/gsl_blas.h>
#include<gsl/gsl_statistics_double.h>
#include"envi.h"
#include"io_bsq.h"
#include"pca.h"

/*This block size works for computers with little RAM*/
#define BLOCK_SIZE 3000000

static void pca_check_allocation(void* p){
  if(!p){
    fprintf(stderr,"Failure in memory\n");
    exit(EXIT_FAILURE);
  }
}

static gsl_vector* pca_mean_vector(envi_header file){
  printf("Calculating mean vector\n");
  gsl_vector* mean_vector = gsl_vector_calloc(file.bands);
  pca_check_allocation(mean_vector);
  int i, size = (file.samples) * (file.lines);
  for(i=0;i<file.bands;++i){
    gsl_vector* band = io_bsq_read_band(file, i);
    double mean = gsl_stats_mean(band->data, 1, size);
    gsl_vector_set(mean_vector, i, mean);
    gsl_vector_free(band);
  }
  printf("Calculation succeded\n");
  return mean_vector;
}

static void pca_normalize_band_vector(gsl_vector* mean_vector, gsl_vector* band_vector, int band){
  gsl_vector_add_constant(band_vector, (-1)*gsl_vector_get(mean_vector, band));
  return;
}

static void pca_normalize_pixel_vector( gsl_vector* mean_vector, gsl_vector* pixel_vector){
  gsl_vector_sub(pixel_vector, mean_vector);
  return;
}

static gsl_matrix* pca_covariance_matrix(envi_header file, gsl_vector* mean_vector){
  printf("Calculating covariance matrix\n");
  int size = (file.samples) * (file.lines);
  gsl_matrix* covariance_matrix = gsl_matrix_calloc(file.bands, file.bands);
  pca_check_allocation(covariance_matrix);
  int i, j;
  for (i=0; i<file.bands;++i){
    for(j=0; j<file.bands;++j){
      if(j>=i){
	  gsl_vector* band_i = io_bsq_read_band(file, i);
	  gsl_vector* band_j = io_bsq_read_band(file, j);
	  pca_normalize_band_vector(mean_vector, band_i, i);
	  pca_normalize_band_vector(mean_vector, band_j, j);
	  double cov = gsl_stats_covariance(band_i->data, 1, band_j->data, 1, size);
	  gsl_vector_free(band_i);
	  gsl_vector_free(band_j);
	  gsl_matrix_set(covariance_matrix, i, j, cov);
	  gsl_matrix_set(covariance_matrix, j, i, cov);
      }
    }
  }
  printf("Calculation succeded\n");
  return covariance_matrix;
}

int pca_vector_is_sorted(gsl_vector* v){
  int i, controle = 1;
  for (i=0; i<((v->size)-1); ++i){
    if(gsl_vector_get(v, i)<gsl_vector_get(v,i+1)){
      controle = 0;
      return controle;
    }
  }
  return controle;
}

static gsl_matrix* pca_sorted_eigen_matrix(envi_header file, gsl_matrix* covariance_matrix){
  printf("Calculating eigen_matrix\n");
  gsl_eigen_symmv_workspace* aux = gsl_eigen_symmv_alloc(file.bands);
  gsl_matrix* eigen_matrix = gsl_matrix_calloc(file.bands, file.bands);
  gsl_vector* eigen_vector = gsl_vector_calloc(file.bands);

  gsl_eigen_symmv(covariance_matrix, eigen_vector, eigen_matrix, aux);

  int i;
  while(!pca_vector_is_sorted(eigen_vector)){
    for(i=0;i<(file.bands)-1;++i){
      if(gsl_vector_get(eigen_vector, i)<gsl_vector_get(eigen_vector, i+1)){
	gsl_vector_swap_elements(eigen_vector, i, i+1);
	gsl_matrix_swap_columns(eigen_matrix,  i, i+1);
      }
    }
  }
  gsl_eigen_symmv_free(aux);
  gsl_vector_free(eigen_vector);
  printf("Calculation succeded\n");
  return eigen_matrix;
}

static void pca_save_eigen_matrix(envi_header file, gsl_matrix* eigen_matrix){
  char realfilename[50];
  sscanf(file.new_filename, "%49[^.]", realfilename);
  char filename[53];
  sprintf(filename, "%s.eigen", realfilename);
  FILE* fp = fopen(filename, "wb");
  if(!fp){
    fprintf(stderr, "Failed to create target: %s\n", filename);
    exit(EXIT_FAILURE);
  }
  gsl_matrix_fwrite(fp, eigen_matrix);
  fclose(fp);
  return;
}

static void pca_xform_pixel_vector(envi_header file, gsl_matrix* eigen_matrix, gsl_vector* pixel_vector, gsl_vector* xform_pixel_vector){
    gsl_blas_dgemv(CblasTrans, 1.0, eigen_matrix, pixel_vector, 0.0, xform_pixel_vector);
    return;
}

extern void pca_xform(envi_header file){
  gsl_vector* mean_vector = pca_mean_vector(file);
  gsl_matrix* covariance_matrix = pca_covariance_matrix(file, mean_vector);
  gsl_matrix* eigen_matrix = pca_sorted_eigen_matrix(file, covariance_matrix);

  pca_save_eigen_matrix(file, eigen_matrix);
  
  gsl_matrix_free(covariance_matrix);
 
  int size = (file.lines)*(file.samples), i;

  printf("Transforming\n");
  gsl_vector** pixel_vector = malloc(BLOCK_SIZE*sizeof(gsl_vector*)), ** xform_pixel_vector = malloc(BLOCK_SIZE*sizeof(gsl_vector*));
  pca_check_allocation(pixel_vector);
  pca_check_allocation(xform_pixel_vector);
  for(i=0;i<BLOCK_SIZE;++i){
    pixel_vector[i]=gsl_vector_calloc(file.bands);
    pca_check_allocation(pixel_vector[i]);
    xform_pixel_vector[i]=gsl_vector_calloc(file.bands);
    pca_check_allocation(xform_pixel_vector[i]);
  }

  io_bsq_initialize_file(file);
  
  
  int n = size/BLOCK_SIZE;
  int r = size - ((BLOCK_SIZE)*n);
  int block;

  for(i=0,block=0;i<n+1;++i,++block){
    int actual_size = BLOCK_SIZE;
    if(i==n){
      actual_size = r;
    }
    int k;
    printf("\tBlock %d\n", block+1);
    io_bsq_read_pixel(file, i*BLOCK_SIZE, actual_size, pixel_vector);
    printf("\t\tBlock read\n");
    for(k=0;k<actual_size;++k){
      pca_normalize_pixel_vector(mean_vector, pixel_vector[k]);
    }
    printf("\t\tBlock normalized\n");
    for(k=0;k<actual_size;++k){
      pca_xform_pixel_vector(file, eigen_matrix, pixel_vector[k], xform_pixel_vector[k]);
    }
    printf("\t\tBlock transformed\n");
    io_bsq_write_pixel(file, i*BLOCK_SIZE, actual_size, xform_pixel_vector);
    printf("\t\tBlock written\n");
  }
  
  for(i=0;i<BLOCK_SIZE;++i){
    gsl_vector_free(pixel_vector[i]);
    gsl_vector_free(xform_pixel_vector[i]);
  }

  
  free(pixel_vector);
  free(xform_pixel_vector);
  
  printf("Done!\n");

  gsl_vector_free(mean_vector);
  gsl_matrix_free(eigen_matrix);
  
  return;
}

