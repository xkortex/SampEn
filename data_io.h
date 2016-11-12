//
// Created by mm on 11/11/16.
//

#ifndef SAMPEN_DATA_IO_H
#define SAMPEN_DATA_IO_H

double *readdata(FILE *ifile, long *filelen);
double *readbinary(FILE *file, long *num_pts);
double *read_line_data(FILE *file, long *lines);
long file_to_binary(char *ifilenm, char *ofilenm);
void test_readback(char *filename, int npts);
long filesize(FILE *pFile);
void printdata(double *ary, int len);
FILE *flexopen(char *filename, FILE *default_io, int source, char *modes);

#endif //SAMPEN_DATA_IO_H
