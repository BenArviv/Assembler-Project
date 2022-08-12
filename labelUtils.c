#include "labelUtils.h"

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
            vars->error = LABEL_TOO_LONG;
        return FALSE;
    }

    /* first character must be a letter */
    if (!isalpha(*word))
    {
        if (colon)
            vars->error = LABEL_INVALID_FIRST_CHAR;
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
                vars->error = LABEL_ONLY_ALPHANUMERIC;
            return FALSE;
        }
    }

    if (!digits)
    {
        if (findCMD(word, vars->cmd) != NOT_FOUND)
        {
            if (colon)
                vars->error = LABEL_CANT_BE_COMMAND;
            return FALSE;
        }
    }

    if (isRegister(word))
    {
        if (colon)
            vars->error = LABEL_CANT_BE_REGISTER;
        return FALSE;
    }

    return TRUE;
}

labelPtr addLabel(char *name, unsigned int address, extVars *vars, boolean external)
{
    labelPtr ptr = vars->symbolsTable;
    labelPtr temp;

    if (isExistingLabel(vars->symbolsTable, name))
    {
        vars->error = LABEL_ALREADY_EXISTS;
        return NULL;
    }

    temp = (labelPtr)malloc(sizeof(labelList));
    if (!temp)
    {
        puts("\nError: cannot allocate memory\n");
        exit(ERROR);
    }

    /* storing label info in temp node */
    strcpy(temp->name, name);
    temp->IsEntry = FALSE;
    temp->address = address;
    temp->IsEntry = external;

    if (!external)
        temp->inActionStatement = external;
    else
        vars->externFlag = TRUE;

    if ((vars->symbolsTable) == NULL)
    {
        vars->symbolsTable = temp;
        temp->next = NULL;
        return temp;
    }

    while (ptr->next != NULL)
        ptr = ptr->next;

    temp->next = NULL;
    ptr->next = temp;

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
        if (!strcmp(head->name, name)) /* we found a label with the given name */
            return head;
        head = head->next;
    }

    return NULL;
}
/* returns address of a given label */
unsigned int getLabelAddress(labelPtr head, char *name)
{
    labelPtr label = getLabel(head, name);
    if (label != NULL)
        return label -> address;
    return FALSE;
}

/* return true/false if label is external */
boolean isLabelExternal(labelPtr head, char *name)
{
    labelPtr label = getLabel(head, name);
    if (label != NULL)
        return label -> IsExternal;
    return FALSE;
}

/* 
 * searching for the given label name and changing it's entry properties to TRUE
 * if failed to find label return false;
 */
boolean entryLabel(labelPtr head, char *name, extVars *vars)
{
    labelPtr temp = getLabel(head, name);
    if (temp != NULL)
    {
        if (temp ->IsExternal)
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

void freeLabels(labelPtr *head)
{
    labelPtr temp;
    while (*head)
    {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}

boolean deleteLabel(labelPtr *head, char *name)
{
    labelPtr temp = *head;
    labelPtr prev;
    while (temp)
    {
        if (!strcmp(temp->name, name))
        {
            if (!strcmp(temp->name, (*head)->name))
            {
                *head = (*head)->next;
                free(temp);
            }
            else
            {
                prev->next = temp->next;
                free(temp);
            }

            return TRUE;
        }

        prev = temp;
        temp = temp->next;
    }

    return FALSE;
}

void offsetAdd(labelPtr label, int num, boolean isData)
{
    while (label)
    {
        if (!(label->IsExternal) && (isData ^ (label->inActionStatement)))
            label->address += num;
        label = label->next;
    }
}

void print_labels(labelPtr h)
{
    while (h)
    {
        printf("\nname: %s, address: %d, external: %d", h->name, h->address, h->IsExternal);
        if (h->IsExternal == 0)
            printf(", is in action statement: %d -> ", h->inActionStatement);
        else
            printf(" -> ");
        h = h->next;
    }

    printf("*");
}
