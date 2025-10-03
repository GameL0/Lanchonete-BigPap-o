#include <stdio.h>
#include "enums.h"

// --- Estruturas dos Pedidos ---

// Representa um item individual (ex: um sanduiche, uma batata)
typedef struct{
    NomePedido nome;
    int tempo_preparo_total;
    int tempo_restante_preparo;
    StatusItem status;
} ItemPedido;

// Representa um pedido completo de um cliente
typedef struct{
    ItemPedido itens[4]; // Um pedido tem no máximo 4 itens (2 de comer, 2 de beber)
    int id;
    int num_itens;
    int tempo_chegada; // Guarda o "tempo_global" em que o pedido chegou
    int tempo_preparo_total; // Chave da prioridade para o Heap. Calculado na chegada.
    int tempo_preparo_local; //Guarda o tempo que falta para terminar de ser preparado.
    StatusPedido status;
} Pedido;


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

// --- Estruturas dos Recursos da Lanchonete ---

// Representa um funcionário e suas capacidades
typedef struct{
    int id;
    Habilidade habilidades[3]; // Um funcionario pode ter multiplas habilidades
    int num_habilidade;
    StatusFuncionario status;
    NomeLocal local_atual;
    NodePedido *pedido_trabalhado; // Para saber qual pedido ele está trabalhando
} Funcionario; // :)^_____^O_O:-];O):O(>_<

typedef struct 
{
    NomePedido nome;
    int tempo_preparo;
    StatusItem status;
    Pedido *pedido;
} ItemPreparo;

typedef struct NodeFuncionario
{
    Funcionario funcionario;
    struct NodeFuncionario *ante;
    struct NodeFuncionario *prox;
} NodeFuncionario;


typedef struct 
{
    NodeFuncionario *cabeca;
    NodeFuncionario *cauda;
    int quantidade;
} ListaFuncionarios;

// Representa um equipamento com capacidade limitada (chapa, fritadeira, etc)
typedef struct{
    NomeEquipamento nome;
    int capacidade_maxima; // Sera definido pelas regras do nome Arthur
    int capacidade_usada;
    ItemPreparo itens_preparo[6];
    ItemPreparo armazenamento[4];
    ListaFuncionarios funcionarios;
    ListaPedidos fila;
} Equipamento;


// --- Estrutura de Controle (Fila de Prioridades) ---

typedef struct
{
    Pedido *pedidos; // Vetor dinâmico para armazenar os pedidos
    int quantidade; // Quantidade atual de pedidos na fila
    int capacidade; // Tamanho máximo do vetor
} Heap;


typedef struct 
{
    NomeLocal nome;
    ListaPedidos fila_espera;
    ListaFuncionarios funcionario;
    Heap heap;
    int capacidade_usada;
    ListaPedidos pedido_sendo_feitos; //Transformar em vetor dinamico
} Locais;
