/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
**************************************************/

#ifndef FIRSTPASS_H
#define FIRSTPASS_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "utils.h"


/* Prototypes */ 
void firstPass(FILE *fp, extVars *vars);
void readLine(char *line, extVars *vars);
void writeNumberToData(int num, int *dc, unsigned int data[]);
void writeStringToData(char *str, int *dc, unsigned int data[]);
int numberOfWords(boolean firstOperand, boolean secondOperand, int firstAddMethod, int secondAddMethod);
int AddMethodNumOfWords(int addMethod);
int handleDir(int type, char *line, extVars *vars);
int handleDataDir(char *line, int *error,int *dc, unsigned int data[]);
int handleStringDir(char *line, int *error,int *dc, unsigned int data[]);
int handleStructDir(char *line, int *error, int *dc, unsigned int data[]);
int handleEntryDir(char *line, int *error);
int handleExternDir(char *line, extVars *vars);
int handleCMD(int type, char *line, extVars *vars);
int methodDetect(char *operand, extVars *vars);
unsigned int encodeWord(int type, boolean firstOperand, boolean secondOperand, int firstAddMethod, int secondAddMethod);
boolean cmdOpernads(int type, boolean firstOp, boolean secondOp);
boolean cmdMethods(int type, int firstAddMethod, int secondAddMethod);

#endif
