#include "codegen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.tab.h"

// --- ESTRUTURAS DE ESCOPO (Para resolver Shadowing) ---
typedef struct VarMap {
    char *original;      // Nome no Lox (ex: "x")
    char *unique;        // Nome no C (ex: "x_1")
    struct VarMap *next;
} VarMap;

typedef struct Scope {
    VarMap *vars;
    struct Scope *next;
} Scope;

static Scope *currentScope = NULL;
static int varUniqueId = 0;

// Inicializa o escopo global
static void initScope() {
    currentScope = malloc(sizeof(Scope));
    currentScope->vars = NULL;
    currentScope->next = NULL;
}

static void pushScope() {
    Scope *newScope = malloc(sizeof(Scope));
    newScope->vars = NULL;
    newScope->next = currentScope;
    currentScope = newScope;
}

static void popScope() {
    if (currentScope) {
        Scope *old = currentScope;
        currentScope = currentScope->next;
        // Limpeza básica (em prod, liberar strings também)
        free(old);
    }
}

// Cria um nome único e registra no escopo atual
static char* declareVar(char* original) {
    char buffer[64];
    sprintf(buffer, "%s_%d", original, varUniqueId++);
    
    VarMap *vm = malloc(sizeof(VarMap));
    vm->original = strdup(original);
    vm->unique = strdup(buffer);
    vm->next = currentScope->vars;
    currentScope->vars = vm;
    
    return strdup(buffer);
}

// Busca o nome único mais recente (do escopo mais interno para fora)
static char* getVar(char* original) {
    Scope *s = currentScope;
    while (s) {
        VarMap *vm = s->vars;
        while (vm) {
            if (strcmp(vm->original, original) == 0) {
                return strdup(vm->unique);
            }
            vm = vm->next;
        }
        s = s->next;
    }
    // Se não achou (pode ser função global ou erro), retorna o original
    return strdup(original);
}

// --- FIM ESTRUTURAS DE ESCOPO ---

static TacNode *head = NULL;
static TacNode *tail = NULL;
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
            char buffer[32]; sprintf(buffer, "%.2f", node->data.number);
            return strdup(buffer);
        }
        case NODE_STRING: {
            char buffer[256]; sprintf(buffer, "\"%s\"", node->data.string);
            return strdup(buffer);
        }
        case NODE_BOOL: return strdup(node->data.boolean ? "true" : "false");
        
        // CORREÇÃO: Busca o nome único da variável no escopo
        case NODE_IDENTIFIER: return getVar(node->data.identifier);
        
        case NODE_THIS: return strdup("this");

        case NODE_GET_ATTR: {
            char *obj = visitarExpressao(node->data.get_attr.object);
            char *res = gerarTemp();
            char attrName[128]; sprintf(attrName, "\"%s\"", node->data.get_attr.name);
            emit(TAC_GET_ATTR, res, obj, attrName);
            free(obj); return res;
        }

        case NODE_SET_ATTR: {
            char *obj = visitarExpressao(node->data.set_attr.object);
            char *val = visitarExpressao(node->data.set_attr.value);
            char attrName[128]; sprintf(attrName, "\"%s\"", node->data.set_attr.name);
            emit(TAC_SET_ATTR, obj, attrName, val);
            char *ret = strdup(val); free(obj); free(val); return ret;
        }

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
            free(t1); free(t2); return tRes;
        }

        case NODE_UNARY_OP: {
            char *t1 = visitarExpressao(node->data.unary_op.operand);
            char *tRes = gerarTemp();
            TacOp op = (node->data.unary_op.op == MINUS) ? TAC_NEG : TAC_NOT;
            emit(op, tRes, t1, NULL);
            free(t1); return tRes;
        }

        case NODE_LOGICAL_OP: {
            char *t1 = visitarExpressao(node->data.binary_op.left);
            char *t2 = visitarExpressao(node->data.binary_op.right);
            char *tRes = gerarTemp();
            TacOp op = (node->data.binary_op.op == OR) ? TAC_OR : TAC_AND;
            emit(op, tRes, t1, t2);
            free(t1); free(t2); return tRes;
        }

        case NODE_ASSIGN: {
            char *val = visitarExpressao(node->data.assign.value);
            // CORREÇÃO: Busca o nome único para atribuição
            char *uniqueName = getVar(node->data.assign.name);
            emit(TAC_COPY, uniqueName, val, NULL);
            free(val); return uniqueName; 
        }

        case NODE_CALL: {
            char *func = visitarExpressao(node->data.call.target);
            NoAST *arg = node->data.call.arguments;
            while (arg) {
                char *av = visitarExpressao(arg);
                emit(TAC_PARAM, av, NULL, NULL);
                free(av); arg = arg->next;
            }
            char *res = NULL;
            if (strcmp(func, "print") != 0) res = gerarTemp();
            emit(TAC_CALL, res, func, NULL);
            free(func); return res;
        }
        default: return NULL;
    }
}

static void visitarStatement(NoAST *node) {
    if (!node) return;
    switch (node->type) {
        case NODE_BLOCK:
            pushScope(); // Novo escopo para o bloco
            for (NoAST *s = node->data.block.statements; s; s = s->next) 
                visitarStatement(s);
            popScope();
            break;

        case NODE_VAR_DECL:
            if (node->data.var_decl.initializer) {
                 char *v = visitarExpressao(node->data.var_decl.initializer);
                 // CORREÇÃO: Declara variável com nome único no escopo atual
                 char *uniqueName = declareVar(node->data.var_decl.name);
                 emit(TAC_COPY, uniqueName, v, NULL); 
                 free(v); // free(uniqueName) não deve ser feito pois é usado na struct
            } else {
                 // Variável sem init (nil)
                 char *uniqueName = declareVar(node->data.var_decl.name);
                 emit(TAC_COPY, uniqueName, "nil", NULL);
            }
            break;

        case NODE_PRINT_STMT: {
            char *r = visitarExpressao(node->data.print_stmt.expression);
            emit(TAC_CALL, NULL, "print", r); free(r);
            break;
        }
        case NODE_EXPR_STMT: { char* r = visitarExpressao(node->data.expr_stmt.expression); if(r) free(r); break; }
        
        case NODE_IF_STMT: {
            char *cond = visitarExpressao(node->data.if_stmt.condition);
            char *l_else = gerarLabel(); char *l_end = gerarLabel();
            emit(TAC_JUMP_FALSE, l_else, cond, NULL);
            visitarStatement(node->data.if_stmt.then_branch);
            emit(TAC_JUMP, l_end, NULL, NULL);
            emit(TAC_LABEL, l_else, NULL, NULL);
            if (node->data.if_stmt.else_branch) visitarStatement(node->data.if_stmt.else_branch);
            emit(TAC_LABEL, l_end, NULL, NULL);
            free(cond); break;
        }
        case NODE_WHILE_STMT: {
             char *l_start = gerarLabel(); char *l_end = gerarLabel();
             emit(TAC_LABEL, l_start, NULL, NULL);
             char *cond = visitarExpressao(node->data.while_stmt.condition);
             emit(TAC_JUMP_FALSE, l_end, cond, NULL);
             visitarStatement(node->data.while_stmt.body);
             emit(TAC_JUMP, l_start, NULL, NULL);
             emit(TAC_LABEL, l_end, NULL, NULL);
             free(cond); break;
        }
        case NODE_FOR_STMT: {
            pushScope(); // For cria escopo (para o inicializador)
            if (node->data.for_stmt.initializer) visitarStatement(node->data.for_stmt.initializer);
            char *label_start = gerarLabel(); char *label_end = gerarLabel();
            emit(TAC_LABEL, label_start, NULL, NULL);
            if (node->data.for_stmt.condition) {
                char *cond = visitarExpressao(node->data.for_stmt.condition);
                emit(TAC_JUMP_FALSE, label_end, cond, NULL);
                free(cond);
            }
            visitarStatement(node->data.for_stmt.body);
            if (node->data.for_stmt.increment) {
                char *inc = visitarExpressao(node->data.for_stmt.increment); free(inc);
            }
            emit(TAC_JUMP, label_start, NULL, NULL);
            emit(TAC_LABEL, label_end, NULL, NULL);
            popScope();
            break;
        }
        
        case NODE_RETURN_STMT: {
            char *rv = node->data.return_stmt.expression ? visitarExpressao(node->data.return_stmt.expression) : NULL;
            emit(TAC_RETURN, rv, NULL, NULL); if(rv) free(rv); break;
        }
        
        case NODE_FUN_DECL: break; // Tratado separadamente
        case NODE_CLASS_DECL: break; // Tratado separadamente
        default: break;
    }
}

static void gerarFuncao(NoAST *funNode) {
    emit(TAC_LABEL, funNode->data.fun_decl.name, NULL, NULL);
    pushScope(); // Novo escopo para a função
    for(NoAST* p=funNode->data.fun_decl.params; p; p=p->next) {
        // Declara parâmetros como variáveis locais com nome único
        char *uniqueParam = declareVar(p->data.identifier);
        emit(TAC_COPY, uniqueParam, "POP_PARAM", NULL);
    }
    visitarStatement(funNode->data.fun_decl.body);
    emit(TAC_RETURN, NULL, NULL, NULL);
    popScope();
}

static void encontrarEGerarFuncoes(NoAST *node) {
    if (!node) return;
    
    if (node->type == NODE_FUN_DECL) {
        gerarFuncao(node);
        encontrarEGerarFuncoes(node->data.fun_decl.body);
    } 
    else if (node->type == NODE_CLASS_DECL) {
        for(NoAST* m=node->data.class_decl.methods; m; m=m->next) {
            char lbl[128]; sprintf(lbl, "%s_%s", node->data.class_decl.name, m->data.fun_decl.name);
            emit(TAC_LABEL, lbl, NULL, NULL);
            pushScope(); // Escopo do método
            emit(TAC_COPY, "this", "POP_PARAM", NULL);
            for(NoAST* p=m->data.fun_decl.params; p; p=p->next) {
                char *u = declareVar(p->data.identifier);
                emit(TAC_COPY, u, "POP_PARAM", NULL);
            }
            visitarStatement(m->data.fun_decl.body);
            emit(TAC_RETURN, NULL, NULL, NULL);
            popScope();
            encontrarEGerarFuncoes(m->data.fun_decl.body);
        }
    }
    else if (node->type == NODE_BLOCK) {
        for (NoAST *s = node->data.block.statements; s; s = s->next) encontrarEGerarFuncoes(s);
    }
    else if (node->type == NODE_IF_STMT) {
        encontrarEGerarFuncoes(node->data.if_stmt.then_branch);
        encontrarEGerarFuncoes(node->data.if_stmt.else_branch);
    }
    else if (node->type == NODE_WHILE_STMT) {
        encontrarEGerarFuncoes(node->data.while_stmt.body);
    }
    else if (node->type == NODE_FOR_STMT) {
        encontrarEGerarFuncoes(node->data.for_stmt.body);
    }
}

TacNode* gerarCodigo(NoAST *ast) {
    head = NULL; tail = NULL; tempCount = 0; labelCount = 0;
    
    initScope(); // Inicializa escopo global
    
    // 1. Gera Funções (Hoisting)
    NoAST *curr = ast;
    while (curr) {
        encontrarEGerarFuncoes(curr);
        curr = curr->next;
    }

    emit(TAC_LABEL, "main", NULL, NULL);

    // 2. Gera Main
    curr = ast;
    while (curr) {
        // Registra globais no escopo principal, mas com nomes únicos também
        if (curr->type == NODE_VAR_DECL) {
            // Globais também precisam ser registradas para serem achadas por blocos dentro da main
            // Mas cuidado: funções externas não enxergam variáveis locais da Main em C
        }
        
        if (curr->type != NODE_CLASS_DECL && curr->type != NODE_FUN_DECL) {
            visitarStatement(curr);
        }
        curr = curr->next;
    }
    return head;
}