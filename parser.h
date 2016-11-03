#ifndef PARSER_H_
#define PARSER_H_

#define MAX_SYMBOL_TABLE_SIZE 100

typedef struct symbol {
	int kind;
	char name[12];
	int val;
	int level;
	int addr;
} symbol;

smybol symbol_table[MAX_SYMBOL_TABLE_SIZE];

void program(void);
void block(void);
void statement(void);
void condition(void);
void expression(void);
void term(void);
void factor(void);

#endif
