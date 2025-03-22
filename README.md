# Assembler in C

This project is a full implementation of a two-pass assembler written in C, developed for a simplified assembly language as part of an academic systems programming assignment.

The assembler reads `.as` source files and generates output files (`.obj`, `.ent`, `.ext`) using the classic two-pass approach:
- **First pass** builds the symbol table and identifies instructions and directives
- **Second pass** resolves symbols and generates final machine code

---

## 📁 File Structure

| File                        | Description                                 |
|-----------------------------|---------------------------------------------|
| `main.c`                   | Entry point of the assembler program        |
| `asembler.h`               | Shared definitions, structs, constants      |
| `first_read_mcro.c`        | Reads and expands macros before assembly    |
| `first_loop.c`             | Implements the first pass                   |
| `function_for_loop1.c`     | Helper functions for parsing and validation |
| `second_loop.c`            | Implements the second pass and code generation |
| `function_for_make_files.c`| Generates `.obj`, `.ent`, `.ext` output files|
| `makefile`                 | Compiles the project using GCC              |

---

## 🔧 Features

- ✅ Full macro support (with macro expansion before the first pass)
- ✅ Support for `.data`, `.string`, `.extern`, `.entry`
- ✅ Full symbol table implementation
- ✅ Validates syntax, symbol usage, operand addressing
- ✅ Supports 16 predefined commands
- ✅ Generates object, entry, and extern files
- ✅ Modular, readable C code

---

## 🚀 How to Compile and Run

1. **Compile with make:**
```bash
make
```

2. **Run the assembler on a source file:**
```bash
./assembler program.as
```

3. **Output files created:**
- `program.obj` – machine code (object file)
- `program.ent` – list of entry symbols
- `program.ext` – list of external symbol references

4. **Clean build files (optional):**
```bash
make clean
```

---

## 🧺as Example Input

```asm
MAIN:   mov  #5, r1
        add  r1, r2
        .entry MAIN
```

## 📄 Output

- `example.obj` – assembled machine code
- `example.ent` – includes label MAIN with its address

---

## 🧠 Educational Purpose

This project was written as part of a university-level systems programming course. It demonstrates core concepts such as:
- Low-level parsing and lexical analysis
- Memory management and pointer usage in C
- Assembly language translation
- Multi-file modular program structure

---

## 👤 Author

Ido  
GitHub: @Ido11118
