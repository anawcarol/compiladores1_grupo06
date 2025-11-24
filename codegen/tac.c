#include "tac.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int tempCount = 0;

char* gerarTemp() {
    char buffer[16];
    sprintf(buffer, "t%d", tempCount++);
    return strdup(buffer);
}

TacNode* criarTac(TacOp op, char *res, char *arg1, char *arg2) {
    TacNode *node = (TacNode*) malloc(sizeof(TacNode));
    node->op = op;
    node->res = res ? strdup(res) : NULL;
    node->arg1 = arg1 ? strdup(arg1) : NULL;
    node->arg2 = arg2 ? strdup(arg2) : NULL;
    node->next = NULL;
    node->prev = NULL;
    return node;
}

void anexarTac(TacNode **head, TacNode **tail, TacNode *novo) {
    if (!novo) return;
    if (*head == NULL) {
        *head = novo;
        *tail = novo;
    } else {
        (*tail)->next = novo;
        novo->prev = *tail;
        *tail = novo;
    }
}

const char* tacOpToString(TacOp op) {
    switch(op) {
        case TAC_ADD: return "+";
        case TAC_SUB: return "-";
        case TAC_MUL: return "*";
        case TAC_DIV: return "/";
        default: return "?";
    }
}

void imprimirTac(TacNode *head) {
    printf("\n--- TAC (Operacoes Basicas) ---\n");
    TacNode *atual = head;
    while (atual) {
        if (atual->op == TAC_COPY) {
            printf("%s = %s\n", atual->res, atual->arg1);
        } else {
            printf("%s = %s %s %s\n", atual->res, atual->arg1, tacOpToString(atual->op), atual->arg2);
        }
        atual = atual->next;
    }
    printf("-------------------------------\n");
}

void liberarTac(TacNode *head) {
    TacNode *atual = head;
    while (atual) {
        TacNode *prox = atual->next;
        free(atual->res); free(atual->arg1); free(atual->arg2);
        free(atual);
        atual = prox;
    }
}