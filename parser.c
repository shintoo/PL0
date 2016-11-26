// parser/code gen.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lexer.h"
#include "parser.h"

void error(int);

Instruction code[CODE_SIZE];
int cx = 0;

FILE *output;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
unsigned symbol_count[MAX_SYMBOL_TABLE_SIZE] = {0};
unsigned symbol_count_total = 0;

int level = 0;

extern token_type t_type;
extern token t;

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
		if (!strcmp(symbol_table[i].name, ident) && symbol_table[i].level <= level)
			return i;
	}

	error(11);
}

void add_to_symbol_table(int kind, char *name, int val, int level, int addr) {
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
	int ctemp = cx;
	emit(JMP, 0, 0);
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

		add_to_symbol_table(proc, label, cx, level, cx);
		advance(); /* move to start of block */
		level++; //sat	
		
		block(); /* process block */
		emit(OPR, 0, RET);
		level--;
		if (t_type != semicolonsym)
			error(17);
		advance();


		// make procedure in symbol table using
		// ident as name, L and M in symbol table
	}

	code[ctemp].m = cx;
	emit(INC, 0, space + 4);
	statement();
}

void statement(void) {
	int ctemp, cx1, cx2, fr;
	
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
