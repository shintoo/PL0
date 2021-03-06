// lexer.c


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "lexer.h"

token t;
token_type t_type;

FILE *source;

void error(int);

void advance() {
	t = getNextToken(source);
	t_type = findTokenType(&t);
}

//moves to the next token, takes it, and returns it
token getNextToken(FILE *fp) {
	token ret; //for storing the token
	char c;
	int in_comment = 0; //for determining if you are in a comment




	//keep moving to the next token
	do {
		c = fgetc(fp);
		if (c == EOF) break;
		//if you see this, then it might be a comment
		//check if the next character is '*', and if it is, you are in a comment
		if (c == '/') {
			char x = fgetc(fp);
			if (x == '*') in_comment = 1;
			else ungetc(x, fp);
		}

		//you are in a comment until you see these two right after each other
		//keep going until you find this config
		if (c == '*' && in_comment) {
			char x = fgetc(fp);
			if (x == '/') {
				in_comment = 0;
				c = fgetc(fp);
			}
		}

	} while (isspace(c) || in_comment); //while you havent found the next token

	//if you hit the end of the token, you are done
	if (c == EOF) {
		ret.which = 3;
	} 
	else if (isalpha(c) || isdigit(c)) { // for if it is a comination of letters and numbers or invalid
		ret = collectToken(fp, c);
	} 

	else {
		ret = specialcase(fp, c); //for if it isnt alphanumeric
	}
	

	return ret; //return the newly made token
}

/* for if you have found something that isnt alphanumeric, check to make sure its valid */
token specialcase(FILE *fp, char c) {
	token ret;
	char x;

	ret.which = 0;

	/* check to see if you need the next token for each of these special cases */
	switch (c) {
		//for these three, the next token might be something you need
		case ':':
		case '<':
		case '>':
			x = fgetc(fp);
			if (x == '=' || (c == '<' && x == '>')) {
				ret.text[0] = c;
				ret.text[1] = x;
				ret.text[2] = '\0';
			}
			else {//if it isn't, you are done, and you can put back what you tried to grab
				ret.text[0] = c;
				ret.text[1] = '\0';
				ungetc(x, fp);
			}
			break;
		default:
			ret.text[0] = c;
			ret.text[1] = '\0';
	}

	return ret;
}

token collectToken(FILE *fp, char c) {
	token ret;//returns the token constructed

	char *p = ret.text;
	//keeps going while the current token is alphanumeric
	do {
		*p++ = c;
		c = fgetc(fp);	
	} while (isalpha(c) || isdigit(c));
	ungetc(c, fp); //the last character isnt alphanumeric, so put it back
	*p = '\0'; //need this for printing
	ret.which = 1; //legacy code?
	return ret;
}

token_type findTokenType(token *t) {
	static char *tokens[] = { //holds all of the different identifiers for tokens
			  "+", "-", "*", "/", "odd", "=", "<>", "<", "<=", ">", ">=", "(", ")",
			  ",", ";", ".", ":=", "begin", "end", "if", "then", "while", "do", 
			  "call", "const", "var", "procedure", "write", "read", "else"
	};

	int i;

	//if it is one of the tokens in the array above, return the one it is
	for (i = 0; i < 30; i++) 
		if(!strcmp(tokens[i], t->text)) 
			return i + 4;
	
	//checks if it is a num or an identifier, and if it is one, then it returns it 
	if (isNum(t->text)) return numbersym;
	if (isIdent(t->text)) return identsym;

	error(27);
	return nulsym;
}

int isIdent(char *name) {
	int i = 0;	
	//if the first one is neither, then it is not an ident
	if (!isalpha(name[0]) && !isdigit(name[0])) return 0;
	//keep checking to make sure it is still valid, and if it ever isnt, return 0
	while(name[i] != '\0') {
		if(!isalpha(name[i]) && !isdigit(name[i]))
			return 0;
		i++;
		if (i > 11) //if you hit 12 characters, that means your string is too long
			error(28);   //notify that this happened
	}
	//this is another error, and it should be notified
	if (isdigit(name[0]))
		error(29);

	return 1;
}

//identifies if something is a number
int isNum(char *text) {
	int num;
	char *t = text;
	//until something isnt a digit or you hit a null terminator, keep going
	while (*t)
		if (!isdigit(*t++))
			return 0;

	num = atoi(text); //turn num into a number
	
	if (num > 65535) { //if its greater than 2^16 - 1, another error has occurred
		error(25);
	}

	return 1;
}
