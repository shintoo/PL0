// main.c
#include <stdio.h>
#include "parser.h"

extern FILE *source;
extern FILE *output;

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
