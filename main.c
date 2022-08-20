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
    refactor(vars) 
    for (i = 1; i < argc; i++)
    {
        filename = createFileName(argv[i], FILE_INPUT);
        fp = fopen(filename, "r");
        if (fp != NULL) /* if we managed to open file */
        {
            vars -> symbolsTable = NULL;
            vars -> externList = NULL;
            vars -> entryFlag = FALSE;
            vars -> externFlag = FALSE;
            vars -> recordedError = FALSE;
            printf("\n-------- Proccessing: %s%s\"%s\"%s%s --------\n\n", BLUE, BOLD, filename, BOLDEND, RESET_COLOR);
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
            else
                vars->recordedError = TRUE;

            free(amFilename); /* we have a pointer, hence no longer needed */
            if (!(vars->recordedError))
                printf("\n-------- Finished %s%s\"[%s]\"%s%s assembling process %ssuccessfully%s --------\n\n", GREEN, BOLD, filename, RESET_COLOR, BOLDEND, GREEN, RESET_COLOR);
            else
                printf("\n-------- Finished %s%s\"[%s]\"%s%s assembling process with %serrors%s --------\n\n", RED, BOLD, filename, RESET_COLOR, BOLDEND, RED, RESET_COLOR);

        }

        else /* if we wasn't able to open file */
            fprintf(stderr, "%s%sERROR%s : Cannot open file: %s%s\n\n", RED, BOLD, RESET_COLOR,BOLD,argv[i]);
        free(filename);
        freeMacroNodes(&macroHead);
    }
    /* Free any allocated memory we've might used */
    freeLabels(&(vars->symbolsTable));
    free(vars);
    return 0;
}
