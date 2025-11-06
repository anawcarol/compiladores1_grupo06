/* * ARQUIVO: ast/ast.c
 * Conteúdo completo e modificado.
 */
#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Para incluir as definições dos tokens (ex: PLUS, MINUS)
// Este arquivo é gerado pelo Bison no diretório 'parser'
#include "../parser/parser.tab.h"

// Função auxiliar genérica para criar um nó
static NoAST* criarNoBase(NodeType type) {
    NoAST *node = (NoAST*) malloc(sizeof(NoAST));
    if (!node) {
        fprintf(stderr, "Erro de alocacao de memoria para a AST\n");
        exit(1);
    }
    node->type = type;
    node->next = NULL;
    extern int yylineno; // Pega o número da linha do lexer
    node->lineno = yylineno;
    return node;
}

// --- Implementação dos Construtores ---

NoAST *criarNoNum(double value) {
    NoAST *node = criarNoBase(NODE_NUM);
    node->data.number = value;
    return node;
}

NoAST *criarNoString(char* value) {
    NoAST *node = criarNoBase(NODE_STRING);
    node->data.string = value; // strdup feito pelo lexer
    return node;
}

NoAST *criarNoBool(int value) {
    NoAST *node = criarNoBase(NODE_BOOL);
    node->data.boolean = value;
    return node;
}

NoAST *criarNoNil(void) {
    return criarNoBase(NODE_NIL);
}

NoAST *criarNoIdentifier(char* name) {
    NoAST *node = criarNoBase(NODE_IDENTIFIER);
    node->data.identifier = name; // strdup feito pelo lexer
    return node;
}

NoAST *criarNoThis(void) {
    return criarNoBase(NODE_THIS);
}

NoAST *criarNoOpUnario(int op, NoAST *operand) {
    NoAST *node = criarNoBase(NODE_UNARY_OP);
    node->data.unary_op.op = op;
    node->data.unary_op.operand = operand;
    return node;
}

NoAST *criarNoOpBinario(int op, NoAST *left, NoAST *right) {
    NoAST *node = criarNoBase(NODE_BINARY_OP);
    node->data.binary_op.op = op;
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

NoAST *criarNoOpLogico(int op, NoAST *left, NoAST *right) {
    NoAST *node = criarNoBase(NODE_LOGICAL_OP);
    node->data.binary_op.op = op; // Reutiliza a struct binary_op
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

NoAST *criarNoAssign(char* name, NoAST *value) {
    NoAST *node = criarNoBase(NODE_ASSIGN);
    node->data.assign.name = name;
    node->data.assign.value = value;
    return node;
}

NoAST *criarNoCall(NoAST *target, NoAST *arguments) {
    NoAST *node = criarNoBase(NODE_CALL);
    node->data.call.target = target;
    node->data.call.arguments = arguments;
    return node;
}

NoAST *criarNoGetAttr(NoAST *object, char *name) {
    NoAST *node = criarNoBase(NODE_GET_ATTR);
    node->data.get_attr.object = object;
    node->data.get_attr.name = name; // strdup feito pelo parser
    return node;
}

NoAST *criarNoSetAttr(NoAST *object, char *name, NoAST *value) {
    NoAST *node = criarNoBase(NODE_SET_ATTR);
    node->data.set_attr.object = object;
    node->data.set_attr.name = name; // strdup feito pelo parser
    node->data.set_attr.value = value;
    return node;
}

NoAST *criarNoVarDecl(char *name, NoAST *initializer) {
    NoAST *node = criarNoBase(NODE_VAR_DECL);
    node->data.var_decl.name = name; // strdup feito pelo parser
    node->data.var_decl.initializer = initializer;
    return node;
}

NoAST *criarNoPrintStmt(NoAST *expression) {
    NoAST *node = criarNoBase(NODE_PRINT_STMT);
    node->data.print_stmt.expression = expression;
    return node;
}

NoAST *criarNoExprStmt(NoAST *expression) {
    NoAST *node = criarNoBase(NODE_EXPR_STMT);
    node->data.expr_stmt.expression = expression;
    return node;
}

NoAST *criarNoBlock(NoAST *statements) {
    NoAST *node = criarNoBase(NODE_BLOCK);
    node->data.block.statements = statements;
    return node;
}

NoAST *criarNoIf(NoAST *condition, NoAST *then_branch, NoAST *else_branch) {
    NoAST *node = criarNoBase(NODE_IF_STMT);
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_branch = then_branch;
    node->data.if_stmt.else_branch = else_branch;
    return node;
}

NoAST *criarNoWhile(NoAST *condition, NoAST *body) {
    NoAST *node = criarNoBase(NODE_WHILE_STMT);
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    return node;
}

NoAST *criarNoFor(NoAST *init, NoAST *cond, NoAST *inc, NoAST *body) {
    NoAST *node = criarNoBase(NODE_FOR_STMT);
    node->data.for_stmt.initializer = init;
    node->data.for_stmt.condition = cond;
    node->data.for_stmt.increment = inc;
    node->data.for_stmt.body = body;
    return node;
}

NoAST *criarNoFunDecl(char *name, NoAST *params, NoAST *body) {
    NoAST *node = criarNoBase(NODE_FUN_DECL);
    node->data.fun_decl.name = name; // strdup feito pelo parser
    node->data.fun_decl.params = params;
    node->data.fun_decl.body = body;
    return node;
}

NoAST *criarNoClassDecl(char *name, NoAST *methods) {
    NoAST *node = criarNoBase(NODE_CLASS_DECL);
    node->data.class_decl.name = name; // strdup feito pelo parser
    node->data.class_decl.methods = methods;
    return node;
}

NoAST *criarNoReturn(NoAST *expression) {
    NoAST *node = criarNoBase(NODE_RETURN_STMT);
    node->data.return_stmt.expression = expression;
    return node;
}

NoAST *criarNoParam(char *name) {
    NoAST *node = criarNoBase(NODE_PARAM);
    node->data.identifier = name; // strdup feito pelo parser
    return node;
}

// --- Funções Utilitárias ---

NoAST *anexarNoLista(NoAST *lista, NoAST *novo) {
    if (!lista) return novo;
    NoAST *atual = lista;
    while (atual->next) {
        atual = atual->next;
    }
    atual->next = novo;
    return lista;
}

static void imprimirIndent(int indent) {
    for (int i = 0; i < indent; i++) printf("  ");
}

static const char* opToString(int op) {
    switch (op) {
        case PLUS: return "+";
        case MINUS: return "-";
        case STAR: return "*";
        case SLASH: return "/";
        case BANG: return "!";
        case EQUAL_EQUAL: return "==";
        case BANG_EQUAL: return "!=";
        case LESS: return "<";
        case LESS_EQUAL: return "<=";
        case GREATER: return ">";
        case GREATER_EQUAL: return ">=";
        case OR: return "or";
        case AND: return "and";
        default: return "?";
    }
}

void imprimirAST(NoAST *node, int indent) {
    if (!node) {
        imprimirIndent(indent);
        printf("(NULL)\n");
        return;
    }

    imprimirIndent(indent);

    switch (node->type) {
        case NODE_NUM: printf("Num: %f\n", node->data.number); break;
        case NODE_STRING: printf("String: \"%s\"\n", node->data.string); break;
        case NODE_BOOL: printf("Bool: %s\n", node->data.boolean ? "true" : "false"); break;
        case NODE_NIL: printf("Nil\n"); break;
        case NODE_IDENTIFIER: printf("Ident: %s\n", node->data.identifier); break;
        case NODE_THIS: printf("This\n"); break;
        
        case NODE_UNARY_OP:
            printf("OpUnario: %s\n", opToString(node->data.unary_op.op));
            imprimirAST(node->data.unary_op.operand, indent + 1);
            break;
            
        case NODE_BINARY_OP:
        case NODE_LOGICAL_OP:
            printf("OpBinario: %s\n", opToString(node->data.binary_op.op));
            imprimirAST(node->data.binary_op.left, indent + 1);
            imprimirAST(node->data.binary_op.right, indent + 1);
            break;

        case NODE_ASSIGN:
            printf("Assign: %s\n", node->data.assign.name);
            imprimirAST(node->data.assign.value, indent + 1);
            break;

        case NODE_VAR_DECL:
            printf("VarDecl: %s\n", node->data.var_decl.name);
            if(node->data.var_decl.initializer)
                imprimirAST(node->data.var_decl.initializer, indent + 1);
            else {
                imprimirIndent(indent+1); printf("(No initializer)\n");
            }
            break;

        case NODE_PRINT_STMT:
            printf("PrintStmt\n");
            imprimirAST(node->data.print_stmt.expression, indent + 1);
            break;
            
        case NODE_EXPR_STMT:
            printf("ExprStmt\n");
            imprimirAST(node->data.expr_stmt.expression, indent + 1);
            break;

        case NODE_BLOCK:
            printf("Block\n");
            for (NoAST *s = node->data.block.statements; s; s = s->next) {
                imprimirAST(s, indent + 1);
            }
            break;
            
        case NODE_IF_STMT:
            printf("IfStmt\n");
            imprimirIndent(indent+1); printf("Cond:\n");
            imprimirAST(node->data.if_stmt.condition, indent + 2);
            imprimirIndent(indent+1); printf("Then:\n");
            imprimirAST(node->data.if_stmt.then_branch, indent + 2);
            if(node->data.if_stmt.else_branch) {
                imprimirIndent(indent+1); printf("Else:\n");
                imprimirAST(node->data.if_stmt.else_branch, indent + 2);
            }
            break;
            
        case NODE_WHILE_STMT:
            printf("WhileStmt\n");
            imprimirIndent(indent+1); printf("Cond:\n");
            imprimirAST(node->data.while_stmt.condition, indent + 2);
            imprimirIndent(indent+1); printf("Body:\n");
            imprimirAST(node->data.while_stmt.body, indent + 2);
            break;
            
        case NODE_FOR_STMT:
            printf("ForStmt\n");
            imprimirIndent(indent+1); printf("Init:\n");
            imprimirAST(node->data.for_stmt.initializer, indent + 2);
            imprimirIndent(indent+1); printf("Cond:\n");
            imprimirAST(node->data.for_stmt.condition, indent + 2);
            imprimirIndent(indent+1); printf("Inc:\n");
            imprimirAST(node->data.for_stmt.increment, indent + 2);
            imprimirIndent(indent+1); printf("Body:\n");
            imprimirAST(node->data.for_stmt.body, indent + 2);
            break;
            
        case NODE_FUN_DECL:
            printf("FunDecl: %s\n", node->data.fun_decl.name);
            imprimirIndent(indent+1); printf("Params:\n");
            for(NoAST *p = node->data.fun_decl.params; p; p = p->next) {
                imprimirAST(p, indent + 2);
            }
            imprimirIndent(indent+1); printf("Body:\n");
            imprimirAST(node->data.fun_decl.body, indent + 2);
            break;

        case NODE_PARAM:
            printf("Param: %s\n", node->data.identifier);
            break;

        case NODE_CLASS_DECL:
            printf("ClassDecl: %s\n", node->data.class_decl.name);
            imprimirIndent(indent+1); printf("Methods:\n");
            for(NoAST *m = node->data.class_decl.methods; m; m = m->next) {
                imprimirAST(m, indent + 2);
            }
            break;
            
        case NODE_RETURN_STMT:
            printf("ReturnStmt\n");
            imprimirAST(node->data.return_stmt.expression, indent + 1);
            break;
            
        case NODE_CALL:
            printf("Call\n");
            imprimirIndent(indent+1); printf("Target:\n");
            imprimirAST(node->data.call.target, indent + 2);
            imprimirIndent(indent+1); printf("Args:\n");
            for (NoAST *a = node->data.call.arguments; a; a = a->next) {
                imprimirAST(a, indent + 2);
            }
            break;
            
        case NODE_GET_ATTR:
            printf("GetAttr: %s\n", node->data.get_attr.name);
            imprimirIndent(indent+1); printf("Object:\n");
            imprimirAST(node->data.get_attr.object, indent + 2);
            break;
            
        case NODE_SET_ATTR:
            printf("SetAttr: %s\n", node->data.set_attr.name);
            imprimirIndent(indent+1); printf("Object:\n");
            imprimirAST(node->data.set_attr.object, indent + 2);
            imprimirIndent(indent+1); printf("Value:\n");
            imprimirAST(node->data.set_attr.value, indent + 2);
            break;

        default:
            printf("No desconhecido: %d\n", node->type);
            break;
    }
}

void liberarAST(NoAST *node) {
    if (!node) return;

    // Liberação pós-ordem
    switch (node->type) {
        case NODE_NUM:
        case NODE_BOOL:
        case NODE_NIL:
        case NODE_THIS:
            break; // Sem filhos ou strings
            
        case NODE_STRING: free(node->data.string); break;
        case NODE_IDENTIFIER: free(node->data.identifier); break;
        
        case NODE_UNARY_OP:
            liberarAST(node->data.unary_op.operand);
            break;
            
        case NODE_BINARY_OP:
        case NODE_LOGICAL_OP:
            liberarAST(node->data.binary_op.left);
            liberarAST(node->data.binary_op.right);
            break;

        case NODE_ASSIGN:
            free(node->data.assign.name);
            liberarAST(node->data.assign.value);
            break;

        case NODE_VAR_DECL:
            free(node->data.var_decl.name);
            liberarAST(node->data.var_decl.initializer);
            break;

        case NODE_PRINT_STMT:
            liberarAST(node->data.print_stmt.expression);
            break;
            
        case NODE_EXPR_STMT:
            liberarAST(node->data.expr_stmt.expression);
            break;

        case NODE_BLOCK:
            for (NoAST *s = node->data.block.statements; s;) {
                NoAST *proximo = s->next;
                liberarAST(s);
                s = proximo;
            }
            break;
            
        case NODE_IF_STMT:
            liberarAST(node->data.if_stmt.condition);
            liberarAST(node->data.if_stmt.then_branch);
            liberarAST(node->data.if_stmt.else_branch);
            break;
            
        case NODE_WHILE_STMT:
            liberarAST(node->data.while_stmt.condition);
            liberarAST(node->data.while_stmt.body);
            break;
            
        case NODE_FOR_STMT:
            liberarAST(node->data.for_stmt.initializer);
            liberarAST(node->data.for_stmt.condition);
            liberarAST(node->data.for_stmt.increment);
            liberarAST(node->data.for_stmt.body);
            break;
            
        case NODE_FUN_DECL:
            free(node->data.fun_decl.name);
            for (NoAST *p = node->data.fun_decl.params; p;) {
                NoAST *proximo = p->next;
                liberarAST(p);
                p = proximo;
            }
            liberarAST(node->data.fun_decl.body);
            break;

        case NODE_PARAM:
            free(node->data.identifier);
            break;
            
        case NODE_CLASS_DECL:
            free(node->data.class_decl.name);
            for (NoAST *m = node->data.class_decl.methods; m;) {
                NoAST *proximo = m->next;
                liberarAST(m);
                m = proximo;
            }
            break;
            
        case NODE_RETURN_STMT:
            liberarAST(node->data.return_stmt.expression);
            break;
            
        case NODE_CALL:
            liberarAST(node->data.call.target);
            for (NoAST *a = node->data.call.arguments; a;) {
                NoAST *proximo = a->next;
                liberarAST(a);
                a = proximo;
            }
            break;
            
        case NODE_GET_ATTR:
            liberarAST(node->data.get_attr.object);
            free(node->data.get_attr.name);
            break;
            
        case NODE_SET_ATTR:
            liberarAST(node->data.set_attr.object);
            free(node->data.set_attr.name);
            liberarAST(node->data.set_attr.value);
            break;
    }
    
    // Libera o próprio nó
    free(node);
}