#include "code_gen.h"
#include "error.h"

Instruction code[CODE_SIZE];
int cx = 0;

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
	for (int i = 0; i < cx; i++) {
		fprintf(out, "%d %d %d\n", code[i].op, code[i].l, code[i].m);
	}
}
