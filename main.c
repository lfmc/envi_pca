#include<stdio.h>
#include<stdlib.h>
#include"envi.h"
#include"new_envi.h"
#include"pca.h"

int main(int argc, char** argv){
  if(argc!=3){
    fprintf(stderr, "Wrong format! <program> <input> <output>\n");
  }
  envi_header file = file_envi(argv[1], argv[2]);
  printf("Initiating transformation\n");
  pca_xform(file);
  printf("Transformation finished\n");
  return 0;
}
