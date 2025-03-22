/*
File name: function_for_loop1.c
program: custom assembler in C.
Include file: a smallest function for rirst loop 
*/


#include "asembler.h"

/* 
Check if the line is empty or contains only whitespace characters.
Return 1 if empty line.
Return 0 if not enpty line.
*/
int isEmptyLine(char *line)
{
    int i;
    for (i = 0; line[i] != '\0'; i++)
    {
        if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
            return 0;
    }
    
    return 1;
}

/* 
Check if the line starts with a comment.
Return 1 if comment line.
Return 0 if not comment line.
*/
int isCommentLine(char *line)
{  
    return line[0] == ';';
}

/* 
Check if the symbol is valid.
Return 1 if the symbol is valid.
Return 0 if not valid.
*/
int isValidSymbol(char* str)
{
    int i;
    if (!isalpha(str[0])) { /* The first character is not a letter */
        return 0;
    }
    for (i = 1; str[i] != '\0'; i++) {
        if (!isalnum(str[i])) { /* Check if there in non letter or digit character */
            return 0;
        }
    }
    return 1;
}

/* 
Check if symbol is defined.
Return the adress (value) of symbol if exsiset.
Return -1 if not exsiset.
*/
int isSymbolDefined(SymbolNode *symbolTable, char *symbol)
{
    SymbolNode* current = symbolTable;
    while (current != NULL) {
        if (strcmp(current->symbol, symbol) == 0) {
            return current->value;
        }
        current = current->next;
    }
    return -1;  /* Symbol not found */
}


/* 
Cheak if the instruction is a valid command.
Return 1 if it's valid command.
Return 0 if not.
*/
int isInstruction(char* str, Instruction *commands)
{
    int i;
    int commandCount = NUM_OF_COMMANDS;
    for (i = 0; i < commandCount; i++) {
        if (strcmp(str, commands[i].name) == 0) {
            return 1;
        }
    }
    return 0;
}

/* 
Cheak if the instruction is a Guidance.
Return 1 if it's valid Guidance.
Return 0 if not.
*/
int isGuidance(char *str)
{
    if (strcmp(str, ".data") == 0 ||
        strcmp(str, ".string") == 0 ||
        strcmp(str, ".entry") == 0 ||
        strcmp(str, ".extern") == 0) {
        return 1;
    }
    return 0;
}

/* Insert the new nude to the end of the table. */
void insertSymbol(SymbolNode** symbolTable, const char* symbol, int value, const char* type)
{
    /* Create a new symbol node */
    SymbolNode* newNode = malloc(sizeof(SymbolNode));
    if (newNode == NULL) {
        printf("Failed to allocate memory for symbol node.\n");
        exit(1);
    }

    /* Copy the symbol, value, and type to the new node */
    strncpy(newNode->symbol, symbol, MAX_SYMBOL_NAME);
    newNode->value = value;
    strncpy(newNode->type, type, MAX_COMMAND_NAME);
    newNode->next = NULL;
    /* Insert the new node at the end of the symbol table */
    if (*symbolTable == NULL) {
        *symbolTable = newNode;
    } else {
        SymbolNode* current = *symbolTable;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}
	
/* 
Check if data is valid.
If it is - Return the number of data and enter the data to DC table.
If not valid - Return -1.
*/
int isValidData(char *restOfLine, AssemblyData *assembly, int lineNumber)
{
    int num, countCommas = 0; /* commas counter */
    char *token; /* get tokens from line */
    char *ptr = restOfLine; /* ptr to count commas in line */
    char *p; /* use for strtol */  
    int flag = 0; /* flag for first loop in while */
    int countNumbers = 0; /* count the number of data */
    
    restOfLine[strcspn(restOfLine, "\n")] = '\0'; /* remove '\n' */
    while (*ptr != '\0') {   /* Count commas and change to space */
        if (*ptr == ',') {
            countCommas++;
        }
        ptr++;  
    }

    while (countCommas >= 0) { /* while there is more tokens in line */
	if (flag == 0 && countCommas > 0) { /* first token in a line with commas */
	    token = strtok(restOfLine, ",") ;
	    flag = 1;
	} else if (countCommas > 0) { /* there is more commas in line */
	    token = strtok(NULL, ","); 
	} else if (flag == 1 && countCommas == 0){ /* lest word in line with commas */
	    token = strtok(NULL, "\n"); 
	} else {  /* a word in line without commas */
	    token = strtok(restOfLine, "\n"); 
	}
	if (token == NULL) { /* no data between commas */
            printf("Error: Missing data in line %d.\n", lineNumber);
            return -1;
	}
	if (isValidInt(token) == 0) { /* if token is not a valid short int number */
	    printf("Error: Unvalid data in line %d.\n", lineNumber);
	    return -1; /* not a valid data */
	}
 
        num = strtol(token, &p,10);
	assembly->dataArry[assembly->DC].number = num;  /* Enter number to DC table */
	assembly->DC++;
        countNumbers++;
	countCommas--;
    }
    return countNumbers;
}   

/*
Check if string is valid integer (can start with '-' or '+').
Return 1 if true, 0 if false.
*/
int isValidInt(char *str)
{
    char *endptr;
    int num;
    
    while (isspace(*str)) { /* Ignore leading white spaces */
        str++;
    }
    if (*str == '\0') {
	return 0;
    }

    num = strtol(str, &endptr, 10);  /* Try to convert the string to a long int */

    while (isspace(*endptr)) {   /* Ignore trailing white spaces */
        endptr++;
    }

    /* If endptr points to the null byte at the end of the string, the entire string is valid */
    if (*endptr == '\0') {
	    if (num > SHRT_MIN && num < SHRT_MAX) { /* if its short int number */
            return 1;
	    }
    }
    return 0; /* Not valid number */
}

/* 
Check if string is valid.
If valid - enter the ascii number of chars to DC table with null (0) at the and, return the number of chars (without null).
If not valid - return -1.
*/
int isValidString(char* restOfLine, AssemblyData* assembly, int lineNumber) 
{
    int i;
    int countChar = 0;

    /* Check if the string is empty */
    if (restOfLine[0] == '\0') {
        printf("Error: there is no string in line %d.\n", lineNumber);
        return -1;
    }

    /* Check if the string starts with double quotes */
    if (restOfLine[0] != '"') {
        printf("Error: Line %d is not a valid string (missing double quotes at start).\n", lineNumber);
        return -1;
    }

    /* Process the characters inside the string */
    for (i = 1; restOfLine[i] != '"' && restOfLine[i] != '\0'; i++) {
        /* Add the ASCII value of each character to the DC array */

	assembly->dataArry[assembly->DC].character = restOfLine[i];
        assembly->DC++;
	countChar++;
    }
    
    /* Check if the string end with double quotes */
    if (restOfLine[i] != '"') {
        printf("Error: Line %d is not a valid string (missing double quotes at end).\n", lineNumber);
        return -1;
    }
    /* Check for any non-whitespace characters after the last double quote */
    i++; /* move past the double quote */
    while (restOfLine[i] != '\0') {
        if (!isspace(restOfLine[i])) { /* isspace is defined in ctype.h, it checks for whitespace characters */
            printf("Error: Line %d has extra characters after the closing double quote.\n", lineNumber);
            return -1;
        }
        i++;
    }

    /* Add a null terminator (0) at the end of the DC array */
    assembly->dataArry[assembly->DC].character = 0;
    assembly->DC++;
    return countChar; /* Return 1 to indicate the string is valid */
}

/* 
Check if there is one valid symbol after guidance.
If true - change 'symbol' to the new symbol, return 1.
If not - return 0.
*/
int isOneSymbol(char* restOfLine, int lineNumber) {   
    char *token; /* use for strtok */
    char *symbol;
    token = strtok(restOfLine, " \t\n"); /* Tokenize on spaces, tabs, and newlines */
    
    if(token == NULL) { /* The line is empty */
	printf("Error: can't find a symbol in line %d.\n", lineNumber);
        return 0;
    }

    symbol = malloc(strlen(token) + 1);   
    if (symbol == NULL) { /* malloc failed */
        printf("Error: memory allocation failed.\n");
	free(symbol);
        return 0;
    }
    strcpy(symbol, token);
    if (isValidSymbol(symbol) == 0) { /*check if symbol name is valid */
	printf("Error: unvalid symbol '%s' in line %d.\n", symbol, lineNumber);
	free(symbol);
	return 0;
    }

    token = strtok(NULL, " \t\n"); /* Try to get another word */
    
    if(token == NULL) { /* There was only one word */
	free(symbol);
        return 1;
    } else {  /* There was more than one word */
	printf("Error: too much argoments in line %d.\n", lineNumber);
	free(symbol);
        return 0;
    }
}

/*
Count the number of words in string.
return the number of words.
*/
int countWords(const char *str) {
    int inWord = 0; /*  0 means not in word, 1 means in word */
    int wordCount = 0; /*  Counter for the number of words */

    while (*str) {
        if (isspace(*str)) {
            inWord = 0; /*  Set inWord to 0 when a white space is encountered */
        } else {
            if (inWord == 0) {
                inWord = 1; /*  Set inWord to 1 when a new word starts */
                wordCount++;
            }
        }
        str++; /* Move to the next character in the string */
    }

    return wordCount; /*  Return the number of words */
}


/*
Check if the address method is valid.
If valid - return the word number.
If not valid - return -1 (-1 cant be a valid word).
*/
int isValidAdress(Instruction* commands, char *instruction , char *operand1 , char *operand2, int lineNumber)  
{
    short int op1; /* short int for source operand , if there is just destination operand - make 0 */
    short int op2; /* short int for destination operand */
    short int theWord; /* the word */
    short int commandCod; /* the command cod from the table in the maman */
    int i;
    op1 = getAdress(operand1, lineNumber);
    op2 = getAdress(operand2, lineNumber);

    if (op1 == -1 || op2 == -1) { 
	return -1;
    }
 
    /* Checking the the address method is valid for each command */ 
    for (i = 0; i < NUM_OF_COMMANDS; ++i) {
        if (strcmp(instruction, commands[i].name) == 0) {
	    commandCod = commands[i].commanCod;

            if (commandCod == 6) {
		if (op1 == 1 || op1 == 5) { 
		    printf("Error: Unvalid address method for '%s' command in line %d.\n", instruction, lineNumber);
		    return -1;
		}
    	    }
            if (commandCod == 0 || commandCod == 13 || (commandCod >= 2 && commandCod <= 11)) {
		if (op2 == 1) {
	 	   printf("Error: Unvalid address method for '%s' command in line %d.\n", instruction, lineNumber);
		    return -1;  
		}
    	    }
	}
    } /* and of 'for' */
    theWord = makeWord(commandCod, op1, op2);
    return theWord;
}

/* 
Get the address method (1/3/5).
If empty operand - return 0.
If not valid return (-1).
*/
int getAdress(char *firstOperand, int lineNumber)
{
    char r;
    char operand[MAX_OPERAND_LENGTH];
    
    if (strcmp(firstOperand,"") == 0) return 0; /* empty operand */

    sscanf(firstOperand, " %s ", operand);

    if (operand[0]=='@' && operand[1]=='r' && strlen(operand)==3) { /* check if valid register (adress type 5) */
	r = operand[2];
	if (r >= '0' && r <= '7') {
	    return 5;
	} else {
	    printf("Error: not a valid adress for '%s' operand on line %d.\n",operand, lineNumber);
	    return -1;
	}
    } else if (isValidSymbol(operand)) { /* check if valid symbol (adress type 3) */
	return 3;

    } else if (isValidInt(operand)) {
	return 1;
    }
    printf("Error: not a valid adress for '%s' operand on line %d.\n",operand, lineNumber);
    return -1; /* not valid operand */
}


/*
Make a word from 2 operands and command.
Return the cammand word by short int number.
*/
short int makeWord(short int commandCod, short int op1, short int op2)
{
    short int word = 0; /* initialize word as 0 */

    word |= (op1 << 9); /* put op1 into bits 9-11 */
    word |= (commandCod << 5); /* put commandCod into bits 5-8 */
    word |= (op2 << 2); /* put op2 into bits 2-4 */
    return word;
}


/*
Check how much word the line need.
Return the number of words (including the firs word).
*/
int numberOfWords(short int firstWord)
{
    int op1 = (firstWord >> 9) & 7; /* source operand */
    int op2 = (firstWord >> 2) & 7; /* destination operand */

    if (op1 == 0 && op2 == 0) { /* command without operands */
        return 1;
    } else if ((op1 == 0 || op2 == 0) || (op1 == 1 && op2 == 1) || (op1 == 5 && op2 == 5)) {  /* one operand or two operand in same adress method */
        return 2;
    }
    return 3; /* two operand in defrent adress method */
}
