#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "utils.h"


/* Prototypes */ 
void firstPass(FILE *fp, stringStruct cmd[], stringStruct dir[], boolean *externFlag, labelPtr *symbolsTable, int *dc, int *ic, unsigned int data[], unsigned int instructions[], boolean *recordedError);
void readLine(char *line, int *error, stringStruct cmd[], stringStruct dir[], boolean *externFlag, labelPtr *symbolsTable, int *dc, int *ic, unsigned int data[], unsigned int instructions[]);
boolean isLabel(char *word, int colon, int *error, stringStruct cmd[]);
labelPtr addLabel(labelPtr *head, char *name, unsigned int address, int *error, boolean *externFlag, boolean external);
boolean isExistingLabel(labelPtr head, char *name);
labelPtr getLabel(labelPtr head, char *name);
void freeLabels(labelPtr *head);
boolean deleteLabel(labelPtr *head, char *name);
int handleCMD(int type, char *line, int *error, stringStruct cmd[], unsigned int instructions[], int *ic);
int methodDetect(char *operand, int *error, stringStruct cmd[]);
boolean cmdOpernads(int type, boolean firstOp, boolean secondOp);
boolean cmdMethods(int type, int firstAddMethod, int secondAddMethod);
unsigned int encodeWord(int type, boolean firstOperand, boolean secondOperand, int firstAddMethod, int secondAddMethod);
int numberOfWords(boolean firstOperand, boolean secondOperand, int firstAddMethod, int secondAddMethod);
int AddMethodNumOfWords(int addMethod);
int handleDir(int type, char *line, int *error, int *dc, unsigned int data[], stringStruct cmd[], labelPtr *symbolsTable, boolean *externFlag);
int handleDataDir(char *line, int *error,int *dc, unsigned int data[]);
int handleStringDir(char *line, int *error,int *dc, unsigned int data[]);
int handleStructDir(char *line, int *error, int *dc, unsigned int data[]);
int handleEntryDir(char *line, int *error);
int handleExternDir(char *line, int *error, int *dc, unsigned int data[], stringStruct cmd[], labelPtr *symbolsTable, boolean *externFlag);
void writeNumberToData(int num, int *dc, unsigned int data[]);
void writeStringToData(char *str, int *dc, unsigned int data[]);
void offsetAdd(labelPtr label, int num, boolean isData);


void print_labels(labelPtr h);