//Compiler Builder 35
//Sean Rapp
//Brody Nissen
//Satya Patel

#ifndef VM_H
#define VM_H

#include <stdlib.h>

#define ABORT(...) do { fprintf(stderr, "error: " __VA_ARGS__); exit(EXIT_FAILURE); } while (0);

#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3

typedef struct _instruction {
	int op; //opcode
	int l;
	int m;
	int line;
} Instruction;

enum {
	LIT = 1, OPR, LOD, STO, CAL, INC, JMP, JPC, SIO
};

int bp = 1;
int pc = 0;
int sp = 0;

int stack[MAX_STACK_HEIGHT] = {0};
Instruction code[MAX_CODE_LENGTH];

Instruction ir;

void usage(const char *pname);
void loadInstructions(const char* path);
void printInstruction(const Instruction* i);
void fetch(void);
void execute(void);
int base(int level, int bp);
#endif
