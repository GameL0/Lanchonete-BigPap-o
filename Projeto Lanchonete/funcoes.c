#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "funcoes.h"

/*----------------- Utilitários -----------------*/
void sair() {
    printf("\nPrograma finalizado!\n");
    exit(0);
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

int removerItemPrioritario(Heap* heap, Pedido* pedidoRemovido) {
    if (heap->quantidade == 0) return 0;
    *pedidoRemovido = heap->pedidos[0];
    heap->quantidade--;
    if (heap->quantidade > 0) {
        heap->pedidos[0] = heap->pedidos[heap->quantidade];
        descerNoHeap(heap, 0);
    }
    return 1;
}

void imprimirHeap(Heap* heap) {
    if (heap->quantidade == 0) {
        printf("Heap vazio\n");
        return;
    }
    printf("Heap[%d]:\n", heap->quantidade);
    for (int i = 0; i < heap->quantidade; ++i) {
        Pedido p = heap->pedidos[i];
        printf("[PRIORIDADE] PEDIDO ID:%d - TempoRestante:%d\n", p.id, p.tempo_restante_preparo);
    }
    printf("\n");
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
        printf("Lista de pedidos vazia.\n");
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
    printf("PEDIDO ID:%d - Itens: %d - TempoTotal:%d\n", 
           pedido.id, pedido.num_itens, pedido.tempo_preparo_total);
    for (int i = 0; i < pedido.num_itens; ++i) {
        printf("  - ");
        imprimirNomeDoItem(pedido.itens[i].nome);
        imprimirStatusItem(pedido.itens[i].status);
        printf(" (t:%d)\n", pedido.itens[i].tempo_preparo_total);
    }
}

/*----------------- Fila de Prioridade -----------------*/
void verificarEPriorizarPedidos(Locais *local, int tempo_atual) {
    NodePedido *atual = local->fila_espera.cabeca;
    while (atual != NULL) {
        NodePedido *proximo = atual->prox;
        if ((tempo_atual - atual->pedido.tempo_chegada) >= 240) { // 4 minutos
            printf("Pedido ID:%d movido para PRIORIDADE (esperou 4min)\n", atual->pedido.id);
            removerListaPedidos_front(&local->fila_espera);
            inserirPedido(&local->heap, atual->pedido);
            free(atual);
        }
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

// ======================== FILA DE MONTAGEM ========================

bool todosItensProntos(Pedido *pedido) {
    for (int i = 0; i < pedido->num_itens; i++) {
        // Ignora itens "NADA"
        if (pedido->itens[i].nome == NADA) continue;
        
        if (pedido->itens[i].status != PRONTO) {
            return false; // Encontrou um item que ainda não está pronto
        }
    }
    return true; // Todos os itens estão prontos
}

void verificarPedidosProntos(ListaPedidos *pedidos_em_preparo, Locais *local_montagem) {
    NodePedido *atual = pedidos_em_preparo->cabeca;
    while (atual != NULL) {
        NodePedido *proximo = atual->prox; // Guarda o próximo para o caso de remoção

        if (todosItensProntos(&atual->pedido)) {
            printf("MONTAGEM: Pedido %d esta com todos os itens prontos. Movendo para a fila de montagem.\n", atual->pedido.id);

            // Remove o pedido da lista de "em preparo"
            removerNodePedido(pedidos_em_preparo, atual);

            // Adiciona na fila de espera da montagem
            adicionarListaPedidos(&local_montagem->fila_espera, atual);
        }
        atual = proximo;
    }
}

void processarMontagem(Locais *local_montagem, ListaFuncionarios *reserva, ListaPedidos *pedidos_entregues, int ciclo) {
    NodeFuncionario *func_atual = local_montagem->funcionario.cabeca;
    
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;
        
        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_em_montagem = func_atual->funcionario.pedido_trabalhado;
            pedido_em_montagem->pedido.tempo_preparo_local -= ciclo;

            if (pedido_em_montagem->pedido.tempo_preparo_local <= 0) {
                // --- PEDIDO ENTREGUE! ---
                pedido_em_montagem->pedido.status = ENTREGUE;
                printf("ENTREGA: Pedido %d foi montado e ENTREGUE!\n", pedido_em_montagem->pedido.id);
                
                // Move o pedido para a lista de entregues
                adicionarListaPedidos(pedidos_entregues, pedido_em_montagem);

                // Libera o funcionário
                liberarFuncionario(func_atual, &local_montagem->funcionario, reserva);
            }
        }
        func_atual = proximo_func;
    }
}

void montarBandeja(Locais *local_montagem, ListaFuncionarios *reserva) {
    // Enquanto houver pedidos na fila e for possível alocar funcionários
    while (local_montagem->fila_espera.cabeca != NULL) {
        
        // Tenta encontrar um funcionário livre com a habilidade certa
        NodeFuncionario *montador = encontrarEAlocarFuncionario(reserva, HABILIDADE_MONTAGEM);

        if (montador == NULL) {
            break; // Se não há montador, não pode iniciar mais tarefas
        }

        // Pega o primeiro pedido da fila de espera
        NodePedido *pedido_para_montar = removerListaPedidos_front(&local_montagem->fila_espera);
        
        // Calcula o tempo de montagem
        int tempo_montagem = 30; // Tempo base
        for (int i = 0; i < pedido_para_montar->pedido.num_itens; i++) {
            if (pedido_para_montar->pedido.itens[i].nome == REFRIGERANTE) {
                tempo_montagem += 5; // Adiciona 5s para o refrigerante
                break; // Adiciona apenas uma vez
            }
        }
        pedido_para_montar->pedido.tempo_preparo_local = tempo_montagem;
        
        // Aloca a tarefa ao funcionário
        montador->funcionario.pedido_trabalhado = pedido_para_montar;
        montador->funcionario.local_atual = MONTAR_BANDEJAS;
        adicionarListaFuncionario(&local_montagem->funcionario, montador);

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

// Função auxiliar para criar um nó de funcionário
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

// Popula a lista de funcionários com a equipe inicial
void inicializarFuncionarios(ListaFuncionarios *lista) {
    printf("Inicializando equipe de funcionarios...\n");
    // 2 Caixas
    adicionarListaFuncionario(lista, criarFuncionario(1, HABILIDADE_CAIXA, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(2, HABILIDADE_CAIXA, NADA, NADA));
    
    // 2 Separadores
    adicionarListaFuncionario(lista, criarFuncionario(3, HABILIDADE_SEPARACAO, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(4, HABILIDADE_SEPARACAO, NADA, NADA));

    // 4 Chapistas (Sanduíches)
    adicionarListaFuncionario(lista, criarFuncionario(5, HABILIDADE_SANDUICHE, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(6, HABILIDADE_SANDUICHE, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(7, HABILIDADE_SANDUICHE, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(8, HABILIDADE_SANDUICHE, NADA, NADA));

    // 2 Fritadores (Batata)
    adicionarListaFuncionario(lista, criarFuncionario(9, HABILIDADE_BATATA, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(10, HABILIDADE_BATATA, NADA, NADA));

    // 2 Preparadores de Suco
    adicionarListaFuncionario(lista, criarFuncionario(11, HABILIDADE_SUCO, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(12, HABILIDADE_SUCO, NADA, NADA));

    // 2 Preparadores de Bebidas (Milk-shake/Refri)
    adicionarListaFuncionario(lista, criarFuncionario(13, HABILIDADE_BEBIDA, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(14, HABILIDADE_BEBIDA, NADA, NADA));

    // 2 Montadores
    adicionarListaFuncionario(lista, criarFuncionario(15, HABILIDADE_MONTAGEM, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(16, HABILIDADE_MONTAGEM, NADA, NADA));

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
    printf("Funcionario %d finalizou a tarefa e voltou para a reserva.\n", node->funcionario.id);
    node->funcionario.status = LIVRE;
    node->funcionario.local_atual = RESERVA;
    node->funcionario.pedido_trabalhado = NULL;
    adicionarListaFuncionario(destino_reserva, node);
}

/*----------------- Lógica da Lanchonete -----------------*/

void processarRecepcao(Locais *local_recepcao, Locais *local_separador, ListaFuncionarios *reserva, int ciclo) {
    NodeFuncionario *func_atual = local_recepcao->funcionario.cabeca;
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;
        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_atendido = func_atual->funcionario.pedido_trabalhado;
            pedido_atendido->pedido.tempo_preparo_local -= ciclo;

            if (pedido_atendido->pedido.tempo_preparo_local <= 0) {
                printf("RECEPCAO: Atendimento do Pedido %d concluido. Movendo para a separacao.\n", pedido_atendido->pedido.id);
                
                // Remove o pedido do funcionário e move para a fila do separador
                func_atual->funcionario.pedido_trabalhado = NULL;
                adicionarListaPedidos(&local_separador->fila_espera, pedido_atendido);
                
                liberarFuncionario(func_atual, &local_recepcao->funcionario, reserva);
            }
        }
        func_atual = proximo_func;
    }
}


void distribuirItens(NodePedido *pedido_node, Equipamento equipamentos[]) {
    if (!pedido_node) return;
    printf("SEPARADOR: Distribuindo itens do Pedido %d para as filas dos equipamentos.\n", pedido_node->pedido.id);

    for (int i = 0; i < pedido_node->pedido.num_itens; i++) {
        ItemPedido *item_original = &pedido_node->pedido.itens[i];
        if (item_original->nome == NADA) continue;

        if (item_original->nome == REFRIGERANTE) {
            item_original->status = PRONTO;
            printf("  -> Item 'Refrigerante' (Pedido %d) é instantâneo.\n", pedido_node->pedido.id);
            continue;
        }

        ItemPreparo item_para_fila;
        item_para_fila.nome = item_original->nome;
        item_para_fila.tempo_preparo_total = item_original->tempo_preparo_total;
        item_para_fila.tempo_restante_preparo = item_original->tempo_preparo_total;
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
        printf("' (Pedido %d) enviado para a fila do equipamento %d.\n", pedido_node->pedido.id, destino);
    }
}

void processarSeparador(Locais *local_separador, Equipamento equipamentos[], ListaFuncionarios *reserva, ListaPedidos *pedidos_em_preparo, int ciclo) {
    NodeFuncionario *func_atual = local_separador->funcionario.cabeca;
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;
        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_sendo_separado = func_atual->funcionario.pedido_trabalhado;
            pedido_sendo_separado->pedido.tempo_preparo_local -= ciclo;

            if (pedido_sendo_separado->pedido.tempo_preparo_local <= 0) {
                printf("SEPARADOR: Funcionario %d finalizou a separacao do Pedido %d.\n", func_atual->funcionario.id, pedido_sendo_separado->pedido.id);
                
                distribuirItens(pedido_sendo_separado, equipamentos);
                pedido_sendo_separado->pedido.status = PREPARANDO_ITENS;
                
                func_atual->funcionario.pedido_trabalhado = NULL;
                adicionarListaPedidos(pedidos_em_preparo, pedido_sendo_separado);
                
                liberarFuncionario(func_atual, &local_separador->funcionario, reserva);
            }
        }
        func_atual = proximo_func;
    }
}

void separador(Locais *local_separador, ListaFuncionarios *reserva) {
    if (local_separador->fila_espera.cabeca == NULL) return;

    NodeFuncionario *separador_func = encontrarEAlocarFuncionario(reserva, HABILIDADE_SEPARACAO);

    if (separador_func != NULL) {
        NodePedido *pedido_para_separar = removerListaPedidos_front(&local_separador->fila_espera);
        if (pedido_para_separar == NULL) {
            adicionarListaFuncionario(reserva, separador_func);
            separador_func->funcionario.status = LIVRE;
            return;
        }

        printf("SEPARADOR: Funcionario %d iniciou a separacao do Pedido %d (duracao: 30s).\n", 
               separador_func->funcionario.id, pedido_para_separar->pedido.id);

        pedido_para_separar->pedido.tempo_preparo_local = 30;
        separador_func->funcionario.pedido_trabalhado = pedido_para_separar;
        separador_func->funcionario.local_atual = SEPARADOR;
        adicionarListaFuncionario(&local_separador->funcionario, separador_func);
    }
}

void processarEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva, int ciclo) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < equipamentos[i].capacidade_maxima; j++) {
            if (equipamentos[i].itens_em_preparo[j].nome != NADA) {
                ItemPreparo *item = &equipamentos[i].itens_em_preparo[j];
                item->tempo_restante_preparo -= ciclo;

                if (item->tempo_restante_preparo <= 0) {
                    item->status = PRONTO;
                    item->item_original->status = PRONTO;

                    printf("EQUIPAMENTO %d: Item '", i);
                    imprimirNomeDoItem(item->nome);
                    printf("' (Pedido %d) ficou PRONTO.\n", item->pedido_pai->id);

                    liberarFuncionario(equipamentos[i].funcionarios_alocados[j], NULL, reserva);
                    equipamentos[i].funcionarios_alocados[j] = NULL;
                    
                    item->nome = NADA;
                    equipamentos[i].capacidade_usada--;
                }
            }
        }
    }
}

void operarEquipamentos(Equipamento equipamentos[], ListaFuncionarios *reserva) {
    for (int i = 0; i < 4; i++) {
        while (equipamentos[i].capacidade_usada < equipamentos[i].capacidade_maxima &&
            equipamentos[i].fila_espera.cabeca != NULL) {
            
            Habilidade habilidade_necessaria;
            switch (equipamentos[i].nome) {
                case CHAPA: habilidade_necessaria = HABILIDADE_SANDUICHE; break;
                case PENEIRA: habilidade_necessaria = HABILIDADE_BATATA; break;
                case LIQUIDIFICADOR_SUCO: habilidade_necessaria = HABILIDADE_SUCO; break;
                case LIQUIDIFICADOR_MILK_SHAKE: habilidade_necessaria = HABILIDADE_BEBIDA; break;
                default: habilidade_necessaria = NADA; break; // Adicionado para segurança
            }

            NodeFuncionario *operador = encontrarEAlocarFuncionario(reserva, habilidade_necessaria);
            if (operador == NULL) break;

            NodeItemPreparo *item_node = removerListaItemPreparo_front(&equipamentos[i].fila_espera);
            
            for (int j = 0; j < equipamentos[i].capacidade_maxima; j++) {
                if (equipamentos[i].itens_em_preparo[j].nome == NADA) {
                    equipamentos[i].itens_em_preparo[j] = item_node->item;
                    equipamentos[i].funcionarios_alocados[j] = operador;
                    equipamentos[i].capacidade_usada++;
                    
                    item_node->item.item_original->status = EM_PREPARO;
                    
                    printf("EQUIPAMENTO %d: Funcionario %d iniciou preparo do item '", i, operador->funcionario.id);
                    imprimirNomeDoItem(item_node->item.nome);
                    printf("' (Pedido %d).\n", item_node->item.pedido_pai->id);

                    free(item_node);
                    break; 
                }
            }
        }
    }
}

static int next_pedido_id = 1;

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

NodePedido* receberPedido(bool res_auto, int *novo_id) {
    Pedido p;
    memset(&p, 0, sizeof(Pedido));
    p.id = next_pedido_id++;
    p.status = RECEBIDO;
    p.num_itens = 0;
    p.tempo_chegada = (int) time(NULL);

    printf("\nEscolha ate 2 itens (1 de comer e 1 de beber)\n");

    printf("\n==== Itens de Comer ====\n");
    printf(" 1-Sanduiche Simples, 2-Sanduiche Medio, 3-Sanduiche Elaborado, 4-Batata Frita, 5-Nada\n");
    printf("Resposta: ");
    int item_comer = receberInput(res_auto, 5, 5);
    if (item_comer != 5) {
        NomePedido np = (NomePedido)(item_comer - 1);
        p.itens[p.num_itens].nome = np;
        p.itens[p.num_itens].tempo_preparo_total = calcularPreparo(np);
        p.itens[p.num_itens].status = AGUARDANDO_PREPARADO;
        p.num_itens++;
    }

    printf("\n==== Itens de Beber ====\n");
    printf("1-Refrigerante, 2-Milk-Shake, 3-Suco, 4-Nada\n");
    printf("Resposta: ");
    int item_beber = receberInput(res_auto, 4, 4);
    if (item_beber != 4) {
        NomePedido np = (NomePedido)(item_beber + 4 - 1);
        p.itens[p.num_itens].nome = np;
        p.itens[p.num_itens].tempo_preparo_total = calcularPreparo(np);
        p.itens[p.num_itens].status = AGUARDANDO_PREPARADO;
        p.num_itens++;
    }

    int soma = 0;
    for (int i = 0; i < p.num_itens; ++i) soma += p.itens[i].tempo_preparo_total;
    p.tempo_preparo_total = soma;
    p.tempo_restante_preparo = soma;

    NodePedido *node = criarNodePedido(p);
    if (novo_id) *novo_id = p.id;
    return node;
}

void recepcao(Locais *r, bool res_auto, ListaFuncionarios *reserva) {
    NodeFuncionario *caixa = encontrarEAlocarFuncionario(reserva, HABILIDADE_CAIXA);
    if (caixa == NULL) {
        printf("RECEPCAO: Nenhum caixa disponivel.\n");
        return; 
    }
    printf("RECEPCAO: Funcionario %d iniciou o atendimento.\n", caixa->funcionario.id);
    
    caixa->funcionario.local_atual = RECEPCAO;
    adicionarListaFuncionario(&r->funcionario, caixa);

    int novo_id = 0;
    NodePedido *novo_pedido = receberPedido(res_auto, &novo_id);
    if (!novo_pedido || novo_pedido->pedido.num_itens == 0) {
        printf("Pedido vazio ou falha ao criar. Devolvendo funcionario para reserva.\n");
        if (novo_pedido) free(novo_pedido);
        liberarFuncionario(caixa, &r->funcionario, reserva);
        return;
    }
    
    novo_pedido->pedido.tempo_preparo_local = 30; // 30s para atendimento
    caixa->funcionario.pedido_trabalhado = novo_pedido;
    
    printf("Pedido %d sendo processado na recepcao (duracao: 30s).\n", novo_id);
}