#ifndef STRUCTS_H
#define STRUCTS_H

#include <stdbool.h>
#include "enums.h"

#define MAX_CAPACIDADE_EQUIPAMENTO 4
#define MAX_ITENS_POR_PEDIDO 50 // Limite de itens por pedido

// Forward declarations para resolver dependências de ponteiros
struct NodePedido;
struct NodeFuncionario;
struct Pedido;
struct Restaurante; // <<< ADICIONADO

// Estrutura para guardar as estatísticas da simulação
typedef struct Estatisticas {
    int total_pedidos_entregues;
    int pedidos_mais_de_5_min;
} Estatisticas;

// Estruturas de Pedidos e Itens
typedef struct ItemPedido {
    NomePedido nome;
    StatusItem status;
    int tempo_preparo_total;
} ItemPedido;

typedef struct Pedido {
    int id; // ID global único
    int id_senha; // Senha lida do arquivo
    TipoPedido tipo; // PRESENCIAL ou IFOOD
    int distancia_rest_1;
    int distancia_rest_2;
    int id_restaurante_atribuido; // 1 ou 2
    
    ItemPedido itens[MAX_ITENS_POR_PEDIDO]; 
    int num_itens;
    StatusPedido status;
    int tempo_chegada;
    int tempo_preparo_total;
    int tempo_restante_preparo;
    int tempo_preparo_local;
} Pedido;

typedef struct NodePedido {
    Pedido pedido;
    struct NodePedido *ante;
    struct NodePedido *prox;
} NodePedido;

typedef struct ListaPedidos {
    NodePedido *cabeca;
    NodePedido *cauda;
    int quantidade;
} ListaPedidos;

typedef struct Heap {
    Pedido* pedidos;
    int quantidade;
    int capacidade;
} Heap;

// Estruturas de Funcionários e Locais
typedef struct Funcionario {
    int id;
    Habilidade habilidades[3];
    int num_habilidade;
    StatusFuncionario status;
    NomeLocal local_atual;
    struct NodePedido *pedido_trabalhado;
} Funcionario;

typedef struct NodeFuncionario {
    Funcionario funcionario;
    struct NodeFuncionario *ante;
    struct NodeFuncionario *prox;
} NodeFuncionario;

typedef struct ListaFuncionarios {
    NodeFuncionario *cabeca;
    NodeFuncionario *cauda;
    int quantidade;
} ListaFuncionarios;

// Estrutura de Estoque (Item Armazenado)
typedef struct ItemArmazenado {
    NomePedido nome;
    int tempo_expiracao;
} ItemArmazenado;

typedef struct NodeItemArmazenado {
    ItemArmazenado item;
    struct NodeItemArmazenado *ante;
    struct NodeItemArmazenado *prox;
} NodeItemArmazenado;

typedef struct ListaItensArmazenados {
    NodeItemArmazenado *cabeca;
    NodeItemArmazenado *cauda;
    int quantidade;
} ListaItensArmazenados;

// Estruturas de Preparo e Equipamentos
typedef struct ItemPreparo {
    NomePedido nome;
    StatusItem status;
    int tempo_preparo_total;
    int tempo_restante_preparo;
    struct Pedido* pedido_pai;
    ItemPedido* item_original;
} ItemPreparo;

typedef struct NodeItemPreparo {
    ItemPreparo item;
    struct NodeItemPreparo *ante, *prox;
} NodeItemPreparo;

typedef struct ListaItemPreparo {
    NodeItemPreparo *cabeca, *cauda;
    int quantidade;
} ListaItemPreparo;

typedef struct Equipamento {
    NomeEquipamento nome;
    int capacidade_maxima;
    int capacidade_usada;
    ListaItemPreparo fila_espera;
    ItemPreparo itens_em_preparo[MAX_CAPACIDADE_EQUIPAMENTO];
    struct NodeFuncionario* funcionarios_alocados[MAX_CAPACIDADE_EQUIPAMENTO];
} Equipamento;


typedef struct Locais {
    NomeLocal nome;
    // <<< ALTERAÇÃO: Voltamos para fila_espera única >>>
    ListaPedidos fila_espera;
    // ListaPedidos fila_ifood;       // <<< REMOVIDO
    // ListaPedidos fila_presencial;  // <<< REMOVIDO
    ListaPedidos pedido_sendo_feitos;
    ListaFuncionarios funcionario;
    Heap heap;
} Locais;


// Estrutura principal
typedef struct Restaurante {
    int id; // 1 ou 2
    Locais locais[5];
    Equipamento equipamentos[4];
    ListaItensArmazenados estoque;
    ListaFuncionarios reserva;
    ListaPedidos pedidos_entregues; // Fila de Itens Feitos
    ListaPedidos pedidos_em_preparo;
    ListaPedidos pedidos_postergados;
    Estatisticas stats;
} Restaurante;


#endif // STRUCTS_H
