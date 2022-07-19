 #include "preAssembler.h"


/*Prints the macro table for inside use.*/
void printMacroTable(struct Macro *head) 
{
    struct Macro *temp = NULL;
    temp = (struct Macro *)malloc(sizeof(struct Macro));
    temp = head->next;
    while (temp != NULL)
    {
        printf("\nName: %s\nContent:\n%s\n", temp->mname, temp->mcontent);
        temp = temp->next;
    }
}

/*returns 1 if met start of a macro, returns 2 if met end of a macro, returns 0 when is neither*/
int macroOperation(char line[])
{
    int index = 0, mindex = 0;
    char macro[MAX];
    memset(macro, '\0', MAX);
    while (isspace(line[index]))
        index++;
    while (!isspace(line[index]) && line[index] != '\n')
    {
        macro[mindex] = line[index];
        mindex++;
        index++;
    }
    if (!strcmp(macro, "macro"))
        return MACRO;
    if (!strcmp(macro, "endmacro"))
        return END_MACRO;
    return NOT_MACRO;
}
/*Pushes macro name in to node*/
void pushMacroName(struct Macro *temp, char line[]) 
{
    int i = 0, nameIndex = 0;
    char name[MAX];
    memset(name, '\0', MAX);
    while (isspace(line[i]))
        i++;
    while (!isspace(line[i]) && line[i] != '\n')
        i++;
    while (isspace(line[i]))
        i++;
    while (!isspace(line[i]) && line[i] != '\n')
    {
        name[nameIndex] = line[i];
        nameIndex++;
        i++;
    }
    strcpy(temp->mname, name);
}
/*Pushes the macro contents into macro node*/
void pushMacroContent(struct Macro *temp, FILE *fp) 
{
    char line[MAX];
    char content[MAX];
    memset(line, '\0', MAX);
    memset(content, '\0', MAX);
    while (fgets(line, MAX, fp) != NULL && macroOperation(line) != END_MACRO )
    {
        strncat(content, line, MAX);
    }
    strcpy(temp->mcontent, content);
}

/*Performing the first pass on the file (inserting the macros into a linkedlist, copying the corresponding rows from the table to the file, etc.)*/
void firstMacroPass(FILE *fp, struct Macro *head)
{
    char line[MAX];
    memset(line, '\0', MAX); 
    while (fgets(line, MAX, fp))
    {

        struct Macro *temp = NULL;
        temp = (struct Macro *)malloc(sizeof(struct Macro));
        if (macroOperation(line) == MACRO)
        {
            pushMacroName(temp, line);
            pushMacroContent(temp, fp);
            head->next = temp;
            head = temp;
        }
    }
}

/*Copy the contents of the corresponding macro to the file from the list, if it is a macro command*/
/* TODO: Maybe use fscanf instead a char at a time */
int isMacroCall(char line[], FILE *fpw, struct Macro *head)
{
    int index = 0, mindex = 0;
    char Mname[MAX];
    struct Macro *temp = NULL;
    temp = (struct Macro *)malloc(sizeof(struct Macro));
    temp = head;
    memset(Mname, '\0', MAX);
    while (isspace(line[index]))
        index++;
    while (!isspace(line[index]) && line[index] != '\n')
    {
        Mname[mindex] = line[index];
        mindex++;
        index++;
    }
    while (temp != NULL)
    {
        if (!strcmp(temp->mname, Mname))
        {
            fprintf(fpw, "%s", temp->mcontent);
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

/* Second Pass - This time we write into the new .am file relvant lines (not macros) and processing the macro  */
void secondMacroPass(FILE *fpr, char *writefilename, struct Macro *head)
{
    int flag = 0;
    char line[MAX];
    FILE *fpw;
    rewind(fpr);
    fpw = fopen(writefilename, "w");
    if (fpw == NULL)
        printf("error: cant open the file: %s \n \n", writefilename);
    while (fgets(line, MAX, fpr))
    {
        if (!flag)
        {
            if (!isMacroCall(line, fpw, head)) /* Check wether is a macro call, if yes replacing the call with the relvant macro content, if not continue */
            {
                if (macroOperation(line) == NOT_MACRO) /* not start or end of macro */
                {
                    fprintf(fpw, "%s", line);
                }
                else if (macroOperation(line) == MACRO) /* inside of a macro */
                {
                    flag = 1;
                }
            }
        }
        else
        {
            if (macroOperation(line) == END_MACRO)
                flag = 0;
        }
    }
    fclose(fpw);
    fclose(fpr);
}
