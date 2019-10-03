#pragma once
/*
File name: atto-C.h
Description: Holds declarations that will be seen by multiple source files
Author: Erik Rivas Contreras
Date: February 11, 2019
Project Title: Lexer2
*/

#include <stdio.h>    // standard input-output declarations: printf, stdin
#include <stdlib.h>   // standard library declarations: exit
#include <ctype.h>    // character type test declarations:  isdigit, isalpha, isalnum

// Token types
#define DOLLAR_TOK 101
#define STAR_TOK 102
#define SLASH_TOK 103
#define PLUS_TOK 104
#define MINUS_TOK 105
#define LT_TOK 106
#define GT_TOK 107
#define LT_EQ_TOK 108
#define GT_EQ_TOK 109
#define COMP_TOK 110
#define NOT_EQ_TOK 111
#define AND_TOK 112
#define OR_TOK 113
#define EQ_TOK 114
#define COMMA_TOK 115
#define IDENT_TOK 116
#define INTEGER_TOK 117 //NUM token
#define EXCLAM_TOK 118
#define UNARY_MINUS_TOK 119
#define L_PAREN_TOK 120
#define R_PAREN_TOK 121
#define STRING_TOK 122
#define SEMICOLON_TOK  123
#define EOF_TOK 124


#define L_cBRACKET_TOK 124
#define R_cBRACKET_TOK 125
#define L_sBRACKET_TOK 126
#define R_sBRACKET_TOK 127

#define QUOTE_TOK 128

#define UNDERSCORE_TOK 129

#define COMMENT_TOK 131


#define AUTO_TOK 132
#define BREAK_TOK 133
#define CASE_TOK 134
#define CHAR_TOK 135
#define CONTINUE_TOK 136
#define DEFAULT_TOK 137
#define DO_TOK 138
#define DOUBLE_TOK 139
#define ELSE_TOK 140
#define ENUM_TOK 141
#define EXTERN_TOK 142
#define FLOAT_TOK 143
#define FOR_TOK 144
#define GOTO_TOK 145
#define IF_TOK 146
#define INT_TOK 147
#define LONG_TOK 148
#define REGISTER_TOK 149
#define RETURN_TOK 150
#define SHORT_TOK 151
#define SIZEOF_TOK 152
#define STATIC_TOK 153
#define STRUCT_TOK 154
#define SWITCH_TOK 155
#define TYPEDEF_TOK 156
#define UNION_TOK 157
#define UNSIGNED_TOK 158
#define WHILE_TOK 159

extern int line_no; //line number in current input file

#define MAX_LEXEME 200
extern char lexeme[MAX_LEXEME]; // The characters of the token
int lexer(void);

//ExprParser additions
//#define DOLLAR_TOK 100


//For shift-reduce array
#define SH 1
#define RE 2
#define EQ 3
#define AC 4
#define ER 5

int expr_parser(void);

//new code, for Main parser
extern int lookahead; //the lookahead token
void program(void);

//new code for Symbol table
struct symbol_t {
	char name[MAX_LEXEME + 1];	
	//variables project code
	char asm_name[MAX_LEXEME + 2];
	int symbol_kind;
};

#define MAXSYMBOL 1000 //this table will only handle 1000 symbols, fancy compilers will have expandable tables
extern struct symbol_t symbol_table[MAXSYMBOL];
extern int symbol_stack_top;
struct symbol_t *push_symbol(char *name);
int start_scope(void);
void end_scope(int prev_stack_top);

//Main driver code
extern FILE *sourcefile;
extern FILE *asmfile;
void preamble(char *sourcefilename);
void postamble(void);

//var project code 4/16/19
// Symbol classification
#define GLOBAL_VARIABLE    1
#define LOCAL_VARIABLE     2
#define FUNCTION_PARAMETER 3
#define FUNCTION           4

//expressions code addition
extern int jump_serial;

//String proj code
#define STRINGS_MAX 1000 //for simplicity of this compiler, non expandable array size
extern char *strings[STRINGS_MAX];
extern int string_serial; //for counting strings
extern void output_strings(void);

//final part
extern int print_flag;

