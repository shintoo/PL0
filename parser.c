#include <stdio.h>
#include "t_types.h"
#include "parser.h"

FILE *output;

void add_to_symbol_table(int kind, char *name, int val, int level, int addr) {
	symbol_table[symbol_count].kind = kind;
	strncpy(symbol_table[symbol_count].name, name, 12);
	symbol_table[symbol_count].val = val;
	symbol_table[symbol_count].level = level;
	symbol_table[symbol_count].addr = addr;

	symbol_count++;
}

void program(void) {
	// advance and call block
	advance();
	block(0);
	// if the end t_type is not a period, error
	if (t_type != periodsym)
		error(9);
}

void block(int level) {
	if (t_type == constsym) { /* make a constant */
		do {
			int value;
			char label[12];
			advance(); /* get ident */
			if (t_type != identsym)
				error(4);
			strncpy(label, t->text, 12);

			advance(); /* get eq */
			if (t_type != eqsym)
				error(3);
			advance(); /* get value */
			if (t_type != numbersym)
				error(2);
			value = atoi(t->text);

			advance(); /* get semivolon or comma */

			add_to_symbol_table(1, label, value, level, /*???*/);
			// create constant variable using given
			// ident as name, number as value, L and M in symbol table
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
			advance(); /* semicolon or comma */
			

			add_to_symbol_table(2, t->text, 0, level, /*???*/);
			// create variable using given ident as name, L and M
			// in the symbol table
		} while (t_type == commasym);
		if (t_type != semicolonsym)
			error(17);
		advance();
	}

	while (t_type == procsym) { /* make a procedure */
		char label[12];
		advance(); /* get procedure name */
		if (t_type != identsym)
			error(4);
		strncpy(labe, t->text, 12);

		advance(); /* get semicolon */
		if (t_type != semicolonsym)
			error(17);
		advance() /* move to start of block */
		block(level + 1); /* process block */
		if (t_type != semicolonsym)
			error(17);
		advance();

		add_to_symbol_table(3, label, 0, level, /*???*/);
		// make procedure in symbol table using
		// ident as name, L and M in symbol table
	}
	statement();
}

void statement(void) {
	switch (t_type) {
		// if it is an identifier
		case identsym:
			advance();
			// must be becomessym
			if (t_type != becomessym)
				error(13);
			// get the next t_type and call expression function
			advance();
			expression();
			break;
		// if it is a callsym
		case callsym:
			// advance and make sure it is an identsym
			advance();
			if (t_type != identsym)
				error(14);
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
			statement();
			break;

		case whilesym:
			advance();
			condition();
			if (t_type != dosym)
				error(18);
			advance();
			statement();
			break;

	}
}

void condition(void) {
	// for an oddsym
	if (t_type == oddsym) {
		advance();
		expression();
	} else {
		expression();
		if (t_type != relation /* fix this later, <> < > = etc */)
			error(20);
		advance();
		expression();
	}
}

void expression(void) {
	// if addition or subtraction
	if (t_type == plussym || t_type == minussym)
		advance();
	term();
	// continue while adding or subtracting
	while (t_type == plussym || t_type == minussym) {
		advance();
		term();
	}
}

void term(void) {
	// call factor and continue while multiplying or dividing
	factor();
	while (t_type == multsym || t_type == slashsym) {
		advance();
		factor();
	}
}

void factor(void) {
	switch (t_type) {
		// advance if it is an identsym or numbersym
		case identsym:
			advance();
			break;
		case numbersym:
			advance();
			break;
		// if if is a parenthesis advance and call expression
		case lparensym:
			advance();
			expression();
			if (t_type != rparensym)
				error(22);
			advance();
			break;
		default:
			error(23);
	}
}

