/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
**************************************************/

#include "preAssembler.h"

/* preAssembler: runs the preAssembler and returns TRUE if succeeded (no macro syntax issues) */
boolean preAssembler(FILE *fpr, char *writefilename, MacroNode **head)
{
    if (firstMacroPass(fpr, head))
        secondMacroPass(fpr, writefilename, head);
    else
    {
        freeMacroNodes(head);
        fclose(fpr);
        return FALSE;
    }
    return TRUE;
}

/* firstMacroPass: performing the first pass on the file 
 * (inserting the macros into a linkedlist, copying the corresponding rows from the table to the file, etc.)
 */
boolean firstMacroPass(FILE *fp, MacroNode **head)
{
    char line[MAX_LINE];
    MacroNode *temp;
    int lineCount = 0;
    memset(line, '\0', MAX_LINE);
    while (fgets(line, MAX_LINE, fp))
    {
        lineCount += 1;
        if (macroOperation(line) == MACRO)
        {
            temp = createNode();
            if (temp == NULL)
            {
                printf("Dynamic allocation error");
                return FALSE;
            }
            if (!pushMacroName(temp, line, lineCount, head)) /* if failed to push macro name (because of syntax error) */
            {
                free(temp); /* frees the newly created node and returns false */
                return FALSE;
            }
            printf("before increasing: %d\n",lineCount);
            pushMacroContent(temp, fp, &lineCount);
                        printf("after increasing: %d\n",lineCount);
            temp -> next = *head;
            *head = temp;
        } 
    }
    return TRUE;
}

/* secondMacroPass: writes relevant lines (not macros) and processing the macro into the new .am file */
void secondMacroPass(FILE *fpr, char *writefilename, MacroNode **head)
{

    boolean inMacro = FALSE;
    char line[MAX_LINE];
    FILE *fpw;
    rewind(fpr);
    fpw = fopen(writefilename, "w");
    if (fpw == NULL){
        printf("Error: can't open file: %s \n \n", writefilename);
        return;
    }
    while (fgets(line, MAX_LINE, fpr))
    {
        if (!inMacro)
        {
            if (!isMacroCall(line, fpw, *head)) /* Check wether is a macro call, if yes replacing the call with the relvant macro content, if not continue */
            {
                if (macroOperation(line) == NOT_MACRO) /* not start or end of macro */
                    fprintf(fpw, "%s", line);
                else if (macroOperation(line) == MACRO) /* inside of a macro */
                    inMacro = TRUE;
            }
        }
        else if (macroOperation(line) == END_MACRO)
            inMacro = FALSE;
    }
    fclose(fpw);
    fclose(fpr);
}

/* macroOperation: checks if line starts with macro or endmacro */
int macroOperation(char line[])
{
    char firstWord[MAX_LINE];
    memset(firstWord, '\0', MAX_LINE);
    if (sscanf(line, "%s", firstWord))
    {
        if (!strcmp(firstWord, "macro"))
            return MACRO;
        if (!strcmp(firstWord, "endmacro"))
            return END_MACRO;
    }
    return NOT_MACRO;
}

/* pushMacroName: pushes macro name into node, returns FALSE if declaration is wrong */
boolean pushMacroName(MacroNode *temp, char line[], int lineCount, MacroNode **head)
{
    char cmd[MAX_LINE];
    char name[MAX_LINE];
    char garbage[MAX_LINE];
    if (sscanf(line, "%s %s %s", cmd, name, garbage) != 2) /* if declaration doesn't have a name or have 1 or more names, EXITING */
    {
        printf("ERROR (line %d): illegal macro declaration\n", lineCount);
        return FALSE;
    }
    if (!isValidName(name, head))
    {
        printf("ERROR (line %d): \"%s\" is a reserved or used name, Can't use this name for macro.\n", lineCount, name);
        return FALSE;
    }
    strcpy(temp -> mname, name);
    return TRUE;
}

/* isValidName: checks if given name isn't a restricted name, or an existing macro name */
boolean isValidName(char *name, MacroNode **head)
{
    int i = 0;
    MacroNode *pointer = *head;
    const char *blackList[BLACKLIST] = {""};
    pointer = *head;
    blackList[0] = "mov";
    blackList[1] = "cmp";
    blackList[2] = "add";
    blackList[3] = "sub";
    blackList[4] = "not";
    blackList[5] = "clr";
    blackList[6] = "lea";
    blackList[7] = "inc";
    blackList[8] = "dec";
    blackList[9] = "jmp";
    blackList[10] = "bne";
    blackList[11] = "get";
    blackList[12] = "prn";
    blackList[13] = "jsr";
    blackList[14] = "rts";
    blackList[15] = "hlt";
    blackList[16] = "r0";
    blackList[17] = "r0";
    blackList[18] = "r1";
    blackList[19] = "r2";
    blackList[20] = "r3";
    blackList[21] = "r4";
    blackList[22] = "r5";
    blackList[23] = "r6";
    blackList[24] = "r7";
    blackList[25] = "PSW";
    blackList[26] = "data";
    blackList[27] = "string";
    blackList[28] = "struct";
    blackList[29] = "entry";
    while (i < BLACKLIST)
    {
        if (!strcmp(name, blackList[i]))
            return FALSE;
        i += 1;
    }
    while (pointer != NULL)
    {
        if (!strcmp(pointer -> mname, name))
            return FALSE;
        pointer = pointer -> next;
    }
    return TRUE;
}


/* pushMacroContent: pushes the macro contents into macro node */
void pushMacroContent(MacroNode *temp, FILE *fp, int *lineCounter)
{
    char line[MAX_LINE];
    char content[MAX_LINE];
    memset(line, '\0', MAX_LINE);
    memset(content, '\0', MAX_LINE);
    while (fgets(line, MAX_LINE, fp) != NULL && macroOperation(line) != END_MACRO) /*TODO: DELETE ISCOMMENT FUNC*/ 
    {
        strncat(content, line, MAX_LINE);
        printf("counter in push macro content: %d\n", *lineCounter);
        (*lineCounter)+=1;
    }
    (*lineCounter)+=1;
    strcpy(temp -> mcontent, content);
}

/* isMacroCall: parsing defined macro calls, returns true if given line is a macro call */
boolean isMacroCall(char line[], FILE *fpw, MacroNode *head)
{
    char Mname[MAX_LINE];
    MacroNode *pointer = head;
    memset(Mname, '\0', MAX_LINE);
    if (sscanf(line, "%s", Mname))
    {
        while (pointer != NULL)
        {
            if (!strcmp(pointer -> mname, Mname))
            {
                fprintf(fpw, "%s", pointer -> mcontent);
                return TRUE;
            }
            pointer = pointer -> next;
        }
    }
    return FALSE;
}

/* createNode: allocates memory for a new node, creates and returns it */
MacroNode *createNode()
{
    MacroNode *newNode = (MacroNode *)malloc(sizeof(MacroNode));
    if (newNode != NULL)
    {
        (*newNode).next = NULL;
        return newNode;
    }
    return newNode;
}

/* freeMacroNodes: frees a given linked-list memory from head to tail */
void freeMacroNodes(MacroNode **head)
{
    MacroNode *tempNode;
    while (*head != NULL)
    {
        tempNode = *head;
        *head = (*head) -> next;
        free(tempNode);
    }
}

