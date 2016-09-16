#include "vm.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) usage(argv[0]);

    loadInstructions(argv[1]);
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

    for (i = 0; !feof(source) && i < MAX_CODE_LENGTH; i++) {
        fgets(buf, 32, source);
        code[i].op = atoi(strtok(buf, " "));
        code[i].l = atoi(strtok(NULL, " "));
        code[i].m = atoi(strtok(NULL, " "));
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
	
}

void execute(void) {

}


