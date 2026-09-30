#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pre_assembler.h"
#include "symbols.h"
#include "first_pass.h"
#include "second_pass.h"

Symbol *symbol_table = NULL;
int symbol_count = 0;


/**
* main func that truns the whole assembler on each file.
*
* @param    argc The number of command line arguments
* @param    argv array of file names we want to assemble
* @return   0 when done
*/
int main (int argc, char *argv[]) {
    int i;
    FILE *fp_am;
    char file_name_am[MAX_FILE_NAME];
    
    /* check if files were provided */
    if (argc < 2) {
      printf("Missing file names\n");
      return 1;
    }
    
    /* loop through all given files */
    for (i = 1; i < argc; i++) {
    
      /*reset symbol counter for current file */
      symbol_count = 0;
      
      /* stage 1: expand macros */
      do_pre_assembler(argv[i]);
      
      /* prep .am filename */
      strcpy(file_name_am, argv[i]);
      strcat(file_name_am, ".am");
        
      fp_am = fopen(file_name_am, "r");
      if (fp_am == NULL)
        printf("Can't open file %s\n", file_name_am);
       
      printf("Launching first pass for %s......\n", argv[i]);
      /*stage 2: first pass- if failed, skip to next file */
      if(do_first_pass(fp_am) == 1) {
        printf("Error found in first pass. Skipping second pass for %s\n", argv[i]);
        fclose(fp_am);
        continue; /*move to next file instead of crashing*/
      }
        
      /* back to start of file */
      rewind(fp_am);
        
      /* stage 3: second pass and output */
      do_second_pass(fp_am, argv[i]);
      
      fclose(fp_am);
      
      /* free memory before next file */
      free(symbol_table);
      symbol_table = NULL;
    }
    return 0;
  }

