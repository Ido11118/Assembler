/*
File name: function_for_make_files.c
program: custom assembler in C.
Include file: function to make files after second loop.
*/


#include "asembler.h"

/*
Create obj, ext and ent files.
If there is no extern - dont make ext file.
If there is no entry - dont make ent file.
*/
void createFiles(AssemblyData* assembly, const char *filename, SymbolNode *symbolTable, ExternalSymbolNode *externals)
{
    createObjFile(assembly, filename);  /* make object file */
    createEntFile(symbolTable, filename); /* make entry file */
    createExternFile(externals, filename);  /* make extern file */
}

/* Function to create obj file */
void createObjFile(AssemblyData* assembly, const char *filename) {
    char *objFilename;
    FILE *objFile;
    const char base64Chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    int i;
    int value;
    char base64[3];

    /* Create the output file name */
    objFilename = (char*)malloc(strlen(filename) + 5); /* +5 for ".obj\0" */
    if (objFilename == NULL) {
        printf("Error: Failed to allocate memory for the .obj file name.\n");
        return;
    }
    strcpy(objFilename, filename);
    strcat(objFilename, ".obj");

    /* Open the output file in write mode */
    objFile = fopen(objFilename, "w");
    if (objFile == NULL) {
        printf("Error: Failed to create the .obj file.\n");
        free(objFilename);
        return;
    }

    /* Write IC and DC on the first line */
    fprintf(objFile, "%d %d\n", assembly->IC, assembly->DC);

    /* Write each binary number in base 64 on separate lines */
    for (i = 0; i < assembly->IC; i++) {
        value = assembly->instructionsArry[i];

        /* Convert the 12-bit value to 3 base64 characters */
        base64[0] = base64Chars[(value >> 6) & 0x3F];
        base64[1] = base64Chars[value & 0x3F];
        base64[2] = '\0';

        fprintf(objFile, "%s\n", base64);  /* Write the base64 value to the file */
    }
    for (i = 0; i < assembly->DC; i++) {
        value = assembly->dataArry[i].number;

        /* Convert the 12-bit value to 3 base64 characters */
        base64[0] = base64Chars[(value >> 6) & 0x3F];
        base64[1] = base64Chars[value & 0x3F];
        base64[2] = '\0';

        fprintf(objFile, "%s\n", base64);  /* Write the base64 value to the file */
    }

    fclose(objFile);     /* Close the file */
    free(objFilename);   /* Free the allocated memory for the file name */
}

/* Function to create ent file */ 
void createEntFile(SymbolNode *symbolTable, const char *filename) {
    char *entFilename;
    FILE *entFile;
    SymbolNode *current;
    int hasEntrySymbol = 0;

    /* Check if there are any entry symbols */
    current = symbolTable;
    while (current != NULL) {
        if (strcmp(current->type, "entry") == 0) {
            hasEntrySymbol = 1;
            break;
        }
        current = current->next;
    }

    /* If there are no entry symbols, return without creating the .ent file */
    if (!hasEntrySymbol) {
        return;
    }

    /* Create the output file name */
    entFilename = (char*)malloc(strlen(filename) + 5); /* +5 for ".ent\0" */
    if (entFilename == NULL) {
        printf("Error: Failed to allocate memory for the .ent file name.\n");
        return;
    }
    strcpy(entFilename, filename);
    strcat(entFilename, ".ent");

    /* Open the output file in write mode */
    entFile = fopen(entFilename, "w");
    if (entFile == NULL) {
        printf("Error: Failed to create the .ent file.\n");
        free(entFilename);
        return;
    }

    /* Write each entry symbol and its value to the file */
    current = symbolTable;
    while (current != NULL) {
        if (strcmp(current->type, "entry") == 0) {
            fprintf(entFile, "%s %d\n", current->symbol, current->value);
        }
        current = current->next;
    }

    fclose(entFile);     /* Close the file */
    free(entFilename);   /* Free the allocated memory for the file name */
}

/* Function to create ext file */
void createExternFile(ExternalSymbolNode *externals, const char *filename) {
    char externFilename[MAX_FILE_NAME_LENGTH + 8]; /* +8 for ".extern\0" */
    FILE *externFile;
    ExternalSymbolNode *current;

    if (externals == NULL) {
        return;
    }

    /* Create the output file name */
    strcpy(externFilename, filename);
    strcat(externFilename, ".ext");

    /* Open the output file in write mode */
    externFile = fopen(externFilename, "w");
    if (externFile == NULL) {
        printf("Error: Failed to create the .extern file.\n");
        return;
    }

    /* Write each external symbol and its address to the file */
    current = externals;
    while (current != NULL) {
        fprintf(externFile, "%s %d\n", current->symbol, current->value);
        current = current->next;
    }

    /* Close the file */
    fclose(externFile);
}

