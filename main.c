/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
**************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "firstPass.h"
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
    MacroNode *macroHead = NULL;
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
        if (fp != NULL) /* if we managed to open file */
        {
            amFilename = createFileName(argv[i], FILE_MACRO); /* .am appended to filename */
            if (preAssembler(fp, amFilename, &macroHead))
            {
                fp = fopen(amFilename, "r");
                firstPass(fp, vars);

                if (!(vars->recordedError)) /* if we had errors, we do not continue to secondPass */ 
                {
                    rewind(fp);
                    secondPass(fp, argv[i], vars);
                }
                fclose(fp);
            }
            free(amFilename); /* we have a pointer, hence no longer needed */
        }

        else
            fprintf(stderr, "cannot open file: %s\n", argv[i]);

        /* Free any allocated memory we've might used */
        freeLabels(&(vars->symbolsTable));
        freeMacroNodes(&macroHead);
        free(filename);
        free(vars);
    }
    return 0;
}
