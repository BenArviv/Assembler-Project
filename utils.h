#ifndef UTILS_H

#define UTILS_H

#include "main.h"
#include "structures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>




/* prototypes: */
char *decToBase32(int);
char *binToBase32(int);
int binToDec(int);
void decTo32(int, char[], char *);
char *createFileName(char *original, int type);
boolean skipLine(char *line);
char* skipWhiteChars(char *line);
boolean isLineEnd(char *line);
void copyWord(char *word, char *line);
int findCMD(char *word, stringStruct cmd[]);
int findStr(char *word, stringStruct cmd[], int arrLen);
boolean isRegister(char *word);
char *nextWord(char *line);
boolean isError(int *error);
int findDirective(char *word, stringStruct dir[]);
char *nextCommaWord(char *word, char *line);
char *nextString(char *word, char *line);
boolean isValidNum(char *word);
boolean isValidString(char *string);
unsigned int encodeARE(unsigned int info, int type);
void encodeInsturction(unsigned int word, unsigned int instructions[], int *ic);
void write_error(int line_num, int error);

#endif
