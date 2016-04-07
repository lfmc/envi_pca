#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"envi.h"
#include"new_envi.h"


static char names[][50]={"samples", "lines", "bands", "data type", "interleave"};

static void envi_read_string(char* line, char* interleave){
  sscanf(line, "%*[^=]%*c%3s", interleave);
  return;
}

static int envi_read_number(char* line){
  int d;
  sscanf(line, "%*[^=]%*c%d", &d);
  return d;
}

static envi_header envi_mount(char* filename){
  envi_header aux;
  char real_name[50];
  sscanf(filename, "%49[^.]", real_name);
  char header_name[50];
  sprintf(header_name, "%s.hdr", real_name);
  FILE* fp = fopen(header_name, "r");
  if(!fp){
    sprintf(header_name, "%s.HDR", real_name);
    fp=fopen(header_name, "r");
    if(!fp){
      fprintf(stderr, "Unable to locate or open correponding header file\n");
      exit(EXIT_FAILURE);
    }
  }
  
  sprintf(aux.filename, "%s", filename);
  sprintf(aux.header, "%s", header_name);

  char line[50];
  int t = fscanf(fp,"%49[^\n]", line);
  fgetc(fp);
  while(t!=EOF){
    if(strncmp(names[0], line, 5) == 0 )
      aux.samples=envi_read_number(line);
    if(strncmp(names[1], line, 5) == 0 )
      aux.lines=envi_read_number(line);
    if(strncmp(names[2], line, 5) == 0 )
      aux.bands=envi_read_number(line);
    if(strncmp(names[3], line, 5) == 0 )
      aux.data_type=envi_read_number(line);
    if(strncmp(names[4], line, 5) == 0 )
      envi_read_string(line, aux.interleave);
    t = fscanf(fp, "%49[^\n]", line);
    fgetc(fp);
  }
  fclose(fp);
  return aux;
}

/*static void envi_check(envi_header header){

  }*/


extern envi_header file_envi(char* filename, char* newfilename){
  envi_header header = envi_mount(filename);
  sprintf(header.new_filename, "%s", newfilename);
  char realnewfilename[50];
  sscanf(newfilename, "%49[^.]", realnewfilename);
  sprintf(header.new_header, "%s.hdr", realnewfilename);
  FILE* fp = fopen(header.new_header, "w");
  if(!fp){
    fprintf(stderr, "Failed to create new envi header.\n");
    exit(EXIT_FAILURE);
  }
  fprintf(fp, "ENVI\n");
  fprintf(fp, "description = { %s transformation}\n", header.filename);
  fprintf(fp, "samples = %d\n", header.samples);
  fprintf(fp, "lines = %d\n", header.lines);
  fprintf(fp, "bands = %d\n", header.bands);
  fprintf(fp, "header offset  = 0\n");
  fprintf(fp, "file type = ENVI Standard\n");
  fprintf(fp, "data type = 5\n");
  fprintf(fp, "interleave = bsq\n");
  fclose(fp);
  return header;
}

