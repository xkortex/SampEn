//
// Created by mm on 11/11/16.
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Locals
#include "aux.h"
#include "data_io.h"


int main(int argc, char **argv) {
    if (argc == 1) die("Must specify filename and/or options");
    int npts = -1;
    long lines = 0;
    char *outname = strjoin(argv[1], ".dat");
    fprintf(stderr, "outname2: %s\n", outname);

    FILE *infile = e_fopen(argv[1], "r");
    fprintf(stderr, "inFile size: %ld\n", filesize(infile));
    double *data = readdata(infile, &lines);//read_line_data(infile, &lines);
    fprintf(stderr, "Number of lines: %ld\n", lines);
    printdata(data, lines);
    npts = file_to_binary(argv[1], outname);

    fprintf(stderr, "n points: %d\n", npts);
    fprintf(stderr, "outname2: %s\n", outname);

    test_readback(outname, npts);
}
