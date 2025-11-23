#include "tabela.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Simbolo *tabela_hash[TAM_HASH];
int escopoAtual = 0;

unsigned int hash(char *nome) {
    unsigned int hash_val = 0;
    while (*nome) {
        hash_val = (hash_val << 5) + *nome++;
    }
    return hash_val % TAM_HASH;
}

void tab_inicializar() {
    escopoAtual = 0;
    for (int i = 0; i < TAM_HASH; i++) {
        tabela_hash[i] = NULL;
    }
}

void tab_entrarEscopo() {
    escopoAtual++;
}

void tab_sairEscopo() {
    if (escopoAtual < 0) return;

    // Debug

    // printf("\n>>> Saindo do Escopo %d. Estado da Tabela antes da limpeza:\n", escopoAtual);
    // tab_imprimirTabela(); 
    // printf(">>> Fim do estado do Escopo %d\n\n", escopoAtual);

    for (int i = 0; i < TAM_HASH; i++) {
        Simbolo *atual = tabela_hash[i];
        Simbolo *anterior = NULL;

        while (atual != NULL) {
            if (atual->escopo == escopoAtual) {
                Simbolo *temp = atual;
                if (anterior == NULL) {
                    tabela_hash[i] = atual->prox;
                } else {
                    anterior->prox = atual->prox;
                }
                atual = atual->prox;
                free(temp);
            } else {
                anterior = atual;
                atual = atual->prox;
            }
        }
    }
    // if (escopoAtual <= 0) return;
    escopoAtual--;
}

Simbolo *tab_buscarSimboloNoEscopo(char *nome, int escopo) {
    unsigned int index = hash(nome);
    for (Simbolo *s = tabela_hash[index]; s != NULL; s = s->prox) {
        if (strcmp(s->nome, nome) == 0 && s->escopo == escopo) {
            return s;
        }
    }
    return NULL;
}

Simbolo *tab_buscarSimboloLocal(char *nome) {
    return tab_buscarSimboloNoEscopo(nome, escopoAtual);
}

Simbolo *tab_buscarSimbolo(char *nome) {
    for (int e = escopoAtual; e >= 0; e--) {
        Simbolo *s = tab_buscarSimboloNoEscopo(nome, e);
        if (s != NULL) {
            return s;
        }
    }
    return NULL;
}

void tab_inserirSimbolo(char *nome, char *tipo) {
    if (tab_buscarSimboloLocal(nome) != NULL) {
        fprintf(stderr, "Erro Semântico: Símbolo '%s' já declarado neste escopo.\n", nome);
        return;
    }

    Simbolo *novo = (Simbolo *)malloc(sizeof(Simbolo));
    if (novo == NULL) {
        fprintf(stderr, "Erro de alocação de memória para novo símbolo.\n");
        exit(EXIT_FAILURE);
    }

    strncpy(novo->nome, nome, sizeof(novo->nome) - 1);
    novo->nome[sizeof(novo->nome) - 1] = '\0';

    strncpy(novo->tipo, tipo, sizeof(novo->tipo) - 1);
    novo->tipo[sizeof(novo->tipo) - 1] = '\0';
    
    novo->escopo = escopoAtual;

    unsigned int index = hash(nome);
    novo->prox = tabela_hash[index];
    tabela_hash[index] = novo;
}

void tab_imprimirTabela() {
    printf("\n--- Tabela de Símbolos (Visão Hash) ---\n");
    for (int i = 0; i < TAM_HASH; i++) {
        if (tabela_hash[i] != NULL) {
            printf("Bucket %d:\n", i);
            for (Simbolo *s = tabela_hash[i]; s != NULL; s = s->prox) {
                printf("  -> Nome: %s, Tipo: %s, Escopo: %d\n", s->nome, s->tipo, s->escopo);
            }
        }
    }
    printf("---------------------------------------\n");
}