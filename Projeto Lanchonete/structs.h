#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdio.h>
#include "enums.h"

// --- Estruturas dos Pedidos ---
// MOVEMOS AS DEFINIÇÕES DE PEDIDO PARA ANTES DE ITEMPREPARO

typedef struct {
    NomePedido nome;
    int tempo_preparo_total;
    int tempo_restante_preparo;
    StatusItem status;
} ItemPedido;

typedef struct {
    ItemPedido itens[4]; // Máx 4 itens (2 comer + 2 beber)
    int id;
    int num_itens;
    int tempo_chegada;
    int tempo_preparo_total; // tempo total de preparo
    int tempo_restante_preparo; // tempo restante para preparo (chave do heap)
    int tempo_preparo_local;
    StatusPedido status;
} Pedido;

typedef struct NodePedido {
    Pedido pedido;
    struct NodePedido *ante;
    struct NodePedido *prox;
} NodePedido;

typedef struct {
    NodePedido *cabeca;
    NodePedido *cauda;
    int quantidade;
} ListaPedidos;


typedef struct ItemPreparo {
    NomePedido nome;
    int tempo_preparo_total;
    int tempo_restante_preparo;
    StatusItem status;
    Pedido *pedido_pai; 
    ItemPedido *item_original; // <--- CORREÇÃO: Removido "struct"
} ItemPreparo;

typedef struct NodeItemPreparo {
    ItemPreparo item;
    struct NodeItemPreparo *ante;
    struct NodeItemPreparo *prox;
} NodeItemPreparo;

typedef struct {
    NodeItemPreparo *cabeca;
    NodeItemPreparo *cauda;
    int quantidade;
} ListaItemPreparo;

// --- Estruturas de Funcionários ---
typedef struct NodePedido NodePedido; // Forward declaration para uso em Funcionario

typedef struct {
    int id;
    Habilidade habilidades[3];
    int num_habilidade;
    StatusFuncionario status;
    NomeLocal local_atual;
    NodePedido *pedido_trabalhado;
} Funcionario;

typedef struct NodeFuncionario {
    Funcionario funcionario;
    struct NodeFuncionario *ante;
    struct NodeFuncionario *prox;
} NodeFuncionario;

typedef struct {
    NodeFuncionario *cabeca;
    NodeFuncionario *cauda;
    int quantidade;
} ListaFuncionarios;

// --- Estrutura de Equipamento ---
#define MAX_CAPACIDADE_EQUIPAMENTO 6

typedef struct {
    NomeEquipamento nome;
    int capacidade_maxima;
    int capacidade_usada;
    ListaItemPreparo fila_espera;
    
    ItemPreparo itens_em_preparo[MAX_CAPACIDADE_EQUIPAMENTO];
    NodeFuncionario* funcionarios_alocados[MAX_CAPACIDADE_EQUIPAMENTO];
} Equipamento;

// --- Heap de pedidos ---
typedef struct {
    Pedido *pedidos;
    int quantidade;
    int capacidade;
} Heap;

// --- Estrutura de Locais ---
typedef struct {
    NomeLocal nome;
    ListaPedidos fila_espera;
    ListaFuncionarios funcionario;
    Heap heap;
    int capacidade_usada;
    ListaPedidos pedido_sendo_feitos;
} Locais;

#endif // STRUCTS_H