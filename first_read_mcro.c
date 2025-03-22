/*
File name: first_read_mcro.c
program: Part of maman 14 program.
Include file: a pre-assembler stage, will return 1 if no errors in stage, else return 0. 
*/


#include "asembler.h"


/* 
Create and write the .am file.
return 1 if file has made.
0 if not.
*/
int createAfterMacroFile(FILE *sourceFile, char *sourceFileName, Instruction *commands) {
    char targetFileName[MAX_FILE_NAME_LENGTH];  /* The target file name */
    FILE *targetFile; 
    MacroNode *macroList = NULL;                /* Pointer to the head of the macro list */

    strcpy(targetFileName, sourceFileName);
    strcat(targetFileName, ".am");
    targetFile = fopen(targetFileName, "w");

    /* Check if the files were opened correctly */
    if (sourceFile == NULL) {
        printf("Failed to open source file ('%s').\n", sourceFileName);
        return 0;
    }
    if (targetFile == NULL) {
        printf("Failed to create the target file for %s file.\n", sourceFileName);
        return 0;
    }

    /* Parse the source file, write to the target file, and manage the macro list */
    if (parseFile(sourceFile, targetFile, &macroList, commands) == 0) {
	freeMacroList(macroList); /* Free the memory allocated for the macro list */
	return 0;
    }
    fclose(sourceFile);
    fclose(targetFile);
    freeMacroList(macroList); /* Free the memory allocated for the macro list */
    return 1;
}

/* 
Parse the source file, write to the target file, and manage the macro list
return 1 if there is no errors.
0 if there is.
*/
int parseFile(FILE* sourceFile, FILE* targetFile, MacroNode **macroList, Instruction *commands) {
    char line[MAX_MACRO_SIZE];       /* Buffer to store each line read from the file */
    int hasMacro = 0;                /* Flag to track if the current line is within a macro definition */
    MacroNode *lastMacro = NULL;     /* Pointer to the last defined macro */
    MacroNode *newMacroNode;

    /* Read each line from the source file */
    while (fgets(line, sizeof(line), sourceFile) != NULL) {
        char firstField[MAX_CHAR_MACRO_NAME] = {0}, secondField[MAX_CHAR_MACRO_NAME] = {0};  /* Buffers to store the first and second fields in each line */
        MacroNode *macroNode;  /* Pointer to the macroNode corresponding to the current macro */
        sscanf(line, "%s%s", firstField, secondField);  /* Extract the first and second fields from the line */

        macroNode = isMacroDefined(firstField, *macroList);

        /* Check if the current line is within a macro definition */
        if (hasMacro && strcmp(firstField, "endmcro") == 0) { /* end of macro */
            hasMacro = 0; 
        } else if (hasMacro && macroNode == NULL) { /* new line in macro that not define before */
            strcat(lastMacro->contents, line);
        } else if (hasMacro == 0 && macroNode != NULL) { /* parse the macro */
            replaceMacro(sourceFile, targetFile, macroNode);
        } else if (strcmp(firstField, "mcro") == 0) { /* start of new macro */
            if (isMacroDefined(secondField, *macroList) != NULL) {
                printf("Error: The name of mcro '%s' is already exsist.\n", secondField);
                return 0;
            }
            if (isMacroNameValid(secondField, commands) == 0) {
                printf("Error: The name of mcro '%s' is not valid (instruction name)\n", secondField);
                return 0;
            }

            hasMacro = 1;
            newMacroNode = malloc(sizeof(MacroNode));  /* Allocate memory for the new macroNode */
            if (newMacroNode == NULL) {
                printf("Failed to allocate memory for a new macro.\n");
                return 0;
            }
            strcpy(newMacroNode->name, secondField);
            newMacroNode->contents[0] = '\0';
            newMacroNode->next = *macroList;
            *macroList = newMacroNode;  /* Insert the new macroNode at the beginning of the list */
            lastMacro = newMacroNode;
        } else {
            fprintf(targetFile, "%s", line);  /* Write the line to the target file */
        }
    }
    return 1;
}

/* Replace a macro with its contents in the target file */
void replaceMacro(FILE *sourceFile, FILE *targetFile, MacroNode *macroNode) {
    fprintf(targetFile, "%s", macroNode->contents);
}

/* 
Check if a macro is defined in the macro list
Return the macro nude if find.
Return null if not.
*/
MacroNode* isMacroDefined(char *name, MacroNode *macroList) {
    MacroNode *node = macroList;  /* Pointer to traverse the macro list */
    /* Traverse the macro list to find a macro with the given name */
    while (node != NULL) {
        if (strcmp(node->name, name) == 0) {
            return node;  /* Return the macroNode if the macro is found */
        }
        node = node->next;
    }
    return NULL;  /* Return NULL if the macro is not found */
}

/* 
Check if a macro name is a valid instruction.
Return 1 if valid.
Return 0 if not valid.
*/
int isMacroNameValid(char *name, Instruction *commands) {
    int i;
    for (i = 0; i < NUM_OF_COMMANDS; i++) {
        if (strcmp(commands[i].name, name) == 0 || isGuidance(name) == 1) {
	        return 0;
	    }
    }
    return 1;
}

/* Free the memory allocated for the macro list */
void freeMacroList(MacroNode *macroList) {
    MacroNode *node = macroList;  /* Pointer to traverse the macro list */
    /* Traverse the macro list and free each macroNode */
    while (node != NULL) {
        MacroNode *nextNode = node->next;
        free(node);
        node = nextNode;
    }
}

