#ifndef satya_h
#define satya_h

typedef struct _instruction {
	int op;
	int l;
	int m;
} Instruction;

#define CODE_SIZE 500

extern Instruction *code;
extern int cx;

void emit(int op, int l, int m);

#endif
