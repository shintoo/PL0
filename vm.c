#include "vm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    
	if (argc != 2) usage(argv[0]);

	loadInstructions(argv[1]);
	while(1) {
		fetch();
		execute();
	}
	return 0;
}

void usage(const char *pname) {
    printf("%s: usage: %s input-file\n", pname, pname);
    ABORT("no input file provided\n");
}

void loadInstructions(const char* filePath) {
   

    FILE *source = fopen(filePath, "r");
    char buf[32];
    char token[32];
    int i;

    if (!source) ABORT("could not load file %s\n", filePath);

    printf("PL/0 code:\n\n");

    for (i = 0; fscanf(source, "%d %d %d", &code[i].op, &code[i].l, &code[i].m); i++) {
        //fgets(buf, 32, source);
       	 
	//code[i].op = atoi(strtok(buf, " "));
        //code[i].l = atoi(strtok(NULL, " "));
        //code[i].m = atoi(strtok(NULL, " "));
        
	printInstruction(code + i);
    }
}

void printInstruction(const Instruction* i) {
    static const char *NAMES[] = {
        "__", "LIT", "OPR", "LOD", "STO", "CAL", "INC",
        "JMP", "JPC", "SIO"
    };
    static const char *OPNAMES[] = {
        "RET", "NEG", "ADD", "SUB", "MUL", "DIV", "ODD",
        "MOD", "EQL", "NEQ", "LSS", "LEQ", "GTR", "GEQ"
    };
    static const char *SIONAMES[] = {
        "OUT", "INP", "HLT"
    };
    static int num = 0;

    printf("%4d  %s\t%d\t%4d\n", num++, 
            i->op == 2 ?
                OPNAMES[i->m] : 
                i->op == 9 ?
                    SIONAMES[i->m] :
                    NAMES[i->op]
            , i->l, i->m
    );
}

void fetch(void) {	
	ir = code[pc];
	pc += 1;

}

void execute(void) {
	
	switch(ir.op) {
		case 1:
			sp += 1;
			stack[sp] = ir.m;
			break;
		case 2:
			switch(ir.m) {
				case 0:
					sp = bp - 1;
					pc = stack[sp + 4];
					bp = stack[sp+3];
					break;
				case 1:
					stack[sp] *= -1;
					break;
				case 2:
					sp -= 1;
					stack[sp] += stack[sp+1];
					break;
				case 3: 
					sp -= 1;
					stack[sp] -= stack[sp+1];
					break;
				case 4:
					sp -= 1;
					stack[sp] *= stack[sp+1];
					break;
				case 5:
					sp -= 1;
					stack[sp] /= stack[sp+1];
					break;
				case 6:
					stack[sp] %= 2;
					break;
				case 7:
					stack[sp] -= 1;
					stack[sp] = stack[sp] % stack[sp+1];
					break;
				case 8:
					sp -= 1;
					stack[sp] = (stack[sp] == stack[sp+1]);
					break;
				case 9:
					sp -= 1;
					stack[sp] = (stack[sp] != stack[sp+1]);
					break;
				case 10:
					sp -= 1;
					stack[sp] = (stack[sp] < stack[sp+1]);
					break;
				case 11:
					sp -= 1;
					stack[sp] = (stack[sp] <= stack[sp+1]);
					break;
				case 12:
					sp -= 1;
					stack[sp] = (stack[sp] > stack[sp+1]);
					break;
				case 13:
					sp -= 1;
					stack[sp] = (stack[sp] >= stack[sp+1]);
					break;
			}

		case 3: 
			sp += 1;
			stack[sp] = stack[base(ir.l, bp) + ir.m];
			break;
		case 4:
			stack[base(ir.l, bp) + ir.m];
			sp -= 1;
			break;
		case 5:
			stack[sp+1] = 0;
			stack[sp+2] = base(ir.l, bp);
			stack[sp+3] = bp;
			stack[sp+4] = pc;
			bp += 1;
			pc = ir.m;
			break;
		case 6:
			sp += ir.m;
			break;
		case 7:
			pc = ir.m;
			break;
		case 8:
			if (!stack[sp]) pc = ir.m;
			sp -= 1;
			break;
		case 9:
			switch(ir.m) {
				case 0: 
					printf("%d\n", stack[sp]);
					sp -= 1;
					break;
				case 1:
					sp += 1;
					scanf("%d", &stack[sp]);
					break;
				case 2:
					exit(0);
					break;

			}
			break;	

	}		

}

int base(int level, int bp) {
	while(level > 0) {
		bp = stack[bp+1];
		level--;
	}
	return bp;
}
