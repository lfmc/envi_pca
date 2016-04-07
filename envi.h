typedef struct x{
  int samples, lines, bands, data_type;
  char interleave[4], filename[50], new_filename[50], header[50], new_header[50];
}envi_header;
