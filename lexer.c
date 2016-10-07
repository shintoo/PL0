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
    valid, ident_too_long, ident_starts_with_num, num_val_exceeds_max, invalid_token
};

int error_num;

typedef struct _token {
	int which; // obsolete
	char text[20];
} token;

token getNextToken(FILE *fp);
token specialcase(FILE *fp, char c);
token collectToken(FILE *fp, char c);
void printTokenType(token t);
token_type findTokenType(token t);
void printsource(FILE*, int);
void getSourceOpts(int, const char **, int*);

int isIdent(char *name);
int isNum(char *text);

int main(int argc, const char *args[]) {
	FILE *fp;
	token t;
    int argvals[2];

    getSourceOpts(argc, args, argvals);

    fp = fopen(args[argvals[1]], "r");
    switch (argvals[0]) {
        case 0: // clean
            printsource(fp, 1);
            rewind(fp);
            break;
        case 1: // comments
            printsource(fp, 0);
            rewind(fp);
            break;
    }

	t = getNextToken(fp);
	do {
		printTokenType(t);
        t = getNextToken(fp);
	} while (!feof(fp));


	fclose(fp);
	return 0;
}

void printTokenType(token t) {
    token_type type = findTokenType(t);

    printf("%s               ", t.text);

    switch (error_num) {
        case valid:
            printf("%d\n", type);
            break;
        case ident_starts_with_num:
            puts("Error: identifier begins with number");
            break;
        case ident_too_long:
            puts("Error: identifier too long");
            break;
        case invalid_token:
            puts("Error: invalid token");
            break;
        case num_val_exceeds_max:
            puts("Error: number value exceeds maximum");
    }
}

token getNextToken(FILE *fp) {
	token ret;
	char c;
	int in_comment = 0;

    error_num = valid;

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

    error_num = invalid_token;
	return nulsym;
}

int isIdent(char *name) {
	int i = 0;	

    if (!isalpha(name[0]) && !isdigit(name[0])) return 0;

	while(name[i] != '\0') {
		if(!isalpha(name[i]) && !isdigit(name[i])) {
			if (isalpha(name[0])) error_num = invalid_token;
            return 0;
        }
        i++;
        if (i > 13) {
            error_num = ident_too_long;
            name[i] = '\0';
            return 1;
        }
	}

    if (isdigit(name[0])) error_num = ident_starts_with_num;
	return 1;
}

int isNum(char *text) {
    int num;
    char *t = text;

    while (*t)
        if (!isdigit(*t++))
            return 0;

    num = atoi(text);
    
    if (num > 65535) {
        error_num = num_val_exceeds_max;
    }

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

    printf("\n\n");
}

void printSourceWithAndWithoutComments(FILE *fp) {
    puts("\nsource code:\n-----------");
    printsource(fp, 0);
    puts("\nsource code without comments:\n-----------------------------");
    printsource(fp, 1);
}

void getSourceOpts(int argc, const char **argv, int *argvals) {
    printf("argc: %d\n", argc);
    for (int i = 0; i < argc; i++) {
        printf("%d:  %s, ", i, argv[i]);
    }
    if (argc < 3) {
        argvals[0] = 3;
        argvals[1] = 1;
        return;
    }

    if (!strcmp(argv[1], "--clean")) {
        argvals[0] = 0;
        argvals[1] = 2;
        return;
    }
    if (!strcmp(argv[1], "--source")) {
        argvals[0] = 1;
        argvals[1] = 2;
        return;
    }

    if (!strcmp(argv[2], "--clean")) {
        argvals[0] = 0;
        argvals[1] = 1;
        return;
    }
    
    if (!strcmp(argv[2], "--source")) {
        argvals[0] = 1;
        argvals[1] = 1;
        return;
    }
}
