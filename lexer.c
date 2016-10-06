/* Satya Patel
   Sean Rapp
   Brody "Broderson" Nissen */

// token size stuff too long error reading it all etc
// yes you would satya
// multiline or 1line comments; nested comments
// output format, example output
// printing errors
// 
#include <stdio.h>
#include <ctype.h>
#include "tokens.h"

typedef struct _token {
	int which; // text 0, num 1, invalid 2, EOF 3
	union {
		char text[20];
		int num;
	} val;
} token;


token getNextToken(FILE *fp);
token specialcase(FILE *fp, char c);
token collectToken(FILE *fp, char c);
void printTokenType(token t);

int main(int argc, char *args[]) {
	FILE *fp = fopen(args[1], "r");	
	token t;

	t = getNextToken(fp);
	do {
        printf("token: \"%s\"\n", t.val.text);
		printTokenType(t);
        t = getNextToken(fp);
	} while (!feof(fp));


	fclose(fp);
	return 0;
}

void printTokenType(token t) {
}

token getNextToken(FILE *fp) {
	token ret;
	char c;
	int in_comment = 0;

	do {
		c = fgetc(fp);
		if (c == EOF) break;
		
		if (c == '/') {
			char x = fgetc(fp);
			if (x == '*') in_comment = 1;
			else ungetc(x, fp);
		}

		if (c == '*' && in_comment) {
			char x = fgetc(fp);
			if (x == '/') {
                in_comment = 0;
                c = fgetc(fp);
            }
		}

	} while (isspace(c) || in_comment);

	if (c == EOF) {
		ret.which = 3;
	} 
	else if (isalpha(c) || isdigit(c)) {
		ret = collectToken(fp, c);
	} 

	else {
		ret = specialcase(fp, c);
	}
	

	return ret;
}


token specialcase(FILE *fp, char c) {
	token ret;
    char x;

    ret.which = 0;

    switch (c) {
        case ':':
        case '<':
        case '>':
            x = fgetc(fp);
            if (x == '=' || (c == '<' && x == '>')) {
                ret.val.text[0] = c;
                ret.val.text[1] = x;
                ret.val.text[2] = '\0';
            }
            else {
                ret.val.text[0] = c;
                ret.val.text[1] = '\0';
                ungetc(x, fp);
            }
            break;
        default:
            ret.val.text[0] = c;
            ret.val.text[1] = '\0';
    }

	return ret;
}

token collectToken(FILE *fp, char c) {
	token ret;

	char *p = ret.val.text;

	do {
		*p++ = c;
		c = fgetc(fp);
		if (p - ret.val.text > 12) {
			printf("error: token size too long!!\n");
			ret.which = 2;
			return ret;
			// entire token here
			
		}
	} while (isalpha(c) || isdigit(c));
	ungetc(c, fp);
    *p = '\0';
	ret.which = 1;
	return ret;
}
