//
// Created by mm on 11/11/16.
//

#ifndef SAMPEN_ARGPARSE_H
#define SAMPEN_ARGPARSE_H

void help(void);

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

#endif //SAMPEN_ARGPARSE_H
