#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX 100

/* Linked-list that contains macro name and content in each node */
struct Macro {
    char mname[MAX];
    char mcontent[MAX];
    struct Macro* next;
};

enum macrotypes{NOT_MACRO, MACRO, END_MACRO};


void printMacroTable(struct Macro *tail);
int macroOperation(char line[]);
void pushMacroName(struct Macro *temp, char line[]);
void pushMacroContent(struct Macro *temp, FILE *fp);
void firstMacroPass(FILE *fp,struct Macro *head); 
int isMacroCall(char line[], FILE *fpw,struct Macro *tail);
void secondMacroPass(FILE *fpr, char *writefilename, struct Macro *tail);
