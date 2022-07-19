#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "preAssembler.h"

#define MAX 100
#define MAX_EXTENSION_LEN 5
#define ERROR 1

int main(int argc, char *argv[]);

/* Types of files that indicate what is the desirable file extension */
enum filetypes {FILE_INPUT, FILE_OBJECT, FILE_ENTRY, FILE_EXTERN,FILE_MACRO};

/* used for creating files and assigning required extensions to them */
char *createFileName(char *original, int type);
