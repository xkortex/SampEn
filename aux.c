//
// Created by mm on 11/11/16.
//
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
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

void *emalloc(size_t numBytes){
    /* Error checked malloc */
    void *buffer = malloc(numBytes);
    if(!buffer){
        die( "Fatal error: malloc error. Out of memory. \n  x_X\n");
    }
    return buffer;
}

FILE *e_fopen(const char *__filename, const char *__modes) {
    /* Error checked file open */
    FILE *infilep = fopen(__filename, __modes);

    if (infilep == NULL) {
        die("Fatal error! Can't open file!\n");
    } else {
        return infilep;
    }
}

char *strjoin(char *s1, char *s2) {
    /* Join two strings. WARNING: CAN CAUSE MEMORY LEAKS! */
    char *newstr = emalloc(strlen(s1) + strlen(s2) + 1);
    strcat(newstr, s1);
    strcat(newstr, s2);
    return newstr;
}

