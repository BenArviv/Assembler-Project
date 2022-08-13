#ifndef SECONDPASS_H
#define SECONDPASS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "firstPass2.h"
#include "labelUtils.h"
#include "utils.h"

void secondPass(FILE *fp, char *filename, extVars *vars);
void readLineSecond(char *line, extVars *vars);
int handleCMDSecond(int cmdType, char *line, extVars *vars);
int encodeAddtional(char *src, char *dest, boolean isSrc, boolean isDest, int srcMethod, int destMethod, extVars *vars);
unsigned int registerOperand(boolean isDest, char *reg);
void additionalWords(boolean isDest, int method, char *op, extVars *vars);
void encodeLabel(char *name, extVars *vars);
void cmdOpernadsSecond(int type, boolean *isSrc, boolean *isDest);
extPtr addExtern(extPtr *head, char *name, unsigned int ref);
void freeExterns(extPtr *head);

boolean writeOutput(char *filename, extVars *vars);
FILE *openFile(char *filename, int type, extVars *vars);
void writeOB(FILE *fp, extVars *vars);
void writeExtern(FILE *fp, extVars *vars);
void writeEntry(FILE *fp, extVars *vars);



#endif