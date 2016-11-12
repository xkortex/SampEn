//
// Created by mm on 11/11/16.
//

#ifndef SAMPEN_AUX_H
#define SAMPEN_AUX_H

void die(const char *format, ...);

void *emalloc(size_t numBytes);
FILE *e_fopen(const char *__filename, const char *__modes);
char *strjoin(char *s1, char *s2);

#endif //SAMPEN_AUX_H
