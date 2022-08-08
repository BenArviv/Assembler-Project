#include <stdio.h>
#include <stdlib.h>
#include "preAssembler.h"
#include "firstPass2.h"
#include "main.h"
#include "structures.h"

int main(int argc, char *argv[])
{
    int i;
    char *filename;
    char *amFilename;

    extVars *vars = (extVars *)malloc(sizeof(extVars));
    vars -> ic = 0;
    vars -> dc = 0;
    vars -> externFlag = FALSE;
    vars -> recordedError = FALSE;
    vars -> symbolsTable = NULL;
    vars -> cmd[] = {{"mov"},
                    {"cmp"},
                    {"add"},
                    {"sub"},
                    {"not"},
                    {"clr"},
                    {"lea"},
                    {"inc"},
                    {"dec"},
                    {"jmp"},
                    {"bne"},
                    {"get"},
                    {"prn"},
                    {"jsr"},
                    {"rts"},
                    {"hlt"}};
    vars -> dir[] = {{".data"},
                    {".string"},
                    {".struct"},
                    {".entry"},
                    {".extern"}};
    /*
    int dc = 0;
    int ic = 0;
    unsigned int data[MACHINE_RAM];
    unsigned int instructions[MACHINE_RAM];
    labelPtr symbolsTable = NULL;
    boolean externFlag = FALSE;
    boolean entryFlag = FALSE;
    boolean recordedError = FALSE;
    */
    FILE *fp;
    MacroNode *head = NULL;
    for (i = 1; i < argc; i++)
    {
        filename = createFileName(argv[i], FILE_INPUT);
        fp = fopen(filename, "r");
        if (fp != NULL)
        {
            amFilename = createFileName(argv[i], FILE_MACRO); /* .am appended to filename */
            if (preAssembler(fp, amFilename, &head))
            {
                fp = fopen(amFilename, "r");
                firstPass2(fp, cmd, dir, vars);
                /*firstPass(fp, cmd, dir, &externFlag, &symbolsTable, &dc, &ic, data, instructions, &recordedError); */
                fclose(fp);
            }
            free(amFilename); /* we have a pointer, hence no longer needed */
        }
        else
            printf("CANNOT_OPEN_FILE\n");
        free(filename);
    }
    printf("Have we recorded an error: --> %d <-- (1 - TRUE | 0 - FALSE) \n", vars->recordedError);
    freeMacroNodes(&head);
    print_labels(vars->symbolsTable);
    freeLabels(&(vars->symbolsTable));
    return 0;
}
