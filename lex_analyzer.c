/*
Author: Erik Rivas Contreras
Date: 5/4/2019
Purpose: Lexical analyzer. To read tokens.
*/

#include "atto-C.h"

#define START     1 //done
#define INTEGER   2 //done
#define EQUAL	  3 //done
#define STRING	  4 //done
#define COMMENT_START   5 //done
#define COMMENT_END 6 //done
#define IDENT     7
#define LETTER    8 //done
#define PIPE_STATE      9
#define SLASH_STATE  10 //done 
#define LT_STATE 11 // done
#define GT_STATE 12 //done
#define EXCLAM_STATE 13
#define COMPARE 14 //done
#define EOL_COMMENT 15 //done
#define AND_STATE  16 //done
#define BACKSLASH_IN_QUOTE 17
#define MINUS_STATE 18
#define FINAL     0


// Special look-ahead character value to indicate none
#define NO_CHAR 0

//ACII new line value
#define NEW_LINE 10

char lexeme[MAX_LEXEME];  // The characters of the token.

#define IDENT_MAX_LENGTH 50
#define NUMBER_MAX_LENGTH 10

#define KEYWORD_COUNT 28

//array attoc special keywords
char *keywords[] = { "auto", "break", "case", "char", "continue", "default", "do", "double", "else",
"enum", "extern", "float", "for", "goto", "if", "int", "long", "register", "return", "short",
"sizeof", "static", "struct", "switch", "typedef", "union", "unsigned", "while" };

int Keyword_Tokens[KEYWORD_COUNT] = { AUTO_TOK, BREAK_TOK, CASE_TOK, CHAR_TOK, CONTINUE_TOK, DEFAULT_TOK, DO_TOK, DOUBLE_TOK, ELSE_TOK,
ENUM_TOK, EXTERN_TOK, FLOAT_TOK, FOR_TOK, GOTO_TOK, IF_TOK, INTEGER_TOK, LONG_TOK, REGISTER_TOK, RETURN_TOK, SHORT_TOK, SIZEOF_TOK, STATIC_TOK,
STRUCT_TOK, SWITCH_TOK, TYPEDEF_TOK, UNION_TOK, UNSIGNED_TOK, WHILE_TOK };

int next_char; // The next character of input.

int line_no = 1; //line number in current in put file
int token_type;
int lexer()
{
	int state;   // The current state of the FSM.
	int lex_spot; // Current spot in lexeme.
	//int token_type;  // The type of token found.
	


	// Infinite loop, doing one token at a time.
	//next_char = NO_CHAR;  // no lookahead character to start with
	while (1)
	{  // Initialize the Finite State Machine.
		state = START;
		lex_spot = 0;
		// Loop over characters of the token.
		while (state != FINAL)
		{
			if (next_char == NO_CHAR)
				next_char = getc(sourcefile);  // get one character from standard input

			int in_comment = 0;
			if (state == COMMENT_START || state == COMMENT_END) {
				in_comment = 1;
			}

			if (next_char == EOF && in_comment != 1) {
				return EOF_TOK;  //exiting 
			}      
		
			switch (state)
			{
			//=====================START================================
			case START:
				if (next_char == '\n') { // just eat the newline and stay in START if nothing is given
					next_char = 0;
					line_no++;
				}  

				//DIGIT
				else if (isdigit(next_char)) 
				{
					state = INTEGER;
					lexeme[lex_spot++] = next_char;  // Add the character to the lexeme
					next_char = NO_CHAR;  // eat the character
					token_type = INTEGER_TOK;
				}

				//STRING
				else if (next_char == '"') {
					state = STRING;
					next_char = NO_CHAR;
					token_type = QUOTE_TOK;
				}

				//LETTER
				else if (isalpha(next_char)) {
					state = LETTER;
					lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
					
				}

				//EQUAL
				else if (next_char == '=') {
					state = EQUAL;
					lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
					
				}

				//SLASH
				else if (next_char == '/') {
					state = SLASH_STATE;
					lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
					token_type = SLASH_TOK;
				}

				//PLUS
				else if (next_char == '+') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					token_type = PLUS_TOK;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				//MINUS
				else if (next_char == '-') {
					state = MINUS_STATE;
					lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
				}

				//STAR
				else if (next_char == '*') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					token_type = STAR_TOK;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				//LESS THAN
				else if (next_char == '<') {
					state = LT_STATE;
					lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
				}

				//GREATER THAN
				else if (next_char == '>') {
					state = GT_STATE;
					lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
				}

				//Exclamation
				else if (next_char == '!') {
					state = EXCLAM_STATE;
					lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
				}

				//logical AND
				else if (next_char == '&') {
					state = AND_STATE;
					lexeme[lex_spot++] = next_char;
					token_type = AND_TOK;
					next_char = NO_CHAR;
				}

				//logical OR
				else if (next_char == '|') {
					state = PIPE_STATE;
					lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
				}

				//opening curly
				else if (next_char == '{') {
					state = FINAL;
					token_type = L_cBRACKET_TOK;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				//closing curly
				else if (next_char == '}') {
					state = FINAL;
					token_type = R_cBRACKET_TOK;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				//opening parenthesis
				else if (next_char == '(') {
					state = FINAL;
					token_type = L_PAREN_TOK;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				//closing parenthesis
				else if (next_char == ')') {
					state = FINAL;
					token_type = R_PAREN_TOK;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				//underscore 
				else if (next_char == '_') {
					state = IDENT;
					token_type = UNDERSCORE_TOK;
					lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
				}

				//semicolon
				else if (next_char == ';') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					token_type = SEMICOLON_TOK;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				//comma
				else if (next_char == ',') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					token_type = COMMA_TOK;
					next_char = NO_CHAR;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}


				else //NO LEGAL STATE, OR JUST BLANK
				{
					//line_no++;
					if (next_char == NEW_LINE || next_char == EOF /*|| next_char == ' ' || next_char == '	'*/) {
						state = FINAL;  
						next_char = NO_CHAR;   
						line_no++;
					}
					else if (next_char == ' ' || next_char == '	') {
						state = FINAL;
						next_char = NO_CHAR;
					}
					else {
						if (print_flag) printf("REJECT %c	 LINE: %d\n", next_char, line_no);  // This is not a legal final state
						state = FINAL;  // but we want to end the token anyway
						next_char = NO_CHAR;   // eat the offending character
					}
					
				}
				break;  // Need "break" at the end of a case, else you will continue 
						// to the next case.
				
//=========================================START STATE END ================================

			case INTEGER:
#pragma region int code
				if (isdigit(next_char)) //is it a digit
				{
					state = INTEGER;
					if (lex_spot == NUMBER_MAX_LENGTH) {
						printf("ERROR: number too long. Truncating. LINE: %d\n", line_no);
					}
					else if (lex_spot < NUMBER_MAX_LENGTH) {
						lexeme[lex_spot++] = next_char;
					}
					//lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
					token_type = INTEGER_TOK;
				}

				else
				{
					lexeme[lex_spot] = 0; // null for end of string
					token_type = INTEGER_TOK;
					if (print_flag) printf("ACCEPT INTEGER %s\n", lexeme);  // This is a final state
					state = FINAL;       // leave next_char alone, for next token
					return token_type;
				}
				break;
#pragma endregion

			case STRING:
#pragma region String code

				if (next_char != '"' && next_char != NEW_LINE && next_char != '\\') {
					state = STRING;
					if (lex_spot == MAX_LEXEME - 3) {
						printf("ERROR: quoted string too long. Truncating. LINE: %d\n", line_no);
						
					}
					else if (lex_spot < MAX_LEXEME - 3) {
						lexeme[lex_spot++] = next_char;
					}
					//lexeme[lex_spot++] = next_char;
					next_char = NO_CHAR;
					token_type = STRING_TOK;
				}

				//check for back slash
				else if (next_char == '\\') {
					state = BACKSLASH_IN_QUOTE;
					next_char = NO_CHAR;
				}

				//second quotation marks end of string
				else if (next_char == '"') {
					lexeme[lex_spot] = 0;
					token_type = QUOTE_TOK;
					if (print_flag) printf("ACCEPT STRING %s\n", lexeme);
					next_char = NO_CHAR;
					token_type = STRING_TOK;
					state = FINAL; //quote state finished
					return token_type;
				}

				else {
					lexeme[lex_spot] = 0; // null for end of string
					printf("REJECT UNFINISHED STRING %s		 LINE: %d\n", lexeme, line_no);  // This is a final state
					state = FINAL;       // leave next_char alone, for next token
				}

				break;
#pragma endregion

			case LETTER:
#pragma region letter code
				//char is just a letter, may form a word/identifier
				if (isalnum(next_char) || next_char == '_'){
					state = LETTER;

					if (lex_spot == IDENT_MAX_LENGTH) {
						printf("ERROR: identifier too long. Truncating. LINE: %d\n", line_no);
					}
					else if (lex_spot < IDENT_MAX_LENGTH) {
						lexeme[lex_spot++] = next_char;
					}

					//lexeme[lex_spot++] = next_char;
					token_type = IDENT_TOK;
					next_char = NO_CHAR;
				}

				
				else {
					lexeme[lex_spot] = 0; // null for end of string
					int kf_found = 0;
					for (int i = 0; i < KEYWORD_COUNT; i++) {
						if (strcmp(lexeme, keywords[i]) == 0) {
							if (print_flag) printf("ACCEPT KEYWORD %s\n", lexeme);
							state = FINAL;
							token_type = Keyword_Tokens[i];
							kf_found = 1;
							return token_type;
						}
					}
					if (kf_found == 0) {
						token_type = IDENT_TOK;
						if (print_flag) printf("ACCEPT IDENTIFIER %s\n", lexeme);  // This is a final state	
						state = FINAL;
						return token_type;
					}
				}
				break;
#pragma endregion

			case EQUAL:
#pragma region equal code
				if (next_char != '=') {
					state = FINAL;
					lexeme[lex_spot] = 0;
					token_type = EQ_TOK;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				//compare 
				else if (next_char == '=') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					token_type = COMP_TOK;
					next_char = NO_CHAR;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}
				break;
#pragma endregion

			case SLASH_STATE:
#pragma region slash code
				//End of line comment. 
				if (next_char == '/') {
					state = EOL_COMMENT;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					token_type = SLASH_TOK;
				}

				//block comment
				else if (next_char == '*') {
					state = COMMENT_START;
					lexeme[lex_spot++] = next_char;
					token_type = STAR_TOK;
					next_char = NO_CHAR;
				}

				//Just a single slash
				else {
					state = FINAL;
					lexeme[lex_spot] = 0;
					token_type = SLASH_TOK;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				break;
#pragma endregion

			case EOL_COMMENT:
#pragma region EOL code
				if (next_char != NEW_LINE) {
					state = EOL_COMMENT;
					next_char = NO_CHAR;
				}
				else {
					state = FINAL;
					token_type = COMMENT_TOK;
					lexeme[lex_spot] = 0;
					if (print_flag) printf("ACCEPT END OF LINE COMMENT %s\n", lexeme);
					line_no++; //LINE++
					
				}
				break;
#pragma endregion

			case COMMENT_START:
#pragma region comment start code
				//this star may mark the end
				if (next_char == '*') {
					state = COMMENT_END;
					if (lexeme[lex_spot - 1] == '*' && lexeme[lex_spot - 2] == '/') {
						lexeme[lex_spot++] = next_char; //dont have to put this in the lexeme
					}
					token_type = STAR_TOK;
					next_char = NO_CHAR;
				}
				else if (next_char != '*' && next_char != NEW_LINE && next_char != '/' && next_char != EOF) {
					state = COMMENT_START;
					//lexeme[lex_spot++] = next_char; don't have to put it in lexeme
					next_char = NO_CHAR;
				}

												//making sure this slash does not come right after "/*"
				else if (next_char == '/' &&  (lexeme[lex_spot-1] != '*' && lexeme[lex_spot-2] != '/') ) {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					token_type = COMMENT_TOK;
					next_char = NO_CHAR;
					if (print_flag) printf("ACCEPT BLOCK COMMENT %s\n", lexeme);
				}

				//new line entered, will accept
				if(next_char == NEW_LINE) {
					line_no++; //LINE++
					next_char = NO_CHAR;
					state = COMMENT_START;
				}
				else if (next_char == EOF) {
					//line_no++; //LINE++
					lexeme[lex_spot] = 0; // null for end of string
					printf("REJECT UNFINISHED BLOCK COMMENT %s		LINE: %d\n", lexeme, line_no);  // This is a final state
					state = FINAL;
				}

				break;
#pragma endregion

			case COMMENT_END:
#pragma region comment end code
				//user wants to put more stars 
				if (next_char == '*') {
					state = COMMENT_START;
					token_type = STAR_TOK;
					next_char = NO_CHAR;
				}

				//ending block comment 
				else if (next_char == '/') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					token_type = COMMENT_TOK;
					if (print_flag) printf("ACCEPT BLOCK COMMENT %s\n", lexeme);
				}

				else if (next_char != '*' && next_char != '/') {
					state = COMMENT_START;
					next_char = NO_CHAR;
				}

				//new line entered, will accept
				else {
					line_no++;
					next_char = NO_CHAR;
					state = COMMENT_START;
				}
				break;
#pragma endregion

			case LT_STATE:
#pragma region LT code
				if (next_char == '=') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					token_type = LT_EQ_TOK;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				else {
					state = FINAL;
					token_type = LT_TOK;
					lexeme[lex_spot] = 0;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}
				break;
#pragma endregion

			case GT_STATE:
#pragma region GT code
				if (next_char == '=') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					token_type = GT_EQ_TOK;
					if (print_flag)printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				else {
					state = FINAL;
					token_type = GT_TOK;
					lexeme[lex_spot] = 0;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}
				break;
#pragma endregion

			case EXCLAM_STATE:
#pragma region exclam code

				if (next_char == '=') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					token_type = NOT_EQ_TOK;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					next_char = NO_CHAR;
					return token_type;
				}

				else {
					state = FINAL;
					token_type = EXCLAM_TOK;
					lexeme[lex_spot] = 0;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}

				break;
#pragma endregion

			case AND_STATE:
#pragma region and code
				if (next_char != '&') {
					state = FINAL;
					lexeme[lex_spot] = 0;
					printf("REJECT SINGLE %s	LINE: %d\n", lexeme, line_no);
				}
				else if (next_char == '&') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					token_type = AND_TOK;
					if (print_flag) printf("ACCEPT TOKEN %s\n",lexeme);
					return token_type;
				}
				break;

#pragma endregion

			case PIPE_STATE:
#pragma region OR code
				if (next_char != '|') {
					state = FINAL;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					printf("REJECT SINGLE %s	LINE: %d\n", lexeme,line_no);
				}

				else if (next_char == '|') {
					state = FINAL;
					lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					token_type = OR_TOK;
					if (print_flag) printf("ACCEPT TOKEN %s\n", lexeme);
					return token_type;
				}
				break;
#pragma endregion
			case IDENT:
#pragma region ident code

				if (isalpha(next_char) || isdigit(next_char) || next_char == '_') {
					state = IDENT;
					if (lex_spot == IDENT_MAX_LENGTH) {
						printf("ERROR: identifier too long. Truncating. LINE: %d\n", line_no);
					}
					else if (lex_spot < IDENT_MAX_LENGTH) {
						lexeme[lex_spot++] = next_char;
					}
					
					next_char = NO_CHAR;
				}


				else {
					state = FINAL;
					token_type = IDENT_TOK;
					lexeme[lex_spot] = 0;
					next_char = NO_CHAR;
					if (print_flag) printf("ACCEPT IDENTIFIER %s\n", lexeme);
					return token_type;
				}
				break;
#pragma endregion

			case BACKSLASH_IN_QUOTE:
#pragma region escape code
				//check what escape sequence it is
				if (next_char == 'n') {
					lexeme[lex_spot++] = '\n';
				}
				else if (next_char == 'b') {
					lexeme[lex_spot++] = '\b';
				}
				else if (next_char == 't') {
					lexeme[lex_spot++] = '\t';
				}
				else if (next_char == '"') {
					lexeme[lex_spot++] = '"';
				}
				else if (next_char == '\\') {
					lexeme[lex_spot++] = '\\';
				}
				else {
					lexeme[lex_spot++] = next_char;
				}

				state = STRING;
				next_char = NO_CHAR;
				break;
#pragma endregion

			case MINUS_STATE:
#pragma region Minus code
				//unary minus
				if (token_type == IDENT_TOK || token_type == INTEGER_TOK || token_type == R_PAREN_TOK) {
					//lexeme[lex_spot++] = next_char;
					lexeme[lex_spot] = 0;
					//next_char = NO_CHAR;
					token_type = MINUS_TOK;
					state = FINAL;
					if (print_flag) printf("ACCEPT TOKEN BINARY MINUS %s\n", lexeme);
					return token_type;

				}

				//it was just something esle
				else {
					token_type = UNARY_MINUS_TOK;
					lexeme[lex_spot] = 0;
					if (print_flag) printf("ACCEPT TOKEN UNARY MINUS %s\n", lexeme);
					state = FINAL;
					return token_type;
				}

#pragma endregion

				

			default:
				printf("INTERNAL ERROR: Illegal state %d\n", state);
				state = FINAL;
				break;

			} // end of switch

		} // end of while state

	}  // end of infinite loop

	return 0;  // successful exit code
} // end of main