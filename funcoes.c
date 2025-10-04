#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "funcoes.h"

// Limite de tempo (em segundos) para um pedido ser considerado antigo e precisar de prioridade
#define PEDIDO_ANTIGO_THRESHOLD 240 // 4 minutos

static int next_pedido_id = 1;

/*----------------- Utilitários -----------------*/
void sair() {
    printf("\nPrograma finalizado!\n");
    exit(0);
}

int fazerPedido(bool res_auto) {
    printf("\n+-------------------------------------------------------------+\n");
    printf("|  BEM VINDO AO BIG PAPÃO, O SANDUICHE MAIS RAPIDO DO NORDESTE!!  |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("Queres fazer a sua escolha?\n");
    printf(" 1 - Fazer Pedido\n");
    printf(" 2 - Nao fazer pedido (avancar ciclo)\n");
    printf(" 3 - Fechar o programa\n");
    printf("Resposta: ");
    return receberInput(res_auto, 2, 3);
}

/*----------------- Input -----------------*/
int receberInput(bool res_auto, int opc_auto, int opc_manual) {
    int var_control;
    if (res_auto) {
        var_control = (rand() % opc_auto) + 1;
        printf("%d\n", var_control);
        return var_control;
    } else {
        if (scanf("%d", &var_control) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF) {}
            return receberInput(res_auto, opc_auto, opc_manual);
        }
        if (var_control > 0 && var_control <= opc_manual)
            return var_control;
        printf("\nResposta inválida! Tente Novamente.\n");
        return receberInput(res_auto, opc_auto, opc_manual);
    }
}

/*----------------- HEAP (min-heap por tempo_preparo_total) -----------------*/
int criarHeap(Heap *heap, int capacidade) {
    heap->pedidos = (Pedido*) malloc(capacidade * sizeof(Pedido));
    if (heap->pedidos == NULL) return 0;
    heap->quantidade = 0;
    heap->capacidade = capacidade;
    return 1;
}

void liberarHeap(Heap* heap) {
    if (heap->pedidos) free(heap->pedidos);
    heap->pedidos = NULL;
    heap->quantidade = 0;
    heap->capacidade = 0;
}

static void swapPedido(Pedido *a, Pedido *b) {
    Pedido tmp = *a;
    *a = *b;
    *b = tmp;
}

static void subirNoHeap(Heap* heap, int indice) {
    while (indice > 0) {
        int pai = (indice - 1) / 2;
        if (heap->pedidos[indice].tempo_restante_preparo < heap->pedidos[pai].tempo_restante_preparo) {
            swapPedido(&heap->pedidos[indice], &heap->pedidos[pai]);
            indice = pai;
        } else break;
    }
}

void inserirPedido(Heap* heap, Pedido novoPedido) {
    if (heap->quantidade >= heap->capacidade) {
        printf("Heap cheio: não foi possível inserir pedido.\n");
        return;
    }
    int idx = heap->quantidade++;
    heap->pedidos[idx] = novoPedido;
    subirNoHeap(heap, idx);
}

static void descerNoHeap(Heap* heap, int indice) {
    int n = heap->quantidade;
    while (true) {
        int l = 2*indice + 1;
        int r = 2*indice + 2;
        int menor = indice;
        if (l < n && heap->pedidos[l].tempo_restante_preparo < heap->pedidos[menor].tempo_restante_preparo) menor = l;
        if (r < n && heap->pedidos[r].tempo_restante_preparo < heap->pedidos[menor].tempo_restante_preparo) menor = r;
        if (menor != indice) {
            swapPedido(&heap->pedidos[indice], &heap->pedidos[menor]);
            indice = menor;
        } else break;
    }
}

Pedido removerDoHeap(Heap* heap) {
    if (heap->quantidade == 0) {
        Pedido pedido_vazio;
        memset(&pedido_vazio, 0, sizeof(Pedido));
        pedido_vazio.id = -1;
        return pedido_vazio;
    }
    
    Pedido raiz = heap->pedidos[0];
    heap->pedidos[0] = heap->pedidos[heap->quantidade - 1];
    heap->quantidade--;
    descerNoHeap(heap, 0);
    
    return raiz;
}

void imprimirHeap(Heap* heap) {
    if (heap->quantidade == 0) {
        printf("  (Heap vazio)\n");
        return;
    }
    for (int i = 0; i < heap->quantidade; ++i) {
        Pedido p = heap->pedidos[i];
        printf("  -> [PRIORIDADE] PEDIDO ID:%d - TempoRestante:%d\n", p.id, p.tempo_restante_preparo);
    }
}


/*----------------- Lista de pedidos (duplamente ligada) -----------------*/
ListaPedidos criarLista() {
    ListaPedidos l;
    l.cabeca = l.cauda = NULL;
    l.quantidade = 0;
    return l;
}

NodePedido* criarNodePedido(Pedido pedido) {
    NodePedido *n = (NodePedido*) malloc(sizeof(NodePedido));
    if (!n) return NULL;
    n->pedido = pedido;
    n->ante = n->prox = NULL;
    return n;
}

void adicionarListaPedidos(ListaPedidos *lista, NodePedido *novo_node) {
    if (!lista || !novo_node) return;
    novo_node->prox = NULL;
    novo_node->ante = lista->cauda;
    if (lista->cauda) lista->cauda->prox = novo_node;
    lista->cauda = novo_node;
    if (!lista->cabeca) lista->cabeca = novo_node;
    lista->quantidade++;
}

void removerNodePedido(ListaPedidos *lista, NodePedido *node) {
    if (!lista || !node) return;
    if (node->ante) node->ante->prox = node->prox;
    else lista->cabeca = node->prox;
    if (node->prox) node->prox->ante = node->ante;
    else lista->cauda = node->ante;
    node->prox = NULL;
    node->ante = NULL;
    lista->quantidade--;
}

NodePedido* removerListaPedidos_front(ListaPedidos *lista) {
    if (!lista || !lista->cabeca) return NULL;
    NodePedido *ret = lista->cabeca;
    lista->cabeca = ret->prox;
    if (lista->cabeca) lista->cabeca->ante = NULL;
    else lista->cauda = NULL;
    ret->prox = ret->ante = NULL;
    lista->quantidade--;
    return ret;
}

void imprimirLista(ListaPedidos lista) {
    NodePedido *atual = lista.cabeca;
    if (!atual) {
        printf("  (Lista vazia)\n");
        return;
    }
    while (atual) {
        imprimirPedido(atual->pedido);
        atual = atual->prox;
    }
}

/*----------------- Impressão de pedidos e itens -----------------*/
void imprimirNomeDoItem(NomePedido nome) {
    switch (nome) {
        case SANDUICHE_SIMPLES: printf("Sanduiche Simples"); break;
        case SANDUICHE_MEDIO: printf("Sanduiche Medio"); break;
        case SANDUICHE_ELABORADO: printf("Sanduiche Elaborado"); break;
        case BATATA_FRITA: printf("Batata Frita"); break;
        case REFRIGERANTE: printf("Refrigerante"); break;
        case MILK_SHAKE: printf("Milk Shake"); break;
        case SUCO: printf("Suco"); break;
        case NADA: printf("Nada"); break;
        default: printf("Desconhecido"); break;
    }
}

void imprimirStatusItem(StatusItem status) {
    switch (status) {
        case AGUARDANDO_PREPARADO: printf(" (Aguardando)"); break;
        case EM_PREPARO: printf(" (Em preparo)"); break;
        case PRONTO: printf(" (Pronto)"); break;
        default: break;
    }
}

void imprimirPedido(Pedido pedido) {
    printf("  -> PEDIDO ID:%d - Itens: %d - TempoTotal:%d\n", 
           pedido.id, pedido.num_itens, pedido.tempo_preparo_total);
    for (int i = 0; i < pedido.num_itens; i++) {
        printf("     - ");
        imprimirNomeDoItem(pedido.itens[i].nome);
        imprimirStatusItem(pedido.itens[i].status);
        printf(" (t:%d)\n", pedido.itens[i].tempo_preparo_total);
    }
}

void imprimirStatusChapa(Equipamento chapa) {
    printf("\n=== STATUS DA CHAPA ===\n");
    printf("  Capacidade Usada: %d de %d\n", chapa.capacidade_usada, chapa.capacidade_maxima);
    printf("  Sanduiches em Preparo:\n");

    for (int i = 0; i < chapa.capacidade_maxima; i++) {
        ItemPreparo item_na_chapa = chapa.itens_em_preparo[i];
        printf("    -> Slot %d: ", i + 1);

        if (item_na_chapa.nome != NADA) {
            imprimirNomeDoItem(item_na_chapa.nome);
            printf(" (Do Pedido ID: %d)", item_na_chapa.pedido_pai->id);
            
            int tempo_restante = item_na_chapa.tempo_restante_preparo > 0 ? item_na_chapa.tempo_restante_preparo : 0;
            printf(" | Tempo Restante: %d s", tempo_restante);

            if (chapa.funcionarios_alocados[i] != NULL) {
                 printf(" | Cozinheiro ID: %d", chapa.funcionarios_alocados[i]->funcionario.id);
            }
            printf("\n");
        } else {
            printf("(Vazio)\n");
        }
    }

    printf("  Fila de Espera para a Chapa (%d sanduiches):\n", chapa.fila_espera.quantidade);
    if (chapa.fila_espera.cabeca == NULL) {
        printf("     (Fila de espera vazia)\n");
    } else {
        NodeItemPreparo* atual = chapa.fila_espera.cabeca;
        while(atual != NULL) {
            printf("    -> ");
            imprimirNomeDoItem(atual->item.nome);
            printf(" (Do Pedido ID: %d)\n", atual->item.pedido_pai->id);
            atual = atual->prox;
        }
    }
    printf("=======================\n");
}


/*----------------- Fila de Prioridade -----------------*/
void priorizarPedidosFimDeExpediente(Locais *local) {
    NodePedido *atual = local->fila_espera.cabeca;
    while (atual != NULL) {
        NodePedido *proximo = atual->prox;
        
        printf("PRIORIDADE (FIM EXPEDIENTE): Pedido ID:%d movido para a fila de prioridade!\n", atual->pedido.id);
        
        removerNodePedido(&local->fila_espera, atual);
        inserirPedido(&local->heap, atual->pedido);
        free(atual);
        
        atual = proximo;
    }
}


/*----------------- Lista de Itens para Preparo -----------------*/
ListaItemPreparo criarListaItemPreparo() {
    ListaItemPreparo l;
    l.cabeca = l.cauda = NULL;
    l.quantidade = 0;
    return l;
}

NodeItemPreparo* criarNodeItemPreparo(ItemPreparo item) {
    NodeItemPreparo *n = (NodeItemPreparo*) malloc(sizeof(NodeItemPreparo));
    if (!n) return NULL;
    n->item = item;
    n->ante = n->prox = NULL;
    return n;
}

void adicionarListaItemPreparo(ListaItemPreparo *lista, NodeItemPreparo *novo_node) {
    if (!lista || !novo_node) return;
    novo_node->prox = NULL;
    novo_node->ante = lista->cauda;
    if (lista->cauda) lista->cauda->prox = novo_node;
    lista->cauda = novo_node;
    if (!lista->cabeca) lista->cabeca = novo_node;
    lista->quantidade++;
}

NodeItemPreparo* removerListaItemPreparo_front(ListaItemPreparo *lista) {
    if (!lista || !lista->cabeca) return NULL;
    NodeItemPreparo *ret = lista->cabeca;
    lista->cabeca = ret->prox;
    if (lista->cabeca) lista->cabeca->ante = NULL;
    else lista->cauda = NULL;
    ret->prox = ret->ante = NULL;
    lista->quantidade--;
    return ret;
}

/*----------------- Montagem -----------------*/
bool todosItensProntos(Pedido *pedido) {
    for (int i = 0; i < pedido->num_itens; i++) {
        if (pedido->itens[i].nome == NADA) continue;
        if (pedido->itens[i].status != PRONTO) {
            return false;
        }
    }
    return true;
}

void verificarPedidosProntos(ListaPedidos *pedidos_em_preparo, Locais *local_montagem) {
    NodePedido *atual = pedidos_em_preparo->cabeca;
    while (atual != NULL) {
        NodePedido *proximo = atual->prox;
        if (todosItensProntos(&atual->pedido)) {
            printf("MONTAGEM: Pedido %d esta com todos os itens prontos. Movendo para a fila de montagem.\n", atual->pedido.id);
            removerNodePedido(pedidos_em_preparo, atual);
            adicionarListaPedidos(&local_montagem->fila_espera, atual);
        }
        atual = proximo;
    }
}

void processarMontagem(Locais *local_montagem, ListaFuncionarios *reserva, ListaPedidos *pedidos_entregues, int ciclo, int tempo_simulacao_atual, Estatisticas *stats) {
    NodeFuncionario *func_atual = local_montagem->funcionario.cabeca;
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;
        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_em_montagem = func_atual->funcionario.pedido_trabalhado;
            pedido_em_montagem->pedido.tempo_preparo_local -= ciclo;

            if (pedido_em_montagem->pedido.tempo_preparo_local <= 0) {
                pedido_em_montagem->pedido.status = ENTREGUE;
                
                stats->total_pedidos_entregues++;
                int tempo_total_do_pedido = tempo_simulacao_atual - pedido_em_montagem->pedido.tempo_chegada;
                
                printf("ENTREGA: Pedido %d foi montado e ENTREGUE em %d segundos!\n", pedido_em_montagem->pedido.id, tempo_total_do_pedido);

                if (tempo_total_do_pedido > 300) {
                    stats->pedidos_mais_de_5_min++;
                }

                removerNodePedido(&local_montagem->pedido_sendo_feitos, pedido_em_montagem);
                adicionarListaPedidos(pedidos_entregues, pedido_em_montagem);
                
                func_atual->funcionario.pedido_trabalhado = NULL;
                liberarFuncionario(func_atual, &local_montagem->funcionario, reserva);
            }
        }
        func_atual = proximo_func;
    }
}

void montarBandeja(Locais *local_montagem, ListaFuncionarios *reserva) {
    while (local_montagem->heap.quantidade > 0 || local_montagem->fila_espera.cabeca != NULL) {
        NodeFuncionario *montador = encontrarEAlocarFuncionario(reserva, HABILIDADE_MONTAGEM);
        if (montador == NULL) break;

        NodePedido *pedido_para_montar = NULL;

        if (local_montagem->heap.quantidade > 0) {
            printf("MONTAGEM: Pegando pedido prioritario (ID: %d) do heap.\n", local_montagem->heap.pedidos[0].id);
            Pedido p = removerDoHeap(&local_montagem->heap);
            if (p.id != -1) {
                pedido_para_montar = criarNodePedido(p);
            }
        } 
        else if (local_montagem->fila_espera.cabeca != NULL) {
            pedido_para_montar = removerListaPedidos_front(&local_montagem->fila_espera);
        }

        if (pedido_para_montar == NULL) {
            liberarFuncionario(montador, NULL, reserva);
            continue;
        }
        
        int tempo_montagem = 30;
        for (int i = 0; i < pedido_para_montar->pedido.num_itens; i++) {
            if (pedido_para_montar->pedido.itens[i].nome == REFRIGERANTE) {
                tempo_montagem += 5;
                break;
            }
        }
        pedido_para_montar->pedido.tempo_preparo_local = tempo_montagem;
        montador->funcionario.pedido_trabalhado = pedido_para_montar;
        montador->funcionario.local_atual = MONTAR_BANDEJAS;
        adicionarListaFuncionario(&local_montagem->funcionario, montador);
        adicionarListaPedidos(&local_montagem->pedido_sendo_feitos, pedido_para_montar);
        printf("MONTAGEM: Funcionario %d iniciou a montagem do Pedido %d (duracao: %ds).\n", 
            montador->funcionario.id, pedido_para_montar->pedido.id, tempo_montagem);
    }
}


/*----------------- Funções de Funcionários -----------------*/
ListaFuncionarios criarListaFuncionarios() {
    ListaFuncionarios l;
    l.cabeca = l.cauda = NULL;
    l.quantidade = 0;
    return l;
}

static NodeFuncionario* criarFuncionario(int id, Habilidade hab1, Habilidade hab2, Habilidade hab3) {
    NodeFuncionario* novo_node = (NodeFuncionario*) malloc(sizeof(NodeFuncionario));
    if (!novo_node) return NULL;
    Funcionario f;
    f.id = id;
    f.num_habilidade = 0;
    if (hab1 != NADA) f.habilidades[f.num_habilidade++] = hab1;
    if (hab2 != NADA) f.habilidades[f.num_habilidade++] = hab2;
    if (hab3 != NADA) f.habilidades[f.num_habilidade++] = hab3;
    f.status = LIVRE;
    f.local_atual = RESERVA;
    f.pedido_trabalhado = NULL;
    novo_node->funcionario = f;
    novo_node->ante = NULL;
    novo_node->prox = NULL;
    return novo_node;
}

void inicializarFuncionarios(ListaFuncionarios *lista) {
    printf("Inicializando equipe de funcionarios...\n");
    adicionarListaFuncionario(lista, criarFuncionario(1, HABILIDADE_CAIXA, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(2, HABILIDADE_CAIXA, HABILIDADE_BEBIDA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(3, HABILIDADE_SEPARACAO, HABILIDADE_SANDUICHE, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(4, HABILIDADE_SEPARACAO, HABILIDADE_CAIXA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(5, HABILIDADE_SANDUICHE, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(6, HABILIDADE_SANDUICHE, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(7, HABILIDADE_SANDUICHE, HABILIDADE_BATATA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(8, HABILIDADE_SANDUICHE, HABILIDADE_BATATA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(9, HABILIDADE_SANDUICHE, HABILIDADE_SUCO, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(10, HABILIDADE_BATATA, HABILIDADE_SANDUICHE, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(11, HABILIDADE_BATATA, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(12, HABILIDADE_BEBIDA, HABILIDADE_MONTAGEM, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(13, HABILIDADE_MONTAGEM, NADA, NADA));
    printf("%d funcionarios contratados e na reserva!\n", lista->quantidade);
}

void adicionarListaFuncionario(ListaFuncionarios *lista, NodeFuncionario *novo_node) {
    if (!lista || !novo_node) return;
    novo_node->prox = NULL;
    novo_node->ante = lista->cauda;
    if (lista->cauda) lista->cauda->prox = novo_node;
    lista->cauda = novo_node;
    if (!lista->cabeca) lista->cabeca = novo_node;
    lista->quantidade++;
}

NodeFuncionario* removerNodeFuncionario(ListaFuncionarios *lista, NodeFuncionario *node_para_remover) {
    if (!lista || !node_para_remover) return NULL;
    if (node_para_remover->ante) node_para_remover->ante->prox = node_para_remover->prox;
    else lista->cabeca = node_para_remover->prox;
    if (node_para_remover->prox) node_para_remover->prox->ante = node_para_remover->ante;
    else lista->cauda = node_para_remover->ante;
    node_para_remover->ante = NULL;
    node_para_remover->prox = NULL;
    lista->quantidade--;
    return node_para_remover;
}

NodeFuncionario* encontrarEAlocarFuncionario(ListaFuncionarios *reserva, Habilidade habilidade) {
    NodeFuncionario *atual = reserva->cabeca;
    while (atual) {
        if (atual->funcionario.status == LIVRE) {
            for (int i = 0; i < atual->funcionario.num_habilidade; i++) {
                if (atual->funcionario.habilidades[i] == habilidade) {
                    atual->funcionario.status = OCUPADO;
                    return removerNodeFuncionario(reserva, atual);
                }
            }
        }
        atual = atual->prox;
    }
    return NULL;
}

void liberarFuncionario(NodeFuncionario *node, ListaFuncionarios *origem, ListaFuncionarios *destino_reserva) {
    if (!node || !destino_reserva) return;
    if (origem) {
        removerNodeFuncionario(origem, node);
    }
    printf("INFO: Funcionario %d finalizou a tarefa e voltou para a reserva.\n", node->funcionario.id);
    node->funcionario.status = LIVRE;
    node->funcionario.local_atual = RESERVA;
    node->funcionario.pedido_trabalhado = NULL;
    adicionarListaFuncionario(destino_reserva, node);
}

/*----------------- Funções de Estoque -----------------*/
ListaItensArmazenados criarListaArmazenados() {
    ListaItensArmazenados l;
    l.cabeca = l.cauda = NULL;
    l.quantidade = 0;
    return l;
}

NodeItemArmazenado* criarNodeItemArmazenado(ItemArmazenado item) {
    NodeItemArmazenado *n = (NodeItemArmazenado*) malloc(sizeof(NodeItemArmazenado));
    if (!n) return NULL;
    n->item = item;
    n->ante = n->prox = NULL;
    return n;
}

void adicionarItemArmazenado(ListaItensArmazenados *lista, NodeItemArmazenado *novo_node) {
    if (!lista || !novo_node) return;
    novo_node->prox = NULL;
    novo_node->ante = lista->cauda;
    if (lista->cauda) lista->cauda->prox = novo_node;
    lista->cauda = novo_node;
    if (!lista->cabeca) lista->cabeca = novo_node;
    lista->quantidade++;
}

void removerNodeArmazenado(ListaItensArmazenados *lista, NodeItemArmazenado *node) {
    if (!lista || !node) return;
    if (node->ante) node->ante->prox = node->prox;
    else lista->cabeca = node->prox;
    if (node->prox) node->prox->ante = node->ante;
    else lista->cauda = node->ante;
    lista->quantidade--;
    free(node);
}

void imprimirListaArmazenados(ListaItensArmazenados lista) {
    NodeItemArmazenado *atual = lista.cabeca;
    if (!atual) {
        printf("  (Estoque vazio)\n");
        return;
    }
    while (atual) {
        printf("  -> Item: ");
        imprimirNomeDoItem(atual->item.nome);
        printf(" | Valido ate t=%d\n", atual->item.tempo_expiracao);
        atual = atual->prox;
    }
}

void verificarEstoqueExpirado(ListaItensArmazenados *estoque, int tempo_atual) {
    NodeItemArmazenado *atual = estoque->cabeca;
    while (atual != NULL) {
        NodeItemArmazenado *proximo = atual->prox;
        if (tempo_atual > atual->item.tempo_expiracao) {
            printf("ESTOQUE: Item '");
            imprimirNomeDoItem(atual->item.nome);
            printf("' expirou e foi descartado (expirou em t=%d).\n", atual->item.tempo_expiracao);
            removerNodeArmazenado(estoque, atual);
        }
        atual = proximo;
    }
}

/*----------------- Lógica da Lanchonete -----------------*/
void atenderFilaRecepcao(Locais *local_recepcao, ListaFuncionarios *reserva) {
    while (local_recepcao->heap.quantidade > 0 || local_recepcao->fila_espera.cabeca != NULL) {
        NodeFuncionario *caixa = encontrarEAlocarFuncionario(reserva, HABILIDADE_CAIXA);
        if (caixa == NULL) {
            break;
        }

        NodePedido *pedido_para_atender = NULL;

        if (local_recepcao->heap.quantidade > 0) {
            printf("RECEPCAO (FILA): Pegando pedido prioritario (ID: %d) do heap.\n", local_recepcao->heap.pedidos[0].id);
            Pedido p = removerDoHeap(&local_recepcao->heap);
            if (p.id != -1) {
                pedido_para_atender = criarNodePedido(p);
            }
        }
        else if (local_recepcao->fila_espera.cabeca != NULL) {
            pedido_para_atender = removerListaPedidos_front(&local_recepcao->fila_espera);
        }

        if (pedido_para_atender == NULL) {
            liberarFuncionario(caixa, NULL, reserva);
            continue;
        }
        
        printf("RECEPCAO (FILA): Funcionario %d iniciou o atendimento do Pedido %d (duracao: 30s).\n", 
               caixa->funcionario.id, pedido_para_atender->pedido.id);
        
        pedido_para_atender->pedido.tempo_preparo_local = 30;
        caixa->funcionario.pedido_trabalhado = pedido_para_atender;
        caixa->funcionario.local_atual = RECEPCAO;
        adicionarListaFuncionario(&local_recepcao->funcionario, caixa);
        adicionarListaPedidos(&local_recepcao->pedido_sendo_feitos, pedido_para_atender);
    }
}

void processarRecepcao(Locais *local_recepcao, Locais *local_separador, ListaFuncionarios *reserva, int ciclo) {
    NodeFuncionario *func_atual = local_recepcao->funcionario.cabeca;
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;
        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_atendido = func_atual->funcionario.pedido_trabalhado;
            pedido_atendido->pedido.tempo_preparo_local -= ciclo;
            if (pedido_atendido->pedido.tempo_preparo_local <= 0) {
                printf("RECEPCAO: Atendimento do Pedido %d concluido. Movendo para a separacao.\n", pedido_atendido->pedido.id);
                removerNodePedido(&local_recepcao->pedido_sendo_feitos, pedido_atendido);
                func_atual->funcionario.pedido_trabalhado = NULL;
                adicionarListaPedidos(&local_separador->fila_espera, pedido_atendido);
                liberarFuncionario(func_atual, &local_recepcao->funcionario, reserva);
            }
        }
        func_atual = proximo_func;
    }
}

void distribuirItens(NodePedido *pedido_node, Equipamento equipamentos[], ListaItensArmazenados *estoque) {
    if (!pedido_node) return;
    printf("SEPARADOR: Distribuindo itens do Pedido %d...\n", pedido_node->pedido.id);
    for (int i = 0; i < pedido_node->pedido.num_itens; i++) {
        ItemPedido *item_original = &pedido_node->pedido.itens[i];
        if (item_original->nome == NADA) continue;

        if (item_original->nome == REFRIGERANTE) {
            item_original->status = PRONTO;
            printf("  -> Item 'Refrigerante' (Pedido %d) e instantaneo.\n", pedido_node->pedido.id);
            continue;
        }

        bool encontrado_no_estoque = false;
        if (item_original->nome == BATATA_FRITA || item_original->nome == SUCO || item_original->nome == MILK_SHAKE) {
            NodeItemArmazenado *item_em_estoque = estoque->cabeca;
            while (item_em_estoque != NULL) {
                if (item_em_estoque->item.nome == item_original->nome) {
                    printf("OTIMIZACAO: Item '");
                    imprimirNomeDoItem(item_original->nome);
                    printf("' retirado do estoque de itens prontos para o Pedido %d.\n", pedido_node->pedido.id);
                    
                    item_original->status = PRONTO;
                    removerNodeArmazenado(estoque, item_em_estoque);
                    encontrado_no_estoque = true;
                    break;
                }
                item_em_estoque = item_em_estoque->prox;
            }
        }
        if (encontrado_no_estoque) {
            continue;
        }

        ItemPreparo item_para_fila;
        item_para_fila.nome = item_original->nome;
        item_para_fila.tempo_preparo_total = calcularPreparo(item_original->nome);
        item_para_fila.tempo_restante_preparo = calcularPreparo(item_original->nome);
        item_para_fila.status = AGUARDANDO_PREPARADO;
        item_para_fila.pedido_pai = &pedido_node->pedido;
        item_para_fila.item_original = item_original;
        NodeItemPreparo *node_item = criarNodeItemPreparo(item_para_fila);
        
        NomeEquipamento destino;
        switch (item_original->nome) {
            case SANDUICHE_SIMPLES: case SANDUICHE_MEDIO: case SANDUICHE_ELABORADO: destino = CHAPA; break;
            case BATATA_FRITA: destino = PENEIRA; break;
            case MILK_SHAKE: destino = LIQUIDIFICADOR_MILK_SHAKE; break;
            case SUCO: destino = LIQUIDIFICADOR_SUCO; break;
            default: free(node_item); continue;
        }
        adicionarListaItemPreparo(&equipamentos[destino].fila_espera, node_item);
        printf("  -> Item '");
        imprimirNomeDoItem(item_original->nome);
        printf("' (Pedido %d) enviado para a fila de producao.\n", pedido_node->pedido.id);
    }
}

void processarSeparador(Locais *local_separador, Equipamento equipamentos[], ListaFuncionarios *reserva, ListaPedidos *pedidos_em_preparo, int ciclo, ListaItensArmazenados *estoque) {
    NodeFuncionario *func_atual = local_separador->funcionario.cabeca;
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;
        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_sendo_separado = func_atual->funcionario.pedido_trabalhado;
            pedido_sendo_separado->pedido.tempo_preparo_local -= ciclo;
            if (pedido_sendo_separado->pedido.tempo_preparo_local <= 0) {
                printf("SEPARADOR: Funcionario %d finalizou a separacao do Pedido %d.\n", func_atual->funcionario.id, pedido_sendo_separado->pedido.id);
                distribuirItens(pedido_sendo_separado, equipamentos, estoque);
                pedido_sendo_separado->pedido.status = PREPARANDO_ITENS;
                removerNodePedido(&local_separador->pedido_sendo_feitos, pedido_sendo_separado);
                func_atual->funcionario.pedido_trabalhado = NULL;
                adicionarListaPedidos(pedidos_em_preparo, pedido_sendo_separado);
                liberarFuncionario(func_atual, &local_separador->funcionario, reserva);
            }
        }
        func_atual = proximo_func;
    }
}

void separador(Locais *local_separador, ListaFuncionarios *reserva, ListaItensArmazenados *estoque) {
    while (local_separador->heap.quantidade > 0 || local_separador->fila_espera.cabeca != NULL) {
        NodeFuncionario *separador_func = encontrarEAlocarFuncionario(reserva, HABILIDADE_SEPARACAO);
        if(separador_func == NULL) break;

        NodePedido *pedido_para_separar = NULL;

        if (local_separador->heap.quantidade > 0) {
            printf("SEPARADOR: Pegando pedido prioritario (ID: %d) do heap.\n", local_separador->heap.pedidos[0].id);
            Pedido p = removerDoHeap(&local_separador->heap);
            if (p.id != -1) {
                pedido_para_separar = criarNodePedido(p);
            }
        }
        else if (local_separador->fila_espera.cabeca != NULL) {
            pedido_para_separar = removerListaPedidos_front(&local_separador->fila_espera);
        }

        if (pedido_para_separar == NULL) {
             liberarFuncionario(separador_func, NULL, reserva);
             continue;
        }

        printf("SEPARADOR: Funcionario %d iniciou a separacao do Pedido %d (duracao: 30s).\n", 
               separador_func->funcionario.id, pedido_para_separar->pedido.id);
        pedido_para_separar->pedido.tempo_preparo_local = 30;
        separador_func->funcionario.pedido_trabalhado = pedido_para_separar;
        separador_func->funcionario.local_atual = SEPARADOR;
        adicionarListaFuncionario(&local_separador->funcionario, separador_func);
        adicionarListaPedidos(&local_separador->pedido_sendo_feitos, pedido_para_separar);
    }
}

void processarEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva, int ciclo, ListaItensArmazenados *estoque, int tempo_simulacao_atual) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < equipamentos[i].capacidade_maxima; j++) {
            if (equipamentos[i].itens_em_preparo[j].nome != NADA) {
                ItemPreparo *item = &equipamentos[i].itens_em_preparo[j];
                item->tempo_restante_preparo -= ciclo;
                if (item->tempo_restante_preparo <= 0) {
                    printf("EQUIPAMENTO %d: Item '", i);
                    imprimirNomeDoItem(item->nome);
                    printf("' (Pedido %d) ficou PRONTO.\n", item->pedido_pai->id);
                    
                    bool armazenavel = false;
                    int validade = 0;
                    switch(item->nome) {
                        case BATATA_FRITA: armazenavel = true; validade = 360; break;
                        case MILK_SHAKE: armazenavel = true; validade = 270; break;
                        case SUCO: armazenavel = true; validade = 270; break;
                        default: break;
                    }

                    if (armazenavel) {
                        ItemArmazenado novo_item;
                        novo_item.nome = item->nome;
                        novo_item.tempo_expiracao = tempo_simulacao_atual + validade;
                        adicionarItemArmazenado(estoque, criarNodeItemArmazenado(novo_item));
                        printf("ESTOQUE: Item '");
                        imprimirNomeDoItem(item->nome);
                        printf("' adicionado ao estoque (valido ate t=%d).\n", novo_item.tempo_expiracao);
                    }
                    item->item_original->status = PRONTO;
                    liberarFuncionario(equipamentos[i].funcionarios_alocados[j], NULL, reserva);
                    equipamentos[i].funcionarios_alocados[j] = NULL;
                    item->nome = NADA;
                    equipamentos[i].capacidade_usada--;
                }
            }
        }
    }
}

/*----------------- Funções de Gerenciamento de Equipamentos -----------------*/
static void gerenciarEquipamentoEspecifico(Equipamento *equip, Habilidade habilidade_necessaria, ListaFuncionarios *reserva) {
    while (equip->capacidade_usada < equip->capacidade_maxima && equip->fila_espera.cabeca != NULL) {
        NodeFuncionario *operador = encontrarEAlocarFuncionario(reserva, habilidade_necessaria);
        if (operador == NULL) {
            break; 
        }

        NodeItemPreparo *item_node = removerListaItemPreparo_front(&equip->fila_espera);
        
        for (int j = 0; j < equip->capacidade_maxima; j++) {
            if (equip->itens_em_preparo[j].nome == NADA) {
                equip->itens_em_preparo[j] = item_node->item;
                equip->funcionarios_alocados[j] = operador;
                equip->capacidade_usada++;
                item_node->item.item_original->status = EM_PREPARO;

                printf("EQUIPAMENTO %d: Funcionario %d iniciou preparo do item '", equip->nome, operador->funcionario.id);
                imprimirNomeDoItem(item_node->item.nome);
                printf("' (Pedido %d).\n", item_node->item.pedido_pai->id);
                
                free(item_node);
                break; 
            }
        }
    }
}

void gerenciarChapa(Equipamento *chapa, ListaFuncionarios *reserva) {
    gerenciarEquipamentoEspecifico(chapa, HABILIDADE_SANDUICHE, reserva);
}

void gerenciarPeneira(Equipamento *peneira, ListaFuncionarios *reserva) {
    gerenciarEquipamentoEspecifico(peneira, HABILIDADE_BATATA, reserva);
}

void gerenciarLiquidificador(Equipamento *liquidificador_suco, Equipamento *liquidificador_milkshake, ListaFuncionarios *reserva) {
    gerenciarEquipamentoEspecifico(liquidificador_suco, HABILIDADE_SUCO, reserva);
    gerenciarEquipamentoEspecifico(liquidificador_milkshake, HABILIDADE_BEBIDA, reserva);
}

void gerenciarTodosEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva) {
    gerenciarPeneira(&equipamentos[PENEIRA], reserva);
    gerenciarChapa(&equipamentos[CHAPA], reserva);
    gerenciarLiquidificador(&equipamentos[LIQUIDIFICADOR_SUCO], &equipamentos[LIQUIDIFICADOR_MILK_SHAKE], reserva);
}

int calcularPreparo(NomePedido pedido) {
    switch (pedido) {
        case BATATA_FRITA: return 190;
        case SANDUICHE_SIMPLES: return 58;
        case SANDUICHE_MEDIO: return 88;
        case SANDUICHE_ELABORADO: return 105;
        case REFRIGERANTE: return 5;
        case SUCO: return 38;
        case MILK_SHAKE: return 60;
        case NADA: return 0;
        default: return 0;
    }
}

NodePedido* receberPedido(bool res_auto, int *novo_id, int tempo_simulacao_atual) {
    Pedido p;
    memset(&p, 0, sizeof(Pedido));
    p.id = next_pedido_id++;
    p.status = RECEBIDO;
    p.num_itens = 0;
    
    p.tempo_chegada = tempo_simulacao_atual;
    printf("\n----------- CRIAÇÃO DE NOVO PEDIDO (t=%d) -----------\n", tempo_simulacao_atual);
    printf("Escolha ate 4 itens (2 de comer e 2 de beber)\n");
    
    for (int i = 0; i < 2; i++) {
        printf("\n--- Escolhendo o combo de itens (%d/2) ---\n", i + 1);

        if (p.num_itens < 4) {
            printf("\n  ==== Itens de Comer ====\n");
            printf("  1-Sanduiche Simples, 2-Sanduiche Medio, 3-Sanduiche Elaborado, 4-Batata Frita, 5-Nada\n");
            printf("  Resposta: ");
            int item_comer = receberInput(res_auto, 5, 5);
            if (item_comer != 5) {
                NomePedido np = (NomePedido)(item_comer - 1);
                p.itens[p.num_itens].nome = np;
                p.itens[p.num_itens].tempo_preparo_total = calcularPreparo(np);
                p.itens[p.num_itens].status = AGUARDANDO_PREPARADO;
                p.num_itens++;
            }
        }

        if (p.num_itens < 4) {
            printf("\n  ==== Itens de Beber ====\n");
            printf("  1-Refrigerante, 2-Milk-Shake, 3-Suco, 4-Nada\n");
            printf("  Resposta: ");
            int item_beber = receberInput(res_auto, 4, 4);
            if (item_beber != 4) {
                NomePedido np = (NomePedido)(item_beber + 4 - 1);
                p.itens[p.num_itens].nome = np;
                p.itens[p.num_itens].tempo_preparo_total = calcularPreparo(np);
                p.itens[p.num_itens].status = AGUARDANDO_PREPARADO;
                p.num_itens++;
            }
        }

        if (p.num_itens >= 4) {
            break;
        }

        if (!res_auto && i == 0) {
            printf("\nDeseja adicionar mais um combo de itens (1-Sim, 2-Nao)? ");
            if (receberInput(false, 2, 2) == 2) {
                break;
            }
        }
    }
    
    int soma = 0;
    for (int i = 0; i < p.num_itens; ++i) soma += p.itens[i].tempo_preparo_total;
    p.tempo_preparo_local = 30;
    p.tempo_preparo_total = soma;
    p.tempo_restante_preparo = soma;
    printf("----------------------------------------------\n");
    NodePedido *node = criarNodePedido(p);
    if (novo_id) *novo_id = p.id;
    return node;
}


void recepcao(Locais *r, bool res_auto, ListaFuncionarios *reserva, ListaItensArmazenados *estoque, int timer_global, int tempo_simulacao_atual) {
    NodeFuncionario *caixa = encontrarEAlocarFuncionario(reserva, HABILIDADE_CAIXA);
    if (caixa == NULL) {
        printf("RECEPCAO: Nenhum caixa disponivel. Novo pedido sera adicionado a fila de espera.\n");
        int novo_id = 0;
        NodePedido* novo_pedido = receberPedido(res_auto, &novo_id, tempo_simulacao_atual);
        if (novo_pedido && novo_pedido->pedido.num_itens > 0) {
            adicionarListaPedidos(&r->fila_espera, novo_pedido);
            printf("RECEPCAO: Pedido %d adicionado a fila de espera.\n", novo_id);
        } else {
            printf("RECEPCAO: Pedido vazio ou falha ao criar. Nenhuma acao tomada.\n");
            if (novo_pedido) free(novo_pedido);
        }
        return; 
    }
    printf("RECEPCAO: Funcionario %d iniciou o atendimento.\n", caixa->funcionario.id);
    caixa->funcionario.local_atual = RECEPCAO;
    adicionarListaFuncionario(&r->funcionario, caixa);
    int novo_id = 0;
    NodePedido *novo_pedido = receberPedido(res_auto, &novo_id, tempo_simulacao_atual);
    if (!novo_pedido || novo_pedido->pedido.num_itens == 0) {
        printf("Pedido vazio ou falha ao criar. Devolvendo funcionario para reserva.\n");
        if (novo_pedido) free(novo_pedido);
        liberarFuncionario(caixa, &r->funcionario, reserva);
        return;
    }
    novo_pedido->pedido.tempo_preparo_local = 30;
    caixa->funcionario.pedido_trabalhado = novo_pedido;
    adicionarListaPedidos(&r->pedido_sendo_feitos, novo_pedido);
    printf("RECEPCAO: Pedido %d sendo processado (duracao: 30s).\n", novo_id);
}

void priorizarPedidosAntigos(Locais locais[], int tempo_simulacao_atual) {
    const char* nomes_locais[] = {"RECEPCAO", "MONTAR_BANDEJAS", "SEPARADOR", "CAIXA", "RESERVA"};
    NomeLocal locais_para_verificar[] = {RECEPCAO, SEPARADOR, MONTAR_BANDEJAS};
    int num_locais = 3;

    for (int i = 0; i < num_locais; i++) {
        Locais *local = &locais[locais_para_verificar[i]];
        NodePedido *atual = local->fila_espera.cabeca;

        while (atual != NULL) {
            NodePedido *proximo = atual->prox;
            int idade_pedido = tempo_simulacao_atual - atual->pedido.tempo_chegada;

            if (idade_pedido > PEDIDO_ANTIGO_THRESHOLD) {
                printf("ALERTA: Pedido ID:%d esta no local '%s' ha %d segundos! PRIORIZANDO AGORA.\n",
                       atual->pedido.id, nomes_locais[local->nome], idade_pedido);

                removerNodePedido(&local->fila_espera, atual);
                inserirPedido(&local->heap, atual->pedido);
                free(atual);
            }
            atual = proximo;
        }
    }
}

bool existemPedidosAtivos(Locais locais[], Equipamento equipamentos[], ListaPedidos *pedidos_em_preparo) {
    for (int i = 0; i < 5; i++) {
        if (locais[i].fila_espera.quantidade > 0) return true;
        if (locais[i].pedido_sendo_feitos.quantidade > 0) return true;
        if (locais[i].heap.quantidade > 0) return true;
    }

    for (int i = 0; i < 4; i++) {
        if (equipamentos[i].capacidade_usada > 0) return true;
        if (equipamentos[i].fila_espera.quantidade > 0) return true;
    }

    if (pedidos_em_preparo->quantidade > 0) return true;

    return false;
}

/*----------------- Funções de Limpeza de Memória -----------------*/
void liberarListaPedidos(ListaPedidos *lista) {
    NodePedido *atual = lista->cabeca;
    while (atual != NULL) {
        NodePedido *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    lista->cabeca = lista->cauda = NULL;
    lista->quantidade = 0;
}

void liberarListaFuncionarios(ListaFuncionarios *lista) {
    NodeFuncionario *atual = lista->cabeca;
    while (atual != NULL) {
        NodeFuncionario *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    lista->cabeca = lista->cauda = NULL;
    lista->quantidade = 0;
}

void liberarListaItensArmazenados(ListaItensArmazenados *lista) {
    NodeItemArmazenado *atual = lista->cabeca;
    while (atual != NULL) {
        NodeItemArmazenado *proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    lista->cabeca = lista->cauda = NULL;
    lista->quantidade = 0;
}