#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "utils.h"

/* 80 + \n + \0 */
#define MAX_LINE 82

/* Sum of defined blacklist */
#define BLACKLIST 30

/* Linked-list that contains macro name and content in each node */
typedef struct MacroList
{
    char mname[MAX_LINE];
    char mcontent[MAX_LINE*MAX_LINE];
    struct MacroList *next;
} MacroNode;

/* enums to make returns more readable */
enum macrotypes
{
    NOT_MACRO,
    MACRO,
    END_MACRO
};

/* Prototypes */
boolean preAssembler(FILE *fpr, char *writefilename, MacroNode **head);
boolean firstMacroPass(FILE *fp, MacroNode **head);
void secondMacroPass(FILE *fpr, char *writefilename, MacroNode **head);
int macroOperation(char line[]);
boolean pushMacroName(MacroNode *temp, char line[], int lineCount, MacroNode **head);
void pushMacroContent(MacroNode *temp, FILE *fp);
boolean isMacroCall(char line[], FILE *fpw, MacroNode *head);
MacroNode *createNode();
void freeMacroNodes(MacroNode **head);
boolean isValidName(char *name, MacroNode **head);
boolean isComment(char *line);

void printMacroTable(MacroNode *head);
