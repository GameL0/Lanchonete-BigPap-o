#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdbool.h>
#include "structs.h"

// Utilitários
void sair();
int receberInput(bool res_auto, int opc_auto, int opc_manual);

// Heap
int criarHeap(Heap *heap, int capacidade);
void liberarHeap(Heap* heap);
void inserirPedido(Heap* heap, Pedido novoPedido);
int removerItemPrioritario(Heap* heap, Pedido* pedidoRemovido);
void imprimirHeap(Heap* heap);

// Fila de Prioridade
void verificarEPriorizarPedidos(Locais *local, int tempo_atual);

// Lista de pedidos
ListaPedidos criarLista();
NodePedido* criarNodePedido(Pedido pedido);
void adicionarListaPedidos(ListaPedidos *lista, NodePedido *novo_node);
NodePedido* removerListaPedidos_front(ListaPedidos *lista);
void imprimirLista(ListaPedidos lista);

// Lista de funcionários
ListaFuncionarios criarListaFuncionarios();
void inicializarFuncionarios(ListaFuncionarios *lista); // Protótipo adicionado
void adicionarListaFuncionario(ListaFuncionarios *lista, NodeFuncionario *novo_node);
NodeFuncionario* removerNodeFuncionario(ListaFuncionarios *lista, NodeFuncionario *node_para_remover);
NodeFuncionario* encontrarEAlocarFuncionario(ListaFuncionarios *reserva, Habilidade habilidade);
void liberarFuncionario(NodeFuncionario *node, ListaFuncionarios *origem, ListaFuncionarios *destino_reserva);

// Lista de itens de preparo
ListaItemPreparo criarListaItemPreparo();
NodeItemPreparo* criarNodeItemPreparo(ItemPreparo item);
void adicionarListaItemPreparo(ListaItemPreparo *lista, NodeItemPreparo *novo_node);
NodeItemPreparo* removerListaItemPreparo_front(ListaItemPreparo *lista);

// Funções de impressão
void imprimirPedido(Pedido pedido);
void imprimirNomeDoItem(NomePedido nome);
void imprimirStatusItem(StatusItem status);

// Recepção / criação de pedidos
NodePedido* receberPedido(bool res_auto, int *novo_id);
void recepcao(Locais *r, bool res_auto, ListaFuncionarios *reserva);
void processarRecepcao(Locais *local_recepcao, Locais *local_separador, ListaFuncionarios *reserva, int ciclo);

// Separador
void distribuirItens(NodePedido *pedido_node, Equipamento equipamentos[]);
void separador(Locais *local_separador, ListaFuncionarios *reserva);
void processarSeparador(Locais *local_separador, Equipamento equipamentos[], ListaFuncionarios *reserva, ListaPedidos *pedidos_em_preparo, int ciclo);

//Montagem
void verificarPedidosProntos(ListaPedidos *pedidos_em_preparo, Locais *local_montagem);
void processarMontagem(Locais *local_montagem, ListaFuncionarios *reserva, ListaPedidos *pedidos_entregues, int ciclo);
void montarBandeja(Locais *local_montagem, ListaFuncionarios *reserva);
void removerNodePedido(ListaPedidos *lista, NodePedido *node);

// Equipamentos
void processarEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva, int ciclo);
void operarEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva);

#endif // FUNCOES_H