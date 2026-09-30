#define MAX_MACRO_COUNT 10
#define MAX_LENGTH 82
#define MAX_MACRO_LINES 20
#define MAX_FILE_NAME 256
#include "symbols.h"

/* Holds macro's name, saves lines of code, and total line count */
typedef struct {
    char name[MAX_LENGTH];
    char lines[MAX_MACRO_LINES ][MAX_LENGTH];
    int total_lines;
} Macro;

/**
* scans the source file for macros, expands them and recreates a clean .am file.
*
@ @param fp The base name of the input file.
*/
void do_pre_assembler(char *fp);
