#include "semantica.h"
#include "../tabela/tabela.h"
#include "../ast/ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../parser/parser.tab.h"

// --- Definições de Contexto ---
// Usadas para rastrear se estamos dentro de uma função ou classe
typedef enum {
    CTX_FUNC_NONE,
    CTX_FUNC_FUNCTION,
    CTX_FUNC_METHOD,
    CTX_FUNC_INITIALIZER
} FunctionType;

typedef enum {
    CTX_CLASS_NONE,
    CTX_CLASS_CLASS
} ClassType;

// --- Protótipos ---
static void visitarNo(NoAST *node, FunctionType funcType, ClassType classType);

// --- Verificações Auxiliares ---

// Conta quantos nós existem numa lista ligada (para params ou args)
static int contarNos(NoAST *lista) {
    int count = 0;
    while (lista) {
        count++;
        lista = lista->next;
    }
    return count;
}

// Verifica se os tipos de uma operação binária são compatíveis (ex: Num + Num)
// Emite AVISOS (warnings) em vez de erros fatais, pois Lox é dinâmica.
static void verificarTiposBinarios(NoAST *node) {
    if (!node) return;

    NoAST *esq = node->data.binary_op.left;
    NoAST *dir = node->data.binary_op.right;
    int op = node->data.binary_op.op;

    // Obtém strings representando os tipos
    char *tipoEsq = obterNomeTipo(esq);
    char *tipoDir = obterNomeTipo(dir);

    // [CORREÇÃO] Ignora verificação se o tipo for "dynamic" OU "param"
    // Parâmetros em Lox podem ser qualquer coisa, então não podemos assumir erro estático.
    if (strcmp(tipoEsq, "dynamic") == 0 || strcmp(tipoDir, "dynamic") == 0 ||
        strcmp(tipoEsq, "param") == 0   || strcmp(tipoDir, "param") == 0) {
        return;
    }

    // Regra para Soma (+): Aceita Num+Num ou String+String
    if (op == PLUS) {
        int esqNum = (strcmp(tipoEsq, "int") == 0 || strcmp(tipoEsq, "float") == 0);
        int dirNum = (strcmp(tipoDir, "int") == 0 || strcmp(tipoDir, "float") == 0);
        int esqStr = (strcmp(tipoEsq, "string") == 0);
        int dirStr = (strcmp(tipoDir, "string") == 0);

        if (!((esqNum && dirNum) || (esqStr && dirStr))) {
            printf("[AVISO Semantico] Linha %d: Operacao '+' entre tipos possivelmente incompativeis: '%s' e '%s'.\n", 
                node->lineno, tipoEsq, tipoDir);
        }
    }
    // Outras operações aritméticas esperam números
    else if (op == MINUS || op == STAR || op == SLASH || 
             op == GREATER || op == GREATER_EQUAL || 
             op == LESS || op == LESS_EQUAL) {
        
        int esqNum = (strcmp(tipoEsq, "int") == 0 || strcmp(tipoEsq, "float") == 0);
        int dirNum = (strcmp(tipoDir, "int") == 0 || strcmp(tipoDir, "float") == 0);

        if (!esqNum || !dirNum) {
            printf("[AVISO Semantico] Linha %d: Operador aritmetico espera numeros, mas encontrou '%s' e '%s'.\n", 
                node->lineno, tipoEsq, tipoDir);
        }
    }
}

static void visitarStatement(NoAST *node, FunctionType funcType, ClassType classType) {
    if (!node) return;

    switch (node->type) {
        case NODE_VAR_DECL:
            // Apenas verificamos a inicialização se houver.
            if (node->data.var_decl.initializer) {
                visitarNo(node->data.var_decl.initializer, funcType, classType);
            }
            
            // Insere a variável na tabela se não existir no escopo local.
            // Necessário porque o 'parser' pode já ter limpado o escopo, 
            // então reinserimos durante a análise semântica.
            if (tab_buscarSimboloLocal(node->data.var_decl.name) == NULL) {
                char *tipo = "var";
                if (node->data.var_decl.initializer) {
                    tipo = obterNomeTipo(node->data.var_decl.initializer);
                }
                tab_inserirSimbolo(node->data.var_decl.name, tipo);
            }
            break;

        case NODE_FUN_DECL: {
            // Garante que a função está na tabela e atualiza Aridade
            if (tab_buscarSimboloLocal(node->data.fun_decl.name) == NULL) {
                tab_inserirSimbolo(node->data.fun_decl.name, "fun");
            }
            Simbolo *s = tab_buscarSimbolo(node->data.fun_decl.name);
            if (s) {
                s->numParams = contarNos(node->data.fun_decl.params);
            }

            // Entra no escopo da função
            tab_entrarEscopo(); 
            
            // Insere os parâmetros na tabela de símbolos
            // Isso corrige os erros de "Variavel 'a' nao declarada" dentro da função.
            for (NoAST *p = node->data.fun_decl.params; p; p = p->next) {
                tab_inserirSimbolo(p->data.identifier, "param");
            }

            // Determina o novo contexto de função
            FunctionType novoFuncType = CTX_FUNC_FUNCTION;
            if (classType != CTX_CLASS_NONE) {
                if (strcmp(node->data.fun_decl.name, "init") == 0)
                    novoFuncType = CTX_FUNC_INITIALIZER;
                else
                    novoFuncType = CTX_FUNC_METHOD;
            }

            visitarNo(node->data.fun_decl.body, novoFuncType, classType);
            tab_sairEscopo();
            break;
        }

        case NODE_CLASS_DECL: {
            // Insere classe na tabela se necessário
            if (tab_buscarSimboloLocal(node->data.class_decl.name) == NULL) {
                tab_inserirSimbolo(node->data.class_decl.name, "class");
            }

            // Atualiza Aridade da Classe baseada no método 'init'
            Simbolo *sClass = tab_buscarSimbolo(node->data.class_decl.name);
            if (sClass) {
                sClass->numParams = 0; // Default: construtor sem argumentos
                for (NoAST *m = node->data.class_decl.methods; m; m = m->next) {
                    if (strcmp(m->data.fun_decl.name, "init") == 0) {
                        sClass->numParams = contarNos(m->data.fun_decl.params);
                        break;
                    }
                }
            }

            // Entra no escopo da classe
            tab_entrarEscopo();
            
            // [NOVO] Inserir 'this' no escopo
             // tab_inserirSimbolo("this", "instance");

            // Percorre os métodos passando o contexto de CLASSE
            for (NoAST *m = node->data.class_decl.methods; m; m = m->next) {
                visitarNo(m, CTX_FUNC_METHOD, CTX_CLASS_CLASS);
            }
            tab_sairEscopo();
            break;
        }

        case NODE_PRINT_STMT:
            visitarNo(node->data.print_stmt.expression, funcType, classType);
            break;
            
        case NODE_EXPR_STMT:
            visitarNo(node->data.expr_stmt.expression, funcType, classType);
            break;
            
        case NODE_RETURN_STMT:
            // VALIDAÇÃO: Return fora de função
            if (funcType == CTX_FUNC_NONE) {
                fprintf(stderr, "Erro Semantico (linha %d): 'return' nao pode ser usado fora de uma funcao.\n", node->lineno);
            }
            
            // VALIDAÇÃO: Return com valor dentro de init()
            if (funcType == CTX_FUNC_INITIALIZER && node->data.return_stmt.expression != NULL) {
                fprintf(stderr, "Erro Semantico (linha %d): Nao pode retornar um valor de um inicializador 'init'.\n", node->lineno);
            }

            if (node->data.return_stmt.expression) {
                visitarNo(node->data.return_stmt.expression, funcType, classType);
            }
            break;

        case NODE_BLOCK:
            // Simula a entrada de escopo
            tab_entrarEscopo();
            for (NoAST *s = node->data.block.statements; s; s = s->next) {
                visitarNo(s, funcType, classType);
            }
            tab_sairEscopo();
            break;

        case NODE_IF_STMT:
            visitarNo(node->data.if_stmt.condition, funcType, classType);
            visitarNo(node->data.if_stmt.then_branch, funcType, classType);
            if (node->data.if_stmt.else_branch) {
                visitarNo(node->data.if_stmt.else_branch, funcType, classType);
            }
            break;
        
        case NODE_WHILE_STMT:
            visitarNo(node->data.while_stmt.condition, funcType, classType);
            visitarNo(node->data.while_stmt.body, funcType, classType);
            break;

        case NODE_FOR_STMT:
            // O For cria um escopo (para a variável de inicialização)
            tab_entrarEscopo();
            visitarNo(node->data.for_stmt.initializer, funcType, classType);
            visitarNo(node->data.for_stmt.condition, funcType, classType);
            visitarNo(node->data.for_stmt.increment, funcType, classType);
            visitarNo(node->data.for_stmt.body, funcType, classType);
            tab_sairEscopo();
            break;

        default:
            visitarNo(node, funcType, classType);
            break;
    }
}

static void visitarExpressao(NoAST *node, FunctionType funcType, ClassType classType) {
    if (!node) return;

    switch (node->type) {
        case NODE_UNARY_OP:
            visitarNo(node->data.unary_op.operand, funcType, classType);
            break;
        
        case NODE_BINARY_OP:
        case NODE_LOGICAL_OP:
            visitarNo(node->data.binary_op.left, funcType, classType);
            visitarNo(node->data.binary_op.right, funcType, classType);
            
            // VALIDAÇÃO: Tipos compatíveis
            verificarTiposBinarios(node);
            break;

        case NODE_ASSIGN:
            visitarNo(node->data.assign.value, funcType, classType);
            // Aqui verificamos se a variável existe
            if (tab_buscarSimbolo(node->data.assign.name) == NULL) {
                fprintf(stderr, "Erro Semantico (linha %d): Variavel '%s' nao declarada.\n", 
                    node->lineno, node->data.assign.name);
            }
            break;

        case NODE_CALL:
            visitarNo(node->data.call.target, funcType, classType);
            for (NoAST *a = node->data.call.arguments; a; a = a->next) {
                visitarNo(a, funcType, classType);
            }

            // VERIFICAÇÃO DE ARIDADE (Chamadas de Identificadores)
            if (node->data.call.target->type == NODE_IDENTIFIER) {
                char *nomeFunc = node->data.call.target->data.identifier;
                Simbolo *s = tab_buscarSimbolo(nomeFunc);
                
                if (s) {
                    // Verifica se é chamável (fun ou class)
                    int isCallable = (strcmp(s->tipo, "fun") == 0 || strcmp(s->tipo, "class") == 0);
                    
                    if (!isCallable) {
                         // Pode adicionar aviso se chamar variável comum
                    } else if (s->numParams != -1) { // Se numParams for -1, ignoramos
                        int numArgs = contarNos(node->data.call.arguments);
                        if (numArgs != s->numParams) {
                            fprintf(stderr, "Erro Semantico (linha %d): Funcao '%s' esperava %d argumentos, mas recebeu %d.\n", 
                                node->lineno, nomeFunc, s->numParams, numArgs);
                        }
                    }
                }
            }
            break;
        
        case NODE_GET_ATTR:
            visitarNo(node->data.get_attr.object, funcType, classType);
            break;
        
        case NODE_SET_ATTR:
            visitarNo(node->data.set_attr.object, funcType, classType);
            visitarNo(node->data.set_attr.value, funcType, classType);
            break;

        case NODE_IDENTIFIER:
            // Verificação de uso de variável
            if (tab_buscarSimbolo(node->data.identifier) == NULL) {
                fprintf(stderr, "Erro Semantico (linha %d): Variavel '%s' nao declarada.\n", 
                    node->lineno, node->data.identifier);
            }
            break;

        case NODE_THIS:
            // VALIDAÇÃO: 'this' fora de classe
            if (classType == CTX_CLASS_NONE) {
                fprintf(stderr, "Erro Semantico (linha %d): 'this' nao pode ser usado fora de uma classe.\n", node->lineno);
            }
            break;

        case NODE_NUM:
        case NODE_STRING:
        case NODE_BOOL:
        case NODE_NIL:
            break;

        default:
            visitarNo(node, funcType, classType);
            break;
    }
}

static void visitarNo(NoAST *node, FunctionType funcType, ClassType classType) {
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
            visitarStatement(node, funcType, classType);
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
            visitarExpressao(node, funcType, classType);
            break;
        
        case NODE_PARAM:
            // Parâmetros são tratados na declaração da função/método
            break;

        default:
            fprintf(stderr, "No de AST desconhecido no resolver: %d\n", node->type);
            break;
    }
}

void resolver(NoAST *ast_root) {
    // Inicia a análise com contexto vazio (sem função, sem classe)
    for (NoAST *node = ast_root; node; node = node->next) {
        visitarNo(node, CTX_FUNC_NONE, CTX_CLASS_NONE);
    }
}