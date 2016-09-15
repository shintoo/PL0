//Compiler Builder 35
//Sean Rapp
//Brody Nissen
//Satya Patel

#ifndef "vm.h"
#define "vm.h"

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct _instruction {
	int op; //opcode
	int l;
	int m;
} Instruction;

enum {
	LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO
}

int bp = 1;
int pc = 0;
int sp = 0;

int stack[MAX_STACK_HEIGHT] = {0};
Instruction code[MAX_CODE_LENGTH];

Instruction ir;

void loadInstructions(const char* path);
void printInstructions(const Instruction* i);
void fetch(void);
void execute(void);




#endif
