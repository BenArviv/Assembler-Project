/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
**************************************************/

#include "utils.h"

/* binToBase32: converts a binary number into a base-32 number using binToDec and decToBase32,
                and returns it as a string */
char *binToBase32(int binary)
{
    int dec = binToDec(binary);
    return decToBase32(dec);
}

/* binToDec: converts a binary number to a decimal number */
int binToDec(int n)
{
    int dec = 0, i = 0, rem;

    while (n != 0)
    {
        rem = n % 10;
        n /= 10;
        dec += rem * pow(2, i);
        ++i;
    }

    return dec;
}

/* decToBase32: converts a decimal number to a base-32 number using decTo32,
                and returns it as a string */
char *decToBase32(int decimal)
{
    /* this array represents the symbols for our base-32 numbers */
    const char base32[32] = {'!', '@', '#', '$', '%', '^', '&', '*', '<', '>',
                             'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j',
                             'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v'};
    char *base32_seq = (char *)malloc(BASE32_SEQUENCE_LENGTH + 250);

    /* to convert from binary to base 32 we can just take the 5 right binary digits and 5 left */
    base32_seq[0] = base32[extractBits(decimal, 5, 9)];
    base32_seq[1] = base32[extractBits(decimal, 0, 4)];
    base32_seq[2] = '\0';

    return base32_seq;
}

/* decTo32: the actual algorithm for converting a decimal number to a base-32 number */
void decTo32(int dec, char key[], char *converted)
{
    if (dec / 32 == 0)
    {
        char *q;

        *converted = key[dec % 32];
        q = (char *)realloc(converted, sizeof(converted) * 2);
        if (!q)
        {
            printf("decTo32: reallocation error.\n");
            exit(0);
        }
        converted = q;
        return;
    }
    else
    {
        decTo32(dec / 32, key, converted++);
        *converted = key[dec % 32];
        return;
    }
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
        break;

    case FILE_MACRO:
        strcat(newFileName, ".am");
        break;
    }
    return newFileName;
}

char *skipWhiteChars(char *line)
{
    if (line == NULL)
        return NULL;
    while (isspace(*line))
        line++;
    return line++;
}
/* Skips line if line is blank or comment */
boolean skipLine(char *line)
{
    line = skipWhiteChars(line);
    return *line == '\0' || *line == ';' || *line == '\n';
}

boolean isLineEnd(char *line)
{
    return line == NULL || *line == '\0' || *line == '\n';
}

void copyWord(char *firstWord, char *line)
{
    sscanf(line, "%s", firstWord);
}

int findCMD(char *word, stringStruct cmd[])
{
    int wordLength = strlen(word);
    if (wordLength > MAX_COMMAND_LENGTH || wordLength < MIN_COMMAND_LENGTH)
        return NOT_FOUND;
    return findStr(word, cmd, NUM_COMMANDS);
}
int findStr(char *word, stringStruct arr[], int arrLen)
{
    int i;
    for (i = 0; i < arrLen; i++)
        if (!strcmp(word, arr[i].name))
            return i;
    return NOT_FOUND;
}

boolean isRegister(char *word)
{
    return strlen(word) == REGISTER_LENGTH && word[0] == 'r' && word[1] >= '0' && word[1] <= '7';
}

char *nextWord(char *line)
{
    if (line == NULL)
        return NULL;
    while (!isspace(*line) && !isLineEnd(line))
        line++;
    line = skipWhiteChars(line);
    if (isLineEnd(line))
        return NULL;
    return line;
}
char *nextCommaWord(char *word, char *line)
{
    if (isLineEnd(line))
    {
        word[0] = '\0';
        return NULL;
    }
    if (isspace(*line))
        line = skipWhiteChars(line);
    if (*line == ',') /* if we met a comma delimeter we copy it into word and return the next char after it */
    {
        word[0] = ',';
        word[1] = '\0';
        return ++line;
    }

    while (!isLineEnd(line) && *line != ',' && !isspace(*line))
    {
        *word = *line;
        word++;
        line++;
    }
    *word = '\0';
    return line;
}

char *nextString(char *word, char *line)
{
    char temp[MAX_LINE];
    line = nextCommaWord(word, line);
    if (*word != '"')
        return line;
    while (!isLineEnd(line) && word[strlen(word) - 1] != '"')
    {
        line = nextCommaWord(temp, line);
        if (line)
            strcat(word, temp);
    }
    return line;
}

boolean isError(int *error)
{
    return *error != OK;
}

int findDirective(char *word, stringStruct dir[])
{
    if (word == NULL || *word != '.')
        return NOT_FOUND;
    return findStr(word, dir, NUM_DIRECTIVES);
}

boolean isValidNum(char *word)
{
    if (isLineEnd(word))
        return FALSE;
    if (*word == '+' || *word == '-')
    {
        word++;
        if (!isdigit(*word++)) /* if number contains only a '+' or '-' */
            return FALSE;
    }
    while (!isLineEnd(word))
    {
        if (!isdigit(*word++))
            return FALSE;
    }
    return TRUE;
}

boolean isValidString(char *string)
{
    if (string == NULL)
        return FALSE;
    if (*string == '"')
        string++;
    else
        return FALSE;
    while (*string && *string != '"')
        string++;
    if (*string != '"')
        return FALSE;
    string++;
    if (*string != '\0')
        return FALSE;
    return TRUE;
}

unsigned int encodeARE(unsigned int info, int type)
{
    return (info << BITS_IN_ARE) | type;
}

void encodeInsturction(unsigned int word, unsigned int instructions[], int *ic)
{
    instructions[((*ic)++)] = word;
}

unsigned int extractBits(unsigned int word, int start, int end)
{
    unsigned int result;
    int len = end - start + 1;              /* bit-sequence length */
    unsigned int on = (int)pow(2, len) - 1; /* turning bits on in desired place */
    on <<= start;                           /* moving on to place */
    result = word & on;
    result >>= start; /* moving the sequence to LSB */
    return result;
}

/* This function receives line number as a parameter and prints a detailed error message
   accordingly to the error global variable */
void printError(int line_num, int error)
{
    fprintf(stderr, "ERROR (line %d): ", line_num);

    switch (error)
    {
    case SYNTAX_ERR:
        fprintf(stderr, "first non-blank character must be a letter or a dot.\n");

        break;

    case LABEL_ALREADY_EXISTS:
        fprintf(stderr, "label already exists.\n");

        break;

    case LABEL_TOO_LONG:
        fprintf(stderr, "label is too long (LABEL_MAX_LENGTH: %d).\n", LABEL_LENGTH);

        break;

    case LABEL_INVALID_FIRST_CHAR:
        fprintf(stderr, "label must start with an alphanumeric character.\n");

        break;

    case LABEL_ONLY_ALPHANUMERIC:
        fprintf(stderr, "label must only contain alphanumeric characters.\n");

        break;

    case LABEL_CANT_BE_COMMAND:
        fprintf(stderr, "label can't have the same name as a command.\n");

        break;

    case LABEL_CANT_BE_REGISTER:
        fprintf(stderr, "label can't have the same name as a register.\n");

        break;

    case LABEL_ONLY:
        fprintf(stderr, "label must be followed by a command or a directive.\n");

        break;

    case DIRECTIVE_NO_PARAMS:
        fprintf(stderr, "directive must have parameters.\n");

        break;

    case DIRECTIVE_INVALID_NUM_PARAMS:
        fprintf(stderr, "illegal number of parameters for a directive.\n");

        break;

    case DATA_COMMAS_IN_A_ROW:
        fprintf(stderr, "incorrect usage of commas in a .data directive.\n");

        break;

    case DATA_EXPECTED_NUM:
        fprintf(stderr, ".data expected a numeric parameter.\n");

        break;

    case DATA_EXPECTED_COMMA_AFTER_NUM:
        fprintf(stderr, ".data expected a comma after a numeric parameter.\n");

        break;

    case DATA_UNEXPECTED_COMMA:
        fprintf(stderr, ".data got an unexpected comma after the last number.\n");

        break;

    case STRING_TOO_MANY_OPERANDS:
        fprintf(stderr, ".string must contain exactly one parameter.\n");

        break;

    case STRING_OPERAND_NOT_VALID:
        fprintf(stderr, ".string operand is invalid.\n");

        break;

    case STRUCT_INVALID_NUM:
        fprintf(stderr, ".struct first parameter must be a number.\n");

        break;

    case STRUCT_EXPECTED_STRING:
        fprintf(stderr, ".struct must have 2 parameters.\n");

        break;

    case STRUCT_INVALID_STRING:
        fprintf(stderr, ".struct second parameter is not a string.\n");

        break;

    case STRUCT_TOO_MANY_OPERANDS:
        fprintf(stderr, ".struct must not have more than 2 operands.\n");

        break;

    case EXPECTED_COMMA_BETWEEN_OPERANDS:
        fprintf(stderr, ".struct must have 2 operands with a comma between them.\n");

        break;

    case EXTERN_NO_LABEL:
        fprintf(stderr, ".extern directive must be followed by a label.\n");

        break;

    case EXTERN_INVALID_LABEL:
        fprintf(stderr, ".extern directive received an invalid label.\n");

        break;

    case EXTERN_TOO_MANY_OPERANDS:
        fprintf(stderr, ".extern must only have one operand that is a label.\n");

        break;

    case COMMAND_NOT_FOUND:
        fprintf(stderr, "invalid command or directive.\n");

        break;

    case COMMAND_UNEXPECTED_CHAR:
        fprintf(stderr, "invalid syntax of a command.\n");

        break;

    case COMMAND_TOO_MANY_OPERANDS:
        fprintf(stderr, "extra charaters at the end, command can't have more than 2 operands.\n");

        break;

    case COMMAND_INVALID_METHOD:
        fprintf(stderr, "operand has invalid addressing method.\n");

        break;

    case COMMAND_INVALID_NUMBER_OF_OPERANDS:
        fprintf(stderr, "number of operands does not match command requirements.\n");

        break;

    case COMMAND_INVALID_OPERANDS_METHODS:
        fprintf(stderr, "operands addressing methods do not match command requirements.\n");

        break;

    case ENTRY_LABEL_DOES_NOT_EXIST:
        fprintf(stderr, ".entry directive must be followed by an existing label.\n");

        break;

    case ENTRY_CANT_BE_EXTERN:
        fprintf(stderr, ".entry can't apply to a label that was defined as external.\n");

        break;

    case COMMAND_LABEL_DOES_NOT_EXIST:
        fprintf(stderr, "label does not exist.\n");

        break;

    case STRUCT_COMMA_IN_A_ROW:
        fprintf(stderr, "multiple consecutive commas in struct.\n");

        break;

    case STRUCT_EXTRA_CHARS:
        fprintf(stderr, "extra characters at the end of struct.\n");

        break;

    case COMMAND_COMMA_IN_A_ROW:
        fprintf(stderr, "multiple consecutive commas in command\n");

        break;

    case LINE_TO_LONG:
        fprintf(stderr, "line beyond charatecter limit (80), skipping line..\n");

        break;

    case CANNOT_OPEN_FILE:
        fprintf(stderr, "there was an error while trying to open the requested file.\n");
    }
}
