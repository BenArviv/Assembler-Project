#ifndef STRUCTURES_H

#define STRUCTURES_H

#include "main.h"
#define MAX_LINE 82 /* max line length */
#define MAX_SYMBOL 31 /* max symbol length */


/* boolean type FALSE = 0 | TRUE = 1 */
typedef enum {FALSE, TRUE} boolean; 

/* linked-list of labels */
typedef struct structLabels * labelPtr;
typedef struct structLabels {
	char name[LABEL_LENGTH]; /* the name of the label */
	unsigned int address; /* the address of the label */
	boolean IsExternal; /* a boolean type variable to store if the label is extern or not */
	boolean inActionStatement; /* a boolean type varialbe to store if the label is in an action statement or not */
	boolean IsEntry; /* a boolean type varialbe to store if the label is entry or not */
	labelPtr next; /* a pointer to the next label in the list */
} labelList;

/* cmd struct */
typedef struct 
{
    char *name;
}stringStruct;




#endif
