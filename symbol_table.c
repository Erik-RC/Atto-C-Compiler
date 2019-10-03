/*
Title: Symbol table
Author: Erik Rivas Contreras
Date: 3/31/2019
*/

#include "atto-C.h"

struct symbol_t symbol_table[MAXSYMBOL];
int symbol_stack_top;

struct symbol_t *push_symbol(char *name) {

	if (print_flag) printf("Adding symbol '%s', table index %d, line %d\n", name, symbol_stack_top, line_no);

	symbol_stack_top++;
	strncpy(symbol_table[symbol_stack_top].name, name, MAX_LEXEME);

	if (symbol_stack_top < MAXSYMBOL) {
		return symbol_table + symbol_stack_top;
	}
	else {
		printf("ERROR: Symbol table has overflown. \n");
		exit(2);
	}
	
}

int start_scope() 
{
	if (print_flag) printf("Starting scope, line %d\n", line_no);
	return symbol_stack_top;
}

void end_scope(int prev_stack_top) {

	if (print_flag) printf("Ending scope, line %d\n", line_no);

	if (prev_stack_top < 0 || prev_stack_top > symbol_stack_top) {
		printf("ERROR: Prev_stack_top arguement violates current stack indeces. \n");
		exit(1);
	}
	symbol_stack_top = prev_stack_top;
}

struct symbol_t *symbol_lookup(char *name) {

	if (print_flag) printf("Looking up symbol '%s', line %d\n", name, line_no);

	int k;

	for (k = symbol_stack_top; k > 0; k--) {
		if (strcmp(name, symbol_table[k].name) == 0) {
			return symbol_table + k;
		}
	}

	return NULL;
}
