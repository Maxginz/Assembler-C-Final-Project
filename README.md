# C Assembler Project

This is my final project for the "Systems Programming Lab" (20465) course at the Open University of Israel.
Basically, it's a custom Two-Pass Assembler written in C that takes assembly files (`.as`) and translates them into machine code (`.ob`), along with entry and extern files if needed.

## What it does:
* **Macro expansion:** The first thing it does is find any macros in the code, saves them, and expands them into a clean `.am` file.
* **Two Passes:** 
  * *Pass 1:* Goes over the code to check for syntax errors, builds the symbol table, and calculates the memory addresses (IC and DC).
  * *Pass 2:* Encodes the instructions into hex machine code.
* **Dynamic Memory:** I used `realloc` and `malloc` for the data structures (like the symbol table and external records) instead of static arrays, so it can scale without hardcoded limits.
* **Error handling:** If you have a typo, missing arguments, or a line that's too long in your assembly code, the program will print exactly which line is wrong and skip creating the output files so it doesn't create garbage data.

## How to build and run:
Everything is configured for a Linux environment. I included a Makefile to make things easy. 

Just run this in your terminal to compile (it uses strict flags like `-Wall -ansi -pedantic`):
```bash
make
