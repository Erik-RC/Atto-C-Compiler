/*
Title: Main Parser
Author: Erik Rivas Contreras
Date created: 3/21/2019
Purpose:to produce code for the compiler project that 
will handle all the initial setting up and final finishing of a compilation of 
an Atto-C source file.
*/

#include "atto-C.h"

int lookahead; //the lookahead token

//prototypes
void external_definition(void);
void function_definition(struct symbol_t *s);
void parameter_list(void);
void data_definition(struct symbol_t *s);
void data_definition_2(struct symbol_t *s);
void declaration_list(void);
void compound_statement(void);
void statement_list(void);
void statement(void);
void expression_statement(void);
void if_statement(void);
void while_statement(void);
void do_statement(void);
void continue_statement(void);
void break_statement(void);
void return_statement(void);
int binary_operation(int lookAheadToken);

//var project code addition
int local_offset; //used to keep track of local offset

//flow control code addition. For "continue" asm keyword
#define LOOPS_MAX 10000
int loop_stack[LOOPS_MAX];
int loop_stacktop;
int return_target;

void program() {
	//symbol table code 4/16/19
	int symbol_table_spot;
	if (print_flag) printf("Entering program, line %d.\n", line_no);
	symbol_table_spot = start_scope();


	//MainParser code
	lookahead = lexer(); //get first token of program
	while (lookahead != EOF_TOK) {
		external_definition();
	}
	end_scope(symbol_table_spot);
	if (print_flag) printf("Leaving program, line %d.\n", line_no);
}


void external_definition(){
	if (print_flag) printf("Entering external definition. \n");
	
	//var project 4/16/19
	struct symbol_t *s;
	//s = push_symbol(lexeme);

	//check for type specifier
	if (lookahead == INTEGER_TOK) {
		lookahead = lexer(); //eat integer
	}
	else {
		printf("ERROR: Function definition missing type specifier. Line %d.\n", line_no);
		exit(1);
	}

	//check for identifier
	if (lookahead == IDENT_TOK) {
		s = push_symbol(lexeme); //symbol table 4/16/19
		lookahead = lexer(); //eat identifier
	}
	else {
		printf("ERROR: Function definition missing identifier. Line %d.\n", line_no);
		exit(2);
	}

	//decide here whether it will be a function or data definition
	if (lookahead == L_PAREN_TOK) {
		function_definition(s); //going to be a function definition
	}
	else { //most likely just a data definition
		data_definition(s);
	}
	
	
}

void function_definition(struct symbol_t *s){
	if(print_flag) printf("Entering function definition. \n");

	//procedure project code additition
	s->symbol_kind = FUNCTION;
	sprintf(s->asm_name, "_%s", s->name);

	fprintf(asmfile, "\nPUBLIC %s\n", s->asm_name);
	fprintf(asmfile, ";    COMDAT %s\n", s->asm_name);

	//var project 
	fprintf(asmfile, "\n_TEXT    SEGMENT\n");
	fprintf(asmfile, "%s    PROC          ; COMDAT\n\n", s->asm_name); //proc project code addition

	//saving old frame pointer and current frame pointer
	fprintf(asmfile, "   push  ebp\n");
	fprintf(asmfile, "   mov   ebp,esp\n");

	fprintf(asmfile, "   sub   esp,%s_locals$\n", s->asm_name);

	//saving registers that are supposed to be preserved (not like newer compilers)
	fprintf(asmfile, "   push  ebx\n");
	fprintf(asmfile, "   push  esi\n");
	fprintf(asmfile, "   push  edi\n");


	//symbol table code 4/16/19
	int symbol_table_spot;
	symbol_table_spot = start_scope();
	


	//must have parenthesis, may not contain parameters though, it is optional
	if (lookahead == L_PAREN_TOK) {
		lookahead = lexer(); //eat left parenthesis
	}
	else {
		printf("ERROR: Function definition missing opening parenthesis. Line %d.\n", line_no);
	}

	if (lookahead == INTEGER_TOK) { //seems like it may have parameters
		parameter_list();

		if (lookahead != R_PAREN_TOK) { //must now have closing parenthesis
			printf("ERROR: Function definition missing closing parenthesis. Line %d.\n", line_no);
			exit(3);
		}
	}

	//must have closing parenthesis
	if (lookahead == R_PAREN_TOK) {
		lookahead = lexer(); //eat closing parenthesis
	}
	else { //does not have one
		printf("ERROR: Function definition missing closing parenthesis. Line %d.\n");
		exit(4);
	}

	//if it did, now must have compound statement
	local_offset = -4;

	//flow control
	return_target = jump_serial;
	jump_serial++;
	compound_statement();
	fprintf(asmfile, "$return@%d:\n", return_target); //flow control
	end_scope(symbol_table_spot);
	

	//pop preserved registers in reverse order
	fprintf(asmfile, "   pop   edi\n");
	fprintf(asmfile, "   pop   esi\n");
	fprintf(asmfile, "   pop   ebx\n");

	//pop local variables at once
	fprintf(asmfile, "   mov esp, ebp\n");

	//restore old stack frame pointer
	fprintf(asmfile, "   pop   ebp\n");

	//return after popping 0 bytes from the stack
	fprintf(asmfile, "   ret   0\n");

	//end procedure
	fprintf(asmfile, "%s   ENDP\n", s->asm_name);

	//define constant size for local variables
	fprintf(asmfile, "%s_locals$ = %d\n", s->asm_name, -local_offset - 4);

	fprintf(asmfile, "_TEXT    ENDS\n");

	if (print_flag) printf("Leaving function definition. \n");

}

void parameter_list(){
	if (print_flag) printf("Entering parameter list, line %d.\n", line_no);
	struct symbol_t *ss; //local variable for symbol table pointer
	int offset = 8; //offset from EBP

	//for speed purposes, I will do the loop instead
	while (lookahead == INTEGER_TOK) {
		lookahead = lexer(); //eat int
		
		if (lookahead == IDENT_TOK) {
			ss = push_symbol(lexeme); //saving pointer to the symbol table entry
			sprintf(ss->asm_name, "_%s$[ebp]", ss->name);
			ss->symbol_kind = FUNCTION_PARAMETER;
			fprintf(asmfile, "_%s$ = %d\n", ss->name, offset);
			offset += 4;

			lookahead = lexer(); //eat identifier
		}
		
		if (lookahead == COMMA_TOK) {
			lookahead = lexer(); //eat comma
		}
	}
	if (print_flag) printf("Leaving parameter list, line %d.\n", line_no);
}

void data_definition(struct symbol_t *s) { 

	if (print_flag) printf("Entering data definition, line %d.\n", line_no);
	//var project 4/16/19
	fprintf(asmfile, "\n_DATA    SEGMENT\n");

	struct symbol_t *ss; //local

	//generating asm name for first identifier
	s->asm_name[0] = '_';
	strncpy(s->asm_name + 1, s->name, MAX_LEXEME);
	fprintf(asmfile, "COMM  %s:DWORD\n", s->asm_name); //Printing name to assembly code
	s->symbol_kind = GLOBAL_VARIABLE;

	
	while (lookahead == COMMA_TOK) {
		lookahead = lexer();
		if (lookahead == IDENT_TOK) {
			ss = push_symbol(lexeme); //saving the pointer
			ss->asm_name[0] = '_';
			strncpy(ss->asm_name + 1, ss->name, MAX_LEXEME);
			fprintf(asmfile, "COMM  %s:DWORD\n", ss->asm_name); //Printing name to assembly code
			ss->symbol_kind = LOCAL_VARIABLE;

			//continue
			lookahead = lexer(); //eat identifier
		}
		else {
			//error
			printf("ERROR: in first else, data_definition(), MainParser.c. \n");
			exit(5);
		}
	}

	if (lookahead == SEMICOLON_TOK) {
		lookahead = lexer(); //eat semicolon
	}
	else {
		printf("ERROR: in second else, data_definition(), MainParser.c. \n");
		exit(6);
	}

	if (print_flag) printf("Leaving data definition, line %d.\n", line_no);
	fprintf(asmfile, "_DATA    ENDS\n");
}    

void data_definition_2(struct symbol_t *s) { // see note h below
	if (print_flag) printf("Entering data definition 2, line %d.\n", line_no);

	//generating asm name for first identifier
	int serial_number = local_offset / (-4);
	sprintf(s->asm_name, "_%s$%d[ebp]", s->name, serial_number);
	s->symbol_kind = LOCAL_VARIABLE;
	fprintf(asmfile, "_%s$%d = %d\n", s->name, serial_number, local_offset);
	local_offset -= 4;

	//for the rest of the variables
	struct symbol_t *ss;

	while (lookahead == COMMA_TOK) {
		lookahead = lexer();

		if (lookahead == IDENT_TOK) {
			//var project code addition
			ss = push_symbol(lexeme); //symbol table 4/16/19
			int serial_number = local_offset / (-4);
			sprintf(ss->asm_name, "_%s$%d[ebp]", ss->name, serial_number);
			ss->symbol_kind = LOCAL_VARIABLE;
			fprintf(asmfile, "_%s$%d = %d\n", ss->name, serial_number, local_offset);
			local_offset -= 4;

			lookahead = lexer(); //eat identifier
		}
		else {
			//error
			printf("ERROR: in first else, data_definition2(), MainParser.c. \n");
			exit(8);
		}
	}

	if (lookahead == SEMICOLON_TOK) {
		lookahead = lexer(); //eat semicolon
	}
	else {
		printf("ERROR: in second else, data_definition(), MainParser.c. \n");
		exit(9);
	}

	if (print_flag) printf("Leaving data definition 2, line %d.\n", line_no);
}  

void declaration_list() {
	if (print_flag) printf("Entering declaration list, line %d.\n", line_no);
	//var project 4/16/19
	struct symbol_t *s;
	//s = push_symbol(lexeme);
	while (lookahead == INTEGER_TOK) {
		lookahead = lexer(); //eat integer token

		//next token should most likely be an ident
		if (lookahead == IDENT_TOK) {
			s = push_symbol(lexeme); //symbol table 4/16/19
			lookahead = lexer(); //eat ident token
		}
		else {
			printf("ERROR: Local variable(s) declaration missing semicolon. Line %d.\n", line_no);
			exit(10);
		}

		data_definition_2(s);
		//data_definition(s);
	}
	if (print_flag) printf("Leaving declaration list, line %d.\n", line_no);
}

void compound_statement() {
	//symbol table code
	int symbol_table_spot = start_scope();

	if (print_flag) printf("Entering compound statement, line %d.\n", line_no);

	if (lookahead != L_cBRACKET_TOK)
	{
		printf("ERROR: Missing left curly brace for compound statement. Line %d.", line_no);
		exit(11);
	}
	lookahead = lexer();  // eat left curly brace

	declaration_list();  // do any local variable declarations

	statement_list();

	if (lookahead != R_cBRACKET_TOK)
	{
		printf("ERROR: Missing right curly brace at end of compound statement. Line %d.",
			line_no);
		exit(12);
	}
	lookahead = lexer();  // eat right curly brace   

	end_scope(symbol_table_spot);
	if (print_flag) printf("Leaving compound statement, line %d.\n", line_no);

}
void statement_list() {
	if (print_flag) printf("Entering statement list, line %d.\n", line_no);

	while (lookahead != R_cBRACKET_TOK) {
		statement();
	}
	if (print_flag) printf("Leave statement list, line %d.\n", line_no);
}

void statement() {
	if (print_flag) printf("Entering statement, line %d.\n", line_no);
	//lookahead = lexer();
	//assuming lookahead token is retrieved from outside of this call
	if (lookahead == L_cBRACKET_TOK) { //start of compound statement
		compound_statement();
	}
	
	//if statement
	else if (lookahead == IF_TOK) {
		if_statement();
	}
	//if-else statement
	//to be implemented

	//while statemment
	else if(lookahead == WHILE_TOK) {
		while_statement();
	}

	//do-while statement
	else if (lookahead == DO_TOK) {
		do_statement();
	}

	//continue
	else if (lookahead == CONTINUE_TOK) {
		continue_statement();
	}

	//break
	else if (lookahead == BREAK_TOK) {
		break_statement();
	}

	//return
	else if (lookahead == RETURN_TOK) {
		return_statement();
	}

	//semi-colon
	else if (lookahead == SEMICOLON_TOK) {
		lookahead = lexer(); //eat semicolon
	}

	//wait, must be more specific here
	else if(lookahead == IDENT_TOK || lookahead == INTEGER_TOK){ //probably just an expression then, identifier or integer token type
		expression_statement();
	}

	else {
		printf("Syntax error. Line %d.\n", line_no);
		exit(50);
	}

	if (print_flag) printf("Leaving statement function.\n");

}

void expression_statement() {
	if (print_flag) printf("Entering expression statement, line %d.\n", line_no);
	//calling expression parser
	expr_parser();

	//expression might be something like printf(x&&4); so it will come back with Right parenthesis
	if (lookahead == R_PAREN_TOK) {
		lookahead = lexer(); //eat right parenthesis
	}

	if (lookahead != SEMICOLON_TOK) {
		printf("Expression missing semicolon at the end. Line %d.\n", line_no);
		exit(13);
	}
	
	if (print_flag) printf("Leaving expression statement.\n");

}



void if_statement() {
	if (print_flag) printf("Entering if_statement.\n");
	
	//flow control code
	int if_target;
	int else_target;
	
	lookahead = lexer(); //eat the if keyword
	if (lookahead != L_PAREN_TOK) {
		printf("ERROR: IF STATEMENT missing left parenthesis. Line %d.\n", line_no);
		exit(14);
	}
	
	lookahead = lexer();
	//expression_statement();
	expr_parser();
	
	//flow control code 5/3/19
	fprintf(asmfile, "   cmp eax, 0\n");
	fprintf(asmfile, "   je  $if@%d\n", jump_serial);
	if_target = jump_serial;
	jump_serial++;

	if (lookahead != R_PAREN_TOK) {
		printf("ERROR: IF STATEMENT missing right parenthesis. Line %d.\n", line_no);
		exit(15);
	}

	lookahead = lexer(); //eat right parenthesis

	//if there is curly brace, call compound statement, else if statement is a one-liner
	if (lookahead == L_cBRACKET_TOK) {
		compound_statement();

		if(lookahead != ELSE_TOK)
			fprintf(asmfile, "$if@%d: \n", if_target); //flow control code. In case there is no else
		
	}
	else { //the if statement has a one-liner code, no need for curly braces
		statement();

		//here it comes back with a semi-colon, but may also have a one-liner else statement
		lookahead = lexer(); //so, eat semicolon

		if (lookahead != ELSE_TOK)
			fprintf(asmfile, "$if@%d: \n", if_target); //flow control code. In case there is no else
	}
	

	//here check if we got the ELSE keyword
	if (lookahead == ELSE_TOK) {
		lookahead = lexer(); //eat else keyword

		//flow control code addition
		fprintf(asmfile, "   jmp  $if@%d\n", jump_serial);
		else_target = jump_serial;
		jump_serial++;
		fprintf(asmfile, "$if@%d: \n", if_target);

		//if there is curly brace, call compound statement, else if statement is a one-liner
		if (lookahead == L_cBRACKET_TOK) {
			compound_statement();
			fprintf(asmfile, "$if@%d: \n", else_target); //flow control code
		}
		else {
			statement();
			fprintf(asmfile, "$if@%d: \n", else_target); //flow control code
		}
	}
	if (print_flag) printf("Leaving if_statement.\n");
}

void while_statement() {
	if (print_flag) printf("Entering while_statement.\n");

	

	lookahead = lexer(); //eat keyword while
	if (lookahead != L_PAREN_TOK) {
		printf("ERROR: WHILE STATEMENT missing left parenthesis. Line %d.\n", line_no);
		exit(16);
	}

	lookahead = lexer();

	int top = jump_serial;
	jump_serial++; //incrementing for second label counter
	int bottom = jump_serial;
	jump_serial++;
	int continue_target = jump_serial;
	jump_serial++;

	fprintf(asmfile, "$while@%d: \n", top); //flow control. while 1
	fprintf(asmfile, "$cont@%d: \n", continue_target);

	//asm continue code
	loop_stack[++loop_stacktop] = continue_target;

	expr_parser();

	fprintf(asmfile, "   cmp eax, 0\n");
	fprintf(asmfile, "   je  $while@%d\n", bottom); //while 2



	//check that the parser came back with right parenthesis, marking end of while condition statement
	if (lookahead != R_PAREN_TOK) {
		printf("ERROR: WHILE STATEMENT missing right parenthesis at the end of condition. Line %d.\n", line_no);
		exit(17);
	}
	else {
		//eat right parenthesis
		lookahead = lexer();
	}
	
	//if the lookahead is a left curly brace, call compound statement
	//else it is just a single statement

	if (lookahead == L_cBRACKET_TOK) {
		compound_statement();
		fprintf(asmfile, "   jmp  $while@%d\n", top); //while 1
	}
	else {
		statement();
		fprintf(asmfile, "   jmp  $while@%d\n", top); //while 1
	}

	fprintf(asmfile, "$while@%d: \n",bottom); //flow control. while 2
	fprintf(asmfile, "$break@%d: \n", continue_target);
	
	loop_stacktop--; //flow control

	if (print_flag) printf("Leaving while statement.\n");
}

void do_statement() {
	if (print_flag) printf("Entering do_statement.\n");

	lookahead = lexer(); //eat keyword do, not quite sure if there is something else that is supposed to follow do

	int do_jump = jump_serial;
	fprintf(asmfile,"$do@%d: \n", do_jump); //flow control
	jump_serial++;

	int continue_jump = jump_serial;
	jump_serial++;
	loop_stack[++loop_stacktop] = continue_jump;
	statement();

	fprintf(asmfile, "$cont@%d: \n", continue_jump); //flow control
	
	if (lookahead != WHILE_TOK && lookahead == SEMICOLON_TOK) {
		lookahead = lexer(); //it is a one-liner do statement, no curly braces. Must eat semicolon
	}
	
	//lookahead must be while keyword
	if (lookahead != WHILE_TOK) {
		printf("ERROR: DO STATEMENT not followed by while statement. Line %d.\n", line_no);
		exit(18);
	}
	
	lookahead = lexer(); //eat while keyword
	if (lookahead != L_PAREN_TOK) {
		printf("ERROR: WHILE STATEMENT missing left parenthesis. Line %d.\n", line_no);
		exit(19);
	}

	lookahead = lexer();
	expr_parser();

	fprintf(asmfile, "   cmp eax, 0\n");
	fprintf(asmfile, "   jne $do@%d\n", do_jump);
	fprintf(asmfile, "$break@%d: \n", continue_jump); //flow control

	//the expression should be enclosed with a parenthesis
	if (lookahead != R_PAREN_TOK) {
		printf("ERROR: right parenthesis missing after 'while' statement. Line %d.\n", line_no);
		exit(20);
	}

	lookahead = lexer();

	//lookahead should come back with a semicolon, marking end of do statement
	if (lookahead != SEMICOLON_TOK) {
		printf("ERROR: DO STATEMENT missing semicolon. Line %d.\n", line_no);
		exit(21);
	}

	loop_stacktop--; //flow control
	//i think that is all
	if (print_flag) printf("Leaving do statement.\n");

}

void continue_statement() {
	if (print_flag) printf("Entering continue statement.\n");

	lookahead = lexer(); //eat continue keyword

	if (loop_stacktop <= 0)
	{
		printf("ERROR: CONTINUE not in a loop. Line %d\n", line_no);
		exit(46);
	}
	fprintf(asmfile, "   jmp $cont@%d\n", loop_stack[loop_stacktop]);


	//must be followed by semi-colon
	if (lookahead != SEMICOLON_TOK) {
		printf("ERROR: CONTINUE statement missing semi-colon. Line %d.\n", line_no);
		exit(22);
	}

	if (print_flag) printf("Leaving continue statement.\n");
}

void break_statement() {
	if (print_flag) printf("Entering break statement.\n");

	lookahead = lexer(); // eat break keyword

	if (loop_stacktop <= 0)
	{
		printf("ERROR: BREAK not in a loop. Line %d\n", line_no);
		exit(47);
	}
	fprintf(asmfile, "   jmp $break@%d\n", loop_stack[loop_stacktop]);

						 //must be followed by semi-colon
	if (lookahead != SEMICOLON_TOK) {
		printf("ERROR: BREAK statement missing semi-colon. Line %d.\n", line_no);
		exit(23);
	}

	if (print_flag) printf("Leaving break statement.\n");
}

void return_statement() {
	if (print_flag) printf("Entering return statement.\n");

	lookahead = lexer(); //eat return keyword

	//now it can either contain an expression, or none at all...
	if (lookahead != SEMICOLON_TOK && lookahead != EOF_TOK) {
		expression_statement(); //there is an expression afterward
		lookahead = lexer(); //eat semicolon
		fprintf(asmfile, "   jmp $return@%d\n", return_target);
	}
	else if(lookahead == SEMICOLON_TOK) { //Returning without an expresssin
		lookahead = lexer(); //eat semi colon
		fprintf(asmfile, "   jmp $return@%d\n", return_target);
	}
	else {
		printf("ERROR: return statement missing a return value or semi-colon. Line %d.\n", line_no);
	}
}

//helper function for expression_statement
int binary_operation(int lookAheadToken) {

	//printf("Entering helper method binary_operation");
	switch (lookAheadToken) {
	case STAR_TOK:
		return STAR_TOK;

	case SLASH_TOK:
		return SLASH_TOK;

	case PLUS_TOK:
		return PLUS_TOK;

	case MINUS_TOK:
		return MINUS_TOK;

	case LT_TOK:
		return LT_TOK;

	case GT_TOK:
		return GT_TOK;

	case GT_EQ_TOK:
		return GT_EQ_TOK;

	case LT_EQ_TOK:
		return LT_EQ_TOK;

	case COMP_TOK:
		return COMP_TOK;

	case NOT_EQ_TOK:
		return NOT_EQ_TOK;

	case AND_TOK:
		return AND_TOK;

	case OR_TOK:
		return OR_TOK;

	default:
		return 404; //not found 
	}

	//printf("Leaving helper method binary_operation");
}


