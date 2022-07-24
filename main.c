#include "main.h"

int main(int argc, char *argv[])
{
    int i;
    char *filename;
    char *amFilename;
    FILE *fp;
    MacroNode *head = NULL;
    for (i = 1; i < argc; i++)
    {
        filename = createFileName(argv[i], FILE_INPUT);
        fp = fopen(filename, "r");
        if (fp != NULL)
        {
            amFilename = createFileName(argv[i], FILE_MACRO); /* .am appended to filename */
            preAssembler(fp, amFilename, &head);
            free(amFilename); /* we have a pointer, hence no longer needed */
        }
        else
            printf("CANNOT_OPEN_FILE\n");
        free(filename);
    }
    freeMacroNodes(&head);
    return 0;
}

char *createFileName(char *original, int type)
{
    char *newFileName = (char *)malloc(sizeof(strlen(original)) + MAX_EXTENSION_LEN);
    if (newFileName == NULL)
    {
        fprintf(stderr, "Dynamic allocation error.");
        exit(ERROR);
    }
    strcpy(newFileName, original); /* Copying original filename to the bigger string */

    /* Concatenating the required file extension */

    switch (type)
    {
    case FILE_INPUT:
        strcat(newFileName, ".as");
        break;

    case FILE_OBJECT:
        strcat(newFileName, ".ob");
        break;

    case FILE_ENTRY:
        strcat(newFileName, ".ent");
        break;

    case FILE_EXTERN:
        strcat(newFileName, ".ext");

    case FILE_MACRO:
        strcat(newFileName, ".am");
        break;
    }
    return newFileName;
}
