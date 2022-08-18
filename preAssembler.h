/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
**************************************************/

#include "utils.h"
#include "main.h"


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
void secondMacroPass(FILE *fpr, char *writefilename, MacroNode **head);
void freeMacroNodes(MacroNode **head);
void pushMacroContent(MacroNode *temp, FILE *fp, int *lineCounter);
int macroOperation(char line[]);
boolean isMacroCall(char line[], FILE *fpw, MacroNode *head);
boolean isValidName(char *name, MacroNode **head);
boolean preAssembler(FILE *fpr, char *writefilename, MacroNode **head);
boolean firstMacroPass(FILE *fp, MacroNode **head);
boolean pushMacroName(MacroNode *temp, char line[], int lineCount, MacroNode **head);
MacroNode *createNode();

