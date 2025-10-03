#ifndef FUNCOES_H
#define FUNCOES_H

#include "structs.h" // Inclui as definições das structs

// --- Protótipos das Funções ---
void sair();
int receberInput(bool res_auto, int opc_auto, int opc_manual);
int criarHeap(Heap *heap, int capacidade);
void liberarHeap(Heap* heap);
void swap(Pedido *a, Pedido *b);
void subirNoHeap(Heap* heap, int indice);
void inserirPedido(Heap* heap, Pedido novoPedido);
void descerNoHeap(Heap* heap, int indice);
int removerItemPrioritario(Heap* heap, Pedido* pedidoRemovido);
void imprimirHeap(Heap* heap);

// Funções de Lista
ListaPedidos criarLista();
ListaPedidos removerLista(ListaPedidos lista);

// Funções de Impressão
void imprimirStatusPedido(StatusPedido status);
void imprimirStatusItem(StatusItem status);
void imprimirNomeDoItem(NomePedido nome);
void imprimirItensPedido(ItemPedido item);
void imprimirPedido(Pedido pedido);
void imprimirLista(ListaPedidos lista);

// Funções de Cálculo
int calcularPreparo(NomePedido pedido);
int tempo_restante(Pedido *pedido, Funcionario funcionario[], Equipamento equipamentos[]);

#endif