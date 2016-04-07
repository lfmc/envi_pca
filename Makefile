SHELL = /bin/sh
GCCFLAGS = -Wall -ansi -g 
CLIBS = -lgsl -lgslcblas -lm
INCLUDE = -I/usr/local/include
LINK = -L/usr/local/lib

OBJS = new_envi.o pca.o io_bsq.o


all: new_envi.o $(OBJS)
	gcc $(GCCFLAGS) main.c $(OBJS) $(CLIBS) $(INCLUDE) $(LINK) -o bsq_pca --static
pca.o: io_bsq.o envi.h pca.h pca.c
	gcc $(GCCFLAGS) pca.c $(CLIBS) $(INCLUDE) $(LINK) -c --static
new_envi.o: new_envi.h envi.h new_envi.c
	gcc $(GCCFLAGS) new_envi.c $(CLIBS) $(INCLUDE) $(LINK) -c --static
io_bsq.o: io_bsq.h envi.h io_bsq.c
	gcc $(GCCFLAGS) io_bsq.c $(CLIBS) $(INCLUDE) $(LINK) -c --static
.PHONY: clean
clean:
	rm *.o

