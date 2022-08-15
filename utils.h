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
void decTo32(int, char[], char *);
void copyWord(char *word, char *line);
void encodeInsturction(unsigned int word, unsigned int instructions[], int *ic);
void write_error(int line_num, int error);
char *decToBase32(int);
char *binToBase32(int);
char *createFileName(char *original, int type);
char* skipWhiteChars(char *line);
char *nextWord(char *line);
char *nextCommaWord(char *word, char *line);
char *nextString(char *word, char *line);
int binToDec(int);
int findCMD(char *word, stringStruct cmd[]);
int findStr(char *word, stringStruct cmd[], int arrLen);
int findDirective(char *word, stringStruct dir[]);
unsigned int extractBits(unsigned int word, int start, int end);
unsigned int encodeARE(unsigned int info, int type);
boolean skipLine(char *line);
boolean isLineEnd(char *line);
boolean isRegister(char *word);
boolean isError(int *error);
boolean isValidNum(char *word);
boolean isValidString(char *string);

#endif
