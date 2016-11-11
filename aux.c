//
// Created by mm on 11/11/16.
//
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "aux.h"

void die(const char *format, ...) {
    // Xill process with message
    va_list argptr;
    va_start(argptr, format);
    vfprintf(stderr, format, argptr);
    va_end(argptr);
    fflush(stderr);
    exit(EXIT_FAILURE);
}
