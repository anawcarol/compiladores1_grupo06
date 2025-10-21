/* * ARQUIVO: scr/main.c
 * Conteúdo completo e corrigido.
 */
#include <stdio.h>
#include <stdlib.h>

/* Incluir a AST */
#include "../ast/ast.h"

int yyparse(void);
extern FILE* yyin;

/* Declarar a raiz da AST que vem do parser */
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

    /* yyparse() retorna 0 se a análise for bem-sucedida */
    int ret = yyparse();

    if (ret == 0) {
        printf("Analise sintatica concluida com sucesso!\n\n");
        printf("--- Arvore Sintatica Abstrata ---\n");

        // --- CORREÇÃO AQUI ---
        // A 'ast_root' é o *início* de uma lista encadeada de statements.
        // Precisamos percorrer essa lista usando o ponteiro 'next'.
        for (NoAST *node = ast_root; node; node = node->next) {
            imprimirAST(node, 0);
        }
        // --- FIM DA CORREÇÃO DE IMPRESSÃO ---

        printf("---------------------------------\n");
        
        // --- CORREÇÃO AQUI TAMBÉM ---
        // Precisamos liberar todos os nós da lista principal.
        NoAST *node = ast_root;
        while (node) {
            NoAST *proximo = node->next; // Guarda o próximo antes de liberar o atual
            liberarAST(node);
            node = proximo;
        }
        // --- FIM DA CORREÇÃO DE LIBERAÇÃO ---
    } else {
        printf("\nFalha na analise sintatica.\n");
    }

    fclose(yyin);
    return ret;
}