#ifndef STRUCTURES.H

#define STRUCTURES.H

#define MAX 100 /* max line length */
#define MAXS 31 /* max symbol length */

/* Symbol: linked-list node that contains a symbol name and its address */
typedef struct symbol_s {
    char sname[MAXS];
    int address;
    symbolSign ssign;
    Symbol *next;
} Symbol;

typedef struct symbol_s *Symbol_ptr; /* a pointer to Symbol's node */

typedef enum {NONE, DATA, CODE} symbolSign;

#endif
