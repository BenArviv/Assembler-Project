/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
**************************************************/

#ifndef LABELS_H
#define LABELS_H

#include "structures.h"
#include "firstPass.h"

/* PROTOTYPES */
void freeLabels(labelPtr *head);
void offsetAdd(labelPtr label, int num, boolean isData);
unsigned int getLabelAddress(labelPtr head, char *name);
boolean isExistingLabel(labelPtr head, char *name);
boolean isLabelExternal(labelPtr head, char *name);
boolean entryLabel(labelPtr head, char *name, extVars *vars);
boolean isLabel(char *word, int colon, extVars *vars);
boolean deleteLabel(labelPtr *head, char *name);
labelPtr getLabel(labelPtr head, char *name);
labelPtr addLabel(char *name, unsigned int address, extVars *vars, boolean external);


#endif
