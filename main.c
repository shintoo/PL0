#include <stdio.h>
#include "parser.h"
#include "lexer.h"

#include "code_gen.h"

int main(int argc, char **argv) {
	if (argc != 3) {
		fprintf(stderr, "usage: %s <input> <output>\n", argv[0]);
		return 127;
	}

	puts("opening source");

	source = fopen(argv[1], "r");

	puts("opened source)");

	if (!source) {
		fprintf(stderr, "%s not found\n", argv[1]);
		return 127;
	}

	puts("opening output");

	output = fopen(argv[2], "w");

	puts("opened output\ncalling program()");

	program();	

	dump(output);

	fclose(output);
	fclose(source);

	return 0;
}
