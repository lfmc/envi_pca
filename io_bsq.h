/*
  Interface for input/output module of standard envi bsq binary files
 */

/*Returns a spectral band vectors*/
gsl_vector* io_bsq_read_band(envi_header file, int band);

/*Returns a set of pixel vectors*/
void io_bsq_read_pixel(envi_header file, int start, int number, gsl_vector** pixel_vector);

/*Writes a set of pixel vectors in bsq fashion*/
void io_bsq_write_pixel(envi_header file, int start, int number, gsl_vector** pixel_vector);

/*Initializes a all-zero file with the necessary size*/
void io_bsq_initialize_file(envi_header file);
