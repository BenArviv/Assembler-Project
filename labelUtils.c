/*********************** AUTHORS **************************
                GAL ISRAEL, BEN ARVIV
 *********************************************************/

#include "labelUtils.h"

/* isLabel: checks if a word from line is a label */
boolean isLabel(char *word, int colon, extVars *vars)
{
    boolean digits = FALSE; /* if there are digits we can be sure it's not a command name */

    int wordLength = strlen(word);
    int i;

    /* if word is too short to be a label - return false */
    if (((colon) && (wordLength < MINIMUM_LABEL_LENGTH_WITH_COLON)) || ((!colon) && wordLength < MINIMUM_LABEL_LENGTH_WITHOUT_COLON) || (word == NULL))
        return FALSE;
    /* if we need a colon but it doesn't exists or not at the end - return false */
    if (colon && word[wordLength - 1] != ':')
        return FALSE;
    /* if word is longer than maximum label length (and we are looking for a label definition) - return false */
    if (wordLength > LABEL_LENGTH)
    {
        if (colon)
            vars -> error = LABEL_TOO_LONG;
        return FALSE;
    }

    /* first character must be a letter */
    if (!isalpha(*word))
    {
        if (colon)
            vars -> error = LABEL_INVALID_FIRST_CHAR;
        return FALSE;
    }

    /* removing the colon if needed so it will be easier to process */
    if (colon)
    {
        word[wordLength - 1] = '\0';
        wordLength--;
    }

    /* after ruling out the previous errors, checking if all of the chars are alphanumeric as defined in page 26 */
    for (i = 1; i < wordLength; i++)
    { /* starting from 1 since we have checked the first char already */
        if (isdigit(word[i]))
            digits = TRUE;
        else if (!isalpha(word[i]))
        {
            if (colon)
                vars -> error = LABEL_ONLY_ALPHANUMERIC;
            return FALSE;
        }
    }

    if (!digits)
    {
        if (findCMD(word, vars -> cmd) != NOT_FOUND)
        {
            if (colon)
                vars -> error = LABEL_CANT_BE_COMMAND;
            return FALSE;
        }
    }

    if (isRegister(word))
    {
        if (colon)
            vars -> error = LABEL_CANT_BE_REGISTER;
        return FALSE;
    }

    return TRUE;
}

/* addLabel: add a new label to the linked list */
labelPtr addLabel(char *name, unsigned int address, extVars *vars, boolean external)
{
    labelPtr ptr = vars -> symbolsTable;
    labelPtr temp;

    if (isExistingLabel(vars -> symbolsTable, name))
    {
        vars -> error = LABEL_ALREADY_EXISTS;
        return NULL;
    }

    temp = (labelPtr)malloc(sizeof(labelList));
    if (!temp)
    {
        puts("\nError: cannot allocate memory\n");
        exit(ERROR);
    }

    /* storing label info in temp node */
    strcpy(temp -> name, name);
    temp -> IsEntry = FALSE;
    temp -> address = address;
    temp -> IsExternal = external;

    if (!external)
        temp -> inActionStatement = external;
    else
        {
            vars -> externFlag = TRUE;
            temp -> IsExternal = TRUE; 
        }
    /* if the list is empty we set temp to be the head of the list */
    if ((vars -> symbolsTable) == NULL)
    {
        vars -> symbolsTable = temp;
        temp -> next = NULL;
        return temp;
    }

    /* setting temp to be the last label in list */
    while (ptr -> next != NULL)
        ptr = ptr -> next;

    temp -> next = NULL;
    ptr -> next = temp;

    return temp;
}

/* isExistingLabel: checks if a given name is a name of a label in the list */
boolean isExistingLabel(labelPtr head, char *name)
{
    return getLabel(head, name) != NULL;
}

/* getLabel: checks if a given label name is in the list.
            returns the label if true, otherwise returns NULL */
labelPtr getLabel(labelPtr head, char *name)
{
    while (head)
    {
        if (!strcmp(head -> name, name)) /* we found a label with the given name */
            return head;
        head = head -> next;
    }

    return NULL;
}
/* getLabelAddress: returns the address of a given label */
unsigned int getLabelAddress(labelPtr head, char *name)
{
    labelPtr label = getLabel(head, name);
    if (label != NULL)
        return label -> address;
    return FALSE;
}

/* isLabelExternal: return boolean value whether a label is external */
boolean isLabelExternal(labelPtr head, char *name)
{
    labelPtr label = getLabel(head, name);
    if (label != NULL)
        return label -> IsExternal;
    return FALSE;
}

/* 
 * entryLabel: finds a label node according to received name, and returns its entry property
 */
boolean entryLabel(labelPtr head, char *name, extVars *vars)
{
    labelPtr temp = getLabel(head, name);
    if (temp != NULL)
    {
        if (temp -> IsExternal)
        {
            vars -> error = ENTRY_CANT_BE_EXTERN;
            return FALSE;
        }
        temp -> IsEntry = TRUE;
        vars -> entryFlag = TRUE;
        return TRUE;
    }
    else
        vars -> error = ENTRY_LABEL_DOES_NOT_EXIST;
    return FALSE;
}

/* freeLabels: frees the labels */ 
void freeLabels(labelPtr *head)
{
    labelPtr temp;
    while (*head != NULL)
    {
        temp = *head;
        *head = (*head) -> next;
        free(temp);
    }
}
/* deleteLabel: deletes the label */
boolean deleteLabel(labelPtr *head, char *name)
{
    labelPtr temp = *head;
    labelPtr prev;
    while (temp)
    {
        if (!strcmp(temp -> name, name))
        {
            if (!strcmp(temp -> name, (*head) -> name))
            {
                *head = (*head) -> next;
                free(temp);
            }
            else
            {
                prev -> next = temp -> next;
                free(temp);
            }

            return TRUE;
        }

        prev = temp;
        temp = temp -> next;
    }

    return FALSE;
}
/* offsetAdd: offsets the address of a group of labels by a given num */
void offsetAdd(labelPtr label, int num, boolean isData)
{
    while (label)
    {
        if (!(label -> IsExternal) && (isData ^ (label -> inActionStatement)))
            label -> address += num;
        label = label -> next;
    }
}
