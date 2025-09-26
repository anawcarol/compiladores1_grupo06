#include <stdio.h>
#include <stdlib.h>

int yyparse(void);
extern FILE* yyin;

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s arquivo.lox\n", argv[0]);
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    int ret = yyparse();

    fclose(yyin);
    return ret;
}
