#include <stdio.h>
#include <stdlib.h>
extern FILE *source, *output;

// error.c
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

