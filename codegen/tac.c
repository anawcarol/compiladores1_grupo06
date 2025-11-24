#include "tac.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static int tempCount = 0;
static int labelCount = 0;

char* gerarTemp() {
    char buffer[16];
    sprintf(buffer, "t%d", tempCount++);
    return strdup(buffer);
}

char* gerarLabel() {
    char buffer[16];
    sprintf(buffer, "L%d", labelCount++);
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
        case TAC_COPY: return "=";
        case TAC_EQ: return "==";
        case TAC_NEQ: return "!=";
        case TAC_GT: return ">";
        case TAC_GE: return ">=";
        case TAC_LT: return "<";
        case TAC_LE: return "<=";
        case TAC_AND: return "&&";
        case TAC_OR: return "||";
        case TAC_NOT: return "!";
        case TAC_NEG: return "neg";
        case TAC_LABEL: return "label";
        case TAC_JUMP: return "jump";
        case TAC_JUMP_TRUE: return "jump_true";
        case TAC_JUMP_FALSE: return "jump_false";
        case TAC_CALL: return "call";
        case TAC_RETURN: return "return";
        case TAC_PARAM: return "param";
        // --- NOVOS OPCODES PARA OO ---
        case TAC_GET_ATTR: return ".";
        case TAC_SET_ATTR: return "=";
        default: return "?";
    }
}

void imprimirTac(TacNode *head) {
    printf("\n--- TAC (Three-Address Code) ---\n");
    TacNode *atual = head;
    while (atual) {
        switch (atual->op) {
            case TAC_COPY:
                printf("%s = %s\n", atual->res, atual->arg1);
                break;
            case TAC_LABEL:
                printf("%s:\n", atual->res);
                break;
            case TAC_JUMP:
                printf("jump %s\n", atual->res);
                break;
            case TAC_JUMP_TRUE:
                printf("if %s jump %s\n", atual->arg1, atual->res);
                break;
            case TAC_JUMP_FALSE:
                printf("ifnot %s jump %s\n", atual->arg1, atual->res);
                break;
            case TAC_CALL:
                if (atual->res) {
                    printf("%s = call %s\n", atual->res, atual->arg1);
                } else {
                    printf("call %s\n", atual->arg1);
                }
                break;
            case TAC_RETURN:
                if (atual->res)
                    printf("return %s\n", atual->res);
                else
                    printf("return\n");
                break;
            case TAC_PARAM:
                printf("param %s\n", atual->res);
                break;
            case TAC_GET_ATTR:
                printf("%s = %s.%s\n", atual->res, atual->arg1, atual->arg2);
                break;
            case TAC_SET_ATTR:
                printf("%s.%s = %s\n", atual->res, atual->arg1, atual->arg2);
                break;
            case TAC_NOT:
            case TAC_NEG:
                printf("%s = %s %s\n", atual->res, tacOpToString(atual->op), atual->arg1);
                break;
            default:
                if (atual->arg2)
                    printf("%s = %s %s %s\n", atual->res, atual->arg1, tacOpToString(atual->op), atual->arg2);
                else if (atual->arg1)
                    printf("%s = %s %s\n", atual->res, tacOpToString(atual->op), atual->arg1);
                else if (atual->res)
                    printf("%s\n", atual->res);
                break;
        }
        atual = atual->next;
    }
    printf("---------------------------------\n");
}

void liberarTac(TacNode *head) {
    TacNode *atual = head;
    while (atual) {
        TacNode *prox = atual->next;
        free(atual->res);
        free(atual->arg1);
        free(atual->arg2);
        free(atual);
        atual = prox;
    }
}