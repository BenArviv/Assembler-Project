/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
**************************************************/

#ifndef SECONDPASS_H
#define SECONDPASS_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include "firstPass.h"
#include "labelUtils.h"
#include "utils.h"

void secondPass(FILE *fp, char *filename, extVars *vars);
void readLineSecond(char *line, extVars *vars);
void printCounters(FILE *fp, char *firstCol, char *secondCol);
void additionalWords(boolean isDest, int method, char *op, extVars *vars);
void encodeLabel(char *name, extVars *vars);
void cmdOpernadsSecond(int type, boolean *isSrc, boolean *isDest);
void freeExterns(extPtr *head);
void writeOB(FILE *fp, extVars *vars);
void writeExtern(FILE *fp, extVars *vars);
void writeEntry(FILE *fp, extVars *vars);
int handleCMDSecond(int cmdType, char *line, extVars *vars);
int encodeAddtional(char *src, char *dest, boolean isSrc, boolean isDest, int srcMethod, int destMethod, extVars *vars);
unsigned int registerOperand(boolean isDest, char *reg);
extPtr addExtern(extPtr *head, char *name, unsigned int ref);
boolean writeOutput(char *filename, extVars *vars);
FILE *openFile(char *filename, int type, extVars *vars);



#endif
