#include <stdio.h>
#include "parser.h"
#include "lexer.h"

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

	fclose(output);
	fclose(source);

	return 0;
}
