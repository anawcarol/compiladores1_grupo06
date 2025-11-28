/* * ARQUIVO: tabela/tipos.h
 * Conteúdo completo e modificado.
 */
#ifndef TIPOS_H
#define TIPOS_H

#include <stdlib.h> // Para size_t
// Enum para todos os tipos de nós possíveis na AST
typedef enum {
    // Literais
    NODE_NUM,
    NODE_STRING,
    NODE_BOOL,
    NODE_NIL,
    NODE_IDENTIFIER,
    NODE_THIS,

    // Expressões
    NODE_UNARY_OP,
    NODE_BINARY_OP,
    NODE_LOGICAL_OP,
    NODE_ASSIGN,
    NODE_CALL,
    NODE_GET_ATTR,
    NODE_SET_ATTR,

    // Declarações e Statements
    NODE_VAR_DECL,
    NODE_PRINT_STMT,
    NODE_EXPR_STMT,
    NODE_BLOCK,
    NODE_IF_STMT,
    NODE_WHILE_STMT,
    NODE_FOR_STMT,
    NODE_FUN_DECL,
    NODE_CLASS_DECL,
    NODE_RETURN_STMT,
    NODE_PARAM, // Para lista de parâmetros de função
    
} NodeType;

// A nova estrutura NoAST
typedef struct NoAST {
    NodeType type;
    int lineno; // Útil para mensagens de erro futuras

    // 'next' é usado para criar listas encadeadas (statements, argumentos, etc.)
    struct NoAST *next; 

    union {
        double number;
        char* string;
        int boolean;
        char* identifier;

        struct {
            int op; // O token do operador (ex: '!', '-')
            struct NoAST *operand;
        } unary_op;

        struct {
            int op; // O token do operador (ex: '+', '>', '==')
            struct NoAST *left;
            struct NoAST *right;
        } binary_op; // Usado para binary_op e logical_op

        struct {
            char *name;
            struct NoAST *initializer; // Pode ser NULL
        } var_decl;

        struct {
            struct NoAST *expression;
        } print_stmt;
        
        struct {
            struct NoAST *expression;
        } expr_stmt;

        struct {
            struct NoAST *statements; // Cabeça da lista de statements
        } block;

        struct {
            struct NoAST *condition;
            struct NoAST *then_branch;
            struct NoAST *else_branch; // Pode ser NULL
        } if_stmt;

        struct {
            struct NoAST *condition;
            struct NoAST *body;
        } while_stmt;
        
        struct {
            struct NoAST *initializer;
            struct NoAST *condition;
            struct NoAST *increment;
            struct NoAST *body;
        } for_stmt;

        struct {
            char *name;
            struct NoAST *params; // Cabeça da lista de NODE_PARAM
            struct NoAST *body;
        } fun_decl; // Usado para funções e métodos

        struct {
            char *name;
            struct NoAST *methods; // Cabeça da lista de NODE_FUN_DECL (métodos)
        } class_decl;
        
        struct {
            struct NoAST *expression; // Pode ser NULL
        } return_stmt;

        struct {
            struct NoAST *target; // A quem a chamada é feita (ex: um identifier)
            struct NoAST *arguments; // Cabeça da lista de argumentos
        } call;

        struct {
            struct NoAST *object;
            char *name;
        } get_attr; // Para obj.propriedade

        struct {
            struct NoAST *object;
            char *name;
            struct NoAST *value;
        } set_attr; // Para obj.propriedade = valor

        struct {
            char *name;
            struct NoAST *value;
        } assign;

        // NODE_PARAM usa o campo 'identifier' da union

    } data;
} NoAST;

#endif