//
// Created by mm on 11/11/16.
//

#include <stdlib.h>
#include <stdio.h>

#include "argparse.h"


void old_arg_processor (char *filenm, int argc, char **argv) {
    int i;
    double r_tolerance = .2;
    int maxepochs = 2;
    int nflag = 0;
    int vflag = 0;
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
                        r_tolerance = atof(argv[i]);
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
                        maxepochs = atoi(argv[i]);
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