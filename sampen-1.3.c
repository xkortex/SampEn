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

double *readdata(char *filenm, int *filelen);

void sampen(double *data, int maxepoch, double r_tol, int npts, int max_npts_j);

void sampen2(double *y, int mm, double r, int n);

void normalize(double *data, int n);

void help(void);

static int vector_flag = 1; //todo: make a properly scoped flag

int main(int argc, char *argv[]) {
    double *data = NULL;
    double r = .2;
    int npts;
    int max_npts_j = 100; // This parameter trades accuracy for speed. Higher = slower, more entropy look-ahead
    int maxepoch = 6;
    char *filenm = NULL;
    int nflag = 0;
    int vflag = 0;
    int i;

    /* Read input and optional arguments. */
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'h':
                    help();
                    exit(0);
                    break;
                case 'r':
                    if (argv[i][2] != '\0') {
                        fprintf(stderr, "sampen:  Unrecognized option '%s'\n",
                                argv[i]);
                        help();
                        exit(1);
                    }
                    if (i + 1 < argc) {
                        i++;
                        r = atof(argv[i]);
                        break;
                    }
                    fprintf(stderr,
                            "sampen:  No value specified for option '-r'\n");
                    help();
                    exit(1);
                    break;
                case 'm':
                    if (argv[i][2] != '\0') {
                        fprintf(stderr, "sampen:  Unrecognized option '%s'\n",
                                argv[i]);
                        help();
                        exit(1);
                    }
                    if (i + 1 < argc) {
                        i++;
                        maxepoch = atoi(argv[i]);
                        break;
                    }
                    fprintf(stderr,
                            "sampen:  No value specified for option '-m'\n");
                    help();
                    exit(1);
                    break;
                case 'n':
                    if (argv[i][2] != '\0') {
                        fprintf(stderr, "sampen:  Unrecognized option '%s'\n",
                                argv[i]);
                        help();
                        exit(1);
                    }
                    nflag = 1;
                    break;
                case 'v':
                    if (argv[i][2] != '\0') {
                        fprintf(stderr, "sampen:  Unrecognized option '%s'\n",
                                argv[i]);
                        help();
                        exit(1);
                    }
                    vflag = 1;
                    break;
                default:
                    fprintf(stderr, "sampen: Unrecognized option '%s'\n",
                            argv[i]);
                    help();
                    exit(1);
            }
        } else {
            if (!filenm)
                filenm = argv[i];
            else {
                fprintf(stderr, "sampen:  Too many inputs\n");
                exit(1);
            }
        }
    }

    if (!filenm)
        filenm = "-";    /* use the standard input if no input file specified */
    data = readdata(filenm, &npts);
    if (maxepoch > npts / 2) {
        fprintf(stderr,
                "sampen:  m too large for time series of length %d\n", npts);
        exit(1);
    }

    if (nflag)
        normalize(data, npts);
    if (vflag)
        sampen2(data, maxepoch, r, npts);
    else
        sampen(data, maxepoch, r, npts, max_npts_j);
    free(data);
    return 0;
}

double *readdata(char *filenm, int *filelen) {
    FILE *ifile;
    long maxdat = 0L, npts = 0L;
    double *data = NULL, y, yp = 0.0;

    if (strcmp(filenm, "-") == 0) {
        filenm = "standard input";
        ifile = stdin;
    } else if ((ifile = fopen(filenm, "rt")) == NULL) {
        fprintf(stderr, "sampen:  Could not open %s \n", filenm);
        exit(1);
    }

    while (fscanf(ifile, "%lf", &y) == 1) {
        if (++npts >= maxdat) {
            double *s;

            maxdat += 5000;    /* allow the input buffer to grow (the
				   increment is arbitrary) */
            if ((s = realloc(data, maxdat * sizeof(double))) == NULL) {
                fprintf(stderr,
                        "sampen: insufficient memory, truncating input at row %ld\n", npts);
                break;
            }
            data = s;
        }
        data[npts - 1] = y;
    }

    fclose(ifile);

    if (npts < 1) {
        fprintf(stderr, "sampen: %s contains no data\n", filenm);
        help();
        exit(1);
    }

    *filelen = npts;
    return (data);
}

/* This function subtracts the mean from data, then divides the data by their
   standard deviation. */
void normalize(double *data, int n) {
    int i;
    double mean = 0;
    double var = 0;
    for (i = 0; i < n; i++)
        mean += data[i];
    mean = mean / n;
    for (i = 0; i < n; i++)
        data[i] = data[i] - mean;
    for (i = 0; i < n; i++)
        var += data[i] * data[i];
    var = sqrt(var / n);
    for (i = 0; i < n; i++)
        data[i] = data[i] / var;
}

/* sampen2 calculates an estimate of sample entropy and the variance of the
   estimate. */
void sampen2(double *y, int mm, double r, int n) {
    double *p = NULL;
    double *v1 = NULL, *v2 = NULL, *s1 = NULL, dv;
    int *R1 = NULL, *R2 = NULL, *F2 = NULL, *F1 = NULL, *F = NULL, FF;
    int *run = NULL, *run1 = NULL;
    double *A = NULL, *B = NULL;
    double *K = NULL, *n1 = NULL, *n2 = NULL;
    int MM;
    int m, m1, i, j, nj, jj, d, d2, i1, i2, dd;
    int nm1, nm2, nm3, nm4;
    double y1;

    mm++;
    MM = 2 * mm;

    if ((run = (int *) calloc(n, sizeof(int))) == NULL)
        exit(1);
    if ((run1 = (int *) calloc(n, sizeof(int))) == NULL)
        exit(1);
    if ((R1 = (int *) calloc(n * MM, sizeof(int))) == NULL)
        exit(1);
    if ((R2 = (int *) calloc(n * MM, sizeof(int))) == NULL)
        exit(1);
    if ((F = (int *) calloc(n * MM, sizeof(int))) == NULL)
        exit(1);
    if ((F1 = (int *) calloc(n * mm, sizeof(int))) == NULL)
        exit(1);
    if ((F2 = (int *) calloc(n * mm, sizeof(int))) == NULL)
        exit(1);
    if ((K = (double *) calloc((mm + 1) * mm, sizeof(double))) == NULL)
        exit(1);
    if ((A = (double *) calloc(mm, sizeof(double))) == NULL)
        exit(1);
    if ((B = (double *) calloc(mm, sizeof(double))) == NULL)
        exit(1);
    if ((p = (double *) calloc(mm, sizeof(double))) == NULL)
        exit(1);
    if ((v1 = (double *) calloc(mm, sizeof(double))) == NULL)
        exit(1);
    if ((v2 = (double *) calloc(mm, sizeof(double))) == NULL)
        exit(1);
    if ((s1 = (double *) calloc(mm, sizeof(double))) == NULL)
        exit(1);
    if ((n1 = (double *) calloc(mm, sizeof(double))) == NULL)
        exit(1);
    if ((n2 = (double *) calloc(mm, sizeof(double))) == NULL)
        exit(1);

    for (i = 0; i < n - 1; i++) {
        nj = n - i - 1;
        y1 = y[i];
        for (jj = 0; jj < nj; jj++) {
            j = jj + i + 1;
            if (((y[j] - y1) < r) && ((y1 - y[j]) < r)) {
                run[jj] = run1[jj] + 1;
                m1 = (mm < run[jj]) ? mm : run[jj];
                for (m = 0; m < m1; m++) {
                    A[m]++;
                    if (j < n - 1)
                        B[m]++;
                    F1[i + m * n]++;
                    F[i + n * m]++;
                    F[j + n * m]++;
                }
            } else
                run[jj] = 0;
        }            /* for jj */

        for (j = 0; j < MM; j++) {
            run1[j] = run[j];
            R1[i + n * j] = run[j];

        }
        if (nj > MM - 1)
            for (j = MM; j < nj; j++)
                run1[j] = run[j];
    }                /* for i */

    for (i = 1; i < MM; i++)
        for (j = 0; j < i - 1; j++)
            R2[i + n * j] = R1[i - j - 1 + n * j];
    for (i = MM; i < n; i++)
        for (j = 0; j < MM; j++)
            R2[i + n * j] = R1[i - j - 1 + n * j];
    for (i = 0; i < n; i++)
        for (m = 0; m < mm; m++) {
            FF = F[i + n * m];
            F2[i + n * m] = FF - F1[i + n * m];
            K[(mm + 1) * m] += FF * (FF - 1);
        }

    for (m = mm - 1; m > 0; m--)
        B[m] = B[m - 1];
    B[0] = (double) n * (n - 1) / 2;
    for (m = 0; m < mm; m++) {
        p[m] = (double) A[m] / B[m];
        v2[m] = p[m] * (1 - p[m]) / B[m];
    }
    dd = 1;
    for (m = 0; m < mm; m++) {
        d2 = m + 1 < mm - 1 ? m + 1 : mm - 1;
        for (d = 0; d < d2 + 1; d++) {
            for (i1 = d + 1; i1 < n; i1++) {
                i2 = i1 - d - 1;
                nm1 = F1[i1 + n * m];
                nm3 = F1[i2 + n * m];
                nm2 = F2[i1 + n * m];
                nm4 = F2[i2 + n * m];
                for (j = 0; j < (dd - 1); j++) {
                    if (R1[i1 + n * j] >= m + 1)
                        nm1--;
                    if (R2[i1 + n * j] >= m + 1)
                        nm4--;
                }
                for (j = 0; j < 2 * (d + 1); j++)
                    if (R2[i1 + n * j] >= m + 1)
                        nm2--;
                for (j = 0; j < (2 * d + 1); j++)
                    if (R1[i2 + n * j] >= m + 1)
                        nm3--;
                K[d + 1 + (mm + 1) * m] +=
                        (double) 2 * (nm1 + nm2) * (nm3 + nm4);
            }
        }
    }

    n1[0] = (double) n * (n - 1) * (n - 2);
    for (m = 0; m < mm - 1; m++)
        for (j = 0; j < m + 2; j++)
            n1[m + 1] += K[j + (mm + 1) * m];
    for (m = 0; m < mm; m++) {
        for (j = 0; j < m + 1; j++)
            n2[m] += K[j + (mm + 1) * m];
    }

    for (m = 0; m < mm; m++) {
        v1[m] = v2[m];
        dv = (n2[m] - n1[m] * p[m] * p[m]) / (B[m] * B[m]);
        if (dv > 0)
            v1[m] += dv;
        s1[m] = (double) sqrt((double) (v1[m]));
    }

    for (m = 0; m < mm; m++) {
        if (p[m] == 0)
            printf("No matches! SampEn((%d,%g,%d) = Inf"
                           " (standard deviation = Inf)!\n", m, r, n);
        else
            printf("SampEn(%d,%g,%d) = %lf (standard deviation = %lf)\n",
                   m, r, n, -log(p[m]), s1[m]);
    }

    free(A);
    free(B);
    free(p);
    free(run);
    free(run1);
    free(s1);
    free(K);
    free(n1);
    free(R1);
    free(R2);
    free(v1);
    free(v2);
    free(F);
    free(F1);
    free(F2);
}

/* sampen() calculates an estimate of sample entropy but does NOT calculate
   the variance of the estimate */
void sampen(double *data, int maxepoch, double r_tol, int npts, int max_npts_j) {
    double *p = NULL;
    double *e = NULL;
    long *run = NULL, *lastrun = NULL, N;
    double *A = NULL, *B = NULL;
    int M1, j, npts_j, j2, m;
    int i;
    double y1;
    int m_start = 0;

    maxepoch++;
    if ((run = (long *) calloc(npts, sizeof(long))) == NULL) exit(1);
    if ((lastrun = (long *) calloc(npts, sizeof(long))) == NULL) exit(1);
    if ((A = (double *) calloc(maxepoch, sizeof(double))) == NULL) exit(1);
    if ((B = (double *) calloc(maxepoch, sizeof(double))) == NULL) exit(1);
    if ((p = (double *) calloc(maxepoch, sizeof(double))) == NULL) exit(1);
//    fprintf(stderr, "Working file \n____\n");
    /* Let's try capping npts_j - yes this results in MASSIVE speedups. Results for 100k test, -m 10
     * No cap: 45.52 s
     * 10000 : 10.96 s
     * 1000  :  1.64 s
     * 100   :  0.34 s
     * 10    :  0.17 s
     *
     * However, this results in deviations in the results (however not very large). Deviations are worse for lower
     * number of epochs (esp. 0). However, even for super low values of npts_j, difference is less than 50%
     */



    /* start running */
    for (i = 0; i < npts - 1; i++) {
        npts_j = npts - i - 1;          // I think this is what is making the algo polytime
        npts_j = (npts_j < max_npts_j) ? npts_j : max_npts_j;
        y1 = data[i];
        for (j2 = 0; j2 < npts_j; j2++) {
            j = j2 + i + 1;
            if (((data[j] - y1) < r_tol) && ((y1 - data[j]) < r_tol)) {
                run[j2] = lastrun[j2] + 1;
                M1 = (maxepoch < run[j2]) ? maxepoch : run[j2];
                for (m = m_start; m < M1; m++) {
                    A[m]++;
                    if (j < npts - 1) {
                        B[m]++;
                    }
                }
            } else {
                run[j2] = 0;
            }

        }            /* for jj */
        for (j = 0; j < npts_j; j++) { // or this
            lastrun[j] = run[j];
        }

    }                /* outer loop: for i */


    FILE *outfile = stdout;

    N = (long) (npts * (npts - 1) / 2);
    p[0] = A[0] / N;
    if (vector_flag) {
        fprintf(stdout, "%lf,", -log(p[0]));
    } else {
        printf("SampEn(0,%g,%d) = %lf\n", r_tol, npts, -log(p[0]));
    }


    for (m = 1; m < maxepoch; m++) {
        p[m] = A[m] / B[m - 1];
        if (vector_flag) {
            if (p[m] == 0) {
                fprintf(outfile, "inf");
            } else {
                fprintf(outfile, "%lf", -log(p[m]));
            }
            if (m < maxepoch - 1) {
                fprintf(outfile, ",");
            } else {
                putc('\0', outfile);
            }
        } else {
            if (p[m] == 0) {
                printf("No matches! SampEn((%d,%g,%d) = Inf!\n", m, r_tol, npts);
            } else {
                printf("SampEn(%d,%g,%d) = %lf\n", m, r_tol, npts, -log(p[m]));
            }
        }

    }

    free(A);
    free(B);
    free(p);
    free(run);
    free(lastrun);
}

static char *help_strings[] = {
        "usage: %s [OPTIONS ...] [TEXT-FILE]\n",
        "where OPTIONS may include:",
        " -h    print this usage summary",
        " -m M  set the maximum epoch length to M (default: 2)",
        " -n    normalize such that the mean of the input is 0 and the sample",
        "       variance is 1",
        " -r R  set the tolerance to R (default: 0.2)",
        " -v    output an estimate of the standard deviation of each SampEn",
        "TEXT-FILE should contain the time series (a column of decimal numbers);",
        "if omitted, sampen reads its standard input.  The output contains values of",
        "SampEn(m,r,N) where m is the epoch length, r is the tolerance, and N is the",
        "length of the input series.",
        NULL
};

void help() {
    int i;

    (void) fprintf(stderr, help_strings[0], "sampen");
    for (i = 1; help_strings[i] != NULL; i++)
        (void) fprintf(stderr, "%s\n", help_strings[i]);
}
