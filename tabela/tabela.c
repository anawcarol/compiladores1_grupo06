#include "tabela.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Ambiente *ambienteAtual = NULL;

void tab_inicializar() {
    ambienteAtual = (Ambiente *)malloc(sizeof(Ambiente));
    if (ambienteAtual == NULL) {
        fprintf(stderr, "Erro de alocação de memória para o escopo global.\n");
        exit(EXIT_FAILURE);
    }
    ambienteAtual->head = NULL;
    ambienteAtual->pai = NULL;
}

void tab_entrarEscopo() {
    Ambiente *novoAmbiente = (Ambiente *)malloc(sizeof(Ambiente));
    if (novoAmbiente == NULL) {
        fprintf(stderr, "Erro de alocação de memória para novo escopo.\n");
        exit(EXIT_FAILURE);
    }
    
    novoAmbiente->head = NULL;
    novoAmbiente->pai = ambienteAtual;
    ambienteAtual = novoAmbiente;
}

void tab_sairEscopo() {
    if (ambienteAtual == NULL) {
        fprintf(stderr, "Erro: Tentando sair de um escopo nulo.\n");
        return;
    }

    Simbolo *s = ambienteAtual->head;
    while (s != NULL) {
        Simbolo *temp = s;
        s = s->prox;
        free(temp); 
    }

    Ambiente *escopoAntigo = ambienteAtual;
    
    ambienteAtual = ambienteAtual->pai;
    
    free(escopoAntigo);
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

    novo->prox = ambienteAtual->head;
    ambienteAtual->head = novo;
}

Simbolo *tab_buscarSimboloLocal(char *nome) {
    if (ambienteAtual == NULL) return NULL;

    for (Simbolo *s = ambienteAtual->head; s != NULL; s = s->prox) {
        if (strcmp(s->nome, nome) == 0) {
            return s;
        }
    }
    
    return NULL;
}

Simbolo *tab_buscarSimbolo(char *nome) {
    for (Ambiente *escopo = ambienteAtual; escopo != NULL; escopo = escopo->pai) {
        for (Simbolo *s = escopo->head; s != NULL; s = s->prox) {
            if (strcmp(s->nome, nome) == 0) {
                return s;
            }
        }
    }
    
    return NULL;
}

void imprimirEscopo(Ambiente *escopo, int nivel) {
    if (escopo == NULL) return;

    imprimirEscopo(escopo->pai, nivel - 1);

    char indent[100] = {0};
    for(int i = 0; i < nivel; i++) strcat(indent, "  ");

    printf("%s--- Escopo Nível %d ---\n", indent, nivel);
    if (escopo->head == NULL) {
        printf("%s (vazio)\n", indent);
    } else {
        for (Simbolo *s = escopo->head; s != NULL; s = s->prox) {
            printf("%s Nome: %s, Tipo: %s\n", indent, s->nome, s->tipo);
        }
    }
}

void tab_imprimirTabela() {
    printf("\n--- Tabela de Símbolos (Visão Atual) ---\n");
    int profundidade = 0;
    for (Ambiente *a = ambienteAtual; a != NULL; a = a->pai) {
        profundidade++;
    }
    imprimirEscopo(ambienteAtual, profundidade - 1);
    printf("------------------------------------------\n");
}