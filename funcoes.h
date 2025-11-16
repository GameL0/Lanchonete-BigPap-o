#ifndef FUNCOES_H
#define FUNCOES_H

#include <stdio.h> // Adicionado para FILE*
#include "structs.h"

// ==================================================================
// DECLARAÇÃO COMPLETA DE TODAS AS FUNÇÕES
// ==================================================================

// Funções de Utilitários e Input
void sair();
int calcularPreparo(NomePedido pedido);

// Funções de Heap
int criarHeap(Heap *heap, int capacidade);
void liberarHeap(Heap* heap);
void inserirPedido(Heap* heap, Pedido novoPedido);
Pedido removerDoHeap(Heap* heap);
void imprimirHeap(Heap* heap, int restaurante_id, const char* local_nome); 

// Funções de Lista de Pedidos
ListaPedidos criarLista();
NodePedido* criarNodePedido(Pedido pedido);
void adicionarListaPedidos(ListaPedidos *lista, NodePedido *novo_node);
void removerNodePedido(ListaPedidos *lista, NodePedido *node);
NodePedido* removerListaPedidos_front(ListaPedidos *lista);
void imprimirLista(ListaPedidos lista, int restaurante_id, const char* label); 
NodePedido* removerPedidoPorPrioridade(Locais *local, const char** origem_fila);

// Funções de Impressão
void imprimirNomeDoItem(NomePedido nome);
void imprimirStatusItem(StatusItem status);
void imprimirPedido(Pedido pedido); 
void imprimirStatusEquipamentos(Restaurante *res); 
// <<< ALTERAÇÃO: Funções de relatório específicas >>>
void imprimirFilaSeparacao(Restaurante *res);
void imprimirFilasPrioridadeRestantes(Restaurante *res);
// void imprimirFilasLocais(Restaurante *res); // <<< REMOVIDA

// Funções de Lista de Itens para Preparo
ListaItemPreparo criarListaItemPreparo();
void adicionarListaItemPreparo(ListaItemPreparo *lista, NodeItemPreparo *novo_node);
NodeItemPreparo* removerListaItemPreparo_front(ListaItemPreparo *lista);
void removerNodeItemPreparo(ListaItemPreparo *lista, NodeItemPreparo *node);


// Funções de Lista de Funcionários
ListaFuncionarios criarListaFuncionarios();
void inicializarFuncionarios(Restaurante *res); 
void adicionarListaFuncionario(ListaFuncionarios *lista, NodeFuncionario *novo_node);
NodeFuncionario* removerNodeFuncionario(ListaFuncionarios *lista, NodeFuncionario *node_para_remover);
NodeFuncionario* encontrarEAlocarFuncionario(ListaFuncionarios *reserva, Habilidade habilidade);
void liberarFuncionario(NodeFuncionario *node, ListaFuncionarios *origem, ListaFuncionarios *destino_reserva);

// Funções de Lista de Estoque
ListaItensArmazenados criarListaArmazenados();
void imprimirListaArmazenados(ListaItensArmazenados lista, int restaurante_id); 
void verificarEstoqueExpirado(Restaurante *res, int tempo_atual); 

void adicionarItemArmazenado(ListaItensArmazenados *lista, NodeItemArmazenado *novo_node);
void removerNodeArmazenado(ListaItensArmazenados *lista, NodeItemArmazenado *node);
NodeItemArmazenado* criarNodeItemArmazenado(ItemArmazenado item);

// Funções de Gerenciamento de Equipamentos
void gerenciarChapa(Restaurante *res, bool restaurante_aberto);
void gerenciarLiquidificador(Restaurante *res, bool restaurante_aberto);
void gerenciarPeneira(Restaurante *res, bool restaurante_aberto);
void gerenciarTodosEquipamentos(Restaurante *res, bool restaurante_aberto);

// Funções de Lógica da Lanchonete
float estimarTempoTotalPedido(Pedido* pedido, Restaurante *res); 
NodePedido* lerNovoPedidoFormatado(FILE *arquivo_entrada, int tempo_simulacao_atual); 

bool recepcao(FILE *arquivo_entrada, Restaurante restaurantes[], int timer_global, int tempo_simulacao_atual, bool restaurante_aberto); 

// Funções de processamento de um restaurante
void atenderFilaRecepcao(Restaurante *res); 
void distribuirItens(NodePedido *pedido_node, Restaurante *res); 
void separador(Restaurante *res); 
void processarRecepcao(Restaurante *res, int ciclo); 
void processarSeparador(Restaurante *res, int ciclo); 
void processarEquipamentos(Restaurante *res, int ciclo, int tempo_simulacao_atual); 
void processarMontagem(Restaurante *res, int ciclo, int tempo_simulacao_atual); 
void montarBandeja(Restaurante *res); 
void verificarPedidosProntos(Restaurante *res); 

void priorizarPedidosFimDeExpediente(Restaurante *res);
void priorizarPedidosAntigos(Restaurante *res, int tempo_simulacao_atual); 
bool existemPedidosAtivos(Restaurante *res); 
int contarPedidosEmProducao(Restaurante *res); 

// Funções de Limpeza de Memória
void liberarListaPedidos(ListaPedidos *lista);
void liberarListaFuncionarios(ListaFuncionarios *lista);
void liberarListaItensArmazenados(ListaItensArmazenados *lista);
void liberarRestaurante(Restaurante *res); 

#endif //FUNCOES_H
