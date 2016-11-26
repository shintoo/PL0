// parser.h

void program(void);
void block(void);
void statement();
void condition(void);
void expression(void);
void term(void);
void factor(void);

#define MAX_SYMBOL_TABLE_SIZE 100
#define CODE_SIZE 500

typedef struct _instruction {
	int op;
	int l;
	int m;
} Instruction;

typedef struct symbol {
	int kind; // const 1 var 2 proc 3
	char name[12];
	int val;
	int level;
	int addr;
} symbol;

void emit(int op, int l, int m);
void dump(FILE *out);

enum {
	LIT=1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO
};

enum {
	RET=0, NEG, ADD, SUB, MUL, DIV, ODD, MOD, EQL, NEQ, LSS, LEQ, GTR, GEQ,
};

enum {
	OUT=0, INP, HLT
};