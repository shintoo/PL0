#ifndef PARSER_H_
#define PARSER_H_

#include <stdio.h>

#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct symbol {
	int kind; // const 1 var 2 proc 3
	char name[12];
	int val;
	int level;
	int addr;
} symbol;

extern symbol symbol_table[];
extern unsigned symbol_count;

void program(void);
void block(int level);
void statement();
void condition(void);
void expression(void);
void term(void);
void factor(void);

extern FILE *output;
extern FILE *input;

#endif
