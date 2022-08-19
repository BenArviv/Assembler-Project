/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
**************************************************/

#ifndef STRUCTURES_H

#define STRUCTURES_H

#include "main.h"



/* boolean type FALSE = 0 | TRUE = 1 */
typedef enum {FALSE, TRUE} boolean; 

/* linked-list of labels */
typedef struct structLabels *labelPtr;
typedef struct structLabels {
	char name[LABEL_LENGTH]; /* the name of the label */
	unsigned int address; /* the address of the label */
	boolean IsExternal; /* a boolean type variable to store if the label is extern or not */
	boolean inActionStatement; /* a boolean type varialbe to store if the label is in an action statement or not */
	boolean IsEntry; /* a boolean type variable to store if the label is entry or not */
	labelPtr next; /* a pointer to the next label in the list */
} labelList;

/* cmd/dir struct */
typedef struct {
    char *name; 
} stringStruct;

typedef struct ext *extPtr;
typedef struct ext {
	char name[LABEL_LENGTH];
	unsigned int address;
	extPtr next;
	extPtr prev;
} ext;

/* a structure that contains important variables that passes to many functions */
typedef struct vars_s {
	int dc; /* data counter */
	int ic; /* instruction counter */
	int error; /* error indicator */
	unsigned int data[MACHINE_RAM];
	unsigned int instructions[MACHINE_RAM];
	boolean entryFlag; /* a flag if encountered an .entry label */
	boolean externFlag; /* a flag if encountered an .extern label */
	boolean recordedError; /* a flag if encountered an error */
	labelPtr symbolsTable;
	stringStruct *cmd;
	stringStruct *dir;
	extPtr externList;
} extVars;




#endif
