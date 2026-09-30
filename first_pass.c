#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbols.h"
#define MAX_DATA_IMAGE 4000
#define MAX_LENGTH 82 
#define NUM_OPCODES 27

extern Symbol *symbol_table;
extern int symbol_count;

unsigned char data_image[MAX_DATA_IMAGE];
int final_dc = 0;
int final_ic = 100;
int i;


/**
* Extract string chars between quotes and add them to the data image
* 
* @param    operands The string containing the text.
* @param     dc Pointer to the data counter to update
*/
void process_string(char *operands, int *dc) {
  char *start = strchr(operands, '\"');
  if(start != NULL) {
    start++;
    while(*start != '\"' && *start != '\0') {
      data_image[(*dc)++] = *start;
      start++;
    }
    /* null terminator */
    data_image[(*dc)++] = 0;
  }
}

/**
* Parses numeric data directives (.db, .dh, .dw) and stores the values in data image.
*
* @param  op_name The type of data directive.
* @param  operands The string containing the numbers to parse.
* @param  dc Pointer to data counter to update after inserting.
*/  

void process_data(char *op_name, char *operands, int *dc) {
  char *token = strtok(operands, " ,\t\n\r");
  while(token != NULL) {
    int val = atoi(token);
    if(strcmp(op_name, ".db") == 0) 
      data_image[(*dc)++] = val & 0xFF;
      
    else if(strcmp(op_name, ".dh") == 0) {
      data_image[(*dc)++] = val & 0xFF;
      data_image[(*dc)++] = (val >> 8) & 0xFF;
    }
    else if (strcmp(op_name, ".dw") ==0) {
      data_image[(*dc)++] = val & 0xFF;
      data_image[(*dc)++] = (val >> 8) & 0xFF;
      data_image[(*dc)++] =  (val >> 16) & 0xFF;
      data_image[(*dc)++] = (val >> 24) & 0xFF;
    }
    token = strtok(NULL, ", \t\n\r");
    }
}

/**
* CHecks if a given string matches any of the allowed assembly commands.
*
* @param    op The command string to check.
* @return   1 if valid, 0 if not recognized.
*/
int is_valid_opcode(char *op) {
  char *valid_ops[] = {"add", "sub", "and", "or",
                      "nor", "move", "mvhi", "mvlo",
                      "addi", "subi", "andi", "ori",
                      "nori", "bne", "beq", "blt",
                      "bgt", "lb", "lw", "sw",
                      "lh", "sh", "jmp", "la",
                      "call", "hlt"};

for(i = 0; i < NUM_OPCODES; i++) {
  if(strcmp(op, valid_ops[i]) == 0)
    return 1;
}
  return 0;
}
                      
/**
* Performs the first pass on .am file to map symbols and then calculate memory address.
*
* @param    fp_am File pointer to the macro expanded scource file.
* @return   1 if syntax error was found, and 0 if the pass was successful.
*/
int do_first_pass(FILE *fp_am) {
  char line[MAX_LENGTH];
  char first_word[MAX_LENGTH], second_word[MAX_LENGTH];
  
  /* init counters */
  int ic = 100;
  int dc = 0;
  int j;
  int line_num = 0;
  int error_found = 0;
  

while(fgets(line, MAX_LENGTH, fp_am)) {
  int is_label = 0;
  char symbol_name[MAX_LENGTH];
  char *op_name;
  char *operands;
  int length;
  
  line_num++;
  
  if(strlen(line) == MAX_LENGTH - 1 && line[MAX_LENGTH - 2] != '\n') {
    printf("Error in line %d: exceeds maximum length of 80 characters.\n", line_num);
    error_found = 1;
    
    while(fgetc(fp_am)!= '\n' && !feof(fp_am)) {
    /* clear the rest of the line */
    }
    continue;
  }
    
  /* reset buffers */
  first_word[0] = '\0';
  second_word[0] = '\0';

  sscanf(line, "%s %s", first_word, second_word);

  length = strlen(first_word);
  
  if(length == 0)
    continue;
  
  /* label check */
  if (first_word[length - 1] == ':') {
    is_label = 1;
    first_word[length - 1] = '\0';

    strcpy(symbol_name, first_word);
    op_name = second_word;
  }

  else 
    op_name = first_word;
  
  operands = strstr(line, op_name) + strlen(op_name);
  
  /* data directives */
  if (strcmp(op_name, ".db") == 0 || strcmp(op_name, ".dw") == 0 ||
      strcmp(op_name, ".dh") == 0 || strcmp(op_name, ".asciz")== 0) {
      
    /* add data label */
    if (is_label == 1) {
      symbol_table = realloc(symbol_table, (symbol_count + 1) * sizeof(Symbol));
      if(symbol_table==NULL) {
        printf("Couldn't allocate memory\n");
        exit(1);
      }
      symbol_table[symbol_count].value = dc;
      strcpy(symbol_table[symbol_count].attributes, "data");
      symbol_count++;
    }
    
    if(strcmp(op_name, ".asciz") == 0) 
      process_string(operands, &dc);
    else
      process_data(op_name, operands, &dc);
  
    continue;
}   
  /* externals */
  if(strcmp(op_name, ".extern")== 0) {
    symbol_table = realloc(symbol_table, (symbol_count + 1) * sizeof(Symbol));
     if(symbol_table == NULL) {
      printf("Couldn't allocate memory\n");
      exit(1);
    }
    strcpy(symbol_table[symbol_count].symbol, second_word);
    symbol_table[symbol_count].value = 0;
    strcpy(symbol_table[symbol_count].attributes, "external");
    symbol_count++;
    continue;
    }
  
  /* skip entry */
  else if( strcmp(op_name, ".entry") ==0)
      continue;
  
  /* validate and count */
  else {
    if(!is_valid_opcode(op_name)){
      printf("Error in line %d: Unknown command '%s'\n", line_num, op_name);
      error_found = 1;
      continue;
    }
    if(is_label == 1) {
      symbol_table = realloc(symbol_table, (symbol_count + 1) * sizeof(Symbol));
      if(symbol_table == NULL) {
        printf("COuldn't allocate memory\n");
        exit(1);
      }
      strcpy(symbol_table[symbol_count].symbol, symbol_name);
      symbol_table[symbol_count].value = ic;
      strcpy(symbol_table[symbol_count].attributes, "code");
      symbol_count++;
   }
  ic+=4;
  }
}
    
    
    final_dc = dc;
    final_ic = ic;
    
    /* update data symbols */
    for (j = 0; j< symbol_count; j++) {
       if (strcmp(symbol_table[j].attributes, "data") == 0) 
       symbol_table[j].value += ic;
       
  }
  return error_found; /* returning error */
}





 
