#ifndef PARSER_H_
#define PARSER_H_

#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct symbol {
	int kind; // const 1 var 2 proc 3
	char name[12];
	int val;
	int level;
	int addr;
} symbol;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
unsigned symbol_count = 0;

void program(void);
void block(void);
void statement(void);
void condition(void);
void expression(void);
void term(void);
void factor(void);

extern FILE *output;
extern FILE *input;

#endif
