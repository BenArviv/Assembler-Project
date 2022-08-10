/* PROTOTYPES */
boolean isLabel(char *word, int colon, extVars *vars);
labelPtr addLabel(char *name, unsigned int address, extVars *vars, boolean external);
boolean isExistingLabel(labelPtr head, char *name);
labelPtr getLabel(labelPtr head, char *name);
void freeLabels(labelPtr *head);
boolean deleteLabel(labelPtr *head, char *name);
void offsetAdd(labelPtr label, int num, boolean isData);


void print_labels(labelPtr h);
