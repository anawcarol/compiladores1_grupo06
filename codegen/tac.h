#ifndef TAC_H
#define TAC_H

#include <stdio.h>

typedef enum {
    TAC_ADD, TAC_SUB, TAC_MUL, TAC_DIV, 
    TAC_COPY,       
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

#endif