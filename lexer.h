#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>


enum {
	valid, ident_too_long, ident_starts_with_num, num_val_exceeds_max, invalid_token
};

int error_num; //identifies which type of error it is

typedef struct _token {
	int which; // obsolete
	char text[100];
} token;

/* moves to the nextToken and then takes it, also setting its type */
token getNextToken(FILE *fp);
/* for handling any special character that doesn't fall under number or ident */
token specialcase(FILE *fp, char c);
/* builds the string that holds the token and returns it */
token collectToken(FILE *fp, char c);
/* prints the token and the type, along with any error messages */
void printTokenType(token t);
/* determines based on the token what type of token it is */
token_type findTokenType(token *t);
/* prints the source code as asked for */
void printsource(FILE*, int);
/* checks if the args are --source or --clean */
void getSourceOpts(int, const char **, int*);

void advance();

FILE *source;
token t;
token_type t_type;

#endif
