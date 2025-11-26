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
    int ret = yyparse();

    if (ret == 0) {
        resolver(ast_root);

        TacNode *codigo = gerarCodigo(ast_root);
        
        gerarCodigoC(codigo);

        liberarTac(codigo);
        liberarAST(ast_root);
    } else {
        fprintf(stderr, "Falha na analise.\n");
    }

    fclose(yyin);
    return ret;
}