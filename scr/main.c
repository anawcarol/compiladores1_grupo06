#include <stdio.h>
#include <stdlib.h>

#include "../ast/ast.h"
#include "../tabela/tabela.h"
#include "../semantica/semantica.h"
#include "../codegen/codegen.h"
#include "../backend/backend_c.h"

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
    
    // Análise Sintática (Parser)
    int parseResult = yyparse();

    if (parseResult == 0) {
        
        // Análise Semântica
        // Imprime erros no stderr
        int errosSemanticos = resolver(ast_root);

        if (errosSemanticos > 0) {
            fprintf(stderr, "\nCompilacao abortada: %d erro(s) semantico(s) encontrado(s).\n", errosSemanticos);
            liberarAST(ast_root);
            fclose(yyin);
            return 1; 
        }

        // Geração de Código
        TacNode *codigo = gerarCodigo(ast_root);
        
        // Redireciona stdout 
        FILE *arquivo_saida = freopen("saida.c", "w", stdout);
        if (arquivo_saida == NULL) {
            fprintf(stderr, "Erro ao criar o arquivo saida.c\n");
            return 1;
        }

        gerarCodigoC(codigo);

        // Fecha stdout (arquivo saida.c) e libera memória
        fclose(stdout);
        
        // Usa stderr para mensagem de sucesso 
        fprintf(stderr, "Sucesso! Codigo gerado em 'saida.c'.\n");

        liberarTac(codigo);
        liberarAST(ast_root);

    } else {
        fprintf(stderr, "Falha na analise sintatica.\n");
        fclose(yyin);
        return 1;
    }

    fclose(yyin);
    return 0;
}