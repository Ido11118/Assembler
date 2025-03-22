/*
File name: main.c
program: custom assembler in C.
Include file: main of program. 
*/


#include "asembler.h"


int main(int argc, char *argv[]) {
    int i; /* use for the index of the file in command line (argv) */
    char *sourceFileName; /* the name of each source file */
    char *filename; /* the source file name with '.am' at and */
    int hasNoErrors = 1; /* flag will torn off if there is Error in process */
    AssemblyData *assembly; /* a struct for the data arry and instruction arry */
    ExternalSymbolNode *externals = NULL; /* Declare the externals list */
    FILE *sourceFile;  /* the source file before macro */
    FILE* file;        /* the file after macro */

    SymbolNode* symbolTable = NULL; /* Define the symbol table */
    Instruction commands[] = { /*Define the commands */
        {"mov", 2, 0},
        {"cmp", 2, 1},
        {"add", 2, 2},
        {"sub", 2, 3},
        {"not", 1, 4},
        {"clr", 1, 5},
        {"lea", 2, 6},
        {"inc", 1, 7},
        {"dec", 1, 8},
        {"jmp", 1, 9},
        {"bne", 1, 10},
        {"red", 1, 11},
        {"prn", 1, 12},
        {"jsr", 1, 13},
        {"rts", 0, 14},
        {"stop", 0, 15}
    };

    if (argc < 2) {  /* Check if there is no file */
        printf("Error: missing files. make sure to run the program in that way: '%s file1 file2 ...'\n", argv[0]);
        return 1;
    }
   
    for (i = 1; i < argc; i++) { /* Process for each file */
	sourceFileName = malloc(MAX_FILE_NAME_LENGTH);
	if (sourceFileName == NULL) { /* check if the memory allocated successfully */
 	   printf("Error: Failed to allocate memory for filename.\n");
  	   return 1;
	}
	filename = malloc(MAX_FILE_NAME_LENGTH);
	if (filename == NULL) { /* check if the memory allocated successfully */
 	   printf("Error: Failed to allocate memory for filename.\n");
 	   free(sourceFileName);
  	   return 1;
	}

	strcpy(sourceFileName, argv[i]); 
        sourceFile = fopen(sourceFileName,"r"); /* open source file */
	printf("\n\nIn file %s:\n", sourceFileName);
	hasNoErrors = createAfterMacroFile(sourceFile,sourceFileName, commands); /* The pre-assembler stage */
        if (hasNoErrors == 0) { /* if there is error in pre-assembler stage */
	    printf("Assembler process failed due to a problems in pre-assembler stage.\n");
 	    free(sourceFileName);
            free(filename);
      	    continue; /* faild to open source file or create target file. */
        }

	assembly = malloc(sizeof(AssemblyData));  
        if (assembly == NULL) { /* check if the memory allocated successfully */
 	    printf("Failed to allocate memory for Data.\n");
 	    free(sourceFileName);
            free(filename);
	    return 1;
   	}
  	assembly->IC = 0; /* define the next IC */
  	assembly->DC = 0; /* define the next DC */

	strcpy(filename, sourceFileName);
        strcat(filename, ".am"); /* add '.am' for source file name */
        file = fopen(filename, "r");
	hasNoErrors = processFirstLoop(file, &symbolTable, commands, assembly);  /* Process the first loop */
        if (hasNoErrors == 0) {  /* if there is error in first loop process stage */
	    printf("Assembler first process failed.\n");
 	    free(sourceFileName);
            free(filename);
	    free(assembly);
	    freeSymbolTable(&symbolTable);
      	    continue; /* faild in first loop prosses. */
        }

	rewind(file);  /* Rewind the file to process the second loop */
	updateDataStringSymbols(symbolTable, assembly); /* update the adress of each symbol of the data arry by add IC value */
	assembly->IC = 0; /* define the next IC to 0 again for second loop */

        hasNoErrors = processSecondLoop(file, symbolTable, commands, assembly, &externals); /* prosses the second loop, return 1 if ok */
        if (hasNoErrors == 0) {   /* if there is error in second loop process stage */
	    printf("Assembler second process failed.\n");
 	    free(sourceFileName);
            free(filename);
	    free(assembly);
	    freeSymbolTable(&symbolTable);
	    freeExternalSymbolNodes(externals);
      	    continue; /* faild in second loop prosses. */
        }

	createFiles(assembly, sourceFileName, symbolTable, externals); /* make files - obj, ext, ent */
	
	printf("Process for '%s' file complite, there is no errors.\n", sourceFileName);
	fclose(file); /* Close the file */
	free(sourceFileName);
	free(filename);
	free(assembly);
	freeSymbolTable(&symbolTable);
	freeExternalSymbolNodes(externals);
    }

    return 0;
}


/* 
Function to update the adress of data array.
Add the IC value for each adress in data arry.
Use between first loop and second loop.
*/
void updateDataStringSymbols(SymbolNode *symbolTable, AssemblyData* assembly)
{
    SymbolNode* current = symbolTable;

    while (current != NULL) {
        if (strcmp(current->type, SYMBOL_DATA) == 0 || strcmp(current->type, SYMBOL_STRING) == 0) {
            current->value += assembly->IC; /* add IC value */
        }
        current = current->next;
    }
}

/* Function to free the symbol table */
void freeSymbolTable(SymbolNode** symbolTable) {
    while (*symbolTable != NULL) {
        SymbolNode* current = *symbolTable;
        *symbolTable = (*symbolTable)->next;
        free(current);
    }
}

/* Function to free the external table */
void freeExternalSymbolNodes(ExternalSymbolNode* externals) {
    while (externals != NULL) {
        ExternalSymbolNode* current = externals;
        externals = externals->next;
        free(current);
    }
}
