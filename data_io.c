//
// Created by mm on 11/11/16.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data_io.h"
#include "argparse.h"
#include "aux.h"

double *readbinary(FILE *file, long *num_pts) {
    long buffersize = 1000L, npts = 0L;
    size_t dataread = 0;
    double num;
    double *data = emalloc(buffersize);
    while (fread(&num, sizeof(double), 1, file) == 1) {
        fprintf(stderr, "read: %lf\n", num);
        if (++npts >= buffersize) { // todo: ugly, refactor this
            double *s;

            buffersize += buffersize / 2;    // Buffer grows exponentially
            if ((s = realloc(data, buffersize * sizeof(double))) == NULL) {
                fprintf(stderr, "sampen: insufficient memory, truncating input at row %ld\n", npts);
                break;
            }
            data = s;
        }
        data[npts - 1] = num;
    }

    fprintf(stderr, "   Points read: %ld\n   Points expected: %ld  dataread: %ld\n", buffersize, npts, dataread);
    if (dataread != npts) die("Mismatch between data size provided and data read from file\n");
    *num_pts = npts;
    return data;
}

double *readdata(FILE *ifile, long *filelen) {
    /* Read data in the form of formatted floats from a file, separated by line breaks. Return a data array */
//    FILE *ifile;
    long buffersize = 1000L, npts = 0L;
    double *data = NULL;
    double *buffer = NULL;
    double *newbuffer;
    double num;

//    if (strcmp(filenm, "-") == 0) {
//        filenm = "standard input";
//        ifile = stdin;
//    } else {
////        die("sampen:  Could not open %s \n", filenm);
//        ifile = e_fopen(filenm, "rt");
//    }
    data = emalloc(buffersize);
    // todo: option for reading in binary data
    while (fscanf(ifile, "%lf", &num) == 1) {
        fprintf(stderr, "%lf ", num);
        if (++npts >= buffersize) { // todo: ugly, refactor this


            buffersize += buffersize / 2;    // Buffer grows exponentially
//            if ((newbuffer = realloc(data, buffersize * sizeof(double))) == NULL) {
//                fprintf(stderr, "sampen: insufficient memory, truncating input at row %ld\n", npts);
//                die("Fatal: heap corrupted, exiting\n"); //break
//            }
            newbuffer = realloc(data, buffersize * sizeof(double));
            if (newbuffer) {
                data = newbuffer;
            } else {
                die("Insufficient memory, terminating\n");
            }
        }
        data[npts - 1] = num;
    }

    fclose(ifile);

    if (npts < 1) {
        fprintf(stderr, "I AM BROKEN\n\n");
        fprintf(stderr, "sampen: contains no data\n");
        help();
        exit(1);
    }

    *filelen = npts;
    return (data);
}

void printdata(double *ary, int len) {
    int i = 0;
    for (i = 0; i < len; i++) { fprintf(stdout, "%lf\n", ary[i]); }
}

double *read_line_data(FILE *file, long *lines) {
    *lines = 0;
    long size = filesize(file);
    char *buffer = (char *) emalloc(size * sizeof(char));
    size_t result = fread(buffer, 1, size, file);
    fprintf(stderr, "size of file : %ld %ld\n", size, result);

    if (result != size) {
        fputs("Reading error", stderr);
        exit(3);
    }
    while (EOF != (scanf("%*[^\n]"), scanf("%*c"))) {
        (*lines)++; // get line count
    }
    rewind(file);
    fprintf(stderr, "Length of file (lines): %ld\n", *lines);
    long i = 0;
    double *data = (double *) emalloc((*lines + 2) * sizeof(double));
    while (fscanf(file, "%lf", data + i) == 1) {
        i++;
    }
    free(buffer);
    return data;

}

long file_to_binary(char *ifilenm, char *ofilenm) {
    long filelen, i;
//    FILE *infile = e_fopen(filenm, "r");
    FILE *outfile = e_fopen(ofilenm, "wb");
    double *data = readdata(ifilenm, &filelen);
    for (i = 0; i < filelen; i++) {
        fwrite(&data[i], sizeof(double), 1, outfile);
//        printf("%lf ", data[i]);
    }
    fclose(outfile);
    return filelen;
}

void test_readback(char *filename, int npts) {
    long num_pts;
    FILE *file = e_fopen(filename, "rb");
    fprintf(stderr, "Readback: %s :%d\n", filename, npts);
    double *data = readbinary(file, &num_pts);// = emalloc(npts * sizeof(double));
    int i;
    printdata(data, npts);
}

long filesize(FILE *pFile) {
    // obtain file size:
    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);
    return lSize;
}

FILE *flexopen(char *filename, FILE *default_io, int source, char *modes) {
    /* Flexible interface for reading/writing to/from a file, or a standard pipe */
    FILE *io;
    if (source && strlen(filename) > 0) {
        io = fopen(filename, modes);
        if (io == NULL) {
            die("Fatal: Unable to open file: %s\n", filename);
        } else {
            return io;
        }
    }
    return default_io;
}