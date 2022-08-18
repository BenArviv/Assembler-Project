#include <stdio.h>
#include <stdlib.h>

#include "firstPass2.h"
#include "labelUtils.h"
#include "main.h"
#include "preAssembler.h"
#include "secondPass.h"
#include "structures.h"

int main(int argc, char *argv[])
{
    int i;
    char *filename;
    char *amFilename;
    FILE *fp;
    MacroNode *head = NULL;
    stringStruct cmdInit[] = {{"mov"},
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
    stringStruct dataInit[] = {{".data"},
                               {".string"},
                               {".struct"},
                               {".entry"},
                               {".extern"}};

    extVars *vars;
    for (i = 1; i < argc; i++)
    {
        refactor(vars)
        filename = createFileName(argv[i], FILE_INPUT);
        fp = fopen(filename, "r");
        if (fp != NULL)
        {
            amFilename = createFileName(argv[i], FILE_MACRO); /* .am appended to filename */
            if (preAssembler(fp, amFilename, &head))
            {
                fp = fopen(amFilename, "r");
                firstPass2(fp, vars);

                if (!(vars -> recordedError))
                {
                    rewind(fp);
                    secondPass(fp, argv[i], vars);
                }
                fclose(fp);
            }
            free(amFilename); /* we have a pointer, hence no longer needed */
        }
        else
            printf("CANNOT_OPEN_FILE\n");
        printf("Have we recorded an error: --> %d <-- (1 - TRUE | 0 - FALSE) \n", vars -> recordedError);
        freeLabels(&(vars -> symbolsTable));
        freeMacroNodes(&head);
        free(filename);
        free(vars);
    }
    return 0;
}
