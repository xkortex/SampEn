/* file: sampen.c	Doug Lake	2 August 2002
			Last revised:	1 November 2004 (by george@mit.edu) 1.2
-------------------------------------------------------------------------------
sampen: calculate Sample Entropy
Copyright (C) 2002-2004 Doug Lake

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA.  You may also view the agreement
at http://www.fsf.org/copyleft/gpl.html.

You may contact the author via electronic mail (dlake@virginia.edu).  For
updates to this software, please visit PhysioNet (http://www.physionet.org/).

_______________________________________________________________________________

Revision history:
  1.0 (2 August 2002, Doug Lake)	Original version
  1.1 (6 January 2004, George Moody)	Removed limits on input series length
  1.2 (1 November 2004, George Moody)	Merged bug fixes from DL (normalize
					by standard deviation, detect and
					avoid divide by zero); changed code to
					use double precision, to avoid loss of
					precision for small m and large N

Compile this program using any standard C compiler, linking with the standard C
math library.  For example, if your compiler is gcc, use:
    gcc -o sampen -O sampen.c -lm

For brief instructions, use the '-h' option:
    sampen -h

Additional information is available at:
    http://www.physionet.org/physiotools/sampen/.

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
// Locals
#include "aux.h"
#include "argparse.h"
#include "data_io.h"


void sampen(double *y, int maxepochs, double r_tol, int npts);
void normalize(double *data, int n);

int main(int argc, char **argv) {
    double *data = NULL;
    double r_tolerance = .2;
    long npts;
    int maxepochs = 2;
    char *filenm = NULL;
    int nflag = 0;
    int vflag = 0;
    int i;
    FILE *infile;

    /* Read input and optional arguments. */
    // Going to make a mess of things before making it cleaner

    infile = flexopen(argv[1], stdin, argc > 0, "r");
    if (argc == 1) {
        data = readbinary(infile, &npts);
    } else {
        fprintf(stderr, "Reading from file\n");
        data = readdata(infile, &npts);
    }
    fprintf(stderr, "Success reading file\n");

    /* use the standard input if no input file specified */
//



    if (maxepochs > npts / 2) {
        fprintf(stderr, "sampen:  m too large for time series of length %ld\n", npts);
        exit(1);
    }

    if (nflag) {
        normalize(data, npts);
    }
    if (vflag) {
//        sampen2(data, maxepochs, r_tolerance, npts);
        die("SampEn2 not available currently\n");
    } else {
        sampen(data, maxepochs, r_tolerance, npts);
    }
    free(data);
    return 0;
}



/* This function subtracts the mean from data, then divides the data by their
   standard deviation. */
void normalize(double *data, int n) {
    int i;
    double mean = 0;
    double var = 0;
    for (i = 0; i < n; i++) {
        mean += data[i];
    }

    mean = mean / n;
    for (i = 0; i < n; i++) {
        data[i] = data[i] - mean;
    }

    for (i = 0; i < n; i++) {
        var += data[i] * data[i];
    }

    var = sqrt(var / n);
    for (i = 0; i < n; i++) {
        data[i] = data[i] / var;
    }

}


/* sampen() calculates an estimate of sample entropy but does NOT calculate
   the variance of the estimate */
void sampen(double *y, int maxepochs,  double r_tol, int npts) {
    double *p = NULL;
    double *e = NULL;
    long *run = NULL, *lastrun = NULL, N;
    double *A = NULL, *B = NULL;
    int M1, j, nj, jj, m;
    int i;
    double y1;

    maxepochs++;
    if ((run = (long *) calloc(npts, sizeof(long))) == NULL) exit(1);
    if ((lastrun = (long *) calloc(npts, sizeof(long))) == NULL) exit(1);
    if ((A = (double *) calloc(maxepochs, sizeof(double))) == NULL) exit(1);
    if ((B = (double *) calloc(maxepochs, sizeof(double))) == NULL) exit(1);
    if ((p = (double *) calloc(maxepochs, sizeof(double))) == NULL) exit(1);

    /* start running */
    for (i = 0; i < npts - 1; i++) {
        nj = npts - i - 1;
        y1 = y[i];
        for (jj = 0; jj < nj; jj++) {
            j = jj + i + 1;
            if (((y[j] - y1) < r_tol) && ((y1 - y[j]) < r_tol)) {
                run[jj] = lastrun[jj] + 1;
                M1 = maxepochs < run[jj] ? maxepochs : run[jj];
                for (m = 0; m < M1; m++) {
                    A[m]++;
                    if (j < npts - 1) {
                        B[m]++;
                    }
                }
            } else
                run[jj] = 0;
        }            /* for jj */
        for (j = 0; j < nj; j++) {
            lastrun[j] = run[j];
        }
    }                /* for i */

    N = (long) (npts * (npts - 1) / 2);
    p[0] = A[0] / N;
    printf("SampEn(0,%g,%d) = %lf\n", r_tol, npts, -log(p[0]));

    for (m = 1; m < maxepochs; m++) {
        p[m] = A[m] / B[m - 1];
        if (p[m] == 0)  printf("No matches! SampEn((%d,%g,%d) = Inf!\n", m, r_tol, npts);
        else  printf("SampEn(%d,%g,%d) = %lf\n", m, r_tol, npts, -log(p[m]));
    }

    free(A);
    free(B);
    free(p);
    free(run);
    free(lastrun);
} // end sampen()


