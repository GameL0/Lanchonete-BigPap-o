#ifndef FUNCOES_H
#define FUNCOES_H

#include "structs.h"

// ==================================================================
// DECLARAÇÃO COMPLETA DE TODAS AS FUNÇÕES
// ==================================================================

// Funções de Utilitários e Input
void sair();
int receberInput(bool res_auto, int opc_auto, int opc_manual);
int calcularPreparo(NomePedido pedido);
int fazerPedido(bool res_auto);

// Funções de Heap
int criarHeap(Heap *heap, int capacidade);
void liberarHeap(Heap* heap);
void inserirPedido(Heap* heap, Pedido novoPedido);
Pedido removerDoHeap(Heap* heap);
void imprimirHeap(Heap* heap);

// Funções de Lista de Pedidos
ListaPedidos criarLista();
NodePedido* criarNodePedido(Pedido pedido);
void adicionarListaPedidos(ListaPedidos *lista, NodePedido *novo_node);
void removerNodePedido(ListaPedidos *lista, NodePedido *node);
NodePedido* removerListaPedidos_front(ListaPedidos *lista);
void imprimirLista(ListaPedidos lista);

// Funções de Impressão
void imprimirNomeDoItem(NomePedido nome);
void imprimirStatusItem(StatusItem status);
void imprimirPedido(Pedido pedido);
void imprimirStatusEquipamentos(Equipamento equipamentos[]);

// Funções de Lista de Itens para Preparo
ListaItemPreparo criarListaItemPreparo();
void adicionarListaItemPreparo(ListaItemPreparo *lista, NodeItemPreparo *novo_node);
NodeItemPreparo* removerListaItemPreparo_front(ListaItemPreparo *lista);
void removerNodeItemPreparo(ListaItemPreparo *lista, NodeItemPreparo *node);


// Funções de Lista de Funcionários
ListaFuncionarios criarListaFuncionarios();
void inicializarFuncionarios(ListaFuncionarios *lista);
void adicionarListaFuncionario(ListaFuncionarios *lista, NodeFuncionario *novo_node);
NodeFuncionario* removerNodeFuncionario(ListaFuncionarios *lista, NodeFuncionario *node_para_remover);
NodeFuncionario* encontrarEAlocarFuncionario(ListaFuncionarios *reserva, Habilidade habilidade);
void liberarFuncionario(NodeFuncionario *node, ListaFuncionarios *origem, ListaFuncionarios *destino_reserva);

// Funções de Lista de Estoque
ListaItensArmazenados criarListaArmazenados();
void imprimirListaArmazenados(ListaItensArmazenados lista);
void verificarEstoqueExpirado(ListaItensArmazenados *estoque, int tempo_atual);
void adicionarItemArmazenado(ListaItensArmazenados *lista, NodeItemArmazenado *novo_node);
void removerNodeArmazenado(ListaItensArmazenados *lista, NodeItemArmazenado *node);
NodeItemArmazenado* criarNodeItemArmazenado(ItemArmazenado item);

// Funções de Gerenciamento de Equipamentos
void gerenciarChapa(Equipamento *chapa, ListaFuncionarios *reserva);
void gerenciarLiquidificador(Equipamento *liquidificador_suco, Equipamento *liquidificador_milkshake, ListaFuncionarios *reserva);
void gerenciarPeneira(Equipamento *peneira, ListaFuncionarios *reserva);
void gerenciarTodosEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva);

// Funções de Lógica da Lanchonete
float estimarTempoTotalPedido(Pedido* pedido, Equipamento equipamentos[], Locais locais[]);
NodePedido* receberPedido(bool res_auto, int *novo_id, int tempo_simulacao_atual);
void atenderFilaRecepcao(Locais *local_recepcao, ListaFuncionarios *reserva);
void distribuirItens(NodePedido *pedido_node, Equipamento equipamentos[], ListaItensArmazenados *estoque);
void recepcao(Locais *r, bool res_auto, ListaFuncionarios *reserva, ListaItensArmazenados *estoque, int tempo_simulacao_atual, Equipamento equipamentos[], ListaPedidos *pedidos_postergados, int timer_global);
void separador(Locais *local_separador, ListaFuncionarios *reserva, ListaItensArmazenados *estoque);
void processarRecepcao(Locais *local_recepcao, Locais *local_separador, ListaFuncionarios *reserva, int ciclo);
void processarSeparador(Locais *local_separador, Equipamento equipamentos[], ListaFuncionarios *reserva, ListaPedidos *pedidos_em_preparo, int ciclo, ListaItensArmazenados *estoque);
void processarEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva, int ciclo, ListaItensArmazenados *estoque, int tempo_simulacao_atual);
void processarMontagem(Locais *local_montagem, ListaFuncionarios *reserva, ListaPedidos *pedidos_entregues, int ciclo, int tempo_simulacao_atual, Estatisticas *stats);
void montarBandeja(Locais *local_montagem, ListaFuncionarios *reserva);
void verificarPedidosProntos(ListaPedidos *pedidos_em_preparo, Locais *local_montagem);
void priorizarPedidosFimDeExpediente(Locais *local);
void priorizarPedidosAntigos(Locais locais[], int tempo_simulacao_atual);
bool existemPedidosAtivos(Locais locais[], Equipamento equipamentos[], ListaPedidos *pedidos_em_preparo);

// Funções de Limpeza de Memória
void liberarListaPedidos(ListaPedidos *lista);
void liberarListaFuncionarios(ListaFuncionarios *lista);
void liberarListaItensArmazenados(ListaItensArmazenados *lista);

#endif //FUNCOES_H