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
#include <string.h>
#include <stdlib.h>

#include "tokens.h"

enum {
    text = 0,
    num, too_long, eof, ident_starts_with_num
};

typedef struct _token {
	int which; // text 0, num 1, too long 2, EOF 3, identstaartswithnum 4
	char text[20];
} token;


token getNextToken(FILE *fp);
token specialcase(FILE *fp, char c);
token collectToken(FILE *fp, char c);
void printTokenType(token t);
token_type findTokenType(token t);
void printSourceWithAndWithoutComments(FILE *fp);

int isIdent(char *name);
int isNum(char *text);

int main(int argc, char *args[]) {
	FILE *fp = fopen(args[1], "r");	
	token t;

    printSourceWithAndWithoutComments(fp);
    rewind(fp);
    puts("\n\n");

	t = getNextToken(fp);
	do {
		printTokenType(t);
        t = getNextToken(fp);
	} while (!feof(fp));


	fclose(fp);
	return 0;
}

void printTokenType(token t) {
	printf("%s\t\t%2d\n", t.text, findTokenType(t));
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
                ret.text[0] = c;
                ret.text[1] = x;
                ret.text[2] = '\0';
            }
            else {
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
	token ret;

	char *p = ret.text;

	do {
		*p++ = c;
		c = fgetc(fp);
		if (p - ret.text > 12) {
			printf("error: token size too long\n");
			ret.which = 2;
            *p = '\0';
			return ret;
			
		}
	} while (isalpha(c) || isdigit(c));
	ungetc(c, fp);
    *p = '\0';
	ret.which = 1;
	return ret;
}

token_type findTokenType(token t) {
	static char *tokens[] = {
	          "+", "-", "*", "/", "odd", "=", "<>", "<", "<=", ">", ">=", "(", ")",
	          ",", ";", ".", ":=", "begin", "end", "if", "then", "while", "do", 
	          "call", "const", "var", "procedure", "write", "read", "else"
    };

	int i;

    if (t.which == 2) {
        printf("error: identifier too long: %d\n", t.text);
        return nulsym;
    }

	for (i = 0; i < 30; i++) 
		if(!strcmp(tokens[i], t.text)) 
			return i + 3;
	

	if (isNum(t.text)) return numbersym;
	if (isIdent(t.text)) return identsym;

	return nulsym;
}

int isIdent(char *name) {
	int i = 0;
	if (!isalpha(name[i++])) return 0;

	while(name[i] != '\0') {
		if(!isalpha(name[i]) && !isdigit(name[i]))
			return 0;
        i++;
	}
	return 1;
}

int isNum(char *text) {
    int num;

    while (*text)
        if (!isdigit(*text++))
            return 0;


    num = atoi(text);
    if (num > 65535)
        printf("error: number too big: %d\n", num);

    return 1;
}

void printsource(FILE *fp, int comments_hidden) {
    char buffer[80];
    int i;
    int in_comment = 0;

    rewind(fp);

    fgets(buffer, 80, fp);

    do {
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
        fgets(buffer, 80, fp);
    } while (!feof(fp));
}

void printSourceWithAndWithoutComments(FILE *fp) {
    puts("\nsource code:\n-----------");
    printsource(fp, 0);
    puts("\nsource code without comments:\n-----------------------------");
    printsource(fp, 1);
}
