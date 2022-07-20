#include "utils.h"


/* binToBase32: converts a binary number into a base-32 number using binToDec and decToBase32, 
                and returns it as a string */
char *binToBase32(int binary){
    int dec = binToDec(binary);
    return decToBase32(dec);
}

/* binToDec: converts a binary number to a decimal number */
int binToDec(int n) {
  int dec = 0, i = 0, rem;

  while (n!=0) {
    rem = n % 10;
    n /= 10;
    dec += rem * pow(2, i);
    ++i;
  }

  return dec;
}

/* decToBase32: converts a decimal number to a base-32 number using decTo32, 
                and returns it as a string */
char *decToBase32(int decimal){
    char *converted = (char *)malloc(sizeof(char));
    
    decTo32(decimal, key, converted);
    return converted;
}

/* decTo32: the actual algorithm for converting a decimal number to a base-32 number */
void decTo32(int dec, char key[], char *converted){
    if (dec / 32 == 0){
        char *q;

        *converted = key[dec % 32];
        q = (char *)realloc(converted, sizeof(converted) * 2);
        if (!q){
            printf("decTo32: reallocation error.\n");
            exit(0);
        }
        converted = q;
        return;
    }
    else {
        decTo32(dec / 32, key, converted++);
        *converted = key[dec % 32];
        return;
    }
}

char *createFileName(char *original, int type)
{
    char *newFileName = (char *)malloc(strlen(original) + MAX_EXTENSION_LEN);
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
