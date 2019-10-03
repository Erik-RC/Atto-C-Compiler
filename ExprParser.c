/*
Author: Erik Rivas Contreras
Date: 5/4/2019
Title: Expression Parser
Purpose: To parse and reduce expression using a full shift-reduce table
*/

#include "atto-C.h"

//step 5 assign 8
struct stack_t {
	int token;
	int expr_after; //1 for nonterminal following and 0 for not
	int integer_value; //for value of the an integer  token 4/17/19
	struct symbol_t *symbol; //expression proj code addition 4/17/19
};

#define STACKMAX 1000
struct stack_t stack[STACKMAX];
int stacktop;

int jump_serial;

//String proj code addition
char *strings[STRINGS_MAX];
int string_serial;


//the array
int action[24][24] = {								  			   //str ; EOF       index
	{ AC,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,AC,SH,AC,AC }, //$ ROW	0
	{ RE,SH,SH,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,ER,RE,SH,SH,SH,SH,SH,RE,SH,RE,ER }, //* ROW	1 [1][1] was SH
	{ RE,SH,SH,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,ER,RE,SH,SH,SH,SH,SH,RE,SH,RE,ER }, // \ ROW	2
	{ RE,RE,SH,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,SH,RE,SH,SH,SH,SH,SH,RE,SH,RE,ER }, // + ROW	3 //col 1 changed from ER to RE
	{ RE,SH,SH,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,SH,RE,SH,SH,SH,SH,SH,RE,SH,RE,ER }, // - ROW	4
	{ RE,SH,SH,SH,SH,RE,RE,RE,RE,RE,RE,SH,SH,ER,SH,SH,SH,SH,SH,SH,RE,SH,RE,ER }, // < ROW	5
	{ RE,SH,SH,SH,SH,RE,RE,RE,RE,RE,RE,SH,SH,ER,SH,SH,SH,SH,SH,SH,RE,SH,RE,ER }, // > ROW	6
	{ RE,SH,SH,SH,SH,RE,RE,RE,RE,RE,RE,SH,SH,ER,SH,SH,SH,SH,SH,SH,RE,SH,RE,ER }, // <= ROW	7
	{ RE,SH,SH,SH,SH,RE,RE,RE,RE,RE,RE,SH,SH,ER,SH,SH,SH,SH,SH,SH,RE,SH,RE,ER }, // >= ROW	8
	{ RE,SH,SH,SH,SH,SH,SH,SH,SH,RE,RE,RE,RE,ER,SH,SH,SH,SH,SH,SH,RE,SH,RE,ER }, // == ROW	9 //modified 
	{ RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,ER,SH,SH,SH,SH,SH,SH,ER,SH,RE,ER }, // != ROW	10 //column 15 changed from RE to SH 4/18/19
	{ RE,SH,SH,SH,SH,RE,RE,RE,RE,RE,RE,RE,RE,ER,SH,SH,SH,RE,RE,SH,AC,RE,RE,ER }, // AND ROW 11 //column 20 changed from ER to AC, SH
	{ RE,SH,SH,SH,SH,RE,RE,RE,RE,RE,RE,RE,RE,ER,SH,SH,SH,RE,RE,SH,AC,SH,RE,ER }, // OR ROW	12 //column 20 changed from ER to AC, SH
	{ RE,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,ER,ER,SH,SH,SH,SH,SH,ER,SH,RE,ER }, // = ROW	13
	{ RE,SH,SH,SH,SH,RE,RE,RE,RE,RE,RE,RE,RE,ER,RE,SH,SH,SH,SH,SH,RE,SH,RE,ER }, //COMMA ROW 14 col 14 changed ER to RE. col 20 SH to RE 4/22/19
	{ RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,SH,RE,ER,RE,RE,RE,SH,RE,RE,RE,ER }, //IDENT ROW 15 col 19 changed from ER to SH 4/22/19
	{ RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,ER,RE,RE,ER,RE,RE,ER,RE,ER,RE,ER }, //NUM ROW 16
	{ RE,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,ER,SH,SH,SH,SH,SH,SH,RE,SH,RE,ER }, //! ROW	17
	{ RE,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,ER,SH,SH,SH,SH,SH,SH,RE,SH,RE,ER }, //- ROW	18
	{ ER,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,EQ,SH,SH,ER,ER }, // ( ROW	19
	{ RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,ER,RE,ER,ER,RE,RE,ER,RE,ER,RE,ER }, // ) ROW	20
	{ RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,RE,ER,RE,RE,ER,RE,RE,ER,RE,ER,RE,ER }, //String row 21
	{ AC,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,SH,AC,AC }, //semi colon ROW 22
	{ ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,ER,AC }, //EOF TOK ROW 23
};	// 0 1  2  3  4   5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23





/*******************************************
procedure: add_string()
purpose: add string to the list for putting in CONST segment
returns the serial number for the string
*/
int add_string(char *string) {
	if (string_serial >= STRINGS_MAX) {
		printf("FATAL ERROR: Too many strings. \n");
		exit(33);
	}
	strings[string_serial] = (char*)calloc(strlen(string) + 1, sizeof(char));
	strcpy(strings[string_serial], string);
	return string_serial++;
}


/*
The arithmetic parsing function, using shift-reduce parsisng.
Returns: the oken immediately after the expression
*/

int expr_parser() {
	//int lookahead; //the current lookahead type
	fprintf(asmfile, "; Line%d\n", line_no);

	//initializing the stack
	stack[0].token = DOLLAR_TOK;
	stack[0].expr_after = 0;
	stacktop = 0;

	//initializing lookahead
	//lookahead = lexer();

	// Kludge for unary minus
	if ((lookahead == MINUS_TOK) && !stack[stacktop].expr_after)
		lookahead = UNARY_MINUS_TOK;

	if ((lookahead == UNARY_MINUS_TOK) && stack[stacktop].expr_after)
		lookahead = MINUS_TOK;

	while (1) {
		switch (action[stack[stacktop].token-101][lookahead-101])
		{

			//shift code
		case SH:
		case EQ:
			if (print_flag) printf("Shift token %d %s line %d\n", lookahead, lexeme, line_no);
			stacktop++;
			//test to see if stack has overflowed
			if (stacktop > STACKMAX) {
				printf("ERROR: Stack has overflowed. \n");
				break;
			}

			//push lookahead token on the stack
			stack[stacktop].token = lookahead;
			stack[stacktop].expr_after = 0;

			//symbol table and expression proj code addition 4/17/19
			if (lookahead == IDENT_TOK)
			{
				stack[stacktop].symbol = symbol_lookup(lexeme);
				if (stack[stacktop].symbol == NULL)
					stack[stacktop].symbol = push_symbol(lexeme);
			}

			//expressions code addition 4/17/19
			if (lookahead == INTEGER_TOK)
				stack[stacktop].integer_value = atoi(lexeme);

			//Strings proj addition
			if (lookahead == STRING_TOK)
				stack[stacktop].integer_value = add_string(lexeme);

			//get next lookahead from lexer
			lookahead = lexer();
			break;

		case RE:
#pragma region Reduce code

			if (print_flag) printf("Reduce \n");

			switch (stack[stacktop].token){
			case DOLLAR_TOK:
				if (stacktop == 0 && stack[0].expr_after ==1) {
					return lookahead;
				}
				else {
					printf("ERROR: There is not a following expression. ($ TOK error). \n");
					exit(1); //exit the program
				}
				break;


			case STAR_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: STAR without following expression, line %d\n", line_no);
					exit(2);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: STAR without preceding expression, line %d\n", line_no);
					exit(3);
				}

				//assembly code, multiplication
				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    imul eax,ebx\n");

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case SLASH_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: SLASH without following expression, line %d\n", line_no);
					exit(4);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: SLASH without preceding expression, line %d\n", line_no);
					exit(5);
				}

				//assembly code, division
				fprintf(asmfile, "    mov ebx,eax\n");
				fprintf(asmfile, "    pop eax\n");
				fprintf(asmfile, "    cdq\n"); //sign extended eax to edx
				fprintf(asmfile, "    idiv ebx\n");
				//quotient left in eax	

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case PLUS_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: PLUS without following expression, line %d\n", line_no);
					exit(6);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: PLUS without preceding expression, line %d\n", line_no);
					exit(7);
				}

				// assembly code, adding top stack values
				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    add eax, ebx\n");


				stacktop--;
				stack[stacktop].expr_after = 1;

			
				break;

			case MINUS_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: MINUS without following expression, line %d\n", line_no);
					exit(8);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: MINUS without preceding expression, line %d\n", line_no);
					exit(9);
				}

				//assembly code, want to substract eax from second stack value
				fprintf(asmfile, "    mov ebx,eax\n");
				fprintf(asmfile, "    pop eax\n");
				fprintf(asmfile, "    sub eax,ebx\n");

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case LT_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: LESS THAN without following expression, line %d\n", line_no);
					exit(10);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: LESS THAN without preceding expression, line %d\n", line_no);
					exit(11);
				}

				//assembly code generation
				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    cmp ebx,eax\n");
				fprintf(asmfile, "    jl  SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    mov eax,0\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
				fprintf(asmfile, "jumper@%d:\n", jump_serial);
				fprintf(asmfile, "    mov eax,1\n");
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
				jump_serial += 2;

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case GT_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: GREATER THAN without following expression, line %d\n", line_no);
					exit(12);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: GREATER THAN without preceding expression, line %d\n", line_no);
					exit(13);
				}

				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    cmp ebx,eax\n");
				fprintf(asmfile, "    jg  SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    mov eax,0\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
				fprintf(asmfile, "jumper@%d:\n", jump_serial);
				fprintf(asmfile, "    mov eax,1\n");
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
				jump_serial += 2;

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case LT_EQ_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: <= without following expression, line %d\n", line_no);
					exit(14);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: <= without preceding expression, line %d\n", line_no);
					exit(15);
				}

				//assembly code generation 
				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    cmp ebx,eax\n");
				fprintf(asmfile, "    jle  SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    mov eax,0\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
				fprintf(asmfile, "jumper@%d:\n", jump_serial);
				fprintf(asmfile, "    mov eax,1\n");
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
				jump_serial += 2;

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case GT_EQ_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: >= without following expression, line %d\n", line_no);
					exit(16);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: >= without preceding expression, line %d\n", line_no);
					exit(17);
				}


				//assembly code generation 
				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    cmp ebx,eax\n");
				fprintf(asmfile, "    jge  SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    mov eax,0\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
				fprintf(asmfile, "jumper@%d:\n", jump_serial);
				fprintf(asmfile, "    mov eax,1\n");
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
				jump_serial += 2;

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case COMP_TOK:
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: COMPARE without preceding expression, line %d\n", line_no);
					exit(18);
				}

				//assembly code generation 
				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    cmp ebx,eax\n");
				fprintf(asmfile, "    je  SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    mov eax,0\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
				fprintf(asmfile, "jumper@%d:\n", jump_serial);
				fprintf(asmfile, "    mov eax,1\n");
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
				jump_serial += 2;


				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case NOT_EQ_TOK:
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: NOT EQUAL TO without preceding expression, line %d\n", line_no);
					exit(20);
				}

				//assembly code generation 
				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    cmp eax,ebx\n");
				fprintf(asmfile, "    jne  SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    mov eax,0\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
				fprintf(asmfile, "jumper@%d:\n", jump_serial);
				fprintf(asmfile, "    mov eax,1\n");
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
				jump_serial += 2;

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case AND_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: AND without following expression, line %d\n", line_no);
					exit(21);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: AND without preceding expression, line %d\n", line_no);
					exit(22);
				}

				//assembly code generation 
				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    cmp eax,0\n");
				fprintf(asmfile, "    je  SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    cmp ebx,0\n");
				fprintf(asmfile, "    je SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    mov eax,1\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
				fprintf(asmfile, "jumper@%d:\n", jump_serial);
				fprintf(asmfile, "    mov eax,0\n");
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
				jump_serial += 2;

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case OR_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: OR without following expression, line %d\n", line_no);
					exit(23);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: OR without preceding expression, line %d\n", line_no);
					exit(24);
				}

				//assembly code generation 
				fprintf(asmfile, "    pop ebx\n");
				fprintf(asmfile, "    cmp eax,0\n");
				fprintf(asmfile, "    je  SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    cmp ebx,0\n");
				fprintf(asmfile, "    je SHORT jumper@%d\n", jump_serial+2); //jump 3
				fprintf(asmfile, "    mov eax,1\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 3); //jump to 4, end

				//jumper 1
				fprintf(asmfile, "jumper@%d:\n", jump_serial);
				fprintf(asmfile, "    cmp ebx,0\n");
				fprintf(asmfile, "    je SHORT jumper@%d\n", jump_serial + 1);
				fprintf(asmfile, "    mov eax,1\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 3); //jump to 4, end

				//jumper 2
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);
				fprintf(asmfile, "    mov eax,0\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 3); //jump to 4, end

				//jumper 3
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 2);
				fprintf(asmfile, "    mov eax,1\n");

				//jumper 4
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 3);


				jump_serial += 2;

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case EQ_TOK:
				//expression proj code addition
				fprintf(asmfile, "    mov DWORD PTR %s,eax\n", stack[stacktop - 1].symbol->asm_name);

				//need to have identifier in previous position
				if (stack[stacktop-1].token == IDENT_TOK) {
					//pop two structures, already minus-ed above
					stacktop--;
					stack[stacktop].expr_after = 1;
					stacktop--;
					stack[stacktop].expr_after = 1;
					break;
				}
				else { //there wasn't an identifier
					printf("ERROR: No Identifier before assignment operator. Line %d\n", line_no);
					exit(25);
				}

				break;

			case COMMA_TOK:
				//checking before and after expressions
				if (!stack[stacktop].expr_after) {
					printf("ERROR: COMMA without following expression, line %d\n", line_no);
					exit(26);
				}
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: COMMA without preceding expression, line %d\n", line_no);
					exit(27);
				}

				if (stack[stacktop - 1].token == L_PAREN_TOK)
					stack[stacktop - 1].expr_after++;
				else
					stack[stacktop - 1].expr_after = 1;
				stacktop--;
				break;


			case IDENT_TOK:
				//need to make sure identifier has been declared
				if (stack[stacktop].symbol->symbol_kind == 0)
				{
					printf("Undeclared identifier '%s', line %d\n",
						stack[stacktop].symbol->name, line_no);
					exit(28);
				}
				if (stack[stacktop].symbol->symbol_kind == FUNCTION)
				{
					printf("Function name '%s' used as a variable, line %d\n",
						stack[stacktop].symbol->name, line_no);
					exit(29);
				}


				//cannot have expressions before or after
				if (stack[stacktop].expr_after || stack[stacktop - 1].expr_after) {
					printf("ERROR: IDENT with preceding and following expression. Line %d\n", line_no);
					exit(30);
				}

				//for assembly code
				fprintf(asmfile, "    push eax\n");
				fprintf(asmfile, "    mov  eax, DWORD PTR %s\n", stack[stacktop].symbol->asm_name);

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case INTEGER_TOK:
				//cannot have expressions before or after
				if (stack[stacktop].expr_after || stack[stacktop - 1].expr_after) {
					printf("ERROR: NUM with preceding and following expression. Line %d\n", line_no);
					exit(40);
				}

				//expression proj code addition
				fprintf(asmfile, "    push eax\n");
				fprintf(asmfile, "    mov  eax,%d\n", stack[stacktop].integer_value);

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case EXCLAM_TOK: //logical not
				//cannot have expressions before or after
				if (!stack[stacktop].expr_after) {
					printf("ERROR: NOT without following expression, line %d\n", line_no);
					exit(41);
				}
				if (stack[stacktop - 1].expr_after) {
					printf("ERROR: NOT with preceding expression, line %d\n", line_no);
					exit(42);
				}

				//assembly code
				fprintf(asmfile, "    cmp eax,0\n");
				fprintf(asmfile, "    jne SHORT jumper@%d\n", jump_serial);
				fprintf(asmfile, "    mov eax, 1\n");
				fprintf(asmfile, "    jmp SHORT jumper@%d\n", jump_serial + 1);
				fprintf(asmfile, "jumper@%d: \n",jump_serial);
				fprintf(asmfile, "    mov eax,0\n");
				fprintf(asmfile, "jumper@%d: \n", jump_serial + 1);

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case UNARY_MINUS_TOK:

				//must have expression after but not before
				if (stack[stacktop - 1].expr_after) {
					printf("ERROR: UNARY MINUS with preceding expression. Line %d\n", line_no);
					exit(43);
				}
				else if (stack[stacktop].expr_after) {
					//assembly code
					fprintf(asmfile, "    neg eax\n");

					stacktop--;
					stack[stacktop].expr_after = 1;
					break;
				}
				break; 


			case L_PAREN_TOK:
				//reducing a left parenthesis on top of stack is always an error
				printf("ERROR: Reducing left parenthesis on top of stack. Line %d\n", line_no);
				
				break;

			case R_PAREN_TOK:

				//Function call project code
				if (stack[stacktop - 1].token != L_PAREN_TOK) {
					exit(101); //it says it should already have that but I check stacktop -2...
				}

				if (stack[stacktop - 2].token != IDENT_TOK) { //then this is ordinary parenthesis, do what it did before
					//must have expression before and left parenthesis before and no expression after
					if (!stack[stacktop - 1].expr_after && stack[stacktop - 2].token != L_PAREN_TOK) {
						printf("ERROR: LEFT_PAREN has invalid expression either before, or expression after. Line %d\n", line_no);
						exit(44);
					}

					else if (stack[stacktop].expr_after) {
						printf("ERROR: LEFT_PAREN has invalid expression either before, or expression after. Line %d\n", line_no);
						exit(45);
					}

					// need to pop off two structures
					stacktop--;
					stack[stacktop].expr_after = 1;

					//second structure
					stacktop--;
					stack[stacktop].expr_after = 1;

					break;
				}
				
				//if stack[stacktop-2] is identifier, then we have a function call. Proceed with code below

				//check that stacktop-3 does not have an expression after it
				if (stack[stacktop - 3].expr_after) {
					exit(102); //some error
				}


				if (stack[stacktop - 2].symbol->symbol_kind == 0)
				{ // must be an external function, such as printf
					stack[stacktop - 2].symbol->symbol_kind = FUNCTION;
					sprintf(stack[stacktop - 2].symbol->asm_name, "_%s", stack[stacktop - 2].symbol->name);
					fprintf(asmfile, "EXTRN %s:PROC\n", stack[stacktop - 2].symbol->asm_name);
				}

				//check that function name is not another identifier
				//if (stack[stacktop - 2].symbol->symbol_kind == 0 || stack[stacktop - 2].symbol->symbol_kind == FUNCTION)
				if (stack[stacktop - 2].symbol->symbol_kind == FUNCTION) {

					//mark identifier as a function call and print assembly code
					stack[stacktop - 2].symbol->symbol_kind = FUNCTION;
					sprintf(stack[stacktop - 2].symbol->asm_name, "_%s",stack[stacktop - 2].symbol->name);

					//have stack pointer point to the first arguement
					if (stack[stacktop - 1].expr_after >= 1)
						fprintf(asmfile, "   push eax\n");
					for (int n = 1; n < stack[stacktop - 1].expr_after; n++)
						fprintf(asmfile, "  push DWORD PTR [esp+%d]\n", 8 * n - 4);

					//call function and clean up stack
					fprintf(asmfile, "   call %s\n", stack[stacktop - 2].symbol->asm_name);
					if (stack[stacktop - 1].expr_after >= 1)
						fprintf(asmfile, "   add  esp,%d\n", 4 * (2 * stack[stacktop - 1].expr_after - 1));

					//fix expression stack
					stacktop -= 3; stack[stacktop].expr_after = 1;
					break;
				}
				
			
			case SEMICOLON_TOK:
				//i think it can have both an expression before or after
				//but must definitely have one before
				if (!stack[stacktop - 1].expr_after) {
					printf("ERROR: SEMICOLON without preceding expression. Line %d\n", line_no);
					exit(46);
				}

				stacktop--;
				stack[stacktop].expr_after = 1;
				break;

			case EOF_TOK:
				stacktop--;
				stack[stacktop].expr_after = 1;
				break; //to be implemented


			case STRING_TOK: //was	QUOTE_TOK
				if (stack[stacktop].expr_after)
				{
					printf("ERROR: Illegal expression after string, line %d\n", line_no);
					exit(48);
				}
				if (stack[stacktop - 1].expr_after)
				{
					printf("ERROR: Illegal expression before string, line %d\n", line_no);
					exit(49);
				}
				fprintf(asmfile, "   push eax\n");
				fprintf(asmfile, "   mov  eax, OFFSET string@%d\n",
					stack[stacktop].integer_value);
				stacktop--; stack[stacktop].expr_after = 1;
				break;

			default:
				printf("ERROR: Bad stack top token %d\n", stack[stacktop].token);
				exit(47);
				break;
			}
			break;

			

#pragma endregion


		case AC:
			//expression proj code addition 4/17/19
			fprintf(asmfile, "    pop ebx        ; clean up stack \n");
			return lookahead;

		case ER:
			printf("Parse error, Line %d, lookahead token %s\n", line_no, lexeme);
			exit(50);
			
		default:
			break;
		}
	}
}
