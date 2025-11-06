#ifndef TABELA_H
#define TABELA_H

typedef struct simbolo {
    char nome[32];
    char tipo[16];
    struct simbolo *prox;
} Simbolo;


typedef struct ambiente {
    Simbolo *head;
    struct ambiente *pai;
} Ambiente;

void tab_inicializar();

void tab_entrarEscopo();

void tab_sairEscopo();

void tab_inserirSimbolo(char *nome, char *tipo);

Simbolo *tab_buscarSimboloLocal(char *nome);

Simbolo *tab_buscarSimbolo(char *nome);

void tab_imprimirTabela();

#endif