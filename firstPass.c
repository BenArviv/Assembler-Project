/*********************** AUTHORS **************************
                GAL ISRAEL, BEN ARVIV
 *********************************************************/

#include "firstPass.h"
#include "labelUtils.h"

/* firstPass: main first pass routine for file.am */
void firstPass(FILE *fp, extVars *vars)
{
    char line[MAX_LINE];
    int lineCount = 1;
    vars->error = OK;
    vars->ic = 0;
    vars->dc = 0;
    while (fgets(line, MAX_LINE, fp))
    {
        vars->error = OK;
        if (!skipLine(line))
        {
            if (strlen(line) > CHAR_LIMIT) /* counting whitechars as a legal chars that included in the 80 chars limit, if line exceeds the limit, we skip it */
                {
                    vars->error = LINE_TOO_LONG;
                    fp = nextLine(fp);
                }
            else
                readLine(line, vars);
        }
        if (isError(&(vars->error)))
        {
            vars->recordedError = TRUE;
            printError(lineCount, vars->error); 
        }

        lineCount++;
    }
    offsetAdd(vars->symbolsTable, MEMORY_START, FALSE);           /* Instruction symbols */
    offsetAdd(vars->symbolsTable, vars->ic + MEMORY_START, TRUE); /* Data symbols */
}

/* readLine: analyzing a given line from file */
void readLine(char *line, extVars *vars)
{
    int cmdType = UNKNOWN_COMMAND;
    int dirType = UNKNOWN_DIR;

    boolean label = FALSE; /* flag for labels */
    labelPtr labelNode = NULL;
    char word[MAX_LINE];
    memset(word, '\0', MAX_LINE);
    line = skipWhiteChars(line);
    if (isLineEnd(line))
        return;
    if (!isalpha(*line) && *line != '.')
    { /* if line doesn't start with '.' or alpha char it's a syntax error */
        vars->error = SYNTAX_ERR;
        return;
    }

    copyWord(word, line);

    if (isLabel(word, COLON, vars))
    { /* if it's a label */
        label = TRUE;
        labelNode = addLabel(word, 0, vars, FALSE);
        if (labelNode == NULL)
            return;
        line = nextWord(line); /* going to the next word in line */
        if (line == NULL)
        {
            vars->error = LABEL_ONLY;
            return;
        }
        copyWord(word, line); /* copy the first word in line into "word" */
    }

    if (isError(&(vars->error)))
        return;

    if ((dirType = findDirective(word, vars->dir)) != NOT_FOUND)
    { /* if it's a directive */
        if (label)
        { /* if we're inside a label creation */
            if (dirType == EXTERN || dirType == ENTRY)
            { /* ignore creation of label if so */
                deleteLabel(&(vars->symbolsTable), labelNode->name);
                label = FALSE;
            }
            else
                labelNode->address = vars->dc;
        }

        line = nextWord(line);
        handleDir(dirType, line, vars);
    }
    else if ((cmdType = findCMD(word, vars->cmd)) != NOT_FOUND)
    { /* if it's a command */
        if (label)
        {
            labelNode->inActionStatement = TRUE;
            labelNode->address = vars->ic;
        }

        line = nextWord(line);
        handleCMD(cmdType, line, vars);
    }
    else
    { /* unknown command */
        vars->error = COMMAND_NOT_FOUND;
    }
}

/* handleCMD: set of actions in case that a command has been read */
int handleCMD(int type, char *line, extVars *vars)
{
    boolean firstOperand = FALSE, secondOperand = FALSE;
    int firstAddMethod, secondAddMethod;
    char firstOp[MAX_LINE], secondOp[MAX_LINE];

    line = nextCommaWord(firstOp, line);
    if (!isLineEnd(firstOp))
    {
        firstOperand = TRUE; /* found first operand */
        line = nextCommaWord(secondOp, line);
        if (!isLineEnd(secondOp))
        {
            /* we need a comma delimeter */
            if (*secondOp != ',')
            {
                vars->error = COMMAND_UNEXPECTED_CHAR;
                return ERROR;
            }
            else
            {
                line = nextCommaWord(secondOp, line);
                if (isLineEnd(secondOp)) /* if we have only a comma without 2nd operand */
                {
                    vars->error = COMMAND_UNEXPECTED_CHAR;
                    return ERROR;
                }
                if (*secondOp == ',')
                {
                    vars->error = COMMAND_COMMA_IN_A_ROW;
                    return ERROR;
                }

                secondOperand = TRUE;
            }
        }
    }
    line = skipWhiteChars(line);
    if (!isLineEnd(line)) /* if we have extra chars after the operands */
    {
        vars->error = COMMAND_TOO_MANY_OPERANDS;
        return ERROR;
    }
    if (firstOperand)
        firstAddMethod = methodDetect(firstOp, vars);
    if (secondOperand)
        secondAddMethod = methodDetect(secondOp, vars);
    if (!isError(&(vars->error)))
    {
        if (cmdOpernads(type, firstOperand, secondOperand))
        {
            if (cmdMethods(type, firstAddMethod, secondAddMethod))
            {
                encodeInsturction(encodeWord(type, firstOperand, secondOperand, firstAddMethod, secondAddMethod), vars->instructions, &(vars->ic));
                vars->ic += numberOfWords(firstOperand, secondOperand, firstAddMethod, secondAddMethod);
            }
            else
            {
                vars->error = COMMAND_INVALID_OPERANDS_METHODS;
                return ERROR;
            }
        }
        else
        {
            vars->error = COMMAND_INVALID_NUMBER_OF_OPERANDS;
            return ERROR;
        }
    }
    return OK;
}

/*
    methodDetect: analyzes the addressing method of the current operand
    Addressing Methods - (0 - Immediate, 1 - Direct, 2 - Struct, 3  - Register)
*/
int methodDetect(char *operand, extVars *vars)
{
    char *structPointer;
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
    else if (isLabel(operand, FALSE, vars))
    {
        return METHOD_DIRECT;
    }

    /* --- Struct --- */
    else if (isLabel(strtok(operand, "."), FALSE, vars))
    {
        structPointer = strtok(NULL, "");
        if (strlen(structPointer) == 1 && (*structPointer == '1' || *structPointer == '2'))
            return METHOD_STRUCT;
    }
    vars->error = COMMAND_INVALID_METHOD;
    return NOT_FOUND;
}

/* cmdOperands: returns the amount of operands needed, for each CMD read */
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

/* cmdMethods: checks if given addressing method matches CMD's addressing methods available */
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

/* encodeWord: gets a word and encodes it to base32 number, based on CMD, operands etc */
unsigned int encodeWord(int type, boolean firstOperand, boolean secondOperand, int firstAddMethod, int secondAddMethod)
{
    unsigned int word = 0;
    word = type;
    word <<= BITS_IN_METHOD;

    if (firstOperand && secondOperand)
        word |= firstAddMethod;
    word <<= BITS_IN_METHOD;

    if (firstOperand && secondOperand)
        word |= secondAddMethod;

    else if (firstOperand)
        word |= firstAddMethod;
    word = encodeARE(word, ABSOLUTE);

    return word;
}

/* numberOfWords: finds how many words are in the current line */
int numberOfWords(boolean firstOperand, boolean secondOperand, int firstAddMethod, int secondAddMethod)
{
    int cnt = 0;
    if (firstOperand)
        cnt += AddMethodNumOfWords(firstAddMethod);
    if (secondOperand)
        cnt += AddMethodNumOfWords(secondAddMethod);

    /* both registers */
    if (firstOperand & secondOperand && firstAddMethod == METHOD_REGISTER && secondAddMethod == METHOD_REGISTER)
        cnt--;

    return cnt;
}

/* addMethodNumOfWords: returns the amount of word, depends on the addressing method */
int AddMethodNumOfWords(int addMethod)
{
    if (addMethod == METHOD_STRUCT)
        return 2;
    return 1;
}

/*
    ########################################################################

        Directives  - (.data, .string, .sturct, .entry, .extern)

    ########################################################################
*/

/* handleDir: set of actions in case that a directive has been read */
int handleDir(int type, char *line, extVars *vars)
{
    if (line == NULL || isLineEnd(line))
    {
        vars->error = DIRECTIVE_NO_PARAMS;
        return ERROR;
    }

    switch (type)
    {
    case DATA:
        return handleDataDir(line, &(vars->error), &(vars->dc), vars->data);

    case STRING:
        return handleStringDir(line, &(vars->error), &(vars->dc), vars->data);

    case STRUCT:
        return handleStructDir(line, &(vars->error), &(vars->dc), vars->data);

    case ENTRY:
        return handleEntryDir(line, &(vars->error)); /* actually only checks syntax */

    case EXTERN:
        return handleExternDir(line, vars);
    }

    return OK;
}

/* handleDataDir: actions for data directive */
int handleDataDir(char *line, int *error, int *dc, unsigned int data[])
{
    char word[MAX_LINE];
    boolean number = FALSE, comma = FALSE;
    memset(word, '\0', MAX_LINE);

    while (!isLineEnd(line))
    {
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
            else if (*word != ',') /* if there was a number we need a comma */
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

    return OK;
}

/* handleStringDir: actions for string directive */
int handleStringDir(char *line, int *error, int *dc, unsigned int data[])
{
    char word[MAX_LINE];
    line = nextString(word, line);
    if (!isLineEnd(word) && isValidString(word)) /* if first argument is a valid string */
    {
        line = skipWhiteChars(line);
        if (isLineEnd(line)) /* if we have only one argument */
        {
            word[strlen(word) - 1] = '\0'; /* removing "" and writing to data */
            writeStringToData(word + 1, dc, data);
        }
        else /* too many arguments */
        {
            *error = STRING_TOO_MANY_OPERANDS;
            return ERROR;
        }
    }
    else /* not a valid string */
    {
        *error = STRING_OPERAND_NOT_VALID;
        return ERROR;
    }

    return OK;
}

/* handleStructDir: actions for struct directive */
int handleStructDir(char *line, int *error, int *dc, unsigned int data[])
{
    char word[MAX_LINE];
    line = nextCommaWord(word, line);
    if (!isLineEnd(word))
    {
        if (isValidNum(word)) /* must be a valid num */
        {
            writeNumberToData(atoi(word), dc, data); /* encoding...*/
            line = nextCommaWord(word, line); /* next argument */

            if (!isLineEnd(word))
            {
                if (*word == ',') /* looking for a comma between arguments */
                {
                    line = nextCommaWord(word, line); /* next word */
                    if (!isLineEnd(word)) 
                    {
                        if (*word == ',') /* if we have another comma it's a syntax error */
                        {
                            *error = STRUCT_COMMA_IN_A_ROW;
                            return ERROR;
                        }
                        if (isValidString(word)) /* second argument must be a valid string */
                        {
                            line = skipWhiteChars(line);
                            if (!isLineEnd(line)) /* if we have more chars at the end it's an error */
                            {
                                *error = STRUCT_EXTRA_CHARS;
                                return ERROR;
                            }

                            word[strlen(word) - 1] = '\0'; /* removing apostrophes ("") and writing to data */
                            writeStringToData(word + 1, dc, data);
                        }
                        else /* we don't have second argument */
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

    return OK;
}

/* handleEntryDir: actions for entry directive (syntax check) */
int handleEntryDir(char *line, int *error)
{
    if (!isLineEnd(nextWord(line)))
    {
        *error = DIRECTIVE_INVALID_NUM_PARAMS;
        return ERROR;
    }
    return OK;
}

/* handleExternDir: actions for extern directive */
int handleExternDir(char *line, extVars *vars)
{
    char word[MAX_LINE];
    copyWord(word, line);
    if (isLineEnd(word)) /* if word is empty, we have no label followed by extern */
    {
        vars->error = EXTERN_NO_LABEL;
        return ERROR;
    }
    if (!isLabel(word, FALSE, vars)) /* word should be a label without a colon */
    {
        vars->error = EXTERN_INVALID_LABEL;
        return ERROR;
    }
    line = nextWord(line);
    if (!isLineEnd(line))
    {
        vars->error = EXTERN_TOO_MANY_OPERANDS;
        return ERROR;
    }
    /* adding label to symbol table */
    if (addLabel(word, EXTERNAL_DEFAULT_ADDRESS, vars, TRUE) == NULL)
        return ERROR;
    return isError(&(vars->error));
}

/* writeNumberToData: encodes a given number to data */
void writeNumberToData(int num, int *dc, unsigned int data[])
{
    data[(*dc)++] = (unsigned int)num;
}

/* writeStringToData: encodes a given string to data */
void writeStringToData(char *str, int *dc, unsigned int data[])
{
    while (!isLineEnd(str))
    {
        data[(*dc)++] = (unsigned int)*str;
        str++;
    }

    data[(*dc)++] = '\0';
}
