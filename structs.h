#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include "enums.h"

// --- Estruturas dos Pedidos ---

typedef struct{
    NomePedido nome;
    int tempo_preparo_total;
    int tempo_restante_preparo;
    StatusItem status;
} ItemPedido;

typedef struct{
    ItemPedido itens[4];
    int id;
    int num_itens;
    int tempo_chegada;            // Momento de chegada (segundos)
    int tempo_preparo_total;      // Chave principal de prioridade (maior = mais prioritário)
    int tempo_preparo_local;      // Uso livre em fases do fluxo
    StatusPedido status;
    bool baixa_prioridade;        // true = vai para o fim da fila
} Pedido;

// Lista (opcional para futuras integrações)
typedef struct NodePedido {
    Pedido pedido;
    struct NodePedido *ante;
    struct NodePedido *prox;
} NodePedido;

typedef struct{
    NodePedido *cabeca;
    NodePedido *cauda;
    int quantidade;
} ListaPedidos;

// --- Estrutura de Controle (Fila de Prioridades) ---

typedef struct
{
    Pedido *pedidos;  // Vetor dinâmico de pedidos
    int quantidade;   // Tamanho atual
    int capacidade;   // Capacidade máxima
} Heap;

#endif

