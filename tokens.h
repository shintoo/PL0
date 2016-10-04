typedef enum {nulsym = 0, idsym, numsym, lparensym, rparensym, plussym, multsym} token_type;

typedef union {
  char *id;
  int num;   
} LVAL; 

