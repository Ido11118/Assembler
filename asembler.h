/*
File name: asembler.h
program: custom assembler in C.
Include file: header for assembler program, includ defines, structs and statement of functions. 
*/


#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <stdint.h>


#define MAX_MACRO_SIZE 100 /* Assumption regarding maximum size of a macro */
#define MAX_CHAR_MACRO_NAME 31  /* max charters for macro name */
#define MAX_FILE_NAME_LENGTH 20 /* max charters for file name */
#define START_ADRESS 100     /* start adress in the imagen computer (define in the maman) */
#define MAX_ARRY_SYZE 100    /* max word in arry (IC and DC tables) */
#define MAX_SYMBOL_NAME 31   /* max charters for symbol */
#define MAX_COMMAND_NAME 20  /* max charters for command or guidnace */
#define MAX_LINE_LENGTH 81   /* max charters at line, includ '\n' */
#define MAX_OPERAND_LENGTH 15 /* max charters for operand */
#define SYMBOL_DATA "data"     /* data type for symbols */
#define SYMBOL_STRING "string"  /* string type for symbols */
#define SYMBOL_EXTERN "extern"  /* extern type for symbols */
#define SYMBOL_ENTRY "entry"   /* entry type for symbols */
#define SYMBOL_COMMAND "code" /* type of symbol in command */
#define NUM_OF_COMMANDS 16   /* the number of difrent commands */
#define MAX_NUMBERS_IN_DATA 10  /* Max numbers for a data command */ 


/* Structure to store macro names and their contents */
typedef struct {
    char name[MAX_CHAR_MACRO_NAME];
    char contents[MAX_MACRO_SIZE];
} Macro;

/* Define the structure for the symbol table node */
typedef struct SymbolNode {
    char symbol[MAX_SYMBOL_NAME];
    int value;
    char type[MAX_COMMAND_NAME];
    struct SymbolNode* next;
} SymbolNode;

/* Define the structure for the command */
typedef struct Instruction {
    char name[MAX_COMMAND_NAME];
    int operandCount; /* the number of operand for the command */
    int commanCod; /* a 0-15 code number for each command (define in the maman) */
} Instruction;

/* a struct for charters and numbers */
typedef union Data {
    short int number;
    char character;
} Data;

/* Define the structure for the command instructions arry and data arry*/
typedef struct AssemblyData {
    short int instructionsArry[MAX_ARRY_SYZE]; /* a table for the command words */
    int IC; /* the next index in the instruction table */
    Data dataArry[MAX_ARRY_SYZE]; /* a table for data words (can store both integers and characters) */
    int DC; /* the next index in the data table */
} AssemblyData;

/* struct to store the external symbols */
typedef struct ExternalSymbolNode {
    char symbol[MAX_SYMBOL_NAME];
    int value;
    struct ExternalSymbolNode* next;
} ExternalSymbolNode;

/* Define a struct for a linked list node to store each macro */
typedef struct macroNode {
    char name[MAX_CHAR_MACRO_NAME];   /* Name of the macro */
    char contents[MAX_MACRO_SIZE];    /* Contents of the macro */
    struct macroNode *next;           /* Pointer to the next macroNode in the list */
} MacroNode;




/* Functions in first_read_mcro.c */
int createAfterMacroFile(FILE *sourceFile, char *sourceFileName, Instruction *commands);
int parseFile(FILE* sourceFile, FILE* targetFile, MacroNode **macroList, Instruction *commands);
void replaceMacro(FILE *sourceFile, FILE *targetFile, MacroNode *macroNode);
MacroNode* isMacroDefined(char *name, MacroNode *macroList);
int isMacroNameValid(char *name, Instruction *commands);
void freeMacroList(MacroNode *macroList);


/* Function in first_loop.c */
int processFirstLoop(FILE *file, SymbolNode **symbolTable, Instruction *commands, AssemblyData *assembly);
int processLine(SymbolNode **symbolTable, Instruction *commands, AssemblyData *assembly, char *line, int lineNumber);
int processSymbolLine(SymbolNode **symbolTable, Instruction *commands, AssemblyData *assembly, char *restOfLine, int lineNumber, char *symbol, char *instruction);
int processGuidanceLine(AssemblyData *assembly, char *restOfLine, int lineNumber, char *instruction, char *type);
int processInstructionLine(Instruction* commands, AssemblyData* assembly, char* restOfLine, int lineNumber, char* instruction);
int processNoSymbolLine(SymbolNode **symbolTable, Instruction *commands, AssemblyData *assembly, char *restOfLine, int lineNumber, char *instruction);


/* Function in function_for_loops.c */
int isEmptyLine(char *line);
int isCommentLine(char *line);
int isValidSymbol(char* str);
int isSymbolDefined(SymbolNode *symbolTable, char *symbol);
int isGuidance(char *instruction);
int isInstruction(char* str, Instruction *commands);
void insertSymbol(SymbolNode **symbolTable, const char* symbol, int value, const char* type);
int isValidData(char *restOfLine, AssemblyData *assembly, int lineNumber);
int isValidInt(char *str);
int isValidString(char *restOfLine, AssemblyData *assembly, int lineNumber);
int isOneSymbol(char* restOfLine, int lineNumber);
int countWords(const char *str);
int isValidAdress(Instruction* commands, char *instruction , char *operand1 , char *operand2, int lineNumber);
int getAdress(char *operand, int lineNumber);
short int makeWord(short int commandCod, short int op1, short int op2);
int numberOfWords(short int firstWord);


/* Function in second_loop.c */
int processSecondLoop(FILE *file, SymbolNode *symbolTable, Instruction *commands, AssemblyData *assembly, ExternalSymbolNode **externals);
int markEntrySymbol(SymbolNode *symbolTable, const char *symbol);
int processCommand(SymbolNode *symbolTable, Instruction *commands, AssemblyData *assembly, char *instruction, char *restOfLine, int lineNumber, ExternalSymbolNode **externals);
int mekeOperandsWord(SymbolNode *symbolTable, AssemblyData *assembly, char *operand1, char *operand2, int op1Method, int op2Method, ExternalSymbolNode **externals);
short int getARE(SymbolNode *symbolTable, char *operand);
int insertExternalSymbol(SymbolNode *symbolTable, ExternalSymbolNode **externals, const char *symbol, int value);


/* Function in function_for_make_files.c */
void createFiles(AssemblyData* assembly, const char *filename, SymbolNode *symbolTable, ExternalSymbolNode *externals);
void createObjFile(AssemblyData* assembly, const char *filename);
void createEntFile(SymbolNode *symbolTable, const char *filename);
void createExternFile(ExternalSymbolNode *externals, const char *filename);


/* Function in main.c */
void updateDataStringSymbols(SymbolNode *symbolTable, AssemblyData* assembly);
void freeSymbolTable(SymbolNode** symbolTable);
void freeExternalSymbolNodes(ExternalSymbolNode* externals);


#endif
