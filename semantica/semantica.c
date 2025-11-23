#include "semantica.h"
#include "../tabela/tabela.h"
#include "../ast/ast.h"
#include <stdio.h>
#include <stdlib.h>

static void visitarNo(NoAST *node);

static void visitarStatement(NoAST *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_VAR_DECL:
            // Apenas verificamos a inicialização se houver.
            if (node->data.var_decl.initializer) {
                visitarNo(node->data.var_decl.initializer);
            }
            break;

        case NODE_FUN_DECL:
            // Mas precisamos entrar no escopo para analisar o corpo corretamente.
            tab_entrarEscopo(); 
            
            // Não inserimos os parâmetros, pois o parser já os colocou na tabela.
            // Apenas percorremos a lista (caso queira validar algo nos params futuramente)
            // for (NoAST *p = node->data.fun_decl.params; p; p = p->next) { ... }

            visitarNo(node->data.fun_decl.body);
            tab_sairEscopo();
            break;

        case NODE_CLASS_DECL:
            // Mesma lógica: simula escopo, mas não insere.
            tab_entrarEscopo();
            for (NoAST *m = node->data.class_decl.methods; m; m = m->next) {
                visitarNo(m);
            }
            tab_sairEscopo();
            break;

        case NODE_PRINT_STMT:
            visitarNo(node->data.print_stmt.expression);
            break;
            
        case NODE_EXPR_STMT:
            visitarNo(node->data.expr_stmt.expression);
            break;
            
        case NODE_RETURN_STMT:
            if (node->data.return_stmt.expression) {
                visitarNo(node->data.return_stmt.expression);
            }
            break;

        case NODE_BLOCK:
            // Simula a entrada de escopo para que 'tab_buscarSimbolo'
            // encontre as variáveis locais criadas pelo parser.
            tab_entrarEscopo();
            for (NoAST *s = node->data.block.statements; s; s = s->next) {
                visitarNo(s);
            }
            tab_sairEscopo();
            break;

        case NODE_IF_STMT:
            visitarNo(node->data.if_stmt.condition);
            visitarNo(node->data.if_stmt.then_branch);
            if (node->data.if_stmt.else_branch) {
                visitarNo(node->data.if_stmt.else_branch);
            }
            break;
        
        case NODE_WHILE_STMT:
            visitarNo(node->data.while_stmt.condition);
            visitarNo(node->data.while_stmt.body);
            break;

        case NODE_FOR_STMT:
            // O For cria um escopo (para a variável de inicialização)
            tab_entrarEscopo();
            visitarNo(node->data.for_stmt.initializer);
            visitarNo(node->data.for_stmt.condition);
            visitarNo(node->data.for_stmt.increment);
            visitarNo(node->data.for_stmt.body);
            tab_sairEscopo();
            break;

        default:
            visitarNo(node);
            break;
    }
}

static void visitarExpressao(NoAST *node) {
    if (!node) return;

    switch (node->type) {
        case NODE_UNARY_OP:
            visitarNo(node->data.unary_op.operand);
            break;
        
        case NODE_BINARY_OP:
        case NODE_LOGICAL_OP:
            visitarNo(node->data.binary_op.left);
            visitarNo(node->data.binary_op.right);
            break;

        case NODE_ASSIGN:
            visitarNo(node->data.assign.value);
            // Aqui verificamos se a variável existe
            if (tab_buscarSimbolo(node->data.assign.name) == NULL) {
                fprintf(stderr, "Erro Semantico (linha %d): Variavel '%s' nao declarada.\n", 
                    node->lineno, node->data.assign.name);
                // Você pode optar por não sair (exit) e apenas contar erros
                // exit(1); 
            }
            break;

        case NODE_CALL:
            visitarNo(node->data.call.target);
            for (NoAST *a = node->data.call.arguments; a; a = a->next) {
                visitarNo(a);
            }
            break;
        
        case NODE_GET_ATTR:
            visitarNo(node->data.get_attr.object);
            break;
        
        case NODE_SET_ATTR:
            visitarNo(node->data.set_attr.object);
            visitarNo(node->data.set_attr.value);
            break;

        case NODE_IDENTIFIER:
            // Verificação de uso de variável
            if (tab_buscarSimbolo(node->data.identifier) == NULL) {
                fprintf(stderr, "Erro Semantico (linha %d): Variavel '%s' nao declarada.\n", 
                    node->lineno, node->data.identifier);
                // exit(1);
            }
            break;

        case NODE_NUM:
        case NODE_STRING:
        case NODE_BOOL:
        case NODE_NIL:
        case NODE_THIS:
            break;

        default:
            visitarNo(node);
            break;
    }
}

static void visitarNo(NoAST *node) {
    if (!node) return;

    switch (node->type) {
        // Statements
        case NODE_VAR_DECL:
        case NODE_FUN_DECL:
        case NODE_CLASS_DECL:
        case NODE_PRINT_STMT:
        case NODE_EXPR_STMT:
        case NODE_RETURN_STMT:
        case NODE_BLOCK:
        case NODE_IF_STMT:
        case NODE_WHILE_STMT:
        case NODE_FOR_STMT:
            visitarStatement(node);
            break;

        // Expressões
        case NODE_UNARY_OP:
        case NODE_BINARY_OP:
        case NODE_LOGICAL_OP:
        case NODE_ASSIGN:
        case NODE_CALL:
        case NODE_GET_ATTR:
        case NODE_SET_ATTR:
        case NODE_IDENTIFIER:
        case NODE_NUM:
        case NODE_STRING:
        case NODE_BOOL:
        case NODE_NIL:
        case NODE_THIS:
            visitarExpressao(node);
            break;
        
        default:
            fprintf(stderr, "No de AST desconhecido no resolver: %d\n", node->type);
            break;
    }
}

void resolver(NoAST *ast_root) {
    for (NoAST *node = ast_root; node; node = node->next) {
        visitarNo(node);
    }
}