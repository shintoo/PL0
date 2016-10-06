#include <stdio.h>

void printsource(FILE *fp, int comments_hidden) {
    char buffer[80];
    int i;
    int in_comment = 0;

    rewind(fp);

    fgets(buffer, 80, fp);

    do {
        for (i = 0; buffer[i]; i++) {
            if (comments_hidden) {
                if (!in_comment) {
                    if (buffer[i] == '/' && buffer[i+1] == '*') {
                        in_comment = 1;
                        i++;
                        printf("  ");
                    }
                    else
                        putchar(buffer[i]);
                } else {
                    if (buffer[i] == '*' && buffer[i+1] == '/') {
                        in_comment = 0;
                        i++;
                    }
                    putchar(' ');
                }
            } else {
                putchar(buffer[i]);
            }
        }
        fgets(buffer, 80, fp);
    } while (!feof(fp));
}

void printSourceWithAndWithoutComments(FILE *fp) {
    puts("\nsource code:\n-----------");
    printsource(fp, 0);
    puts("\nsource code without comments:\n-----------------------------");
    printsource(fp, 1);
}

int main(int argc, char **argv) {
    FILE *fp = fopen(argv[1], "r");

    printSourceWithAndWithoutComments(fp);

    fclose(fp);
    return 0;
}
