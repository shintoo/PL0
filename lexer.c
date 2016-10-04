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
#include "tokens.h"

typedef struct _token {
	int which; // text 0, num 1, invalid 2, EOF 3
	union {
		char[20] text;
		int num;
	} val;
} token;


token getNextToken(FILE *fp);
token specialcases(FILE *fp, char c);
token collectToken(FILE *fp, char c);

int main(int argc, char *args[]) {
	FILE *fp = fopen(args[1], "r");	
	token t;

	while (!feof(fp)) {
		t = getNextToken(fp);
		printTokenType(t);
	}


	fclose(fp);
	return 0;
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
			if (x == '/') in_comment = 0;
			else ungetc(x, fp);
		}

	} while (isspace(c) && in_comment)

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


token specialcases(FILE *fp, char c) {
	token ret;

	char *p = ret.text.value;

	do {
		*p++ = c;
		c = fgetc(fp);
	} while (!isspace(c) && c != EOF);

	if (c == EOF) ungetc(c, fp);

	ret.which = 0; 
	return ret;
}

token collectToken(fp, c) {
	token ret;

	char *p = ret.text.value;

	do {
		*p++ = c;
		c = fgetc(fp);
		if (p - ret.text.value > 12) {
			printf("error: token size too long!!\n");
			ret.which = 2;
			return ret;
			// entire token here
			
		}
	} while (isalpha(c) && isdigit(c));
	ungetc(c, fp);

	ret.which = 1;
	return ret;
}
