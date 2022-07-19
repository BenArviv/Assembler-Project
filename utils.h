#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* prototypes: */
char *decToBase32(int);
char *binToBase32(int);
int binToDec(int);
void decTo32(int, char[], char *);

/* this array represents the symbols for our base-32 numbers */
char key[32] = {'!', '@', '#', '$', '%', '^', '&', '*', '<', '>',
                     'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 
                     'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v'};
