/* Satya Patel
   Sean Rapp
   Brody "Broderson" Nissen */

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct _instruction {
	int op;
	int l;
	int m;
} Instruction;

#define CODE_SIZE 500

void emit(int op, int l, int m);
void dump(FILE *out);

enum {
	constant = 1, var, proc
};

typedef struct symbol {
	int kind; // const 1 var 2 proc 3
	char name[12];
	int val;
	int level;
	int addr;
} symbol;

void program(void);
void block(void);
void statement();
void condition(void);
void expression(void);
void term(void);
void factor(void);

Instruction code[CODE_SIZE];
int cx = 0;

FILE *output;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
unsigned symbol_count[MAX_SYMBOL_TABLE_SIZE] = {0};
unsigned symbol_count_total = 0;

FILE *source;

typedef enum token {
  nulsym = 1, identsym, numbersym, plussym, minussym,
  multsym, slashsym, oddsym, eqsym, neqsym, lessym, leqsym,
  gtrsym, geqsym, lparentsym, rparentsym, commasym, semicolonsym,
  periodsym, becomessym, beginsym, endsym, ifsym, thensym,
  whilesym, dosym, callsym, constsym, varsym, procsym, writesym,
  readsym , elsesym
} token_type;

enum {
	LIT=1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO
};

enum {
	RET=0, NEG, ADD, SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ,
};

enum {
	OUT=0, INP, HLT
};

enum {
	valid, ident_too_long, ident_starts_with_num, num_val_exceeds_max, invalid_token
};

int error_num; //identifies which type of error it is

typedef struct _token {
	int which; // obsolete
	char text[100];
} token;

token t;
token_type t_type;

int level = 0;

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

int isNum(char *text);
int isIdent(char *text);

// ========

void advance() {
	t = getNextToken(source);
	t_type = findTokenType(&t);
	printf("%s\n", t.text);
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

void error(int num) {


	static const char *errors[] = {"this is the 0 placeholder fam..............", // 0
		       "Used = instead of :=.",  // 1
		       "= must be followed by a number.", // 2
		       "Constant identifier must be followed by =.", // 3
		       "const, var, procedure must be followed by identifier.", // 4
		       "Comma missing.", // 5
		       "Incorrect symbol after procedure declaration", // 6
		       "Statement expected", // 7
		       "Incorrect symbol after statement part in block", // 8
		       "Period expected.", // 9
		       "Semicolon between statements missing.", // 10
		       "Undeclared identifier", // 11
		       "Assignment to constant or procedure is not allowed", // 12
		       "Assignment operator expected", // 13
		       "call must be followed by an identifier.", // 14
		       "call of a constant or variable is meaningless.",  // 15
		       "then expected.", // 16
		       "Semicolon or end expected.", // 17
		       "do expected.", // 18
		       "Incorrect symbol following statement.", // 19
		       "Relational operator expected", // 20
		       "Expression must not contain a procedure identifier.", // 21
		       "Right parenthesis missing.", // 22
		       "The preceding factor cannot begin with this symbol.", // 23
		       "An expression cannot begin with this symbol.", // 24
		       "This number is too large.", // 25
               "Identifier expected after read or write.", // 26
			   "Invalid token.", //27
			   "Token too long.", //28
			   "Identifier cannot start with a digit." //29
	};


	fprintf(stderr, "Error %d: %s", num, errors[num]);
	fclose(source);
	fclose(output);
	exit(EXIT_FAILURE);
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
		if(!isalpha(name[i]) && !isdigit(name[i])) {
			if (isalpha(name[0])) error_num = invalid_token;
			return 0;
		}
		i++;
		if (i > 11) { //if you hit 12 characters, that means your string is too long
			error(28);   //notify that this happened
			name[i] = '\0';
			return 1; 
		}
	}
	//this is another error, and it should be notified
	if (isdigit(name[0])) {
		error(29);
	}
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

void emit(int op, int l, int m) {
	if (cx > CODE_SIZE)
		error(25);
	else {
		code[cx].op = op;
		code[cx].l = l;
		code[cx++].m = m;
	}
}

void dump(FILE *out) {
	for (int i = 0; i < cx; i++)
		fprintf(out, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
}

int find_symbol(char *ident) {
	for (int i = 0; i < symbol_count_total; i++) {
		printf(">\"%s\" %d\n", symbol_table[i].name, symbol_table[i].level);
		if (!strcmp(symbol_table[i].name, ident) && symbol_table[i].level <= level)
			return i;
	}

	error(11);
}

void add_to_symbol_table(int kind, char *name, int val, int level, int addr) {
	printf(">adding \"%s\"\n", name);
	symbol_table[symbol_count_total].kind = kind;
	strncpy(symbol_table[symbol_count_total].name, name, 12);
	symbol_table[symbol_count_total].val = val;
	symbol_table[symbol_count_total].level = level;
	symbol_table[symbol_count_total].addr = addr;

	symbol_count[level]++;
	symbol_count_total++;
}

// PARSER ------------

void program(void) {
	emit(JMP, 0, 0);
	// advance and call block
	advance();
	block();
	// if the end t_type is not a period, error
	if (t_type != periodsym)
		error(9);

	emit(SIO, 0, 2);
}

void block(void) {
	int space = 0;

	if (t_type == constsym) { /* make a constant */
		do {
			int value;
			char label[12];
			advance(); /* get ident */
			if (t_type != identsym)
				error(4);
			strncpy(label, t.text, 12);

			advance(); /* get eq */
			if (t_type != eqsym)
				error(3);
			advance(); /* get value */
			if (t_type != numbersym)
				error(2);
			value = atoi(t.text);

			advance(); /* get semivolon or comma */

			add_to_symbol_table(constant, label, value, level, symbol_count[level] + 4);
			// create constant variable using given
			// ident as name, number as value, L and M in symbol table
			space++;
		} while (t_type == commasym);

		if (t_type != semicolonsym)
			error(17);
		advance();
	}

	if (t_type == varsym) { /* make a variable */
		do {
			advance(); /* get ident */
			if (t_type != identsym)
				error(4);	
			add_to_symbol_table(var, t.text, 0, level, symbol_count[level] + 4);
			advance(); /* semicolon or comma */
			// create variable using given ident as name, L and M
			// in the symbol table
			space++;
		} while (t_type == commasym);
		if (t_type != semicolonsym) {
			if (t_type == identsym)
				error(5);
			error(17);
		}
		advance();
	}

	

	while (t_type == procsym) { /* make a procedure */
		char label[12];
		advance(); /* get procedure name */
		if (t_type != identsym)
			error(4);
		strncpy(label, t.text, 12);

		advance(); /* get semicolon */
		if (t_type != semicolonsym)
			error(17);

		add_to_symbol_table(proc, label, cx-1, level, cx);
		advance(); /* move to start of block */
		level++; //sat
		emit(JMP, 0, cx+1);
		block(); /* process block */
		emit(OPR, 0, RET);
		level--;
		if (t_type != semicolonsym)
			error(17);
		advance();


		// make procedure in symbol table using
		// ident as name, L and M in symbol table
	}
	
	emit(INC, 0, space + 4);
	statement();
}

void statement(void) {
	int ctemp, cx1, cx2, fr;
	if (level == 0 && code[0].m == 0)
		code[0].m = cx-1;
	
	switch (t_type) {
		// if it is an identifier
		case identsym:
			fr = find_symbol(t.text);
			advance();
			// must be becomessym
			if (t_type != becomessym) {
				if (t_type == eqsym)
					error(1);
				else
					error(13);
			}

			// get the next t_type and call expression function
			advance();
			expression();

			emit(STO, level - symbol_table[fr].level, symbol_table[fr].addr);
			break;
		// if it is a callsym
		case callsym:
			advance();
			if (t_type != identsym)
				error(14);
			fr = find_symbol(t.text);
			if (symbol_table[fr].kind != proc)
				error(15);

			emit(CAL, level, symbol_table[fr].addr);

			advance();
			break;
		// if it is a beginsym
		case beginsym:
			// advance and call the statement funciton
			advance();
			statement();
			// contiue calling statemnt while the t_type is a semicolon
			while (t_type == semicolonsym) {
				advance();
				statement();
			}
			if (t_type != endsym)
				error(17);

			advance();
			break;
		// if it is an if statment
		case ifsym:
			// advance and call the condition function
			advance();
			condition();
			// make sure the next t_type is a thensym
			if (t_type != thensym)
				error(16);
			// advance and call the statement function
			advance();
			ctemp = cx;
			
			emit(JPC, 0, 0);
			statement();
			
			printf("%d: %s\n", __LINE__, t.text);
			code[ctemp].m = cx;

			if (t_type == elsesym) {
				code[ctemp].m++;
				ctemp = cx;
				emit(JMP, 0, 0);

				advance();
				statement();
				code[ctemp].m = cx;
			}

			break;

		case whilesym:		
			cx1 = cx;
			advance();
			condition();
			cx2 = cx;
			emit(JPC, 0, 0);
			if (t_type != dosym)
				error(18);
			advance();

			statement();
			emit(JMP, 0, cx1);
			code[cx2].m = cx;
			break;

		case readsym:
			advance();
			if (t_type != identsym)
				error(26);
			fr = find_symbol(t.text);
			if (symbol_table[fr].kind != var)
				error(26);
			emit(SIO, 0, 1);
			emit(STO, level - symbol_table[fr].level, symbol_table[fr].addr);

			advance();
			break;

		case writesym:
			advance();
			if (t_type != identsym) {
				error(26);
			}

			fr = find_symbol(t.text);

			if (symbol_table[fr].kind != var && symbol_table[fr].kind != constant) {
				error(26);
			}

			if (symbol_table[fr].kind == var)
				emit(LOD, level - symbol_table[fr].level, symbol_table[fr].addr);
			else if (symbol_table[fr].kind == constant)
				emit(LIT, 0, symbol_table[fr].val);

			emit(SIO, 0, 0);

			advance();
	}
}

int relation_op(void) {
	switch (t_type) {
		case eqsym:
			return EQL;
		case neqsym:
			return NEQ;
		case lessym:
			return LSS;
		case leqsym:
			return LEQ;
		case gtrsym:
			return GTR;
		case geqsym:
			return GEQ;
		default:
			return 0;
	}
}


void condition(void) {
	int r;

	// for an oddsym
	if (t_type == oddsym) {
		advance();
		expression();
		emit(ODD, 0, 0);
	} else {
		expression();
		if (t_type < eqsym || t_type > geqsym)
			error(20);
		r = relation_op();
		if (!r)
			error(28); 
		advance();
		expression();
		emit(OPR, 0, r); 
	}
}

void expression(void) {
	int addop;
	// if addition or subtraction
	if (t_type == plussym || t_type == minussym) {
		addop = t_type;
		advance();
		term();
		if (addop == minussym)
			emit(OPR, 0, NEG); //negate
	}
	else {
		term();
	}

	// continue while adding or subtracting
	while (t_type == plussym || t_type == minussym) {
		addop = t_type;
		advance();
		term();
		if (addop == plussym)
			emit(OPR, 0, ADD); //addition
		else
			emit(OPR, 0, SUB);
	}
}

void term(void) {
	// call factor and continue while multiplying or dividing
	int mulop;
	factor();
	while (t_type == multsym || t_type == slashsym) {
		mulop = t_type;
		advance();
		factor();
		if (mulop == multsym)
			emit(OPR, 0, MUL);
		else 
			emit(OPR, 0, DIV);
	}
}

void factor(void) {
	int fr = -1;
	switch (t_type) {
		// advance if it is an identsym or numbersym
		case identsym:
			fr = find_symbol(t.text);

			if (symbol_table[fr].kind == var)
				emit(LOD, level - symbol_table[fr].level, symbol_table[fr].addr);
			else if (symbol_table[fr].kind == constant)
				emit(LIT, 0, symbol_table[fr].val);
			else
				error(21);
			advance();
			break;
		case numbersym:
			fr = atoi(t.text);
			emit(LIT, 0, atoi(t.text));
			advance();
			break;
		// if if is a parenthesis advance and call expression
		case lparentsym:
			advance();
			expression();
			if (t_type != rparentsym)
				error(22);
			advance();
			break;
		default:
			error(23);
	}
}

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <input> <output>\n", argv[0]);
		return 127;
	}

	source = fopen(argv[1], "r");

	if (!source) {
		fprintf(stderr, "%s not found\n", argv[1]);
		return 127;
	}

	output = fopen(argv[2], "w");

	program();	

	dump(output);

	puts("No errors, program is syntactically correct.");

	fclose(output);
	fclose(source);

	return 0;
}
