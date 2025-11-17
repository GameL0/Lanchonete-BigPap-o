#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "funcoes.h" 
#include <limits.h>

#define PEDIDO_ANTIGO_THRESHOLD 180 
#define LIMITE_POSTERGAR_PEDIDO 280 
#define LIMITE_TEMPO_PEDIDO 300 
#define MIN_ESTOQUE_PROATIVO 2 
#define MAX_PEDIDOS_EM_PRODUCAO 5 

static int next_pedido_id = 1; 

void sair() {
    printf("\n[SISTEMA] Programa finalizado!\n");
    exit(0);
}

int receberInput(int opc_manual) {
    int var_control;
    if (scanf("%d", &var_control) != 1) {
        int c; while ((c = getchar()) != '\n' && c != EOF) {}
        return receberInput(opc_manual);
    }
    if (var_control > 0 && var_control <= opc_manual)
        return var_control;
    printf("\n[ATENÇÃO] Resposta inválida! Tente Novamente.\n");
    return receberInput(opc_manual);
}

int fazerPedido() {
    printf("\n+-------------------------------------------------------------+\n");
    printf("|  BEM VINDO AO BIG PAPÃO, O SANDUICHE MAIS RAPIDO DO NORDESTE!!  |\n");
    printf("+-------------------------------------------------------------+\n");
    printf("Queres fazer a sua escolha?\n");
    printf(" 1 - Ler Pedido do arquivo ('entrada.txt')\n");
    printf(" 2 - Avancar ciclo (Nao ler pedido)\n");
    printf(" 3 - Fechar o programa\n");
    printf("Resposta: ");
    return receberInput(3);
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
        printf("  [Heap] Heap cheio: não foi possível inserir pedido (ID: %d).\n", novoPedido.id);
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

void imprimirHeap(Heap* heap, int restaurante_id, const char* local_nome) {
    printf("  [R%d] Fila PRIORIDADE %s (%d pedidos):\n", restaurante_id, local_nome, heap->quantidade);
    if (heap->quantidade == 0) {
        printf("    (Fila de prioridade vazia)\n");
        return;
    }
    for (int i = 0; i < heap->quantidade; ++i) {
        Pedido p = heap->pedidos[i];
        const char* tipo_str = (p.tipo == IFOOD) ? "(IFOOD)" : "(PRESENCIAL)";
        printf("    -> Pedido ID:%d %s (Itens: %d, TempoRest: %ds)\n", 
            p.id, tipo_str, p.num_itens, p.tempo_restante_preparo);
    }
}

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

NodePedido* removerPedidoPorPrioridade(Locais *local, const char** origem_fila) {
    if (local->heap.quantidade > 0) {
        Pedido p = removerDoHeap(&local->heap);
        if (p.id != -1) {
            *origem_fila = "HEAP (Prioridade)";
            return criarNodePedido(p);
        }
    } 
    
    NodePedido *atual = local->fila_espera.cabeca;
    while (atual != NULL) {
        if (atual->pedido.tipo == IFOOD) {
            *origem_fila = "iFood (Fila Normal)";
            removerNodePedido(&local->fila_espera, atual);
            return atual;
        }
        atual = atual->prox;
    }

    if (local->fila_espera.cabeca != NULL) {
        *origem_fila = "Presencial (Fila Normal)";
        return removerListaPedidos_front(&local->fila_espera);
    }

    *origem_fila = "Nenhum";
    return NULL;
}

void imprimirLista(ListaPedidos lista, int restaurante_id, const char* label) {
    printf("\n[R%d] %s (%d pedidos):\n", restaurante_id, label, lista.quantidade);
    NodePedido *atual = lista.cabeca;
    if (!atual) {
        printf("    (Lista vazia)\n");
        return;
    }
    while (atual) {
        imprimirPedido(atual->pedido);
        atual = atual->prox;
    }
}

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
    const char* tipo_str = (pedido.tipo == IFOOD) ? "iFood" : "Presencial";
    printf("    -> Pedido ID:%d (Senha: %d, Tipo: %s, Chegou: %ds, Duracao: %ds)\n", 
        pedido.id, pedido.id_senha, tipo_str, pedido.tempo_chegada, pedido.tempo_preparo_total);
}

void imprimirStatusEquipamentos(Restaurante *res) {
    const char* nomes_equipamentos[] = {"PENEIRA", "CHAPA", "LIQUIDIFICADOR_MILK_SHAKE", "LIQUIDIFICADOR_SUCO"};
    printf("\n[R%d] === EQUIPAMENTOS: ITENS SENDO FEITOS E FILAS ===\n", res->id);
    for (int i = 0; i < 4; i++) {
        if (i > 0) printf("\n"); 
        Equipamento equip = res->equipamentos[i];
        printf("  --- Equipamento: %s (Capacidade: %d/%d) ---\n", nomes_equipamentos[i], equip.capacidade_usada, equip.capacidade_maxima);
        
        bool itens_no_equip = false;
        for (int j = 0; j < equip.capacidade_maxima; j++) {
            ItemPreparo item_no_equip = equip.itens_em_preparo[j];
            if (item_no_equip.nome != NADA) {
                if (!itens_no_equip) {
                    printf("    Itens em Preparo (Slots):\n");
                    itens_no_equip = true;
                }
                printf("      -> Slot %d: ", j + 1);
                imprimirNomeDoItem(item_no_equip.nome);
                if (item_no_equip.pedido_pai != NULL) {
                    printf(" (Do Pedido ID: %d)", item_no_equip.pedido_pai->id);
                } else {
                    printf(" (Para Estoque)");
                }
                int tempo_restante = item_no_equip.tempo_restante_preparo > 0 ? item_no_equip.tempo_restante_preparo : 0;
                printf(" | Tempo Restante: %d s", tempo_restante);
                if (equip.funcionarios_alocados[j] != NULL) {
                        printf(" | Funcionario ID: %d", equip.funcionarios_alocados[j]->funcionario.id);
                }
                printf("\n");
            }
        }
        if (!itens_no_equip) {
            printf("    (Nenhum item em preparo)\n");
        }

        printf("\n"); 
        printf("    Fila de Espera (%d itens):\n", equip.fila_espera.quantidade);
        if (equip.fila_espera.cabeca == NULL) {
            printf("      (Fila de espera vazia)\n");
        } else {
            NodeItemPreparo* atual = equip.fila_espera.cabeca;
            while(atual != NULL) {
                printf("      -> ");
                imprimirNomeDoItem(atual->item.nome);
                if (atual->item.pedido_pai != NULL) {
                    printf(" (Do Pedido ID: %d)\n", atual->item.pedido_pai->id);
                } else {
                    printf(" (Para Estoque)\n");
                }
                atual = atual->prox;
            }
        }
    }
    printf("[R%d] ===============================================\n", res->id);
}

void imprimirFilaSeparacao(Restaurante *res) {
    Locais *local = &res->locais[SEPARADOR];
    
    printf("\n[R%d] === FILA DE SEPARACAO ===\n", res->id);
    imprimirLista(local->fila_espera, res->id, "Fila de Espera (iFood/Presencial)");
    
    if (local->heap.quantidade > 0) {
        imprimirHeap(&local->heap, res->id, "SEPARADOR");
    } else {
        printf("  [R%d] Fila PRIORIDADE SEPARADOR (vazia)\n", res->id);
    }
    
    imprimirLista(local->pedido_sendo_feitos, res->id, "Pedidos Sendo Separados Agora");
    printf("[R%d] =========================\n", res->id);
}

void imprimirFilasPrioridadeRestantes(Restaurante *res) {
    printf("\n[R%d] === OUTRAS FILAS DE PRIORIDADE ===\n", res->id);
    
    Locais *local_r = &res->locais[RECEPCAO];
    if(local_r->heap.quantidade > 0) {
        imprimirHeap(&local_r->heap, res->id, "RECEPCAO");
    } else {
        printf("  [R%d] Fila PRIORIDADE RECEPCAO (vazia)\n", res->id);
    }

    Locais *local_m = &res->locais[MONTAR_BANDEJAS];
    if(local_m->heap.quantidade > 0) {
        imprimirHeap(&local_m->heap, res->id, "MONTAR_BANDEJAS");
    } else {
        printf("  [R%d] Fila PRIORIDADE MONTAGEM (vazia)\n", res->id);
    }
    printf("[R%d] ================================\n", res->id);
}

void priorizarPedidosFimDeExpediente(Restaurante *res) {
    for (int i = 0; i < 5; i++) { 
        Locais *local = &res->locais[i];
        const char* nome_local_str = "DESCONHECIDO";
        if (i == RECEPCAO) nome_local_str = "RECEPCAO";
        else if (i == SEPARADOR) nome_local_str = "SEPARADOR";
        else if (i == MONTAR_BANDEJAS) nome_local_str = "MONTAGEM";
        
        NodePedido *atual = local->fila_espera.cabeca;
        while (atual != NULL) {
            NodePedido *proximo = atual->prox;
            const char* tipo_str = (atual->pedido.tipo == IFOOD) ? "iFood" : "Presencial";
            
            printf("[R%d] [ALERTA FIM EXPEDIENTE] Pedido %s ID:%d movido para o heap do local '%s'!\n", 
                res->id, tipo_str, atual->pedido.id, nome_local_str);
            
            removerNodePedido(&local->fila_espera, atual);
            inserirPedido(&local->heap, atual->pedido);
            free(atual);
            atual = proximo;
        }
    }
}

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

void removerNodeItemPreparo(ListaItemPreparo *lista, NodeItemPreparo *node) {
    if (!lista || !node) return;
    if (node->ante) node->ante->prox = node->prox;
    else lista->cabeca = node->prox;
    if (node->prox) node->prox->ante = node->ante;
    else lista->cauda = node->ante;
    node->prox = NULL;
    node->ante = NULL;
    lista->quantidade--;
    return;
}

bool todosItensProntos(Pedido *pedido) {
    for (int i = 0; i < pedido->num_itens; i++) {
        if (pedido->itens[i].nome == NADA) continue;
        if (pedido->itens[i].status != PRONTO) {
            return false;
        }
    }
    return true;
}

void verificarPedidosProntos(Restaurante *res) {
    Locais *local_montagem = &res->locais[MONTAR_BANDEJAS];
    
    NodePedido *atual = res->pedidos_em_preparo.cabeca;
    while (atual != NULL) {
        NodePedido *proximo = atual->prox;
        if (todosItensProntos(&atual->pedido)) {
            printf("[R%d] [COZINHA->MONTAGEM] Pedido %d (Itens Prontos) -> Fila de Montagem.\n", res->id, atual->pedido.id);
            removerNodePedido(&res->pedidos_em_preparo, atual);
            adicionarListaPedidos(&local_montagem->fila_espera, atual);
        }
        atual = proximo;
    }
}

void processarMontagem(Restaurante *res, int ciclo, int tempo_simulacao_atual) {
    Locais *local_montagem = &res->locais[MONTAR_BANDEJAS];
    
    NodeFuncionario *func_atual = local_montagem->funcionario.cabeca;
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;
        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_em_montagem = func_atual->funcionario.pedido_trabalhado;
            pedido_em_montagem->pedido.tempo_preparo_local -= ciclo;

            if (pedido_em_montagem->pedido.tempo_preparo_local <= 0) {
                pedido_em_montagem->pedido.status = ENTREGUE;
                
                res->stats.total_pedidos_entregues++;
                int tempo_total_do_pedido = tempo_simulacao_atual - pedido_em_montagem->pedido.tempo_chegada;
                
                printf("[R%d] [ENTREGA] Pedido %d (Senha %d) foi montado e ENTREGUE. Tempo total: %d segundos!\n", 
                    res->id, pedido_em_montagem->pedido.id, pedido_em_montagem->pedido.id_senha, tempo_total_do_pedido);

                if (tempo_total_do_pedido > 300) {
                    res->stats.pedidos_mais_de_5_min++;
                    printf("[R%d] [ATRASO] Pedido %d ultrapassou 5 minutos!\n", res->id, pedido_em_montagem->pedido.id);
                }

                removerNodePedido(&local_montagem->pedido_sendo_feitos, pedido_em_montagem);
                adicionarListaPedidos(&res->pedidos_entregues, pedido_em_montagem);
                
                func_atual->funcionario.pedido_trabalhado = NULL;
                liberarFuncionario(func_atual, &local_montagem->funcionario, &res->reserva);
            }
        }
        func_atual = proximo_func;
    }
}

void montarBandeja(Restaurante *res) {
    Locais *local_montagem = &res->locais[MONTAR_BANDEJAS];
    
    while (local_montagem->heap.quantidade > 0 || local_montagem->fila_espera.cabeca != NULL) {
        NodeFuncionario *montador = encontrarEAlocarFuncionario(&res->reserva, HABILIDADE_MONTAGEM);
        if (montador == NULL) break; 

        const char* origem_fila = "";
        NodePedido *pedido_para_montar = removerPedidoPorPrioridade(local_montagem, &origem_fila);

        if (pedido_para_montar == NULL) {
            liberarFuncionario(montador, NULL, &res->reserva); 
            break; 
        }
        
        int tempo_montagem = 25;
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
        
        printf("[R%d] [MONTAGEM] Func %d iniciou Pedido %d (Fila: %s, Duracao: %ds).\n", 
            res->id, montador->funcionario.id, pedido_para_montar->pedido.id, origem_fila, tempo_montagem);
    }
}

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

void inicializarFuncionarios(Restaurante *res) {
    ListaFuncionarios *lista = &res->reserva;
    printf("[SISTEMA] Inicializando equipe de funcionarios para Restaurante %d...\n", res->id);
    
    adicionarListaFuncionario(lista, criarFuncionario(1, HABILIDADE_SANDUICHE, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(2, HABILIDADE_SANDUICHE, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(3, HABILIDADE_SANDUICHE, HABILIDADE_BATATA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(4, HABILIDADE_SANDUICHE, HABILIDADE_BATATA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(5, HABILIDADE_SANDUICHE, HABILIDADE_SUCO, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(6, HABILIDADE_BATATA, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(7, HABILIDADE_BATATA, HABILIDADE_SANDUICHE, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(8, HABILIDADE_BEBIDA, HABILIDADE_MONTAGEM, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(9, HABILIDADE_MONTAGEM, NADA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(10, HABILIDADE_SEPARACAO, HABILIDADE_CAIXA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(11, HABILIDADE_SEPARACAO, HABILIDADE_SANDUICHE, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(12, HABILIDADE_CAIXA, HABILIDADE_BEBIDA, NADA));
    adicionarListaFuncionario(lista, criarFuncionario(13, HABILIDADE_CAIXA, NADA, NADA)); 

    printf("[SISTEMA] %d funcionarios contratados e na reserva (R%d)!\n", lista->quantidade, res->id);
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
    NodeFuncionario *melhor_candidato = NULL;
    int min_habilidades = INT_MAX; 

    while (atual) {
        if (atual->funcionario.status == LIVRE) {
            bool tem_habilidade = false;
            for (int i = 0; i < atual->funcionario.num_habilidade; i++) {
                if (atual->funcionario.habilidades[i] == habilidade) {
                    tem_habilidade = true;
                    break;
                }
            }
            if (tem_habilidade) {
                if (atual->funcionario.num_habilidade < min_habilidades) {
                    melhor_candidato = atual;
                    min_habilidades = atual->funcionario.num_habilidade;
                }
            }
        }
        atual = atual->prox;
    }

    if (melhor_candidato != NULL) {
        melhor_candidato->funcionario.status = OCUPADO;
        return removerNodeFuncionario(reserva, melhor_candidato);
    }
    return NULL;
}

void liberarFuncionario(NodeFuncionario *node, ListaFuncionarios *origem, ListaFuncionarios *destino_reserva) {
    if (!node || !destino_reserva) return;
    if (origem) {
        removerNodeFuncionario(origem, node);
    }
    node->funcionario.status = LIVRE;
    node->funcionario.local_atual = RESERVA;
    node->funcionario.pedido_trabalhado = NULL;
    adicionarListaFuncionario(destino_reserva, node);
}

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

void imprimirListaArmazenados(ListaItensArmazenados lista, int restaurante_id) {
    printf("\n[R%d] === ESTOQUE DE ITENS PRONTOS ===\n", restaurante_id);
    NodeItemArmazenado *atual = lista.cabeca;
    if (!atual) {
        printf("    (Estoque vazio)\n");
    }
    while (atual) {
        printf("    -> Item: ");
        imprimirNomeDoItem(atual->item.nome);
        printf(" | Valido ate t=%d\n", atual->item.tempo_expiracao);
        atual = atual->prox;
    }
    printf("[R%d] ================================\n", restaurante_id);
}

void verificarEstoqueExpirado(Restaurante *res, int tempo_atual) {
    ListaItensArmazenados *estoque = &res->estoque;
    NodeItemArmazenado *atual = estoque->cabeca;
    while (atual != NULL) {
        NodeItemArmazenado *proximo = atual->prox;
        if (tempo_atual > atual->item.tempo_expiracao) {
            printf("[R%d] [ESTOQUE] Item '", res->id);
            imprimirNomeDoItem(atual->item.nome);
            printf("' expirou e foi descartado (expirou em t=%d).\n", atual->item.tempo_expiracao);
            removerNodeArmazenado(estoque, atual);
        }
        atual = proximo;
    }
}

float estimarTempoTotalPedido(Pedido* pedido, Restaurante *res) {
    float tempo_estimado = 0;
    float maior_tempo_equipamento = 0;
    
    tempo_estimado += (res->locais[SEPARADOR].fila_espera.quantidade + res->locais[SEPARADOR].pedido_sendo_feitos.quantidade) * 25;

    for (int i = 0; i < pedido->num_itens; i++) {
        ItemPedido* item_pedido = &pedido->itens[i];
        if (item_pedido->nome == NADA || item_pedido->nome == REFRIGERANTE) continue;

        NomeEquipamento destino = -1;
        switch (item_pedido->nome) {
            case SANDUICHE_SIMPLES: case SANDUICHE_MEDIO: case SANDUICHE_ELABORADO: destino = CHAPA; break;
            case BATATA_FRITA: destino = PENEIRA; break;
            case MILK_SHAKE: destino = LIQUIDIFICADOR_MILK_SHAKE; break;
            case SUCO: destino = LIQUIDIFICADOR_SUCO; break;
            default: continue;
        }

        if (destino != -1) {
            Equipamento* equip = &res->equipamentos[destino];
            float tempo_espera_equipamento = 0;
            
            for (int j = 0; j < equip->capacidade_maxima; j++) {
                if (equip->itens_em_preparo[j].nome != NADA) {
                    tempo_espera_equipamento += equip->itens_em_preparo[j].tempo_restante_preparo;
                }
            }

            NodeItemPreparo* atual = equip->fila_espera.cabeca;
            while(atual != NULL) {
                tempo_espera_equipamento += atual->item.tempo_preparo_total;
                atual = atual->prox;
            }

            if (equip->capacidade_maxima > 0) {
                tempo_espera_equipamento /= equip->capacidade_maxima;
            }
            
            if (tempo_espera_equipamento > maior_tempo_equipamento) {
                maior_tempo_equipamento = tempo_espera_equipamento;
            }
        }
    }
    
    tempo_estimado += maior_tempo_equipamento;
    tempo_estimado += 25;

    return tempo_estimado;
}

void atenderFilaRecepcao(Restaurante *res) {
    Locais *local_recepcao = &res->locais[RECEPCAO];

    while (local_recepcao->heap.quantidade > 0 || local_recepcao->fila_espera.cabeca != NULL) {
        NodeFuncionario *caixa = encontrarEAlocarFuncionario(&res->reserva, HABILIDADE_CAIXA);
        if (caixa == NULL) {
            break;
        }

        const char* origem_fila = "";
        NodePedido *pedido_para_atender = removerPedidoPorPrioridade(local_recepcao, &origem_fila);

        if (pedido_para_atender == NULL) {
            liberarFuncionario(caixa, NULL, &res->reserva);
            break; 
        }
        
        pedido_para_atender->pedido.tempo_preparo_local = 20;
        printf("[R%d] [CAIXA] Func %d iniciou Pedido %d (Fila: %s, Duracao: %ds).\n", 
            res->id, caixa->funcionario.id, pedido_para_atender->pedido.id, origem_fila, pedido_para_atender->pedido.tempo_preparo_local);
        
        caixa->funcionario.pedido_trabalhado = pedido_para_atender;
        caixa->funcionario.local_atual = RECEPCAO;
        adicionarListaFuncionario(&local_recepcao->funcionario, caixa);
        adicionarListaPedidos(&local_recepcao->pedido_sendo_feitos, pedido_para_atender);
    }
}

void processarRecepcao(Restaurante *res, int ciclo) {
    Locais *local_recepcao = &res->locais[RECEPCAO];
    Locais *local_separador = &res->locais[SEPARADOR];
    
    NodeFuncionario *func_atual = local_recepcao->funcionario.cabeca;
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;
        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_atendido = func_atual->funcionario.pedido_trabalhado;
            pedido_atendido->pedido.tempo_preparo_local -= ciclo;
            if (pedido_atendido->pedido.tempo_preparo_local <= 0) {
                printf("[R%d] [CAIXA->SEPARADOR] Atendimento do Pedido %d concluido. Movendo para fila de separacao.\n", res->id, pedido_atendido->pedido.id);
                removerNodePedido(&local_recepcao->pedido_sendo_feitos, pedido_atendido);
                func_atual->funcionario.pedido_trabalhado = NULL;
                adicionarListaPedidos(&local_separador->fila_espera, pedido_atendido);
                liberarFuncionario(func_atual, &local_recepcao->funcionario, &res->reserva);
            }
        }
        func_atual = proximo_func;
    }
}

void distribuirItens(NodePedido *pedido_node, Restaurante *res) {
    if (!pedido_node) return;
    printf("[R%d] [SEPARADOR] Distribuindo itens do Pedido %d:\n", res->id, pedido_node->pedido.id);
    for (int i = 0; i < pedido_node->pedido.num_itens; i++) {
        ItemPedido *item_original = &pedido_node->pedido.itens[i];
        if (item_original->nome == NADA) continue;

        if (item_original->nome == REFRIGERANTE) {
            item_original->status = PRONTO;
            printf("    -> Item 'Refrigerante' (Pedido %d) e instantaneo.\n", pedido_node->pedido.id);
            continue;
        }

        bool encontrado_no_estoque = false;
        if (item_original->nome == BATATA_FRITA || item_original->nome == SUCO || item_original->nome == MILK_SHAKE) {
            NodeItemArmazenado *item_em_estoque = res->estoque.cabeca;
            while (item_em_estoque != NULL) {
                if (item_em_estoque->item.nome == item_original->nome) {
                    printf("    -> [OTIMIZACAO] Item '");
                    imprimirNomeDoItem(item_original->nome);
                    printf("' (Pedido %d) retirado do estoque.\n", pedido_node->pedido.id);
                    
                    item_original->status = PRONTO;
                    removerNodeArmazenado(&res->estoque, item_em_estoque);
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
        adicionarListaItemPreparo(&res->equipamentos[destino].fila_espera, node_item);
        printf("    -> Item '");
        imprimirNomeDoItem(item_original->nome);
        printf("' (Pedido %d) enviado para a fila do equipamento %d.\n", pedido_node->pedido.id, destino);
    }
}

void processarSeparador(Restaurante *res, int ciclo) {
    Locais *local_separador = &res->locais[SEPARADOR];
    
    NodeFuncionario *func_atual = local_separador->funcionario.cabeca;
    while (func_atual != NULL) {
        NodeFuncionario *proximo_func = func_atual->prox;
        if (func_atual->funcionario.status == OCUPADO && func_atual->funcionario.pedido_trabalhado != NULL) {
            NodePedido *pedido_sendo_separado = func_atual->funcionario.pedido_trabalhado;
            pedido_sendo_separado->pedido.tempo_preparo_local -= ciclo;
            if (pedido_sendo_separado->pedido.tempo_preparo_local <= 0) {
                printf("[R%d] [SEPARADOR] Func %d finalizou a separacao do Pedido %d.\n", res->id, func_atual->funcionario.id, pedido_sendo_separado->pedido.id);
                distribuirItens(pedido_sendo_separado, res);
                pedido_sendo_separado->pedido.status = PREPARANDO_ITENS;
                removerNodePedido(&local_separador->pedido_sendo_feitos, pedido_sendo_separado);
                func_atual->funcionario.pedido_trabalhado = NULL;
                adicionarListaPedidos(&res->pedidos_em_preparo, pedido_sendo_separado);
                liberarFuncionario(func_atual, &local_separador->funcionario, &res->reserva);
            }
        }
        func_atual = proximo_func;
    }
}

void separador(Restaurante *res) {
    Locais *local_separador = &res->locais[SEPARADOR];
    
    while (local_separador->heap.quantidade > 0 || local_separador->fila_espera.cabeca != NULL) {
        NodeFuncionario *separador_func = encontrarEAlocarFuncionario(&res->reserva, HABILIDADE_SEPARACAO);
        if(separador_func == NULL) break;

        const char* origem_fila = "";
        NodePedido *pedido_para_separar = removerPedidoPorPrioridade(local_separador, &origem_fila);


        if (pedido_para_separar == NULL) {
            liberarFuncionario(separador_func, NULL, &res->reserva);
            break;
        }

        pedido_para_separar->pedido.tempo_preparo_local = 25;
        printf("[R%d] [SEPARADOR] Func %d iniciou Pedido %d (Fila: %s, Duracao: %ds).\n", 
            res->id, separador_func->funcionario.id, pedido_para_separar->pedido.id, origem_fila, pedido_para_separar->pedido.tempo_preparo_local);

        separador_func->funcionario.pedido_trabalhado = pedido_para_separar;
        separador_func->funcionario.local_atual = SEPARADOR;
        adicionarListaFuncionario(&local_separador->funcionario, separador_func);
        adicionarListaPedidos(&local_separador->pedido_sendo_feitos, pedido_para_separar);
    }
}

void processarEquipamentos(Restaurante *res, int ciclo, int tempo_simulacao_atual) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < res->equipamentos[i].capacidade_maxima; j++) {
            if (res->equipamentos[i].itens_em_preparo[j].nome != NADA) {
                ItemPreparo *item = &res->equipamentos[i].itens_em_preparo[j];
                item->tempo_restante_preparo -= ciclo;

                if (item->tempo_restante_preparo <= 0) {
                    if (item->pedido_pai != NULL) {
                        printf("[R%d] [EQUIP %d] Item '", res->id, i);
                        imprimirNomeDoItem(item->nome);
                        printf("' (Pedido %d) ficou PRONTO.\n", item->pedido_pai->id);
                        item->item_original->status = PRONTO;
                    } else {
                        printf("[R%d] [EQUIP %d] Item '", res->id, i);
                        imprimirNomeDoItem(item->nome);
                        printf("' (para estoque) ficou PRONTO.\n");

                        bool item_usado_para_pedido = false;
                        NodeItemPreparo *item_na_fila = res->equipamentos[i].fila_espera.cabeca;
                        while(item_na_fila != NULL) {
                            NodeItemPreparo *proximo = item_na_fila->prox;
                            if (item_na_fila->item.nome == item->nome) {
                                printf("[R%d] [OTIMIZACAO] Item '", res->id);
                                imprimirNomeDoItem(item->nome);
                                printf("' recem-produzido foi usado para o Pedido %d (fila).\n", item_na_fila->item.pedido_pai->id);
                                
                                item_na_fila->item.item_original->status = PRONTO;
                                removerNodeItemPreparo(&res->equipamentos[i].fila_espera, item_na_fila);
                                free(item_na_fila);
                                
                                item_usado_para_pedido = true;
                                break; 
                            }
                            item_na_fila = proximo;
                        }

                        if (!item_usado_para_pedido) {
                            bool armazenavel = false;
                            int validade = 0;
                            switch(item->nome) {
                                case BATATA_FRITA: armazenavel = true; validade = 3600; break;
                                case MILK_SHAKE: armazenavel = true; validade = 2700; break;
                                case SUCO: armazenavel = true; validade = 2700; break;
                                default: break;
                            }

                            if (armazenavel) {
                                ItemArmazenado novo_item;
                                novo_item.nome = item->nome;
                                novo_item.tempo_expiracao = tempo_simulacao_atual + validade;
                                adicionarItemArmazenado(&res->estoque, criarNodeItemArmazenado(novo_item));
                                printf("[R%d] [ESTOQUE] Item '", res->id);
                                imprimirNomeDoItem(item->nome);
                                printf("' adicionado ao estoque (valido ate t=%d).\n", novo_item.tempo_expiracao);
                            }
                        }
                    }

                    liberarFuncionario(res->equipamentos[i].funcionarios_alocados[j], NULL, &res->reserva);
                    res->equipamentos[i].funcionarios_alocados[j] = NULL;
                    item->nome = NADA;
                    res->equipamentos[i].capacidade_usada--;
                }
            }
        }
    }
}

static void gerenciarEquipamentoEspecifico(Equipamento *equip, Habilidade habilidade_necessaria, ListaFuncionarios *reserva, ListaItensArmazenados *estoque, bool restaurante_aberto, int restaurante_id) {
    while (equip->capacidade_usada < equip->capacidade_maxima && equip->fila_espera.cabeca != NULL) {
        NodeFuncionario *operador = encontrarEAlocarFuncionario(reserva, habilidade_necessaria);
        if (operador == NULL) break; 

        NodeItemPreparo *item_node = removerListaItemPreparo_front(&equip->fila_espera);
        if (item_node == NULL) { 
            liberarFuncionario(operador, NULL, reserva); 
            break;
        }

        int slot_vazio = -1;
        for (int i = 0; i < equip->capacidade_maxima; i++) {
            if (equip->itens_em_preparo[i].nome == NADA) {
                slot_vazio = i;
                break;
            }
        }

        if (slot_vazio != -1) {
            equip->itens_em_preparo[slot_vazio] = item_node->item;
            equip->funcionarios_alocados[slot_vazio] = operador;
            equip->capacidade_usada++;
            
            if(item_node->item.item_original) {
                item_node->item.item_original->status = EM_PREPARO;
            }

            printf("[R%d] [EQUIP %d] Func %d iniciou '", restaurante_id, equip->nome, operador->funcionario.id);
            imprimirNomeDoItem(item_node->item.nome);
            if (item_node->item.pedido_pai != NULL) {
                 printf("' (Pedido %d).\n", item_node->item.pedido_pai->id);
            } else {
                 printf("' (Para Estoque - vindo da fila).\n");
            }
            free(item_node); 
        } else {
            adicionarListaItemPreparo(&equip->fila_espera, item_node); 
            liberarFuncionario(operador, NULL, reserva);
            break; 
        }
    } 
    
    if (!restaurante_aberto) {
        return; 
    }

    bool is_stockable = (equip->nome == PENEIRA || equip->nome == LIQUIDIFICADOR_MILK_SHAKE);
    if (!is_stockable) {
        return; 
    }

    NomePedido item_para_estoque_nome = (equip->nome == PENEIRA) ? BATATA_FRITA : MILK_SHAKE;

    int estoque_atual = 0;
    int em_preparo_para_estoque = 0;

    NodeItemArmazenado *item_em_estoque = estoque->cabeca;
    while (item_em_estoque != NULL) {
        if (item_em_estoque->item.nome == item_para_estoque_nome) {
            estoque_atual++;
        }
        item_em_estoque = item_em_estoque->prox;
    }

    for (int k = 0; k < equip->capacidade_maxima; k++) {
        if (equip->itens_em_preparo[k].nome == item_para_estoque_nome && 
            equip->itens_em_preparo[k].pedido_pai == NULL) { 
            em_preparo_para_estoque++;
        }
    }

    int faltam_para_estoque = MIN_ESTOQUE_PROATIVO - (estoque_atual + em_preparo_para_estoque);
    if (faltam_para_estoque <= 0) {
        return; 
    }

    while (equip->capacidade_usada < equip->capacidade_maxima && faltam_para_estoque > 0) {
        NodeFuncionario *operador_extra = encontrarEAlocarFuncionario(reserva, habilidade_necessaria);
        if (operador_extra == NULL) {
            break; 
        }

        int slot_extra = -1;
        for (int j = 0; j < equip->capacidade_maxima; j++) {
            if (equip->itens_em_preparo[j].nome == NADA) {
                slot_extra = j;
                break;
            }
        }

        if (slot_extra != -1) {
            ItemPreparo item_para_estoque_extra;
            item_para_estoque_extra.nome = item_para_estoque_nome;
            item_para_estoque_extra.tempo_preparo_total = calcularPreparo(item_para_estoque_nome);
            item_para_estoque_extra.tempo_restante_preparo = calcularPreparo(item_para_estoque_nome);
            item_para_estoque_extra.status = EM_PREPARO;
            item_para_estoque_extra.pedido_pai = NULL; 
            item_para_estoque_extra.item_original = NULL;

            equip->itens_em_preparo[slot_extra] = item_para_estoque_extra;
            equip->funcionarios_alocados[slot_extra] = operador_extra;
            equip->capacidade_usada++;
            faltam_para_estoque--; 

            printf("[R%d] [PRODUCAO EXTRA] Func %d iniciou '", restaurante_id, operador_extra->funcionario.id);
            imprimirNomeDoItem(item_para_estoque_nome);
            printf("' para estoque (Nivel: %d, Em Preparo: %d, Faltando: %d).\n", 
                   estoque_atual, em_preparo_para_estoque + 1, faltam_para_estoque);
        } else {
            liberarFuncionario(operador_extra, NULL, reserva);
            break; 
        }
    } 
}

void gerenciarChapa(Restaurante *res, bool restaurante_aberto) {
    gerenciarEquipamentoEspecifico(&res->equipamentos[CHAPA], HABILIDADE_SANDUICHE, &res->reserva, &res->estoque, restaurante_aberto, res->id);
}

void gerenciarPeneira(Restaurante *res, bool restaurante_aberto) {
    gerenciarEquipamentoEspecifico(&res->equipamentos[PENEIRA], HABILIDADE_BATATA, &res->reserva, &res->estoque, restaurante_aberto, res->id);
}

void gerenciarLiquidificador(Restaurante *res, bool restaurante_aberto) {
    gerenciarEquipamentoEspecifico(&res->equipamentos[LIQUIDIFICADOR_SUCO], HABILIDADE_SUCO, &res->reserva, &res->estoque, restaurante_aberto, res->id);
    gerenciarEquipamentoEspecifico(&res->equipamentos[LIQUIDIFICADOR_MILK_SHAKE], HABILIDADE_BEBIDA, &res->reserva, &res->estoque, restaurante_aberto, res->id);
}

void gerenciarTodosEquipamentos(Restaurante *res, bool restaurante_aberto) {
    gerenciarPeneira(res, restaurante_aberto);
    gerenciarChapa(res, restaurante_aberto);
    gerenciarLiquidificador(res, restaurante_aberto);
}

static void adicionarItemAoPedido(Pedido* p, NomePedido nome, int quantidade) {
    if (nome == NADA || quantidade <= 0) { return; }
    for (int i = 0; i < quantidade; i++) {
        if (p->num_itens >= MAX_ITENS_POR_PEDIDO) {
            printf("  [PEDIDO] AVISO: Pedido %d atingiu o limite maximo de itens (%d). Alguns itens foram ignorados.\n", 
                   p->id, MAX_ITENS_POR_PEDIDO);
            return;
        }
        p->itens[p->num_itens].nome = nome;
        p->itens[p->num_itens].tempo_preparo_total = calcularPreparo(nome);
        p->itens[p->num_itens].status = AGUARDANDO_PREPARADO;
        p->num_itens++;
    }
}

NodePedido* lerNovoPedidoFormatado(FILE *arquivo_entrada, int tempo_simulacao_atual) {
    Pedido p;
    memset(&p, 0, sizeof(Pedido));
    p.id = next_pedido_id++; 
    p.status = RECEBIDO;
    p.num_itens = 0;
    p.tempo_chegada = tempo_simulacao_atual;

    char buffer[100];
    char tipo_pedido_str[20]; 
    char senha_str[20]; 

    printf("\n[SISTEMA] Lendo pedido do arquivo 'entrada.txt' (t=%d)...\n", tempo_simulacao_atual);

    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) {
        printf("[SISTEMA] Fim do arquivo de entrada.\n");
        next_pedido_id--; 
        return NULL; 
    }
    sscanf(buffer, "%s", tipo_pedido_str); 

    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; } 
    sscanf(buffer, "%s", senha_str);

    if (strcmp(senha_str, "0") == 0 || strcmp(senha_str, "-") == 0) {
        p.id_senha = 0; 
        for (int i = 0; i < 14; i++) {
            if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) {
                return NULL; 
            }
        }
        NodePedido *node = criarNodePedido(p);
        return node;
    }
    
    sscanf(senha_str, "%d", &p.id_senha);

    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    sscanf(buffer, "%d", &p.distancia_rest_1);

    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    sscanf(buffer, "%d", &p.distancia_rest_2);

    int qtd_batata, qtd_simples, qtd_medio, qtd_elaborado, qtd_refri, qtd_suco, qtd_milkshake;
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    sscanf(buffer, "%d", &qtd_batata);
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    sscanf(buffer, "%d", &qtd_simples);
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    sscanf(buffer, "%d", &qtd_medio);
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    sscanf(buffer, "%d", &qtd_elaborado);
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    sscanf(buffer, "%d", &qtd_refri);
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    sscanf(buffer, "%d", &qtd_suco);
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    if (!fgets(buffer, sizeof(buffer), arquivo_entrada)) { return NULL; }
    sscanf(buffer, "%d", &qtd_milkshake);

    adicionarItemAoPedido(&p, BATATA_FRITA, qtd_batata);
    adicionarItemAoPedido(&p, SANDUICHE_SIMPLES, qtd_simples);
    adicionarItemAoPedido(&p, SANDUICHE_MEDIO, qtd_medio);
    adicionarItemAoPedido(&p, SANDUICHE_ELABORADO, qtd_elaborado);
    adicionarItemAoPedido(&p, REFRIGERANTE, qtd_refri);
    adicionarItemAoPedido(&p, SUCO, qtd_suco);
    adicionarItemAoPedido(&p, MILK_SHAKE, qtd_milkshake);

    printf("  [PEDIDO] Pedido %d (Senha: %d) lido com %d itens.\n", 
           p.id, p.id_senha, p.num_itens);

    int soma = 0;
    for (int i = 0; i < p.num_itens; ++i) {
        soma += p.itens[i].tempo_preparo_total;
    }
    
    if (soma >= LIMITE_TEMPO_PEDIDO) {
        printf("  [PEDIDO RECUSADO] Pedido ID %d (Senha %d) recusado. Tempo total (%ds) excede %ds.\n",
               p.id, p.id_senha, soma, LIMITE_TEMPO_PEDIDO);
        printf("----------------------------------------------------------------\n");
        next_pedido_id--; 
        return NULL; 
    }
    
    if (p.num_itens == 0) {
        printf("  [PEDIDO RECUSADO] Pedido ID %d (Senha %d) recusado. 0 itens.\n", p.id, p.id_senha);
        printf("----------------------------------------------------------------\n");
        next_pedido_id--; 
        return NULL; 
    }

    p.tempo_preparo_total = soma;
    p.tempo_restante_preparo = soma;
    
    printf("----------------------------------------------------------------\n");
    
    NodePedido *node = criarNodePedido(p);
    return node;
}

bool recepcao(FILE *arquivo_entrada, Restaurante restaurantes[], int timer_global, int tempo_simulacao_atual, bool restaurante_aberto) {
    NodePedido *novo_pedido = lerNovoPedidoFormatado(arquivo_entrada, tempo_simulacao_atual);
    
    if (novo_pedido == NULL) {
        if (feof(arquivo_entrada)) {
             printf("[ROTEADOR] Fim do arquivo de entrada.\n");
            return false; 
        }
        printf("[ROTEADOR] Pedido descartado (erro de leitura, >300s, ou 0 itens).\n");
        return true; 
    }

    if (novo_pedido->pedido.id_senha == 0) {
        printf("[ROTEADOR] Pausa detectada. Cozinha processando...\n");
        free(novo_pedido); 
        return true; 
    }

    Restaurante *restaurante_alvo = NULL;
    int d1 = novo_pedido->pedido.distancia_rest_1;
    int d2 = novo_pedido->pedido.distancia_rest_2;
    int id_senha = novo_pedido->pedido.id_senha;

    if (d1 == 0) { 
        restaurante_alvo = &restaurantes[0];
        novo_pedido->pedido.tipo = PRESENCIAL;
    } else if (d2 == 0) { 
        restaurante_alvo = &restaurantes[1];
        novo_pedido->pedido.tipo = PRESENCIAL;
    } else { 
        novo_pedido->pedido.tipo = IFOOD;
        if (d1 < d2) {
            restaurante_alvo = &restaurantes[0]; 
        } else if (d2 < d1) {
            restaurante_alvo = &restaurantes[1]; 
        } else {
            restaurante_alvo = (id_senha % 2 != 0) ? &restaurantes[0] : &restaurantes[1];
        }
    }
    
    novo_pedido->pedido.id_restaurante_atribuido = restaurante_alvo->id;
    printf("[ROTEADOR] Pedido %d (Senha %d) atribuido ao Restaurante %d (Dist R1:%d, Dist R2:%d).\n", 
        novo_pedido->pedido.id, id_senha, restaurante_alvo->id, d1, d2);

    if (!restaurante_aberto) {
        printf("[ROTEADOR] Restaurante fechado. Pedido %d (Senha %d) rejeitado.\n",
               novo_pedido->pedido.id, novo_pedido->pedido.id_senha);
        free(novo_pedido);
        return true; 
    }
    
    NodeFuncionario *caixa = encontrarEAlocarFuncionario(&restaurante_alvo->reserva, HABILIDADE_CAIXA);
    if (caixa == NULL) {
        printf("[R%d] [REJEITADO] Pedido %d (Senha %d) rejeitado. Nenhum caixa disponivel.\n",
               restaurante_alvo->id, novo_pedido->pedido.id, id_senha);
        free(novo_pedido);
        return true; 
    }

    int total_em_producao = contarPedidosEmProducao(restaurante_alvo);
    if (total_em_producao >= MAX_PEDIDOS_EM_PRODUCAO) {
        printf("[R%d] [BLOQUEIO] Cozinha com %d pedidos (limite: %d). Pedido %d descartado.\n",
               restaurante_alvo->id, total_em_producao, MAX_PEDIDOS_EM_PRODUCAO, novo_pedido->pedido.id);
        liberarFuncionario(caixa, NULL, &restaurante_alvo->reserva); 
        free(novo_pedido);
        return true; 
    }

    float tempo_total_estimado = estimarTempoTotalPedido(&novo_pedido->pedido, restaurante_alvo);
    if (tempo_total_estimado > LIMITE_POSTERGAR_PEDIDO || tempo_total_estimado > timer_global) {
        printf("[R%d] [ADIADO] Pedido %d (cozinha: %.0fs) sera postergado.\n", 
            restaurante_alvo->id, novo_pedido->pedido.id, tempo_total_estimado);
        liberarFuncionario(caixa, NULL, &restaurante_alvo->reserva); 
        adicionarListaPedidos(&restaurante_alvo->pedidos_postergados, novo_pedido);
        return true; 
    }

    Locais *local_recepcao = &restaurante_alvo->locais[RECEPCAO];
    const char* tipo_str = (novo_pedido->pedido.tipo == IFOOD) ? "(iFood)" : "(Presencial)";
    printf("[R%d] [CAIXA] Func %d atende Pedido %d %s.\n", restaurante_alvo->id, caixa->funcionario.id, novo_pedido->pedido.id, tipo_str);
    caixa->funcionario.local_atual = RECEPCAO;
    adicionarListaFuncionario(&local_recepcao->funcionario, caixa);
    novo_pedido->pedido.tempo_preparo_local = 20; 
    caixa->funcionario.pedido_trabalhado = novo_pedido;
    adicionarListaPedidos(&local_recepcao->pedido_sendo_feitos, novo_pedido);
    
    return true; 
}

void priorizarPedidosAntigos(Restaurante *res, int tempo_simulacao_atual) {
    const char* nomes_locais[] = {"RECEPCAO", "MONTAR_BANDEJAS", "SEPARADOR", "CAIXA", "RESERVA"};
    NomeLocal locais_para_verificar[] = {RECEPCAO, SEPARADOR, MONTAR_BANDEJAS};
    int num_locais = 3;

    for (int i = 0; i < num_locais; i++) {
        Locais *local = &res->locais[locais_para_verificar[i]];
        
        NodePedido *atual = local->fila_espera.cabeca;
        while (atual != NULL) {
            NodePedido *proximo = atual->prox;
            int idade_pedido = tempo_simulacao_atual - atual->pedido.tempo_chegada;
            if (idade_pedido > PEDIDO_ANTIGO_THRESHOLD) {
                const char* tipo_str = (atual->pedido.tipo == IFOOD) ? "iFood" : "Presencial";
                printf("[R%d] [ALERTA IDADE] Pedido %s ID:%d esta no local '%s' ha %d segundos! PRIORIZANDO.\n",
                    res->id, tipo_str, atual->pedido.id, nomes_locais[local->nome], idade_pedido);
                removerNodePedido(&local->fila_espera, atual);
                inserirPedido(&local->heap, atual->pedido);
                free(atual);
            }
            atual = proximo;
        }
    }
}

bool existemPedidosAtivos(Restaurante *res) {
    for (int i = 0; i < 5; i++) {
        if (res->locais[i].fila_espera.quantidade > 0) return true; 
        if (res->locais[i].pedido_sendo_feitos.quantidade > 0) return true;
        if (res->locais[i].heap.quantidade > 0) return true;
    }

    for (int i = 0; i < 4; i++) {
        if (res->equipamentos[i].capacidade_usada > 0) return true;
        if (res->equipamentos[i].fila_espera.quantidade > 0) return true;
    }

    if (res->pedidos_em_preparo.quantidade > 0) return true;

    return false;
}

int contarPedidosEmProducao(Restaurante *res) {
    int total = 0;
    
    total += res->locais[SEPARADOR].fila_espera.quantidade;
    total += res->locais[SEPARADOR].heap.quantidade;
    total += res->locais[SEPARADOR].pedido_sendo_feitos.quantidade;
    
    total += res->pedidos_em_preparo.quantidade;

    total += res->locais[MONTAR_BANDEJAS].fila_espera.quantidade;
    total += res->locais[MONTAR_BANDEJAS].heap.quantidade;
    total += res->locais[MONTAR_BANDEJAS].pedido_sendo_feitos.quantidade;

    return total;
}

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

void liberarRestaurante(Restaurante *res) {
    printf("[SISTEMA] Limpando memoria do Restaurante %d...\n", res->id);
    for (int i = 0; i < 5; ++i) {
        liberarHeap(&res->locais[i].heap);
        liberarListaPedidos(&res->locais[i].fila_espera); 
        liberarListaPedidos(&res->locais[i].pedido_sendo_feitos);
        liberarListaFuncionarios(&res->locais[i].funcionario);
    }
    liberarListaItensArmazenados(&res->estoque);
    liberarListaFuncionarios(&res->reserva);
    liberarListaPedidos(&res->pedidos_entregues);
    liberarListaPedidos(&res->pedidos_em_preparo);
    liberarListaPedidos(&res->pedidos_postergados);
}