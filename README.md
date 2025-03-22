# Assembler in C

This project implements an assembler in C for a simplified assembly language, as part of the Maman 14 assignment.

## 🛠 Features

- First pass: builds symbol table and identifies labels, directives, and instructions
- Second pass: resolves symbols and generates machine code
- Supports directives such as `.data`, `.string`, `.extern`, `.entry`
- Detects syntax errors and prints appropriate error messages

## 📁 Structure

- `main.c` – Entry point of the program
- `first_pass.c` – Logic for the first pass over the code
- `second_pass.c` – Handles label resolution and code generation
- `symbol_table.c` – Manages the symbol table
- `parser.c` – Parses assembly lines and instructions

## 🚀 Usage

1. Compile the project:
   ```bash
   gcc -Wall -g -o assembler *.c
