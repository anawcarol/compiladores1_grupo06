#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.tab.h"

static TacNode *head = NULL;
static TacNode *tail = NULL;

static void emit(TacOp op, char *res, char *arg1, char *arg2) {
    TacNode *novo = criarTac(op, res, arg1, arg2);
    anexarTac(&head, &tail, novo);
}

static char* visitarExpressao(NoAST *node) {
    if (!node) return NULL;

    switch (node->type) {
        case NODE_NUM: {
            char buffer[32];
            sprintf(buffer, "%.2f", node->data.number); // Usa float com 2 casas para exemplo
            return strdup(buffer);
        }
        case NODE_IDENTIFIER:
            return strdup(node->data.identifier);

        case NODE_BINARY_OP: {
            // 1. Resolve filhos recursivamente
            char *t1 = visitarExpressao(node->data.binary_op.left);
            char *t2 = visitarExpressao(node->data.binary_op.right);
            
            // 2. Gera novo temporário
            char *tRes = gerarTemp();
            
            // 3. Mapeia operação e emite instrução
            TacOp op = TAC_ADD; // Default
            switch(node->data.binary_op.op) {
                case PLUS: op = TAC_ADD; break;
                case MINUS: op = TAC_SUB; break;
                case STAR: op = TAC_MUL; break;
                case SLASH: op = TAC_DIV; break;
            }
            
            emit(op, tRes, t1, t2);
            
            free(t1); free(t2);
            return tRes; // Retorna o nome do temporário (ex: "t0")
        }

        case NODE_ASSIGN: {
            char *val = visitarExpressao(node->data.assign.value);
            // Emite: variavel = valor_calculado
            emit(TAC_COPY, node->data.assign.name, val, NULL);
            // Retorna o nome da variável como resultado da expressão
            return strdup(node->data.assign.name);
        }

        // Agrupamento (Parênteses) é implícito na estrutura da árvore, 
        // então não precisa de case especial se o parser já montou a hierarquia.

        default:
            return NULL;
    }
}

static void visitarStatement(NoAST *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_BLOCK:
            for (NoAST *s = node->data.block.statements; s; s = s->next)
                visitarStatement(s);
            break;

        case NODE_VAR_DECL:
            if (node->data.var_decl.initializer) {
                char *val = visitarExpressao(node->data.var_decl.initializer);
                emit(TAC_COPY, node->data.var_decl.name, val, NULL);
                free(val);
            }
            break;

        case NODE_EXPR_STMT: {
            char *res = visitarExpressao(node->data.expr_stmt.expression);
            if (res) free(res);
            break;
        }
        
        default:
            break;
    }
}

TacNode* gerarCodigo(NoAST *ast) {
    head = NULL; tail = NULL;
    NoAST *atual = ast;
    while (atual) {
        visitarStatement(atual);
        atual = atual->next;
    }
    return head;
}