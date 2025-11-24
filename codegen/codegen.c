#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.tab.h"

static TacNode *head = NULL;
static TacNode *tail = NULL;

// Variáveis globais para contadores
int tempCount = 0;
int labelCount = 0;

static void emit(TacOp op, char *res, char *arg1, char *arg2) {
    TacNode *novo = criarTac(op, res, arg1, arg2);
    anexarTac(&head, &tail, novo);
}

static char* visitarExpressao(NoAST *node) {
    if (!node) return NULL;

    switch (node->type) {
        case NODE_NUM: {
            char buffer[32];
            sprintf(buffer, "%.2f", node->data.number);
            return strdup(buffer);
        }
        case NODE_STRING:
            return strdup(node->data.string);
        case NODE_BOOL:
            return strdup(node->data.boolean ? "true" : "false");
        case NODE_IDENTIFIER:
            return strdup(node->data.identifier);

        case NODE_BINARY_OP: {
            char *t1 = visitarExpressao(node->data.binary_op.left);
            char *t2 = visitarExpressao(node->data.binary_op.right);
            char *tRes = gerarTemp();
            
            TacOp op = TAC_ADD;
            switch(node->data.binary_op.op) {
                case PLUS: op = TAC_ADD; break;
                case MINUS: op = TAC_SUB; break;
                case STAR: op = TAC_MUL; break;
                case SLASH: op = TAC_DIV; break;
                case GREATER: op = TAC_GT; break;
                case GREATER_EQUAL: op = TAC_GE; break;
                case LESS: op = TAC_LT; break;
                case LESS_EQUAL: op = TAC_LE; break;
                case EQUAL_EQUAL: op = TAC_EQ; break;
                case BANG_EQUAL: op = TAC_NEQ; break;
            }
            
            emit(op, tRes, t1, t2);
            free(t1); free(t2);
            return tRes;
        }

        case NODE_UNARY_OP: {
            char *t1 = visitarExpressao(node->data.unary_op.operand);
            char *tRes = gerarTemp();
            
            TacOp op;
            switch(node->data.unary_op.op) {
                case MINUS: op = TAC_NEG; break;
                case BANG: op = TAC_NOT; break;
                default: op = TAC_COPY; break;
            }
            
            emit(op, tRes, t1, NULL);
            free(t1);
            return tRes;
        }

        case NODE_LOGICAL_OP: {
            char *t1 = visitarExpressao(node->data.binary_op.left);
            char *t2 = visitarExpressao(node->data.binary_op.right);
            char *tRes = gerarTemp();
            
            TacOp op;
            if (node->data.binary_op.op == OR) {
                op = TAC_OR;
            } else {
                op = TAC_AND;
            }
            
            emit(op, tRes, t1, t2);
            free(t1); free(t2);
            return tRes;
        }

        case NODE_ASSIGN: {
            char *val = visitarExpressao(node->data.assign.value);
            emit(TAC_COPY, node->data.assign.name, val, NULL);
            free(val);
            return strdup(node->data.assign.name);
        }

        case NODE_CALL: {
            char *func = visitarExpressao(node->data.call.target);
            
            // Processar argumentos
            NoAST *arg = node->data.call.arguments;
            while (arg) {
                char *arg_val = visitarExpressao(arg);
                emit(TAC_PARAM, arg_val, NULL, NULL);
                free(arg_val);
                arg = arg->next;
            }
            
            // Para funções que retornam valor, criar temporário
            char *result = NULL;
            if (strcmp(func, "print") != 0) {
                result = gerarTemp();
            }
            
            emit(TAC_CALL, result, func, NULL);
            free(func);
            return result;
        }

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

        case NODE_PRINT_STMT: {
            char *res = visitarExpressao(node->data.print_stmt.expression);
            emit(TAC_PARAM, res, NULL, NULL);
            emit(TAC_CALL, NULL, "print", NULL);
            free(res);
            break;
        }

        case NODE_IF_STMT: {
            char *cond = visitarExpressao(node->data.if_stmt.condition);
            char *label_else = gerarLabel();
            char *label_end = gerarLabel();
            
            emit(TAC_JUMP_FALSE, label_else, cond, NULL);
            visitarStatement(node->data.if_stmt.then_branch);
            
            if (node->data.if_stmt.else_branch) {
                emit(TAC_JUMP, label_end, NULL, NULL);
                emit(TAC_LABEL, label_else, NULL, NULL);
                visitarStatement(node->data.if_stmt.else_branch);
                emit(TAC_LABEL, label_end, NULL, NULL);
            } else {
                emit(TAC_LABEL, label_else, NULL, NULL);
            }
            
            free(cond);
            break;
        }

        case NODE_WHILE_STMT: {
            char *label_start = gerarLabel();
            char *label_end = gerarLabel();
            
            emit(TAC_LABEL, label_start, NULL, NULL);
            char *cond = visitarExpressao(node->data.while_stmt.condition);
            emit(TAC_JUMP_FALSE, label_end, cond, NULL);
            visitarStatement(node->data.while_stmt.body);
            emit(TAC_JUMP, label_start, NULL, NULL);
            emit(TAC_LABEL, label_end, NULL, NULL);
            
            free(cond);
            break;
        }

        case NODE_FOR_STMT: {
            // Inicialização
            if (node->data.for_stmt.initializer)
                visitarStatement(node->data.for_stmt.initializer);
            
            char *label_start = gerarLabel();
            char *label_end = gerarLabel();
            
            emit(TAC_LABEL, label_start, NULL, NULL);
            
            // Condição
            if (node->data.for_stmt.condition) {
                char *cond = visitarExpressao(node->data.for_stmt.condition);
                emit(TAC_JUMP_FALSE, label_end, cond, NULL);
                free(cond);
            }
            
            // Corpo
            visitarStatement(node->data.for_stmt.body);
            
            // Incremento
            if (node->data.for_stmt.increment) {
                char *inc = visitarExpressao(node->data.for_stmt.increment);
                free(inc);
            }
            
            emit(TAC_JUMP, label_start, NULL, NULL);
            emit(TAC_LABEL, label_end, NULL, NULL);
            break;
        }

        case NODE_FUN_DECL: {
            char *label_func = gerarLabel();
            emit(TAC_LABEL, label_func, NULL, NULL);
            
            // Processar corpo da função
            visitarStatement(node->data.fun_decl.body);
            // Não emitir return automático para manter simples
            break;
        }

        case NODE_RETURN_STMT: {
            char *ret_val = NULL;
            if (node->data.return_stmt.expression) {
                ret_val = visitarExpressao(node->data.return_stmt.expression);
            }
            emit(TAC_RETURN, ret_val, NULL, NULL);
            if (ret_val) free(ret_val);
            break;
        }

        default:
            break;
    }
}

TacNode* gerarCodigo(NoAST *ast) {
    head = NULL; 
    tail = NULL;
    tempCount = 0;
    labelCount = 0;
    
    NoAST *atual = ast;
    while (atual) {
        visitarStatement(atual);
        atual = atual->next;
    }
    return head;
}