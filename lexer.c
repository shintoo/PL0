/* Satya Patel
   Sean Rapp
   Brody "Broderson" Nissen */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "tokens.h"

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

/* for identifying if something is a num or ident */
int isIdent(char *name);
int isNum(char *text);

int main(int argc, const char *args[]) {
	FILE *fp;
	token t;
	
	int argvals[2];
	//reads in which sources to print, and which argument has the filepath
	//it is stored into argvals
	getSourceOpts(argc, args, argvals);

	fp = fopen(args[argvals[1]], "r");
	//based on argvals[0], prints the source code in the expected formats
	switch (argvals[0]) {
		case 0: // clean
			puts("\nsource code without comments:\n-----------------------------");
			printsource(fp, 1);
			rewind(fp);
			break;
		case 1: // comments
			puts("\nsource code:\n-----------");
			printsource(fp, 0);
			rewind(fp);
			break;
		case 2: // both
			puts("\nsource code:\n-----------");
			printsource(fp, 0);
			rewind(fp);
			puts("\nsource code without comments:\n-----------------------------");
			printsource(fp, 1);
			rewind(fp);
	}

	puts("\ntokens:\n-------");
	//goes to each token and prints it until there are no more tokens left
	t = getNextToken(fp);
	do {
		printTokenType(t);
		t = getNextToken(fp);
	} while (!feof(fp));


	fclose(fp);
	return 0;
}

//prints the type of token something is
void printTokenType(token t) {
	token_type type = findTokenType(&t);

	printf("%-12s", t.text);

	switch (error_num) {
		case valid:
			printf(" %d\n", type);
			break;
		case ident_starts_with_num:
			puts(" Error: identifier begins with number");
			break;
		case ident_too_long:
			puts(" Error: identifier too long");
			break;
		case invalid_token:
			puts(" Error: invalid token");
			break;
		case num_val_exceeds_max:
			puts(" Error: number value exceeds maximum");
	}
}

//moves to the next token, takes it, and returns it
token getNextToken(FILE *fp) {
	token ret; //for storing the token
	char c;
	int in_comment = 0; //for determining if you are in a comment

	//says that it currently is a valid token until otherwise noted
	error_num = valid;

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

	error_num = invalid_token;
	return nulsym;
}

int isIdent(char *name) {
	int i = 0;	
	//if the first one is neither, then it is not an ident
	if (!isalpha(name[0]) && !isdigit(name[0])) return 0;
	//keep checking to make sure it is still valid, and if it ever isnt, return 0
	while(name[i] != '\0') {
		if(!isalpha(name[i]) && !isdigit(name[i])) {
			if (isalpha(name[0])) error_num = invalid_token;
			return 0;
		}
		i++;
		if (i > 11) { //if you hit 12 characters, that means your string is too long
			error_num = ident_too_long; //notify that this happened
			name[i] = '\0';
			return 1; 
		}
	}
	//this is another error, and it should be notified
	if (isdigit(name[0])) error_num = ident_starts_with_num;
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
		error_num = num_val_exceeds_max;
	}

	return 1;
}

void printsource(FILE *fp, int comments_hidden) {
	char buffer[80];
	int i;
	int in_comment = 0;

	rewind(fp);



	while (!feof(fp)) {
		fgets(buffer, 80, fp);
		for (i = 0; buffer[i]; i++) {
			if (comments_hidden) {
				if (!in_comment) {
					if (buffer[i] == '/' && buffer[i+1] == '*') {
						in_comment = 1;
						i++;
						printf("  ");
					}
					else
						putchar(buffer[i]);
				} else {
					if (buffer[i] == '*' && buffer[i+1] == '/') {
						in_comment = 0;
						i++;
					}
					putchar(' ');
				}
			} else {
				putchar(buffer[i]);
			}
		}

	}

	putchar('\n');
}
/* this finds out how many inputs were given, and then identifies what they are */
void getSourceOpts(int argc, const char **argv, int *argvals) {
	int clean = 0;
	int source = 0;
	int file = 0;
	//for each input, it identifies what it is
	//if it is something we are looking for, it says that we have seen it
	for (int i = 0; i < argc; i++) {
		if (!strcmp(argv[i], "--clean")) {
			clean = 1;
		} else if (!strcmp(argv[i], "--source")) {
			source = 1;
		} else {
			file = i;
		}
	}

	//this sets argvals, the object modified, to what values it should be
	if (clean && source) argvals[0] = 2;
	else if (clean) argvals[0] = 0;
	else if (source) argvals[0] = 1;
	else argvals[0] = 99;
	argvals[1] = file;
}
