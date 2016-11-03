


void program(void) {
	// advance and call block
	advance();
	block();
	// if the end token is not a period, error
	if (token != periodsym)
		error(9);
}

void block(void) {
	if (token == constsym) { /* make a constant */
		do {
			advance(); /* get ident */
			if (token != identsym)
				error(4);
			advance(); /* get eq */
			if (token != eqsym)
				error(3);
			advance(); /* get value */
			if (token != numbersym)
				error(2);
			advance(); /* get semivolon or comma */

			// create constant variable using given
			// ident as name, number as value, L and M in symbol table
		} while (token == commasym);

		if (token != semicolonsym)
			error(17);
		advance();
	}

	if (token == varsym) { /* make a variable */
		do {
			advance(); /* get ident */
			if (token != identsym)
				error(4);
			advance(); /* semicolon or comma */
			
			// create variable using given ident as name, L and M
			// in the symbol table
		} while (token == commasym);
		if (token != semicolonsym)
			error(17);
		advance();
	}

	while (token == procsym) { /* make a procedure */
		advance(); /* get procedure name */
		if (token != identsym)
			error(4);
		advance(); /* get semicolon */
		if (token != semicolonsym)
			error(17);
		advance() /* move to start of block */
		block(); /* process block */
		if (token != semicolonsym)
			error(17);
		advance();

		// make procedure in symbol table using
		// ident as name, L and M in symbol table
	}
	statement();
}

void statement(void) {
	switch (token) {
		// if it is an identifier
		case identsym:
			advance();
			// must be becomessym
			if (token != becomessym)
				error(13);
			// get the next token and call expression function
			advance();
			expression();
			break;
		// if it is a callsym
		case callsym:
			// advance and make sure it is an identsym
			advance();
			if (token != identsym)
				error(14);
			advance();
			break;
		// if it is a beginsym
		case beginsym:
			// advance and call the statement funciton
			advance();
			statement();
			// contiue calling statemnt while the token is a semicolon
			while (token == semicolonsym) {
				advance();
				statement();
			}
			if (token != endsym)
				error(17);
			break;
		// if it is an if statment
		case ifsym:
			// advance and call the condition function
			advance();
			condition();
			// make sure the next token is a thensym
			if (token != thensym)
				error(16);
			// advance and call the statement function
			advance();
			statement();
			break;

		case whilesym:
			advance();
			condition();
			if (token != dosym)
				error(18);
			advance();
			statement();
			break;

	}
}

void condition(void) {
	// for an oddsym
	if (token == oddsym) {
		advance();
		expression();
	} else {
		expression();
		if (token != relation /* fix this later, <> < > = etc */)
			error(20);
		advance();
		expression();
	}
}

void expression(void) {
	// if addition or subtraction
	if (token == plussym || token == minussym)
		advance();
	term();
	// continue while adding or subtracting
	while (token == plussym || token == minussym) {
		advance();
		term();
	}
}

void term(void) {
	// call factor and continue while multiplying or dividing
	factor();
	while (token == multsym || token == slashsym) {
		advance();
		factor();
	}
}

void factor(void) {
	switch (token) {
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
			if (token != rparensym)
				error(22);
			advance();
			break;
		default:
			error(23);
	}
}

