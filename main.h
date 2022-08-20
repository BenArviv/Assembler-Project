/*********************** AUTHORS **************************
 * GAL ISRAEL
 * BEN ARVIV
 **************************************************/

/*
    This file contains constants and enums that are used all over the program.
*/

#ifndef MAIN_H

#define MAIN_H

/**************************************** Defaults ****************************************/

#define MEMORY_START 100
#define EMPTY_WORD 0
#define EXTERNAL_DEFAULT_ADDRESS 0
#define NOT_FOUND -1
#define OK 0
#define ERROR 1

/**************************************** STYLES ****************************************/

#define BOLDEND "\x1B[0m"
#define BOLD "\x1B[1m" /*Bold Text Formula*/

#define RED "\033[31m"
#define GREEN "\033[32m"
#define BLUE "\033[34m"
#define RESET_COLOR "\033[m"

/**************************************** Limitations ****************************************/

#define MAX_LINE 90   /* beyond max line to check for 80 char limit */
#define CHAR_LIMIT 82 /* char limit per maman's rules */
#define MINIMUM_LABEL_LENGTH_WITH_COLON 2
#define MINIMUM_LABEL_LENGTH_WITHOUT_COLON 1
#define LABEL_LENGTH 30 /* maximum chars per label */

#define MAX_COMMAND_LENGTH 4 /* maximum number of characters in a command */
#define MIN_COMMAND_LENGTH 3 /* minimum number of characters in a command */

#define REGISTER_LENGTH 2 /* a register's name contains 2 characters */

#define MAX_EXTENSION_LEN 5

#define BASE32_SEQUENCE_LENGTH 3 /* A base32 sequence of a word consists of 2 digits (and '\0' ending) */

/**************************************** Other Constants ****************************************/

#define NUM_DIRECTIVES 5 /* number of existing directives*/
#define NUM_COMMANDS 16  /* number of existing commands */

/* Bit-related info */
#define BITS_IN_METHOD 2
#define BITS_IN_ARE 2
#define BITS_IN_REGISTER 4

/* Addressing methods bits location in the first word of a command */
#define SRC_METHOD_START_POS 4
#define SRC_METHOD_END_POS 5
#define DEST_METHOD_START_POS 2
#define DEST_METHOD_END_POS 3

/* as defined in forums, we can ignore that number and set it to a big number to avoid seg-faults
 *  could've implement with malloc but we decided to go with static array since it's allowed
 */
#define MACHINE_RAM 2560

/**************************************** Macros ****************************************/

/* Vars struct variable refactor */
#define refactor(a)                         \
    a = (extVars *)malloc(sizeof(extVars)); \
    a->ic = 0;                              \
    a->dc = 0;                              \
    a->externFlag = FALSE;                  \
    a->entryFlag = FALSE;                   \
    a->recordedError = FALSE;               \
    a->symbolsTable = NULL;                 \
    a->cmd = cmdInit;                       \
    a->dir = dataInit;                      \
    a->externList = NULL;

/**************************************** Enums ****************************************/

/* Directives types */
enum directives
{
    DATA,
    STRING,
    STRUCT,
    ENTRY,
    EXTERN,
    UNKNOWN_DIR
};

/* Enum of commands ordered by their opcode */
enum commands
{
    MOV,
    CMP,
    ADD,
    SUB,
    NOT,
    CLR,
    LEA,
    INC,
    DEC,
    JMP,
    BNE,
    GET,
    PRN,
    JSR,
    RTS,
    HLT,
    UNKNOWN_COMMAND
};

/* Enum of all errors that can be outputted */
enum errors
{
    SYNTAX_ERR = 1,
    LABEL_ALREADY_EXISTS,
    LABEL_TOO_LONG,
    LABEL_INVALID_FIRST_CHAR,
    LABEL_ONLY_ALPHANUMERIC,
    LABEL_CANT_BE_COMMAND,
    LABEL_ONLY,
    LABEL_CANT_BE_REGISTER,
    DIRECTIVE_NO_PARAMS,
    DIRECTIVE_INVALID_NUM_PARAMS,
    DATA_COMMAS_IN_A_ROW,
    DATA_EXPECTED_NUM,
    DATA_EXPECTED_COMMA_AFTER_NUM,
    DATA_UNEXPECTED_COMMA,
    STRING_TOO_MANY_OPERANDS,
    STRING_OPERAND_NOT_VALID,
    STRUCT_EXPECTED_STRING,
    STRUCT_COMMA_IN_A_ROW,
    STRUCT_EXTRA_CHARS,
    STRUCT_INVALID_STRING,
    EXPECTED_COMMA_BETWEEN_OPERANDS,
    STRUCT_MISSING_STR,
    STRUCT_NO_OPERANDS,
    STRUCT_INVALID_NUM,
    STRUCT_TOO_MANY_OPERANDS,
    EXTERN_NO_LABEL,
    EXTERN_INVALID_LABEL,
    EXTERN_TOO_MANY_OPERANDS,
    COMMAND_NOT_FOUND,
    COMMAND_UNEXPECTED_CHAR,
    COMMAND_TOO_MANY_OPERANDS,
    COMMAND_COMMA_IN_A_ROW,
    COMMAND_INVALID_METHOD,
    COMMAND_INVALID_NUMBER_OF_OPERANDS,
    COMMAND_INVALID_OPERANDS_METHODS,
    ENTRY_LABEL_DOES_NOT_EXIST,
    ENTRY_CANT_BE_EXTERN,
    COMMAND_LABEL_DOES_NOT_EXIST,
    CANNOT_OPEN_FILE,
    LINE_TO_LONG,
    ILLEGAL_MACRO
};

/* When we need to specify if label should contain a colon or not */
enum
{
    NO_COLON,
    COLON
};

/* Addressing methods ordered by their code */
enum methods
{
    METHOD_IMMEDIATE,
    METHOD_DIRECT,
    METHOD_STRUCT,
    METHOD_REGISTER,
    METHOD_UNKNOWN
};

/* A/R/E modes ordered by their numerical value */
enum ARE
{
    ABSOLUTE,
    EXTERNAL,
    RELOCATABLE
};

/* Types of files that indicate what is the desirable file extension */
enum filetypes
{
    FILE_INPUT,
    FILE_OBJECT,
    FILE_ENTRY,
    FILE_EXTERN,
    FILE_MACRO
};

#endif
