#include<stdio.h>
#include<stdlib.h>
#include<gsl/gsl_vector.h>
#include"envi.h"
#include"io_bsq.h"

static void io_bsq_check_allocation(void* pointer){
  if(!pointer){
    fprintf(stderr,"Failure in memory\n");
    exit(EXIT_FAILURE);
  }
}

extern gsl_vector* io_bsq_read_band(envi_header file, int band){
  FILE* fp = fopen(file.filename, "rb");
  if(!fp){
    fprintf(stderr, "Failed to open target: %s\n", file.filename);
    exit(EXIT_FAILURE);
  }
  int size = (file.samples) * (file.lines);
  gsl_vector* band_vector = gsl_vector_calloc(size);
  io_bsq_check_allocation(band_vector);
  int ix;
  int x = file.data_type;
  if(x==1){
    unsigned char c;
    fseek(fp, sizeof(unsigned char)*size*band, SEEK_SET);
    for(ix=0;ix<size;++ix){
      fread(&c, sizeof(unsigned char), 1, fp);
      gsl_vector_set(band_vector, ix, (double) c);
    }    
  }
  if(x==2){
    short int c;
    fseek(fp, sizeof(short int)*size*band, SEEK_SET);
    for(ix=0;ix<size;++ix){
      fread(&c, sizeof(short int), 1, fp);
      gsl_vector_set(band_vector, ix, (double) c);
    }    
  }
  if(x==3){
    int c;
    fseek(fp, sizeof(int)*size*band, SEEK_SET);
    for(ix=0;ix<size;++ix){
      fread(&c, sizeof(int), 1, fp);
      gsl_vector_set(band_vector, ix, (double) c);
    }
  }
  if(x==4){
    float c;
    fseek(fp, sizeof(float)*size*band, SEEK_SET);
    for(ix=0;ix<size;++ix){
      fread(&c, sizeof(float), 1, fp);
      gsl_vector_set(band_vector, ix, (double) c);
    }    
  }
  if(x==5){
    double c;
    fseek(fp, sizeof(double)*size*band, SEEK_SET);
    for(ix=0;ix<size;++ix){
      fread(&c, sizeof(double), 1, fp);
      gsl_vector_set(band_vector, ix, (double) c);
    }    
  }
  if(x==12){
    unsigned short int c;
    fseek(fp, sizeof(unsigned short int)*size*band, SEEK_SET);
    for(ix=0;ix<size;++ix){
      fread(&c, sizeof(unsigned short int), 1, fp);
      gsl_vector_set(band_vector, ix, (double) c);
    }    
  }
  if(x==13){
    unsigned int c;
    fseek(fp, sizeof(unsigned int)*size*band, SEEK_SET);
    for(ix=0;ix<size;++ix){
      fread(&c, sizeof(unsigned int), 1, fp);
      gsl_vector_set(band_vector, ix, (double) c);
    }    
  }
  fclose(fp);
  return band_vector;
}

extern void io_bsq_read_pixel(envi_header file, int start, int number, gsl_vector** pixel_vector){
  FILE* fp = fopen(file.filename, "rb");
  if(!fp){
    fprintf(stderr, "Failure to open target: %s\n", file.filename);
    exit(EXIT_FAILURE);
  }
  int size = (file.lines) * (file.samples);
  int x = file.data_type;
  if(x==1){
    int band;
    for(band=0;band<file.bands;++band){
      fseek(fp, sizeof(unsigned char)*((size*band)+start), SEEK_SET);
      int i;
      for(i=0;i<number;++i){
	unsigned char c;
	fread(&c, sizeof(unsigned char), 1, fp);
	gsl_vector_set(pixel_vector[i], band, (double) c);
      }
    }
  }
  if(x==2){
    int band;
    for(band=0;band<file.bands;++band){
      fseek(fp, sizeof(short int)*((size*band)+start), SEEK_SET);
      int i;
      for(i=0;i<number;++i){
	short int c;
	fread(&c, sizeof(short int), 1, fp);
	gsl_vector_set(pixel_vector[i], band, (double) c);
      }
    }
  }
  if(x==3){
    int band;
    for(band=0;band<file.bands;++band){
    fseek(fp, sizeof(int)*((size*band)+start), SEEK_SET);
    int i;
    for(i=0;i<number;++i){
      int c;
      fread(&c, sizeof(int), 1, fp);
      gsl_vector_set(pixel_vector[i], band, (double) c);
    }
  }
  if(x==4){
    fseek(fp, sizeof(float)*((size*band)+start), SEEK_SET);
    int i;
    for(i=0;i<number;++i){
      float c;
      fread(&c, sizeof(float), 1, fp);
      gsl_vector_set(pixel_vector[i], band, (double) c);
    }
  }
  }
  if(x==5){
    int band;
    for(band=0;band<file.bands;++band){
    fseek(fp, sizeof(double)*((size*band)+start), SEEK_SET);
    int i;
    for(i=0;i<number;++i){
      double c;
      fread(&c, sizeof(double), 1, fp);
      gsl_vector_set(pixel_vector[i], band, (double) c);
    }
    }
  }
  if(x==12){
    int band;
    for(band=0;band<file.bands;++band){
    fseek(fp, sizeof(unsigned short int)*((size*band)+start), SEEK_SET);
    int i;
    for(i=0;i<number;++i){
      unsigned short int c;
      fread(&c, sizeof(unsigned short int), 1, fp);
      gsl_vector_set(pixel_vector[i], band, (double) c);
    }
    }
  }
  if(x==13){
    int band;
    for(band=0;band<file.bands;++band){
    fseek(fp, sizeof(unsigned int)*((size*band)+start), SEEK_SET);
    int i;
    for(i=0;i<number;++i){
      unsigned int c;
      fread(&c, sizeof(unsigned int), 1, fp);
      gsl_vector_set(pixel_vector[i], band, (double) c);
    }
    }
  }
  fclose(fp);
  return;
}

extern void io_bsq_write_pixel(envi_header file, int start, int number, gsl_vector** pixel_vector){
  FILE* fp = fopen(file.new_filename, "r+b");
  if(!fp){
    fprintf(stderr, "Failed to open target: %s\n", file.new_filename);
    exit(EXIT_FAILURE);
  }
  int size = (file.samples) * (file.lines);
  int i, j;
  double d;
  for(i=0;i<file.bands;++i){
    fseek(fp, sizeof(double)*((size*i)+start), SEEK_SET);
    for(j=0;j<number;++j){
      d = gsl_vector_get(pixel_vector[j], i);
      fwrite(&d, sizeof(double), 1, fp);
    }
  }
  fclose(fp);
  return;
}

extern void io_bsq_initialize_file(envi_header file){
  FILE* fp = fopen(file.new_filename, "wb");
  if(!fp){
    fprintf(stderr, "Unable to initialize target file: %s\n", file.new_filename);
    exit(EXIT_FAILURE);
  }
  int size = (file.lines)* (file.samples);
  double d =0;
  int i;
  for(i=0;i<size;++i){
    fwrite(&d, sizeof(double), 1, fp);
  }
  fclose(fp);
  return;
}
