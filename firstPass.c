#include "firstPass.h"
/*
*                         ----- MESSAGE TO BEN -----
* I didn't had time working on it today(August 1st) so i just committed for you to browse.
*
* Fully based (not copied) on the other project i've sent you with some major & minor changes.
* The code is working as intended, although it needs a little bit more upgrades (eg. use a single struct instead of transferring 10 params to firstPass())
* print_labels and write_error are 100% copy-pasted just for testing, Will be changed.
* Any ideas are definitely welcome.
*/
void firstPass(FILE *fp, stringStruct cmd[], stringStruct dir[], boolean *externFlag, labelPtr *symbolsTable, int *dc, int *ic, unsigned int data[], unsigned int instructions[], boolean *recordedError)
{
    char line[MAX_LINE];
    int lineCount = 1;
    int error = NO_ERROR;
    *ic = 0;
    *dc = 0;
    puts("--------IN FIRST PASS---------");
    while (fgets(line, MAX_LINE, fp))
    {
        error = NO_ERROR;
        if (!skipLine(line))
            readLine(line, &error, cmd, dir, externFlag, symbolsTable, dc, ic, data, instructions);
        if (isError(&error))
        {
            *recordedError = TRUE;
            write_error(lineCount, error); /* TODO: END THIS: */
        }

        lineCount++;
    }
    printf("IC: %d --- dc: %d \n", *ic, *dc);
    offsetAdd(*symbolsTable, MEMORY_START, FALSE); /* Instruction Symbols */ 
    offsetAdd(*symbolsTable, *ic+MEMORY_START, TRUE); /* Data Symbols */ 
}

/*Analyzing a given line from file */
void readLine(char *line, int *error, stringStruct cmd[], stringStruct dir[], boolean *externFlag, labelPtr *symbolsTable, int *dc, int *ic, unsigned int data[], unsigned int instructions[])
{
    int cmdType = UNKNOWN_COMMAND;
    int dirType = UNKNOWN_DIR;

    boolean label = FALSE; /*Flag for lables */
    labelPtr labelNode = NULL;
    char word[MAX_LINE];
    memset(word, '\0', MAX_LINE);
    line = skipWhiteChars(line);
    if (isLineEnd(line))
        return;
    if (!isalpha(*line) && *line != '.') /*If line doesn't start with . or alpha char it's a syntax error */
    {
        *error = SYNTAX_ERR;
        return;
    }

    copyWord(word, line);

    if (isLabel(word, COLON, error, cmd)) /*if it's label */
    {
        label = TRUE;
        labelNode = addLabel(symbolsTable, word, 0, error, externFlag, FALSE);
        if (labelNode == NULL)
            return;
        line = nextWord(line); /*going to the next word in line */
        if (line == NULL)
        {
            *error = LABEL_ONLY;
            return;
        }

        copyWord(word, line); /*Copy the first word in line into "word" */
    }

    if (isError(error))
        return;

    if ((dirType = findDirective(word, dir)) != NOT_FOUND) /*if it's a directive */
    {
        if (label) /*if we're inside a label creation */
        {
            if (dirType == EXTERN || dirType == ENTRY) /*ignore creation of label if so */
            {
                deleteLabel(symbolsTable, labelNode->name);
                label = FALSE;
            }
            else
                labelNode->address = *dc;
        }

        line = nextWord(line);
        handleDir(dirType, line, error, dc, data, cmd, symbolsTable, externFlag);
    }
    else if ((cmdType = findCMD(word, cmd)) != NOT_FOUND) /*if it's a command */
    {
        if (label)
        {
            labelNode->inActionStatement = TRUE;
            labelNode->address = *ic;
        }

        line = nextWord(line);
        handleCMD(cmdType, line, error, cmd, instructions, ic);
    }
    else /*Unknown command */
    {
        *error = COMMAND_NOT_FOUND;
    }

}

int handleCMD(int type, char *line, int *error, stringStruct cmd[], unsigned int instructions[], int *ic)
{
    boolean firstOperand = FALSE, secondOperand = FALSE;
    int firstAddMethod, secondAddMethod;
    char firstOp[MAX_LINE], secondOp[MAX_LINE];

    line = nextCommaWord(firstOp, line);
    if (!isLineEnd(firstOp))
    {
        firstOperand = TRUE; /* found 1st operand */
        line = nextCommaWord(secondOp, line);
        if (!isLineEnd(secondOp))
        {
            /* we need a comma delimeter */
            if (*secondOp != ',')
            {
                *error = COMMAND_UNEXPECTED_CHAR;
                return ERROR;
            }
            else
            {
                line = nextCommaWord(secondOp, line);
                if (isLineEnd(secondOp)) /* if we have only a comma without 2nd operand */
                {
                    *error = COMMAND_UNEXPECTED_CHAR;
                    return ERROR;
                }
                secondOperand = TRUE;
            }
        }
    }
    line = skipWhiteChars(line);
    if (!isLineEnd(line)) /* if we have extra chars after the operands */
    {
        *error = COMMAND_TOO_MANY_OPERANDS;
        return ERROR;
    }
    if (firstOperand)
        firstAddMethod = methodDetect(firstOp, error, cmd);
    if (secondOperand)
        secondAddMethod = methodDetect(secondOp, error, cmd);
     if (!isError(error))
        {
            if (cmdOpernads(type, firstOperand, secondOperand))
            {
                if (cmdMethods(type, firstAddMethod, secondAddMethod))
                {
                    encodeInsturction(encodeWord(type, firstOperand, secondOperand, firstAddMethod, secondAddMethod), instructions, ic);
                    *ic += numberOfWords(firstOperand, secondOperand, firstAddMethod, secondAddMethod);
                }
                else
                {
                    *error = COMMAND_INVALID_OPERANDS_METHODS;
                    return ERROR;
                }
            }
            else
            {
                *error = COMMAND_INVALID_NUMBER_OF_OPERANDS;
                return ERROR;
            }
        }
        return NO_ERROR; 
}

/*
    Addressing Methods - (0 - Immediate, 1 - Direct, 2 - Struct, 3  - Register)
*/
int methodDetect(char *operand, int *error, stringStruct cmd[])
{
    char *structPointer;
    puts("in method detect");
    puts(operand);
    if (isLineEnd(operand))
        return NOT_FOUND;

    /* --- Immediate --- */
    if (*operand == '#')
    {
        operand++;
        if (isValidNum(operand))
            return METHOD_IMMEDIATE;
    }

    /* --- Register --- */
    else if (isRegister(operand))
        return METHOD_REGISTER;

    /* --- Direct --- */
    else if (isLabel(operand, FALSE, error, cmd))
    {
        return METHOD_DIRECT;
    }

    /* --- Struct --- */
    else if (isLabel(strtok(operand, "."), FALSE, error, cmd))
    {
        structPointer = strtok(NULL, "");
        if (strlen(structPointer) == 1 && (*structPointer == '1' || *structPointer == '2'))
            return METHOD_STRUCT;
    }
    *error = COMMAND_INVALID_METHOD;
    return NOT_FOUND;
}

boolean cmdOpernads(int type, boolean firstOp, boolean secondOp)
{
    switch (type)
    {
    /* 2 operands CMDs */
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        return firstOp && secondOp;

    /* 1 operands CMDs */
    case NOT:
    case CLR:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case GET:
    case PRN:
    case JSR:
        return firstOp && !secondOp;

    /* 0 operands CMDs */
    case RTS:
    case HLT:
        return !firstOp && !secondOp;
    }
    return FALSE;
}

boolean cmdMethods(int type, int firstAddMethod, int secondAddMethod)
{
    switch (type)
    {
    /*
     * Source: ALL
     * Dest: Direct, Struct, Register
     */
    case MOV:
    case ADD:
    case SUB:
        return (firstAddMethod == METHOD_IMMEDIATE || firstAddMethod == METHOD_DIRECT || firstAddMethod == METHOD_STRUCT || firstAddMethod == METHOD_REGISTER) &&
               (secondAddMethod == METHOD_DIRECT || secondAddMethod == METHOD_STRUCT || secondAddMethod == METHOD_REGISTER);

    /*
     * Source: Direct, Struct
     * Dest: Direct, Struct, Register
     */
    case LEA:
        return (firstAddMethod == METHOD_DIRECT || firstAddMethod == METHOD_STRUCT) &&
               (secondAddMethod == METHOD_DIRECT || secondAddMethod == METHOD_STRUCT || secondAddMethod == METHOD_REGISTER);

    /*
     * Source: NONE
     * Dest: Direct, Struct, Register
     */
    case NOT:
    case CLR:
    case INC:
    case DEC:
    case JMP:
    case BNE:
    case GET:
    case JSR:
        return firstAddMethod == METHOD_DIRECT || firstAddMethod == METHOD_STRUCT || firstAddMethod == METHOD_REGISTER;

    /*
     * others
     */
    case PRN:
    case CMP:
    case RTS:
    case HLT:
        return TRUE;
    }
    return FALSE;
}

unsigned int encodeWord(int type, boolean firstOperand, boolean secondOperand, int firstAddMethod, int secondAddMethod)
{
    unsigned int word = 0;
    word = type;
    word <<= BITS_IN_METHOD;

    if(firstOperand && secondOperand)
        word |= firstAddMethod;
    word <<= BITS_IN_METHOD;

    if(firstOperand && secondOperand)
        word |= secondAddMethod;

    else if(firstOperand)
        word |= firstAddMethod;
    word = encodeARE(word, ABSOLUTE);

    return word;
}

int numberOfWords(boolean firstOperand, boolean secondOperand, int firstAddMethod, int secondAddMethod)
{
    int cnt = 0;
    if(firstOperand)
        cnt += AddMethodNumOfWords(firstAddMethod);
    if (secondOperand)
        cnt += AddMethodNumOfWords(secondAddMethod);
    
    /* both registers */
    if(firstOperand & secondOperand && firstAddMethod == METHOD_REGISTER && secondAddMethod == METHOD_REGISTER)
        cnt--;
    
    return cnt;
    
}

int AddMethodNumOfWords(int addMethod)
{
    if(addMethod == METHOD_STRUCT)
        return 2;
    return 1;
}


/*
    ########################################################################

        Directives  - (.data, .string, .sturct, .entry, .extern)

    ########################################################################
*/

int handleDir(int type, char *line, int *error, int *dc, unsigned int data[], stringStruct cmd[], labelPtr *symbolsTable, boolean *externFlag)
{
    if (line == NULL || isLineEnd(line))
    {
        *error = DIRECTIVE_NO_PARAMS;
        return ERROR;
    }

    switch (type)
    {
    case DATA:
        return handleDataDir(line, error, dc, data);

    case STRING:
        return handleStringDir(line, error, dc, data);

    case STRUCT:
        return handleStructDir(line, error, dc, data);

    case ENTRY:
        return handleEntryDir(line, error); /* actually only checks syntax */

    case EXTERN:
        return handleExternDir(line, error, dc, data, cmd, symbolsTable, externFlag);
    }

    return NO_ERROR;
}

int handleDataDir(char *line, int *error, int *dc, unsigned int data[])
{
    char word[MAX_LINE];
    boolean number = FALSE, comma = FALSE;
    memset(word, '\0', MAX_LINE);

    while (!isLineEnd(line))
    {
        puts("in handleDataDir");
        line = nextCommaWord(word, line);
        if (strlen(word))
        {
            if (!number)
            {
                if (comma && *word == ',')
                {
                    *error = DATA_COMMAS_IN_A_ROW;
                    return ERROR;
                }

                if (!isValidNum(word))
                {
                    *error = DATA_EXPECTED_NUM;
                    return ERROR;
                }
                else
                {
                    number = TRUE;
                    comma = FALSE;
                    writeNumberToData(atoi(word), dc, data);
                }
            }
            else if (*word != ',') /*if there was a number we need a comma */
            {
                *error = DATA_EXPECTED_COMMA_AFTER_NUM;
                return ERROR;
            }
            else
            {
                if (comma)
                {
                    *error = DATA_COMMAS_IN_A_ROW;
                    return ERROR;
                }
                else
                {
                    comma = TRUE;
                    number = FALSE;
                }
            }
        }
    }

    if (comma)
    {
        *error = DATA_UNEXPECTED_COMMA;
        return ERROR;
    }

    return NO_ERROR;
}

int handleStringDir(char *line, int *error, int *dc, unsigned int data[])
{
    char word[MAX_LINE];
    line = nextString(word, line);
    if (!isLineEnd(word) && isValidString(word))
    {
        line = skipWhiteChars(line);
        if (isLineEnd(line))
        {
            word[strlen(word) - 1] = '\0';
            writeStringToData(word + 1, dc, data);
        }
        else
        {
            *error = STRING_TOO_MANY_OPERANDS;
            return ERROR;
        }
    }
    else
    {
        *error = STRING_OPERAND_NOT_VALID;
        return ERROR;
    }

    return NO_ERROR;
}

int handleStructDir(char *line, int *error, int *dc, unsigned int data[])
{
    char word[MAX_LINE];
    line = nextCommaWord(word, line);
    if (!isLineEnd(word))
    {
        if (isValidNum(word))
        {
            writeNumberToData(atoi(word), dc, data);
            line = nextCommaWord(word, line);

            if (!isLineEnd(word))
            {
                if (*word == ',')
                {
                    line = nextCommaWord(word, line);
                    if (!isLineEnd(word))
                    {
                        if (isValidString(word))
                        {
                            word[strlen(word) - 1] = '\0';
                            writeStringToData(word + 1, dc, data);
                        }
                        else
                        {
                            *error = STRUCT_INVALID_STRING;
                            return ERROR;
                        }
                    }
                    else
                    {
                        *error = STRUCT_EXPECTED_STRING;
                        return ERROR;
                    }
                }
                else
                {
                    *error = EXPECTED_COMMA_BETWEEN_OPERANDS;
                    return ERROR;
                }
            }
            else
            {
                *error = STRUCT_MISSING_STR;
                return ERROR;
            }
        }
        else
        {
            *error = STRUCT_INVALID_NUM;
            return ERROR;
        }
    }
    /* checked before got into the directive if it got operands, but just in case, so error #9 might be produced instead of #21*/
    else
    {
        *error = STRUCT_NO_OPERANDS;
        return ERROR;
    }

    if (!isLineEnd(nextCommaWord(word, line)))
    {
        *error = STRUCT_TOO_MANY_OPERANDS;
        return ERROR;
    }

    return NO_ERROR;
}

int handleEntryDir(char *line, int *error)
{
    if (!isLineEnd(nextWord(line)))
    {
        *error = DIRECTIVE_INVALID_NUM_PARAMS;
        return ERROR;
    }
    return NO_ERROR;
}

int handleExternDir(char *line, int *error, int *dc, unsigned int data[], stringStruct cmd[], labelPtr *symbolsTable, boolean *externFlag)
{
    char word[MAX_LINE];
    copyWord(word, line);
    if (isLineEnd(word))
    {
        *error = EXTERN_NO_LABEL;
        return ERROR;
    }
    if (!isLabel(word, FALSE, error, cmd))
    {
        *error = EXTERN_INVALID_LABEL;
        return ERROR;
    }
    line = nextWord(line);
    if (!isLineEnd(line))
    {
        *error = EXTERN_TOO_MANY_OPERANDS;
        return ERROR;
    }
    if (addLabel(symbolsTable, word, EXTERNAL_DEFAULT_ADDRESS, error, externFlag, TRUE) == NULL)
        return ERROR;
    return isError(error);
}

void writeNumberToData(int num, int *dc, unsigned int data[])
{
    puts("in writeNumberToData");
    data[(*dc)++] = (unsigned int)num;
}

void writeStringToData(char *str, int *dc, unsigned int data[])
{
    while (!isLineEnd(str))
    {
        data[(*dc)++] = (unsigned int)*str;
        str++;
    }

    data[(*dc)++] = '\0';
}

/*##################
       LABELS
   ################## */

boolean isLabel(char *word, int colon, int *error, stringStruct cmd[])
{
    boolean digits = FALSE; /*If there are digits we can be sure it's not a command name */

    int wordLength = strlen(word);
    int i;

    /*if word is to short to be a label return false */
    if (((colon) && (wordLength < MINIMUM_LABEL_LENGTH_WITH_COLON)) || ((!colon) && (wordLength < MINIMUM_LABEL_LENGTH_WITHOUT_COLON)) || (word == NULL))
        return FALSE;
    /*if we need a colon but it doesn't exist or not at the end return false */
    if (colon && word[wordLength - 1] != ':')
        return FALSE;
    /*if word is longer than maximum label length (and we are looking for a label definition) return false */
    if (wordLength > LABEL_LENGTH)
    {
        if (colon)
            *error = LABEL_TOO_LONG;
        return FALSE;
    }

    /*first character must be a letter */
    if (!isalpha(*word))
    {
        if (colon)
            *error = LABEL_INVALID_FIRST_CHAR;
        return FALSE;
    }

    /*removing the colon if needed so it will be easier to proccess */
    if (colon)
    {
        word[wordLength - 1] = '\0';
        wordLength--;
    }

    /*After ruling out the previous errors, checking if the all chars are alphanumeric as defined in page 26*/
    for (i = 1; i < wordLength; i++) /*starting from 1 since we have checked the 1st char already */
    {
        if (isdigit(word[i]))
            digits = TRUE;
        else if (!isalpha(word[i]))
        {
            if (colon)
                *error = LABEL_ONLY_ALPHANUMERIC;
            return FALSE;
        }
    }

    if (!digits)
    {

        if (findCMD(word, cmd) != NOT_FOUND)
        {
            if (colon)
                *error = LABEL_CANT_BE_COMMAND;
            return FALSE;
        }
    }

    if (isRegister(word))
    {
        if (colon)
            *error = LABEL_CANT_BE_REGISTER;
        return FALSE;
    }

    return TRUE;
}

labelPtr addLabel(labelPtr *head, char *name, unsigned int address, int *error, boolean *externFlag, boolean external)
{
    labelPtr ptr = *head;
    labelPtr temp;

    if (isExistingLabel(*head, name))
    {
        *error = LABEL_ALREADY_EXISTS;
        return NULL;
    }

    temp = (labelPtr)malloc(sizeof(labelList));
    if (!temp)
    {
        puts("\nError, Cannot allocate memory\n");
        exit(ERROR);
    }

    /*Storing label info in temp node */
    strcpy(temp->name, name);
    temp->IsEntry = FALSE;
    temp->address = address;
    temp->IsExternal = external;

    if (!external)
        temp->inActionStatement = external;
    else
        *externFlag = TRUE;

    if (!(*head))
    {
        *head = temp;
        temp->next = NULL;
        return temp;
    }

    while (ptr->next != NULL)
        ptr = ptr->next;
    temp->next = NULL;
    ptr->next = temp;

    return temp;
}

/*checks if a given name is a name of a label in the list */
boolean isExistingLabel(labelPtr head, char *name)
{
    return getLabel(head, name) != NULL;
}

/*checks if a given label name is in the list if so return the lable else returns NULL. */
labelPtr getLabel(labelPtr head, char *name)
{
    while (head)
    {
        if (!strcmp(head->name, name)) /*we found a label with the name given */
            return head;
        head = head->next;
    }

    return NULL;
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
