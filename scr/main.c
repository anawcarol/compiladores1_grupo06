#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "../tabela/tabela.h"

int yyparse(void);
extern FILE* yyin;

extern NoAST *ast_root;

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

    tab_inicializar();
    int ret = yyparse();

    if (ret == 0) {
        printf("Analise sintatica concluida com sucesso!\n\n");
        printf("--- Arvore Sintatica Abstrata ---\n");

        for (NoAST *node = ast_root; node; node = node->next) {
            imprimirAST(node, 0);
        }

        printf("---------------------------------\n");
        
        tab_imprimirTabela();
        
        NoAST *node = ast_root;
        while (node) {
            NoAST *proximo = node->next;
            liberarAST(node);
            node = proximo;
        }
        
    } else {
        printf("\nFalha na analise sintatica.\n");
    }

    fclose(yyin);
    return ret;
}