#ifndef SYMBOLS_H
#define SYMBOLS_H

#define MAX_SYMBOLS 100
#define MAX_LENGTH 82

/* Represents a label in the symbol table */
typedef struct {
  char symbol[MAX_SYMBOLS];
  char attributes[MAX_SYMBOLS];
  int value;
} Symbol;

/* Holds the pring for a valid assembly command (name, opcode, funct and format type) */
typedef struct {
  char *name;
  int opcode;
  int funct;
  char type;
} Instruction;

#endif 
