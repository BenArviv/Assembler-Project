#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "external_variables.h"
#include "structures.h"
#include "utils.h"
#include "assembler.h"

#define nextWord(a, b)\
        a = strtok(b, ' ');\
        b += strlen(a);

#define DATA ".data"
#define STRING ".string"
#define STRUCT ".struct"

// minimal symbol length (2)
// skip white space function
boolean skipLine(char *);
boolean isSymbol(char *);
boolean isData(char *);
void addToSymbolsTable(char *, int, int, Symbol_ptr);
void printError(int);
void updateDataTable(char *);

void firstPass(FILE *fp, Symbol_ptr head){
    ic = 0;
    dc = 0;
    int decAddress = 100, index;
    char *line, *word;
    boolean symbol = FALSE;
    
    insArr = (char *)malloc(sizeof(char));
    dataArr = (char *)malloc(sizeof(char));
    line = (char *)malloc(sizeof(char) * MAX);

    while (fgets(line, MAX, fp)){
        symbol = FALSE;
        if (skipLine(line)){
            decAddress++;
            continue;
        }
        nextWord(word, line)
        if (isSymbol(word)){
            symbol = TRUE;
            nextWord(word, line)
        }
        if (isData(word)){
            if (symbol){
                addToSymbolsTable(word, dc++, DATA, head);
                updateDataTable(line);
            }
        }
    }

    if (error)
        return;
}

void updateDataTable(char *line){
    int i = 0;
    char *temp;

    while (*(line + i) != EOF){
        dataArr[i] = *(line + i);
        if (i = 1 + sizeof(dataArr)){
            temp = (char *)realloc(dataArr, 2 * sizeof(dataArr));
            if (!temp){
                error = MEMORY_ALLOCATION_ERROR;
                printError(error);
                exit(error);
            }
            dataArr
        }
    }
}

/* isData: checks if it is a data storage label */
boolean isData(char *word){
    boolean data = FALSE;

    if (!(strcmp(word, DATA) && strcmp(word, STRING) && strcmp(word, STRUCT)))
        data = TRUE;
    
    return data;
}

/* addToSymbolsTable: gets a name, an adderss and a sign, and adds a new Symbol node
                    to the linked-list if there isn't exists a Symbol with similar name */
void addToSymbolsTable(char *word, int address, int sign, Symbol_ptr node){
    Symbol_ptr new = (Symbol_ptr)malloc(sizeof(Symbol));

    /* checks for existing label */
    while (node -> next != NULL){
        if (!strcmp(node -> sname, word)){
            error = LABAL_ALREADY_EXISTS;
            printError(error);
            return;
        }
    }

    /* creates a new Symbol node */
    strcpy(new -> sname, word);
    new -> address = address;
    new -> ssign = sign;

    /* adds the new Symbol node to the end of the symbols' linked-list */
    node -> next = new;
}

boolean skipLine(char *line){
    int i = 0;
    boolean skip = TRUE;
    
    while (line[i] != EOF){
        if (!isspace(line[i])){
            skip = FALSE;
            break;
        }
        i++;
    }

    return skip;
}

boolean isSymbol(char *line){
    char word[MAXS];
    int i = 0; 
    boolean symbol = TRUE;

    memset(word, '\0', MAX);

    while (isspace(line[i++])); /* skips white spaces */
    if (!isalpha(line[i++])){ /* checks validity of first letter */
        symbol = FALSE;
        return 0;
    }
    while (!isspace(line[i])){
        if (!isalnum(line[i])){
            if (line[i] == ':' && (isspace(line[i + 1]) || line[i + 1] == EOF))
                break;
            else {
                symbol = FALSE;
                break;
            }
        }
        i++;
    }

    return symbol;
}
