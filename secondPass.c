/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
**************************************************/

#include "secondPass.h"

/* secondPass: main second pass routine for file.am */
void secondPass(FILE *fp, char *filename, extVars *vars)
{
    char line[MAX_LINE];
    int lineCount = 1;
    vars -> ic = 0;

    while (fgets(line, MAX_LINE, fp))
    {
        vars -> error = OK;
        if (!skipLine(line))
        {
            readLineSecond(line, vars);
        }
        if (isError(&(vars -> error)))
        {
            vars -> recordedError = TRUE;
            printError(lineCount, vars -> error);
        }

        lineCount++;
    }
    if (!(vars -> recordedError)) /* creating output only if we havn't recorded errors */
    {
        writeOutput(filename, vars);
    }
    freeLabels(&(vars -> symbolsTable));
    freeExterns(&(vars -> externList));
    
    /* TODO: Free labels and externs */
}

/* readLineSecond: analyzing a given line from file */
void readLineSecond(char *line, extVars *vars)
{
    int dirType;
    int cmdType;
    char word[MAX_LINE];         /* current word to analyze */
    line = skipWhiteChars(line); /* skipping to the first non-white char*/
    if (isLineEnd(line))         /* bkipping blank lines */
        return;
    copyWord(word, line);           /* copying the first word from line */
    if (isLabel(word, COLON, vars)) /* if it's a label we are skipping this word */
    {
        line = nextWord(line);
        copyWord(word, line);
    }
    if ((dirType = findDirective(word, vars -> dir)) != NOT_FOUND)
    {
        line = nextWord(line);
        if (dirType == ENTRY)
        {
            copyWord(word, line);
            entryLabel(vars -> symbolsTable, word, vars);
        }
    }
    else if ((cmdType = findCMD(word, vars -> cmd)) != NOT_FOUND)
    {
        line = nextWord(line);
        handleCMDSecond(cmdType, line, vars);
    }
}

/* writeOutput: creates files as needed, and writes the output on each file */
boolean writeOutput(char *filename, extVars *vars)
{
    FILE *fp;
    fp = openFile(filename, FILE_OBJECT, vars);
    writeOB(fp, vars);

    if (vars -> externFlag)
    {
        fp = openFile(filename, FILE_EXTERN, vars);
        writeExtern(fp, vars);
    }
    if (vars -> entryFlag)
    {
        fp = openFile(filename, FILE_ENTRY, vars);
        writeEntry(fp,vars);
    }
    
    fclose(fp);

    return OK;
}

/* openFile: opens a file according to file type */
FILE *openFile(char *filename, int type, extVars *vars)
{
    
    FILE *fp;
    filename = createFileName(filename, type);

    fp = fopen(filename, "w");
    free(filename);

    if (fp == NULL)
    {
        vars -> error = CANNOT_OPEN_FILE;
        return NULL;
    }
    return fp;
}

/* writeExtern: writes the content for file.ext */
void writeExtern(FILE *fp, extVars *vars)
{
    char *address;
    extPtr ptr = vars -> externList;
    do
    {
        address = decToBase32(ptr -> address);
        fprintf(fp, "%s\t%s\n", ptr -> name, address);
        free(address);
        ptr = ptr -> next;
    } while (ptr != vars -> externList);
    fclose(fp);
}

/* writeEntry: writes the content for file.ent */
void writeEntry(FILE *fp, extVars *vars)
{
    char *address;
    labelPtr ptr = vars -> symbolsTable;
    while (ptr != NULL)
    {
        if (ptr -> IsEntry && ptr -> address != 0)
        {
            address = decToBase32(ptr -> address);
            fprintf(fp, "%s\t%s\n", ptr -> name, address);
            free(address);
        }
        ptr = ptr -> next;       
    }
    
}

/* writeOB: writes the content for file.ob */
void writeOB(FILE *fp, extVars *vars)
{
    unsigned int address = MEMORY_START;
    int i;
    char *firstCol = decToBase32(vars -> ic);
    char *secondCol = decToBase32(vars -> dc);
    printCounters(fp, firstCol, secondCol);
    free(firstCol);
    free(secondCol);

    /* insturctions */
    for (i = 0; i < vars -> ic; i++, address++)
    {
        firstCol = decToBase32(address);
        secondCol = decToBase32(vars -> instructions[i]);

        fprintf(fp, "%s\t%s\n", firstCol, secondCol);
        free(firstCol);
        free(secondCol);
    }
    /* data */
    for (i = 0; i < vars -> dc; i++, address++)
    {
        firstCol = decToBase32(address);
        secondCol = decToBase32(vars -> data[i]);

        fprintf(fp, "%s\t%s\n", firstCol, secondCol);
        free(firstCol);
        free(secondCol);
    }
    fclose(fp);
}

/* printCounters: a routine for formatted IC and DC printing (without first ! character) */
void printCounters(FILE *fp, char *firstCol, char *secondCol){
    int i = 0, j = 0; /* determines from where to print the number */

    if (*firstCol == '!')
        i++;
    if (*secondCol == '!')
        j++;
    
    fprintf(fp, "%s\t%s\n", (firstCol + i), (secondCol + j));
}

/* handkeCMDSecond: handles CMDs for second pass */ 
int handleCMDSecond(int cmdType, char *line, extVars *vars)
{
    char firstOp[MAX_LINE];  /* first cmd operand */
    char secondOp[MAX_LINE]; /* second cmd operand */

    char *src = firstOp;
    char *dest = secondOp;

    boolean isSrc = FALSE, isDest = FALSE;
    int srcMethod = METHOD_UNKNOWN, destMethod = METHOD_UNKNOWN;

    cmdOpernadsSecond(cmdType, &isSrc, &isDest);

    if (isSrc)
        srcMethod = extractBits(vars -> instructions[vars -> ic], SRC_METHOD_START_POS, SRC_METHOD_END_POS);
    if (isDest)
        destMethod = extractBits(vars -> instructions[vars -> ic], DEST_METHOD_START_POS, DEST_METHOD_END_POS);

    if (isSrc || isDest)
    {
        line = nextCommaWord(firstOp, line);
        if (isSrc && isDest)
        {
            line = nextCommaWord(secondOp, line);
            nextCommaWord(secondOp, line);
        }
        else /* if there's only 1 operand, it's a dest operand */
        {
            dest = firstOp;
            src = NULL;
        }
    }
    vars -> ic++;
    return encodeAddtional(src, dest, isSrc, isDest, srcMethod, destMethod, vars);
}

/* encodeAdditional: encodes additional words to memory */
int encodeAddtional(char *src, char *dest, boolean isSrc, boolean isDest, int srcMethod, int destMethod, extVars *vars)
{
    if (isSrc && isDest && srcMethod == METHOD_REGISTER && destMethod == METHOD_REGISTER)
        encodeInsturction((registerOperand(FALSE, src) | registerOperand(TRUE, dest)), vars -> instructions, &(vars -> ic));
    else
    {
        if (isSrc)
            additionalWords(FALSE, srcMethod, src, vars);
        if (isDest)
            additionalWords(TRUE, destMethod, dest, vars);
    }
    return isError(&(vars -> error));
}

/* registerOperand: additional words for a register operand */
unsigned int registerOperand(boolean isDest, char *reg)
{
    unsigned int word = (unsigned int)atoi(reg + 1);
    if (!isDest)
        word <<= BITS_IN_REGISTER;
    word = encodeARE(word, ABSOLUTE);
    return word;
}

/* additionalWords: encodes additional words to instructions memory, given the addressing method */
void additionalWords(boolean isDest, int method, char *op, extVars *vars)
{
    unsigned int word = EMPTY_WORD;
    char *temp;

    switch (method)
    {
    case METHOD_IMMEDIATE:
        word = (unsigned int)atoi(op + 1);
        word = encodeARE(word, ABSOLUTE);
        encodeInsturction(word, vars -> instructions, &(vars -> ic));
        break;

    case METHOD_DIRECT:
        encodeLabel(op, vars);
        break;
    case METHOD_STRUCT:
        temp = strchr(op, '.');
        *temp = '\0';
        encodeLabel(op, vars);
        *temp++ = '.';
        word = (unsigned int)atoi(temp);
        word = encodeARE(word, ABSOLUTE);
        encodeInsturction(word, vars -> instructions, &(vars -> ic));
        break;
    case METHOD_REGISTER: 
        word = registerOperand(isDest, op);
        encodeInsturction(word, vars -> instructions, &(vars -> ic));
    }
}

/* encodeLabel: encodes a label to the memory */
void encodeLabel(char *name, extVars *vars)
{
    unsigned int word; /* to be encoded */

    if (isExistingLabel(vars -> symbolsTable, name))
    {
        word = getLabelAddress(vars -> symbolsTable, name); /* getting address */

        if (isLabelExternal(vars -> symbolsTable, name))
        {
            addExtern(&(vars -> externList), name, vars -> ic + MEMORY_START);
            word = encodeARE(word, EXTERNAL);
        }
        else
            word = encodeARE(word, RELOCATABLE);

        encodeInsturction(word, vars -> instructions, &(vars -> ic));
    }
    else
    {
        vars -> ic++;
        vars -> error = COMMAND_LABEL_DOES_NOT_EXIST;
    }
}
/* cmdOperandsSecond: sets param flags to true or false for each command */
void cmdOpernadsSecond(int type, boolean *isSrc, boolean *isDest)
{
    switch (type)
    {
    /* 2 operands CMDs */
    case MOV:
    case CMP:
    case ADD:
    case SUB:
    case LEA:
        *isSrc = TRUE;
        *isDest = TRUE;
        break;

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
        *isSrc = FALSE;
        *isDest = TRUE;
        break;

    /* 0 operands CMDs */
    case RTS:
    case HLT:
        *isSrc = FALSE;
        *isDest = FALSE;
    }
}

/* addExtern: adds a label to the extern's linked-list */
extPtr addExtern(extPtr *head, char *name, unsigned int ref)
{
    extPtr t = *head;
    extPtr temp;

    temp = (extPtr)malloc(sizeof(ext));
    if (!temp)
    {
        puts("FATAL ERROR, Cannot allocate memory\n");
        exit(1);
    }
    temp -> address = ref;
    strcpy(temp -> name, name);
    if (!(*head))
    {
        *head = temp;
        temp -> next = temp;
        temp -> prev = temp;
        return temp;
    }

    ((*head) -> prev) -> next = temp;
    temp -> next = t;
    temp -> prev = t -> prev;
    (*head) -> prev = temp;

    return temp;
}

/* freeExterns: frees the extern's linked-list */
void freeExterns(extPtr *head)
{
    extPtr ptr = *head;
    if (ptr != NULL)
    {
        unsigned int prev = ((*head) -> prev) -> address;
        unsigned int curr = 0;
        do
        {
            ptr = *head;
            curr = ptr -> address;
            *head = (*head) -> next;
            free(ptr);
        } while (curr != prev);
        
    }
}
