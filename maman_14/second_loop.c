/*
File name: second_loop.c
program: Part of maman 14 program.
Include file: the second loop process.
*/

#include "asembler.h"

/*
Start of the second loop process on evry line.
complite the instruction table.
Return 1 if there is no errors.
Return 0 if there is an error.
*/
int processSecondLoop(FILE *file, SymbolNode *symbolTable, Instruction *commands, AssemblyData *assembly, ExternalSymbolNode **externals)
{
    char line[MAX_LINE_LENGTH]; /* A string for the line */
    char firsWord[MAX_LINE_LENGTH];
    char restOfLine[MAX_LINE_LENGTH]; /* A string for the line without symbol and instruction */
    int lineNumber = 1; /* Count the number of the current line */
    char instruction[MAX_COMMAND_NAME]; /* the instruction in line */
    char symbol[MAX_SYMBOL_NAME]; /* symbol for entry instruction */
    int flagHasNoErors = 1; /* 1 if there is no errors */

    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {
        /* Skip empty lines and comments */
        if (isEmptyLine(line) || isCommentLine(line)) {
            lineNumber++;
            continue;
        }
	sscanf(line, "%s", firsWord);
	if (firsWord[strlen(firsWord)-1] == ':') {      /* if first word is symbol */
	    sscanf(line, "%*s %s %[^\n]", instruction, restOfLine); /* keep the second word in line and the rest of the line */
	} else {
	    sscanf(line, "%s %[^\n]", instruction, restOfLine); /* keep the line without the symbol and instruction */
	}  
    
        /* Process the line */
        if (isGuidance(instruction)) {
            if (strcmp(instruction, ".data") == 0 || strcmp(instruction, ".string") == 0 || strcmp(instruction, ".extern") == 0) {   
                lineNumber++; /* Continue to the next line */
                continue;
            } else if (strcmp(instruction, ".entry") == 0) {
                    /* Mark the symbol in the symbol table (currently, we're ignoring it) */
                    sscanf(restOfLine, "%s", symbol);
                    if (markEntrySymbol(symbolTable, symbol) == 0) { 
		        flagHasNoErors = 0;
		    }
	    }
        } else if (isInstruction(instruction, commands)) { /* Process the command line and update IC */
                if (processCommand(symbolTable, commands, assembly, instruction, restOfLine, lineNumber, externals) == 0) { 
		    flagHasNoErors = 0;
		}
        }
        lineNumber++;
    } /* end of while */

    return flagHasNoErors;
}

/*
Check if symbol is entry type, if it is - change the type to entry.
Return 1 if there is no errors.
Return 0 if there is an error (the symbol is entry and extern).
*/
int markEntrySymbol(SymbolNode *symbolTable, const char *symbol)
{
    SymbolNode *current = symbolTable;
    while (current != NULL)
    {
        if (strcmp(current->symbol, symbol) == 0)
        {
            /* Mark the symbol as an entry symbol */           
	    if (strcmp(current->type, SYMBOL_EXTERN) == 0) {
	        printf("Error: can't use '%s' Symbol as en antry and extern.\n", symbol);
	        return 0;
	    }
            strncpy(current->type, SYMBOL_ENTRY, MAX_COMMAND_NAME);
            return 1;
        }
        current = current->next;
    }

    /* If the symbol is not found, print an error message */
    printf("Error: Symbol '%s' not found in the symbol table.\n", symbol);
    return 0;
}

/*
Make a process for command line.
Return 1 if there is no errors.
Return 0 if there is an error.
*/
int processCommand(SymbolNode *symbolTable, Instruction *commands, AssemblyData *assembly, char *instruction, char *restOfLine, int lineNumber, ExternalSymbolNode **externals)
{
    short int theFirstWord; /* the command word of the line */
    char operand1[MAX_OPERAND_LENGTH]; /* surce operand */
    char operand2[MAX_OPERAND_LENGTH]; /* destenetion operand */
    int op1Method; /* operand 1 adress method (source operand) */
    int op2Method; /* operand 2 adress method (destenation operand) */
    int L; /* number of words for command */
    int opCount; /* the number of operands for command */
    int i;
    int flag = 1; /* 1 if no errors */
    theFirstWord = assembly->instructionsArry[assembly->IC];
    L = numberOfWords(theFirstWord);


    /* Find the corresponding instruction in the commands array, and it amount of operands */
    for (i = 0; i < NUM_OF_COMMANDS; i++) {
        if (strcmp(commands[i].name, instruction) == 0) {
            opCount = commands[i].operandCount;
            break;
        }
    }

    switch (opCount) {
        case 0: /* no more words */
            break;

        case 1:
            /* Extract the first operand */
            sscanf(restOfLine, "%s", operand2);
	    op1Method = 0;
    	    op2Method = getAdress(operand2, lineNumber);
	    flag = mekeOperandsWord(symbolTable, assembly, "", operand2, op1Method, op2Method, externals);
            break;

        case 2:
            /* Extract two operands separated by a comma */
            sscanf(restOfLine, "%[^,],%s", operand1, operand2);
            sscanf(operand1, " %[^ ]", operand1);
            sscanf(operand2, " %[^ ]", operand2);
	    op1Method = getAdress(operand1, lineNumber);
    	    op2Method = getAdress(operand2, lineNumber);
	    flag = mekeOperandsWord(symbolTable, assembly, operand1, operand2, op1Method, op2Method, externals);
	    break;
    }
    assembly->IC = assembly->IC + L;
    return flag;
}

/*
Make a words from operands and enter the words for the instruction arry.
Return 1 if there is no errors.
Return 0 if there is an error.
*/
int mekeOperandsWord(SymbolNode *symbolTable, AssemblyData *assembly, char *operand1, char *operand2, int op1Method, int op2Method, ExternalSymbolNode **externals)
{
    short int sorceOp;  /* sorce bynary number */
    short int destOp; /* destenation bynary number */
    char *p; /* use for strtol */
    short int word1 = 0;  /* word for instruction arry */
    short int word2 = 0;  /* second word for instruction arry */   
    short int AREnum1 = 0 ; /* operand for A,R,E */
    short int AREnum2 = 0; /* operand for A,R,E */
    int flag = 1; /* 1 if no errors */

    if (op1Method == 1) sorceOp = strtol(operand1, &p,10); /* get the number value */
    if (op2Method == 1) destOp = strtol(operand2, &p,10);  /* get the number value */

    if (op1Method == 3) {
        sorceOp = isSymbolDefined(symbolTable, operand1);  /* get the adress value */
	if (sorceOp == -1) {/* symbol has not define */
	    printf("Error: Symbol '%s' has not define.\n", operand1);
	    return 0;
	}
	AREnum1 = getARE(symbolTable, operand1);
    }
    if (op2Method == 3) {
        destOp = isSymbolDefined(symbolTable, operand2);  /* get the adress value */
	if (destOp == -1) {/* symbol has not define */
	    printf("Error: Symbol '%s' has not define.\n", operand2);
	    return 0;
	}
	AREnum2 = getARE(symbolTable, operand2);
    }

    if (op1Method == 5) sorceOp = operand1[2] - '0';  /* get the number of the register */
    if (op2Method == 5) destOp = operand2[2] - '0';   /* get the number of the register */

    /* use difrent cases for 1 operand / 2 same operand of numbers or registers / 2 defrent operands or 2 symbols */
    if (op1Method == 0) {  /* one operand case */
	word1 |= (destOp << 2); /* put destenation operand into bits 2-11 */
	word1 |= (AREnum2); /* put A,R,E operand into bits 0-1 */
	assembly->instructionsArry[(assembly->IC) + 1] = word1;
    } else if (op1Method == op2Method && op1Method != 3) { /* 2 reisters or 2 numbers case (one word) */
        word1 |= (sorceOp << 7); /* put op1 into bits 7-11 */
        word1 |= (destOp << 2); /* put op2 into bits 2-6 */
	assembly->instructionsArry[(assembly->IC) + 1] = word1;
    } else {
	word1 |= (sorceOp << 7); /* put destenation operand into bits 2-11 in word 1 */
	word2 |= (destOp << 2); /* put destenation operand into bits 2-11 in word 2 */
	word1 |= (AREnum1); /* put A,R,E operand into bits 0-1 */
	word2 |= (AREnum2); /* put A,R,E operand into bits 0-1 */
	assembly->instructionsArry[(assembly->IC) + 1] = word1;
	assembly->instructionsArry[(assembly->IC) + 2] = word2;
    }

    /* process to keep enxtern symbols value for extern file */
    if (op1Method == 3) { /* case - two operands, the first is symbol */
	flag = insertExternalSymbol(symbolTable, externals, operand1, assembly->IC + 1);
    }
    if (op2Method == 3 && op1Method == 0) { /* case - one symbol operand */
	flag = insertExternalSymbol(symbolTable, externals, operand2, assembly->IC + 1);
    }
    if (op2Method == 3 && op1Method != 0) { /* case - two operands, the second is symbol */
	if (insertExternalSymbol(symbolTable, externals, operand2, assembly->IC + 2) == 0) {
	    flag = 0;
	}
    }
    return flag;
}

/*
Check the A/R/E of symbol.
For absolute (A) - return 0.
For relocatable (R) - return 2.
For external (E) - return 1.
*/
short int getARE(SymbolNode *symbolTable, char *operand) {
    SymbolNode* current = symbolTable;

    while (current != NULL) {
        if (strcmp(current->symbol, operand) == 0) {
            if (strcmp(current->type, SYMBOL_EXTERN) == 0) {
                return 1; /* Return 1 for "extern" type */
            } else {
                return 2; /* Return 0 for any other type */
            }
        }
        current = current->next;
    }

    return -1;    /* the symbol is not found. */
}

/* 
Function to insert a new external symbol node into the linked list.
Return 1 if there is no errors.
Return 0 if there is an error.
*/
int insertExternalSymbol(SymbolNode *symbolTable, ExternalSymbolNode **externals, const char *symbol, int value) {
    ExternalSymbolNode *newNode = (ExternalSymbolNode *)malloc(sizeof(ExternalSymbolNode));
    SymbolNode* current = symbolTable; /* pointer to check symbol type */

    if (newNode == NULL) {
        printf("Error: Memory allocation failed for an external symbol node.\n");
        return 0;
    }

    while (current != NULL) { /* find the symbol in symbole table to get the type */
        if (strcmp(current->symbol, symbol) == 0) {
	    if (strcmp(current->type, SYMBOL_EXTERN) != 0)  return 1; /* if not extern symbol */
        }
        current = current->next;
    }

    strncpy(newNode->symbol, symbol, MAX_SYMBOL_NAME);
    newNode->value = value + START_ADRESS;
    newNode->next = NULL;

    if (*externals == NULL) {
        *externals = newNode;
    } else {
        ExternalSymbolNode *current = *externals;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
    return 1;
}

