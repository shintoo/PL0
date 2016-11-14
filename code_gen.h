#ifndef satya_h
#define satya_h

#include <stdio.h>

typedef struct _instruction {
	int op;
	int l;
	int m;
} Instruction;

#define CODE_SIZE 500

extern Instruction code[];
extern int cx;

void emit(int op, int l, int m);
void dump(FILE *out);

#endif
