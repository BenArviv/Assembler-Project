#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifndef UTILS.H

#define UTILS.H

#define MAX_EXTENSION_LEN 5 /* for file prefix */

/* prototypes: */
char *decToBase32(int);
char *binToBase32(int);
int binToDec(int);
void decTo32(int, char[], char *);
char *createFileName(char *, int);

/* this array represents the symbols for our base-32 numbers */
const char key[32] = {'!', '@', '#', '$', '%', '^', '&', '*', '<', '>',
                     'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
                     'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v'};

enum suffix {FILE_INPUT, FILE_MACRO, FILE_OBJECT, FILE_ENTRY, FILE_EXTERN};

typedef enum {FALSE, TRUE} boolean; /* definition of boolean type */


#endif
