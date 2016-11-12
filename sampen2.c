//
// Created by mm on 11/11/16.
//

#include "sampen2.h"


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

    if ((run = (int *) calloc(n, sizeof(int))) == NULL) exit(1);
    if ((run1 = (int *) calloc(n, sizeof(int))) == NULL) exit(1);
    if ((R1 = (int *) calloc(n * MM, sizeof(int))) == NULL) exit(1);
    if ((R2 = (int *) calloc(n * MM, sizeof(int))) == NULL) exit(1);
    if ((F = (int *) calloc(n * MM, sizeof(int))) == NULL) exit(1);
    if ((F1 = (int *) calloc(n * mm, sizeof(int))) == NULL) exit(1);
    if ((F2 = (int *) calloc(n * mm, sizeof(int))) == NULL) exit(1);
    if ((K = (double *) calloc((mm + 1) * mm, sizeof(double))) == NULL) exit(1);
    if ((A = (double *) calloc(mm, sizeof(double))) == NULL) exit(1);
    if ((B = (double *) calloc(mm, sizeof(double))) == NULL) exit(1);
    if ((p = (double *) calloc(mm, sizeof(double))) == NULL) exit(1);
    if ((v1 = (double *) calloc(mm, sizeof(double))) == NULL) exit(1);
    if ((v2 = (double *) calloc(mm, sizeof(double))) == NULL) exit(1);
    if ((s1 = (double *) calloc(mm, sizeof(double))) == NULL) exit(1);
    if ((n1 = (double *) calloc(mm, sizeof(double))) == NULL) exit(1);
    if ((n2 = (double *) calloc(mm, sizeof(double))) == NULL) exit(1);

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
                    if (j < n - 1) {
                        B[m]++;
                    }
                    F1[i + m * n]++;
                    F[i + n * m]++;
                    F[j + n * m]++;
                }
            } else {
                run[jj] = 0;
            }
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
