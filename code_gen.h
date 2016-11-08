#ifndef satya_h
#define satya_h

typedef struct _instruction {
	int op;
	int l;
	int m;
} Instruction;

#define CODE_SIZE 500

Instruction code[CODE_SIZE];
int cx = 0;

void emit(int, int, int);

#endif
