#ifndef TAC_H
#define TAC_H

#include <stdio.h>

typedef enum {
    TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV, 
    TAC_COPY,
    // Operações lógicas e comparativas
    TAC_EQ, TAC_NEQ, TAC_GT, TAC_GE, TAC_LT, TAC_LE,
    TAC_AND, TAC_OR, TAC_NOT,
    TAC_NEG,
    // Controle de fluxo
    TAC_LABEL,
    TAC_JUMP,
    TAC_JUMP_TRUE,
    TAC_JUMP_FALSE,
    // Funções
    TAC_CALL,
    TAC_RETURN,
    TAC_PARAM,
    // OPCODES PARA OO
    TAC_GET_ATTR,
    TAC_SET_ATTR
} TacOp;

typedef struct TacNode {
    TacOp op;
    char *res;   
    char *arg1;  
    char *arg2;  
    struct TacNode *next;
    struct TacNode *prev;
} TacNode;

// Funções
TacNode* criarTac(TacOp op, char *res, char *arg1, char *arg2);
void anexarTac(TacNode **head, TacNode **tail, TacNode *novo);
void imprimirTac(TacNode *head);
void liberarTac(TacNode *head);

// Gerador de temporários (t0, t1...)
char* gerarTemp();
char* gerarLabel();

#endif