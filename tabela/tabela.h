#ifndef TABELA_H
#define TABELA_H

#include <stdlib.h>

#define TAM_HASH 101

typedef struct simbolo {
    char nome[32];
    char tipo[16];
    int escopo;
    int numParams;
    struct simbolo *prox;
} Simbolo;

void tab_inicializar();
void tab_entrarEscopo();
void tab_sairEscopo();
void tab_inserirSimbolo(char *nome, char *tipo);
Simbolo *tab_buscarSimboloLocal(char *nome);
Simbolo *tab_buscarSimbolo(char *nome);
void tab_imprimirTabela();

#endif