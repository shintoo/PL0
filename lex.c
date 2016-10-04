//
// lexical tokens: 
// identifiers letter (digit | letter)*
// plus, mult, left paren, right paren

// I follow the conventions of lex
// the token type is returned by the lex function
// the token value (if any) is availalbe in the global variable lval

#include <stdio.h>
#include "tokens.h"

LVAL lval;

token_type lex() {
  char c;
  // ignore space, tab, newline
  while ((c=getchar()) == ' ' || c== '\t' || c == '\n')
    ; 
  if (c == EOF) return nulsym;

  // identifier
  if (isalpha(c)) {
    char sbuf[100], *p = sbuf;
    do {
      *p++ = c;
    } while ((c=getchar()) != EOF && isalnum(c));
    ungetc(c, stdin);
    *p = '\0';
    lval.id = sbuf;
    return idsym;
  }

  // number
  if (isdigit(c)) {
    ungetc(c, stdin);
    scanf("%d", &lval.num);
    return numsym;
  } 

  switch (c) {
    case '+' :
      return plussym;
    case '*' :
      return multsym;
    case '(' :
      return lparensym;
    case ')' :
      return rparensym;
    default  :
      return -1; 
  }
}

