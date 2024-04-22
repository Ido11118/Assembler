/*
File name: first_loop.c
program: Part of maman 14 program.
Include file: the rirst loop process
*/

#include "asembler.h"

/*
Start of the first loop process on evry line.
If there is no erros - return 1;
Else return 0;
*/
int processFirstLoop(FILE *file, SymbolNode **symbolTable, Instruction *commands, AssemblyData *assembly)
{
    char line[MAX_LINE_LENGTH]; /* a string for the line */
    int lineNumber = 1; /* Count the number of the current line */
    int hasErrors = 1; /* flag will turn on if there is Error in process */    


    while (fgets(line, MAX_LINE_LENGTH, file) != NULL)
    {   
        /* Skip empty lines and comments */
        if (isEmptyLine(line) || isCommentLine(line)) { 
	    lineNumber++; 
            continue;
	}
     
        /* Process the line */
        if (processLine(symbolTable, commands, assembly, line, lineNumber) == 0) {
	    hasErrors = 0;
	}
	lineNumber++; 
    }
    return hasErrors;
}

/* 
This function get a line and make a symbol or no symbol process.
Return 1 if there is no errors.
Return 0 if there is an error.
*/
int processLine(SymbolNode **symbolTable, Instruction *commands, AssemblyData *assembly, char *line, int lineNumber)
{
    char *symbol = NULL; /* symbol in line, if there is no symbol stay null */
    char instruction[MAX_COMMAND_NAME]; /* the instruction of the line */
    char restOfLine[MAX_LINE_LENGTH]; /* the rest of the line - without symbol (if exsist) and instruction */
    int prossesFlag; /* 1 if no errors, 0 else */

    sscanf(line, "%s", instruction); /*Keep the first word in line at instruction */
    if (instruction[strlen(instruction) - 1] == ':') { /* Check if first world is a valid symbol */
	instruction[strlen(instruction) - 1] = '\0'; /* delete the ':' char from symbol */
        if (isValidSymbol(instruction) == 0) { /* check if symbol valid */
	    if (instruction==NULL) { /* no symbol before ':' char */
		printf("Error: Symbol not found in line %d.\n", lineNumber);
	        return 0;
	    }
	    printf("Error: Unvalid symbol '%s' in line %d.\n", instruction, lineNumber); /* symbol not valid */
	    return 0;
	}

	/* keep symbol in symbol argument, keep the next word in instruction argument */
	symbol = malloc(strlen(instruction) + 1);  
	strcpy(symbol, instruction);
	sscanf(line, "%*s %s", instruction);
    }

    if (symbol != NULL) /* Process for a symbol line */
    {
        sscanf(line, "%*s%*s %[^\n]", restOfLine); /* keep the line without the symbol and instruction */
        prossesFlag = processSymbolLine(symbolTable, commands, assembly, restOfLine, lineNumber, symbol, instruction);    
    } else {  /* Process for a no symbol line */
	sscanf(line, "%*s %[^\n]", restOfLine); /* keep the line without the instruction */
	prossesFlag = processNoSymbolLine(symbolTable, commands, assembly, restOfLine, lineNumber, instruction);
    }
    free(symbol);  
    return prossesFlag;
}

/*
Start a process for a symbol line.
Choose the instruction process (command or guidnace).
If the line is valid - entring the data and first words to IC and DC tabels.
Entring the symbol with the type and adress to symbol table.
Return 1 if there is no errors.
Return 0 if there is an error.
*/
int processSymbolLine(SymbolNode **symbolTable, Instruction *commands, AssemblyData *assembly, char *restOfLine, int lineNumber, char *symbol, char *instruction)
{
    int value; /* Value of symbol (adress in IC or DC arrys) */
    char type[MAX_COMMAND_NAME]; /* type of symbol */

    /* Check if the symbol already exists in the symbol table */
    value = isSymbolDefined(*symbolTable, symbol); /* get the IC of the exsiset symbol */
    if (value != -1) { /* symbol define */
	value = value + START_ADRESS;
        printf("Error: Symbol '%s' already defined at adress %d\n", symbol, value);     
        return 0;
    }
    
    if (isGuidance(instruction)) { /* process for guidance instruction */
	value = processGuidanceLine(assembly, restOfLine, lineNumber, instruction, type);
	if (value == -1) { /* if line is not valid */
	    return 0;
	}
	if (strcmp(instruction, ".extern") == 0) strcpy(symbol, restOfLine); 

    } else if (isInstruction(instruction, commands)) { 
	value = assembly->IC + START_ADRESS; /* Get the value of the symbol based on the current IC value */
	if (processInstructionLine(commands, assembly, restOfLine, lineNumber, instruction) == 0) { 
	    return 0;
	}
	strcpy(type, SYMBOL_COMMAND);
    } else {
	printf("Error: Invalid instruction '%s' on line %d\n", instruction, lineNumber);
	return 0; 
    }

    if (strcmp(instruction, ".entry") == 0) return 1; /* if entry - don't insert and continue */
    /* Insert the symbol into the symbol table*/ 
    insertSymbol(symbolTable, symbol, value, type);  
    return 1;
}

/*
Process on guidance line.
For valid 'data' or 'string' guidance - entring the data to DC tabel, return the adress of the first data.
For valid 'entry' or 'extern' guidance - entring the symbol to symbol table with the line adress, return the line adress.
If the line isn't valid - return -1.
*/
int processGuidanceLine(AssemblyData *assembly, char *restOfLine, int lineNumber, char *instruction, char *type)
{
    int value; /* use to keep the value of IC or DC */
    int operandsCount = 0; /* count the operands in line */

    if (strcmp(instruction, ".data") == 0) { /* if guidance is data */
	strcpy(type, SYMBOL_DATA); 
        operandsCount = isValidData(restOfLine, assembly, lineNumber); 
	if (operandsCount == -1) { /* not a valid operands */  
	    return -1;
	}
	value = (assembly->DC) - operandsCount + START_ADRESS; /* keep the value of DC before entring data */
    }  
    else if (strcmp(instruction, ".string") == 0) { /* if guidance is string */
	strcpy(type, SYMBOL_STRING); 
	value = assembly->DC + START_ADRESS; 
        operandsCount = isValidString(restOfLine, assembly, lineNumber); /* check if valid, enter to DC and update DC */
	if (operandsCount == -1) {   
	    return -1;
	}
    }
    else if (strcmp(instruction, ".extern") == 0 || strcmp(instruction, ".entry") == 0) { /* if relocateble */
	if (strcmp(instruction, ".extern") == 0) { /* chose the type */
	    strcpy(type, SYMBOL_EXTERN); 
	} else {
	    strcpy(type, SYMBOL_ENTRY); 
	}
	if (isOneSymbol(restOfLine, lineNumber) == 0) { /* check if have not valid symbol or more then one word */
	    return -1;
      	}

	value = 0;
    }
    return value; /* procces complite (value >= 0) */
}


/*
Process on command line.
If the line is valid - entring the first words to IC tabels and return 1.
If the line isn't valid - return 0.
*/
int processInstructionLine(Instruction* commands, AssemblyData* assembly, char* restOfLine, int lineNumber, char* instruction) {
    char operand1[MAX_OPERAND_LENGTH];
    char operand2[MAX_OPERAND_LENGTH]; 
    char* token;
    int numOfWord;  /* the number of words in rest of line */
    int i, opCount = 0; /* the number of operand for command (get from cammand stract) */
    char word1[MAX_SYMBOL_NAME]; /* use to check 2 operands */
    char word2[MAX_SYMBOL_NAME]; /* use to check 2 operands */
    short int commandWord;
    int L; /* number of words for the current line */

    /* Find the corresponding instruction in the commands array, and it amount of operands */
    for (i = 0; i < NUM_OF_COMMANDS; i++) {
        if (strcmp(commands[i].name, instruction) == 0) {
            opCount = commands[i].operandCount;
            break;
        }
    }
    /* Check for operands if required */
    numOfWord = countWords(restOfLine);

    switch (opCount) {
        case 0:
            /* Make sure there are no additional characters after the instruction */
            if (numOfWord != 0) {
		printf("Error: on line %d - there is not operands allowed in for that unstruction.\n", lineNumber);
                return 0; /* There shouldn't be any operand for this instruction */
            }
	    commandWord = isValidAdress(commands, instruction , "" , "", lineNumber);
            break;
        case 1:
            /* Extract the first operand */

            sscanf(restOfLine, "%s", operand1);
            if (numOfWord == 0) {
		printf("Error: missing operand for '%s' command on line %d.\n", instruction, lineNumber);
                return 0; /* Failed to extract operand */
            } else if (numOfWord != 1) {
		printf("Error: not a valid input for '%s' command on line  %d - too much operands.\n",instruction, lineNumber);
                return 0; /* Failed to extract operand */
            }

	    commandWord = isValidAdress(commands, instruction , "", operand1, lineNumber); /* get the first word */
            break;
        case 2:
            /* Extract two operands separated by a comma */

	    token = strtok(restOfLine, ",");
  	    if (token == NULL) {
	        printf("Error: missing operand for '%s' command on line %d.\n", instruction, lineNumber);
	        return 0;
	    }	
	    if (token != NULL) {
                strcpy(operand1, token);

                token = strtok(NULL, " \t\n"); /* Continue tokenizing from the last position */
                if (token == NULL) {
		    printf("Error: missing operand for '%s' command on line %d.\n", instruction, lineNumber);
		    return 0;
		} else {
                    strcpy(operand2, token);
		}

                /* Check if the second part contains one word */
                numOfWord = sscanf(operand1, "%s%s", word1, word2);
                if (numOfWord == 0) {
                    printf("Error: missing operand for '%s' command on line %d.\n", instruction, lineNumber);
		    return 0;
		} else if (numOfWord != 1) {
                    printf("Error: too many operands for '%s' command on line %d.\n", instruction, lineNumber);
		    return 0;
		}
	    }
	    commandWord = isValidAdress(commands, instruction , operand1 , operand2, lineNumber); /* get the first word */
            break;
        default:
            return 0; /* Invalid operand count in the struct */
    }

    if (commandWord == -1) { /* if not valid */
	return 0;
    }
    L = numberOfWords(commandWord); /* get the number of words in line */    
    assembly->instructionsArry[assembly->IC] = commandWord; /* add command word for IC table */
    assembly->IC = (assembly->IC) + L;   /* Update IC */
    return 1; /* valid instruction */
}

/*
Start a process for a no symbol line.
Choose the instruction process (command or guidnace).
If the line is valid - entring the data and first words to IC and DC tabels.
Return 1 if there is no errors.
Return 0 if there is an error.
*/
int processNoSymbolLine(SymbolNode **symbolTable, Instruction *commands, AssemblyData *assembly, char *restOfLine, int lineNumber, char *instruction)                                  
{    
    char type[MAX_COMMAND_NAME]; /* type of symbol - not for use, just to go in the functions */
    char symbol[MAX_SYMBOL_NAME]; /* symbol for extern instruction */
    int value; /* Adress in IC or DC arrys */

    if (isGuidance(instruction)) { /* process for guidance instruction */
	value = processGuidanceLine(assembly, restOfLine, lineNumber, instruction, type);
	if (value == -1) { /* if line is not valid */
	    return 0;
	}
	if (strcmp(instruction, ".extern") == 0) {
	    strcpy(symbol, restOfLine);
	    insertSymbol(symbolTable, symbol, 0, type);
	}	
    } else if (isInstruction(instruction, commands)) {   
	if (processInstructionLine(commands, assembly, restOfLine, lineNumber, instruction) == 0) { 
	    return 0;
	}
    } else {
	printf("Error: Invalid instruction '%s' on line %d\n", instruction, lineNumber);
	return 0; 
    }
    return 1;
}


