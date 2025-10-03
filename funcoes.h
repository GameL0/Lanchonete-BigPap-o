#ifndef FUNCOES_H
#define FUNCOES_H

#include "structs.h"

// Criação/Liberação do heap
int criarHeap(Heap *heap, int capacidade);
void liberarHeap(Heap *heap);

// Operações do heap (max-heap por maior prioridade)
int inserirPedidoComSLA(Heap *heap, Pedido pedido, int janelaSegundos);
int removerItemPrioritario(Heap *heap, Pedido *pedidoRemovido);
void reclassificarSLA(Heap *heap, int janelaSegundos);
void imprimirHeap(const Heap *heap);

// Cálculos
int calcularPreparoItem(NomePedido nome);
int calcularPreparoPedido(Pedido *pedido);
int somarCargaAltaPrioridade(const Heap *heap);

// Utilidades (demo)
Pedido criarPedido(int id, int tempo_chegada,
                   NomePedido a, NomePedido b, NomePedido c, NomePedido d);

#endif

