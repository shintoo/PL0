void program(void) {
	advance();
	block();
	if (token != periodsym)
		error();
}

void block(void) {
	if (token == constsym) { /* make a constant */
		do {
			advance(); /* get ident */
			if (token != identsym)
				error();
			advance(); /* get eq */
			if (token != eqsym)
				error();
			advance(); /* get value */
			if (token != numbersym)
				error();
			advance(); /* get semivolon or comma */

			// create constant variable using given
			// ident as name, number as value, L and M in symbol table
		} while (token == commasym);

		if (token != semicolonsym)
			error();
		advance();
	}

	if (token == varsym) { /* make a variable */
		do {
			advance(); /* get ident */
			if (token != identsym)
				error();
			advance(); /* semicolon or comma */
			
			// create variable using given ident as name, L and M
			// in the symbol table
		} while (token == commasym);
		advance();
	}

	while (token == procsym) { /* make a procedure */
		advance(); /* get procedure name */
		if (token != identsym)
			error();
		advance(); /* get semicolon */
		if (token != semicolonsym)
			error();
		advance() /* move to start of block */
		block(); /* process block */
		if (token != semicolonsym)
			error();
		advance();

		// make procedure in symbol table using
		// ident as name, L and M in symbol table
	}
	statement();
}

void statement(void) {
	switch (token) {
		case identsym:
			advance();
			if (token != becomessym)
				error();
			advance();
			expression();
			break;
		case callsym:
			advance();
			if (token != identsym)
				error();
			advance();
			break;
		case beginsym:
			advance();
			statement();
			while (token == semicolonsym) {
				advance();
				statement();
			}
			break;
		case ifsym:
			advance();
			condition();
			if (token != thensym)
				error();
			advance();
			statement();
			break;
	}
}

void condition(void) {
	if (token == oddsym) {
		advance();
		expression();
	} else {
		expression();
		if (token != relation /* fix this later, <> < > = etc */)
			error();
		advance();
		expression();
	}
}

void expression(void) {
	if (token == plussym || token == minussym)
		advance();
	term();
	while (token == plussym || token == minussym) {
		advance();
		term();
	}
}

void term(void) {
	factor();
	while (token == multsym || token == slashsym) {
		advance();
		factor();
	}
}

void factor(void) {
	switch (token) {
		case identsym:
			advance();
			break;
		case numbersym:
			advance();
			break;
		case lparensym:
			advance();
			expression();
			if (token != rparensym)
				error();
			advance();
			break;
		default:
			error();
	}
}

