#include "firstPass2.h"
#include "labelUtils.h"

/* firstPass: main first pass routine for file.am */
void firstPass2(FILE *fp, extVars *vars){
    char line[MAX_LINE];
    int lineCount = 1;
    vars -> error = NO_ERROR;
    vars -> ic = 0;
    vars -> dc = 0;
    puts("--------IN FIRST PASS---------");
    while (fgets(line, MAX_LINE, fp)){
        vars -> error = NO_ERROR;
        if (!skipLine(line))
            readLine(line, vars);
        if (isError(&(vars -> error))){
            vars -> recordedError = TRUE;
            write_error(lineCount, vars -> error); /* TODO: end this */
        }

        lineCount++;
    }
    offsetAdd(vars -> symbolsTable, MEMORY_START, FALSE); /* Instruction symbols */
    offsetAdd(vars -> symbolsTable, vars -> ic + MEMORY_START, TRUE); /* Data symbols */
}

/* readLine: analyzing a given line from file */
void readLine(char *line, extVars *vars){
    int cmdType = UNKNOWN_COMMAND;
    int dirType = UNKNOWN_DIR;

    boolean label = FALSE; /* flag for labels */
    labelPtr labelNode = NULL;
    char word[MAX_LINE];
    memset(word, '\0', MAX_LINE);
    line = skipWhiteChars(line);
    if (isLineEnd(line))
        return;
    if (!isalpha(*line) && *line != '.'){ /* if line doesn't start with '.' or alpha char it's a syntax error */
        vars -> error = SYNTAX_ERR;
        return;
    }

    copyWord(word, line);

    if (isLabel(word, COLON, vars)){ /* if it's a label */
        label = TRUE;
        labelNode = addLabel(word, 0, vars, FALSE);
        if (labelNode == NULL)
            return;
        line = nextWord(line); /* going to the next word in line */
        if (line == NULL){
            vars -> error = LABEL_ONLY;
            return;        
        }
        copyWord(word, line); /* copy the first word in line into "word" */
    }

    if (isError(&(vars -> error)))
        return;

    if ((dirType = findDirective(word, vars -> dir)) != NOT_FOUND){ /* if it's a directive */
        if (label){ /* if we're inside a label creation */
            if (dirType == EXTERN || dirType == ENTRY){ /* ignore creation of label if so */
                deleteLabel(&(vars -> symbolsTable), labelNode -> name);
                label = FALSE;
            }
            else 
                labelNode -> address = vars -> dc;
        }

        line = nextWord(line);
        handleDir(dirType, line, vars);
    }
    else if ((cmdType = findCMD(word, vars -> cmd)) != NOT_FOUND){ /* if it's a command */
        if (label){
            labelNode -> inActionStatement = TRUE;
            labelNode -> address = vars -> ic;
        }

        line = nextWord(line);
        handleCMD(cmdType, line, vars);
    }
    else{ /* unknown command */
        vars -> error = COMMAND_NOT_FOUND;
    }
}

/*  */
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
                vars -> error = COMMAND_UNEXPECTED_CHAR;
                return ERROR;
            }
            else
            {
                line = nextCommaWord(secondOp, line);
                if (isLineEnd(secondOp)) /* if we have only a comma without 2nd operand */
                {
                    vars -> error = COMMAND_UNEXPECTED_CHAR;
                    return ERROR;
                }
                if (*secondOp == ',')
                {
                    vars -> error = COMMAND_COMMA_IN_A_ROW;
                    return ERROR;
                }
                
                secondOperand = TRUE;
            }
        }
    }
    line = skipWhiteChars(line);
    if (!isLineEnd(line)) /* if we have extra chars after the operands */
    {
        vars -> error = COMMAND_TOO_MANY_OPERANDS;
        return ERROR;
    }
    if (firstOperand)
        firstAddMethod = methodDetect(firstOp, vars);
    if (secondOperand)
        secondAddMethod = methodDetect(secondOp, vars);
     if (!isError(&(vars -> error)))
        {
            if (cmdOpernads(type, firstOperand, secondOperand))
            {
                if (cmdMethods(type, firstAddMethod, secondAddMethod))
                {
                    encodeInsturction(encodeWord(type, firstOperand, secondOperand, firstAddMethod, secondAddMethod), vars -> instructions, &(vars -> ic));
                    vars -> ic += numberOfWords(firstOperand, secondOperand, firstAddMethod, secondAddMethod);
                }
                else
                {
                    vars -> error = COMMAND_INVALID_OPERANDS_METHODS;
                    return ERROR;
                }
            }
            else
            {
                vars -> error = COMMAND_INVALID_NUMBER_OF_OPERANDS;
                return ERROR;
            }
        }
        return NO_ERROR; 
}

/*
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
    vars -> error = COMMAND_INVALID_METHOD;
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

int handleDir(int type, char *line, extVars *vars){
    if (line == NULL || isLineEnd(line)){
        vars -> error = DIRECTIVE_NO_PARAMS;
        return ERROR;
    }
    
    switch (type){
        case DATA:
            return handleDataDir(line, &(vars -> error), &(vars -> dc), vars -> data);
        
        case STRING:
            return handleStringDir(line, &(vars -> error), &(vars -> dc), vars -> data);

        case STRUCT:
            return handleStructDir(line, &(vars -> error), &(vars -> dc), vars -> data);
        
        case ENTRY:
            return handleEntryDir(line, &(vars -> error)); /* actually only checks syntax */

        case EXTERN:
            return handleExternDir(line, vars);
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
                        if (*word == ',')
                        {
                            *error = STRUCT_COMMA_IN_A_ROW;
                            return ERROR;
                        }
                        if (isValidString(word))
                        {
                            line = skipWhiteChars(line);
                            if (!isLineEnd(line))
                            {
                                *error = STRUCT_EXTRA_CHARS;
                                return ERROR;
                            }
                            
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

int handleExternDir(char *line, extVars *vars){
    char word[MAX_LINE];
    copyWord(word, line);
    if (isLineEnd(word)){
        vars -> error = EXTERN_NO_LABEL;
        return ERROR;
    }
    if (!isLabel(word, FALSE, vars)){
        vars -> error = EXTERN_INVALID_LABEL;
        return ERROR;
    }
    line = nextWord(line);
    if (!isLineEnd(line)){
        vars -> error = EXTERN_TOO_MANY_OPERANDS;
        return ERROR;
    }
    if (addLabel(word, EXTERNAL_DEFAULT_ADDRESS, vars, TRUE) == NULL)
        return ERROR;
    return isError(&(vars -> error));
}

void writeNumberToData(int num, int *dc, unsigned int data[])
{
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
