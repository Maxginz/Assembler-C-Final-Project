assembler: assembler.o pre_assembler.o first_pass.o second_pass.o
	gcc -ansi -Wall -pedantic assembler.o pre_assembler.o first_pass.o second_pass.o -o assembler

assembler.o: assembler.c pre_assembler.h first_pass.h symbols.h
	gcc -c -ansi -Wall -pedantic assembler.c -o assembler.o

pre_assembler.o: pre_assembler.c pre_assembler.h symbols.h
	gcc -c -ansi -Wall -pedantic pre_assembler.c -o pre_assembler.o
	
first_pass.o: first_pass.c symbols.h
	gcc -c -ansi -Wall -pedantic first_pass.c -o first_pass.o

second_pass.o: second_pass.c symbols.h
	gcc -c -ansi -Wall -pedantic second_pass.c -o second_pass.o
