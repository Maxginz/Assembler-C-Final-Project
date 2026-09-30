#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "symbols.h"
#define MAX_LENGTH 82 
#define MAX_CODE_IMAGE 1000
#define MAX_EXT_REC 100
#define MAX_FILE_NAME 256


extern Symbol *symbol_table;
extern int symbol_count;
extern unsigned char data_image[];
extern int final_dc;

unsigned int code_image[MAX_CODE_IMAGE];

/* ext struct */
typedef struct {
  char name[MAX_LENGTH];
  int address;
} ExtRecord;

ExtRecord *ext_records = NULL;
int ext_count = 0;

/* opcodes table */
Instruction opcodes_table[] = {
  {"add", 0, 1, 'R'},
  {"sub", 0, 2, 'R'},
  {"and", 0, 3, 'R'},
  {"or", 0, 4, 'R'},
  {"nor", 0, 5, 'R'},
  {"move", 1, 1, 'R'},
  {"mvhi", 1, 2, 'R'},
  {"mvlo", 1, 3, 'R'},
  
  {"addi", 10, 0, 'I'},
  {"subi", 11, 0, 'I'},
  {"andi", 12, 0, 'I'},
  {"ori", 13, 0, 'I'},
  {"nori", 14, 0, 'I'},
  {"bne", 15, 0, 'I'},
  {"beq", 16, 0, 'I'},
  {"blt", 17, 0, 'I'},
  {"bgt", 18, 0, 'I'},
  {"lb", 19, 0, 'I'},
  {"sb", 20, 0, 'I'},
  {"lw", 21, 0, 'I'},
  {"sw", 22, 0, 'I'},
  {"lh", 23, 0, 'I'},
  {"sh", 24, 0, 'I'},
  
  {"jmp", 30, 0, 'J'},
  {"la", 31, 0, 'J'},
  {"call", 32, 0, 'J'},
  {"hlt", 63, 0, 'J'}
};  

/* find a symbol address by its name */
int get_symbol_address(char *name) {
  int k;
  for( k = 0; k < symbol_count; k++) {
    if(strcmp(name, symbol_table[k].symbol) == 0) {
      return symbol_table[k].value;
    }
  }
  return 0;
}

/* check if external */
int is_symbol_external(char *name) {
  int k;
  for (k = 0; k < symbol_count; k++) {
    if(strcmp(name, symbol_table[k].symbol) == 0) {
      if(strstr(symbol_table[k].attributes, "external") != NULL) 
        return 1;
    }
  }
  return 0;
}

/**
* Runs second pass to finish the binary encoding and to generate ouytput files.
*
* @param    fp_am File pointer to .am file.
* @param    filename THe original filename used to create the .ob .ent and .ext files.
*/
void do_second_pass(FILE *fp_am, char *filename) {
  char line[MAX_LENGTH];
  char first_word[MAX_LENGTH];
  char second_word[MAX_LENGTH];
  int ic = 100;
  int line_num = 0;
  int error_found = 0;
  int i;
  int op_index;
  int num_opcodes = sizeof(opcodes_table) / sizeof(opcodes_table[0]);
  
  FILE *fp_ob, *fp_ent, *fp_ext;
  char ob_name[MAX_FILE_NAME], ent_name[MAX_FILE_NAME], ext_name[MAX_FILE_NAME];
  int addr, has_entry = 0;
  
  ext_count = 0;
  ext_records = NULL;
  
  printf("Starting second pass.....\n");

  /* Read file loop */
  while(fgets(line, MAX_LENGTH, fp_am)) {
  
    char *op_name;
    char *operands;
    int length;
    
    /* track line num */
    line_num++;

    if(strlen(line) == MAX_LENGTH - 1 && line[MAX_LENGTH - 2] != '\n') {
      printf("Error in line %d: exceeds maximum length of 80 characters.\n", line_num);
      error_found = 1;
    
     
      while (fgetc(fp_am) != '\n' && !feof(fp_am)){
        /*clear the rest of the line */
      }
      continue;
    }  
    strcpy(ob_name, filename);
    
    
    
    /* CLean the word buffers to prevent garbage from previous iterationss */
    first_word[0] = '\0';
    second_word[0]= '\0';
    
    
    sscanf(line, "%s %s", first_word, second_word);
    length = strlen(first_word);
    
    /* Skip empty lines */
    if (length==0)
      continue;
    
    /* skil label */
    if(first_word[length -1] == ':') 
      op_name = second_word;
    else
      op_name = first_word;
      
    operands = strstr(line, op_name) + strlen(op_name);
      
    /* ignore data here */ 
    if (strcmp(op_name, ".db")== 0 || strcmp(op_name, ".dw") == 0 || strcmp(op_name, ".dh") == 0 || strcmp(op_name, ".asciz") == 0 || strcmp(op_name, ".extern") == 0) 
      continue;
    
    /* update entry flag */
    else if (strcmp(op_name, ".entry") == 0) {
      for (i = 0; i < symbol_count; i++){
       if (strcmp(second_word, symbol_table[i].symbol) == 0) {
        strcat(symbol_table[i].attributes, ", entry");
        break;
       }
      } 
      continue;
     } 
    
     /* find op */
     op_index = -1;
     for(i = 0; i< num_opcodes; i++) {
      if (opcodes_table[i].name != NULL&& strcmp(op_name, opcodes_table[i].name)==0) {
        op_index = i;
          break;
      }
    }
      
    if(op_index == -1) {
      if(strlen(op_name) > 0) {
        printf("Error: Uknown command '%s'\n", op_name);
        error_found = 1;
      }
    }  
    else {
      
      unsigned int machine_code;
        
      if(opcodes_table[op_index].type == 'R') {
        int rs = 0, rt = 0, rd = 0;
          
        /* build R type */
        if (opcodes_table[op_index].opcode ==1) 
            sscanf(operands, " $%d, $%d", &rd, &rs);
        else
          sscanf(operands, " $%d, $%d, $%d", &rs, &rt, &rd);
          
        machine_code = (opcodes_table[op_index].opcode << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (opcodes_table[op_index].funct << 6);
          
        printf("Machine code for %s: %08X\n", op_name, machine_code);
        }
          
        /* build I type */
        else if (opcodes_table[op_index].type == 'I') {
          int rs = 0, rt = 0, immed = 0;
          char label[MAX_LENGTH];
            
          if(opcodes_table[op_index].opcode>= 15 && opcodes_table[op_index].opcode <= 18) {
            sscanf(operands, " $%d, $%d, %s", &rs, &rt, label);
            immed = get_symbol_address(label) - ic;
          }
          else
            sscanf(operands, "$%d, %d, $%d", &rs, &immed, &rt);
            
          machine_code = (opcodes_table[op_index].opcode << 26) | (rs << 21) | (rt << 16) | (immed & 0xFFFF);
          printf("Machine code for %s: %08X\n", op_name, machine_code);
          }
          
          /* build J type */
          else if(opcodes_table[op_index].type == 'J') {
            int reg_flag = 0;
            int address = 0;
            char target[MAX_LENGTH];
            
            if(opcodes_table[op_index].opcode!=63) {
              sscanf(operands, "%s", target);
              if(target[0] == '$') {
                reg_flag = 1;
                sscanf(target, "$%d", &address);
              }
            
            
              else {
                reg_flag= 0;
                address = get_symbol_address(target);    
                
                /* save ext */
                if (is_symbol_external(target)) {
                  ext_records = realloc (ext_records, (ext_count + 1) * sizeof(ExtRecord));
                  if(ext_records==NULL) {
                    printf("Memory allocation failed\n");
                    exit(1);
                  }
                  strcpy(ext_records[ext_count].name, target);
                  ext_records[ext_count].address = ic;
                  ext_count++;
               }
             }
          }
          machine_code = (opcodes_table[op_index].opcode << 26) | (reg_flag << 25) | (address & 0x1FFFFFF);
              
            
          code_image[(ic - 100) / 4] = machine_code;
          ic += 4;
    }
  }
}
    if(error_found == 1) {
      printf("Error found in the file. Output files (.ob .ent .ext) will not be generated\n");
      return; /* stops the function here so files aren't created */
    }
    
    /* Setup file names */
    strcpy(ob_name, filename);
    strcat(ob_name, ".ob");
    strcpy(ent_name, filename);
    strcat(ent_name, ".ent");
    strcpy(ext_name, filename);
    strcat(ext_name, ".ext");
    
    /* Output object file (header, code, data) */
    fp_ob = fopen(ob_name, "w");
    if(fp_ob!=NULL) {
      fprintf(fp_ob, "  %d %d\n", ic - 100, final_dc);
      
    for(addr = 100; addr < ic; addr +=4) {
    unsigned int inst = code_image[(addr - 100) / 4];
    fprintf(fp_ob, "%04d %02X %02X %02X %02X\n", addr, inst & 0xFF, (inst >> 8) & 0xFF, (inst >> 16) & 0xFF, (inst >> 24) & 0xFF);
      }
      for (i = 0; i <final_dc; i+=4) {
        unsigned char d1 = 0, d2 = 0, d3 = 0;
        if(i + 1 < final_dc)
          d1 = data_image[i+1];
         if(i + 2 < final_dc)
          d2 = data_image[i+2];
        if(i+ 3 < final_dc)
          d3 = data_image[i+3];
          
        fprintf(fp_ob, "%04d %02X %02X %02X %02X\n", ic + i, data_image[i], d1, d2, d3);
      }
        
    
    fclose(fp_ob);
    }
    
    /* dump externals to file */
    for(i = 0; i < symbol_count; i++) {
      if(strstr(symbol_table[i].attributes, "entry") != NULL) {
        if(has_entry == 0) {
          fp_ent = fopen(ent_name, "w");
          has_entry = 1;
          }
          fprintf(fp_ent, "%s %04d\n", symbol_table[i].symbol, symbol_table[i].value);
        }
      }
    
    if (has_entry)
      fclose(fp_ent);
    
    /* Write the externals file (.ext) using the external records saved during the pass */
    if(ext_count > 0) {
      fp_ext = fopen(ext_name, "w");
      for( i = 0; i< ext_count; i++) {
        fprintf(fp_ext, "%s %04d\n", ext_records[i].name, ext_records[i].address);
      }
      fclose(fp_ext);
      /* Free dynamic allocation */
      free(ext_records);
      ext_records = NULL;
      
    
    
       printf("Output worked successfully!\n");
    }
  }


