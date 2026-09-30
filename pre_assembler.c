#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pre_assembler.h"
#define MAX_FILE_NAME 256

/**
* scans the source file for macros, expands them and recreates a clean .am file.
*
@ @param fp The base name of the input file.
*/
void do_pre_assembler(char *fp) {

    char first_word[MAX_LENGTH];
    char line[MAX_LENGTH];
    char macro_name[MAX_LENGTH];
    int j;
    int k;
    int macro = 0;
    int curr_line = 0;
    int is_macro_call = 0;
    
    Macro *macro_arr = NULL;
    int macro_count = 0;

    FILE *fp_as;
    FILE *fp_am;
    char file_name_as[MAX_FILE_NAME];
    char file_name_am[MAX_FILE_NAME];
    
    /* create .as and .am file names */
    strcpy(file_name_as, fp);
    strcat(file_name_as, ".as");
    
    strcpy(file_name_am, fp);
    strcat(file_name_am, ".am");
    
    fp_as = fopen (file_name_as, "r");
    fp_am = fopen (file_name_am, "w");
        
    if (fp_as == NULL) {
        printf("Can't open file.\n");
        return;
        }
            
    else 
      printf("Successfully opened file: %s\n", fp); 

        
    
    /* read file loop */  
    while (fgets(line, MAX_LENGTH, fp_as)) {
          sscanf(line, "%s %s", first_word, macro_name);
          
          /* start of macro */
        if (strcmp(first_word, "mcro")==0) {
            macro = 1;
            curr_line = 0;
            macro_arr = realloc (macro_arr, (macro_count + 1) *sizeof(Macro));
              if(!macro_arr) {
                printf("Failed allocating memory\n");
                exit(1);
              }
                strcpy(macro_arr[macro_count].name, macro_name);    
              
          }    
          
          /* end of macro */
          else if (strcmp(first_word, "mcroend")== 0) {
              macro = 0;             
              macro_arr[macro_count].total_lines = curr_line;
              macro_count++;
          }   
          else {
              
              /* inside macro */
              if ((macro == 1) && strcmp(first_word, "mcroend")!=0) {
                  strcpy(macro_arr[macro_count].lines[curr_line], line);
                  curr_line++;
              }
              else {
                  is_macro_call = 0;
                  /* expand macro */
                  for (j = 0; j < macro_count; j++) {
                      if (strcmp(first_word, macro_arr[j].name) == 0) {
                          is_macro_call = 1;

                          for (k = 0; k < macro_arr[j].total_lines; k++) {
                              fputs(macro_arr[j].lines[k], fp_am);
                          }
                         
                      }    
                  }
                /* Not a macro, just write the original line to the .am file */  
                if (!is_macro_call)
                fputs(line, fp_am);            
                
            }
        
   }
    
}   
    /* Clean up and close the files */
    fclose(fp_as);
    fclose(fp_am);
    
    free(macro_arr);
}

