#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef UTILS_H

#define UTILS_H

#define MAX_EXTENSION_LEN 5 /* for file prefix */

/* prototypes: */
char *decToBase32(int);
char *binToBase32(int);
int binToDec(int);
void decTo32(int, char[], char *);
char *createFileName(char *original, int type);

#define ERROR 1



enum suffix {FILE_INPUT, FILE_MACRO, FILE_OBJECT, FILE_ENTRY, FILE_EXTERN};

typedef enum {FALSE, TRUE} boolean; /* definition of boolean type */


#endif
