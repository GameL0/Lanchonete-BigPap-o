#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "funcoes.h"

// ---------------------- Helpers de Comparação (Max-Heap) ----------------------

static inline int prioridadeRank(const Pedido *p)
{
    // 1 = alta prioridade, 0 = baixa prioridade
    return p->baixa_prioridade ? 0 : 1;
}

static bool prioridadeMaior(const Pedido *a, const Pedido *b)
{
    int ra = prioridadeRank(a);
    int rb = prioridadeRank(b);
    if (ra != rb) return ra > rb; // alta prioridade acima de baixa

    // Quando ambos são de baixa prioridade, ordenar por chegada (FIFO)
    if (ra == 0 /* ambos baixa */) {
        if (a->tempo_chegada != b->tempo_chegada)
            return a->tempo_chegada < b->tempo_chegada; // mais antigo primeiro
        return a->id < b->id;
    }

    // Ambos alta prioridade: maior tempo primeiro
    if (a->tempo_preparo_total != b->tempo_preparo_total)
        return a->tempo_preparo_total > b->tempo_preparo_total;

    if (a->tempo_chegada != b->tempo_chegada)
        return a->tempo_chegada < b->tempo_chegada; // chegou antes, desempata acima

    return a->id < b->id; // id menor desempata acima
}

static void swapPedido(Pedido *a, Pedido *b)
{
    Pedido tmp = *a;
    *a = *b;
    *b = tmp;
}

static void heapifyUp(Heap *heap, int idx)
{
    while (idx > 0) {
        int pai = (idx - 1) / 2;
        if (prioridadeMaior(&heap->pedidos[idx], &heap->pedidos[pai])) {
            swapPedido(&heap->pedidos[idx], &heap->pedidos[pai]);
            idx = pai;
        } else {
            break;
        }
    }
}

static void heapifyDown(Heap *heap, int idx)
{
    while (1) {
        int esq = 2 * idx + 1;
        int dir = 2 * idx + 2;
        int maior = idx;

        if (esq < heap->quantidade && prioridadeMaior(&heap->pedidos[esq], &heap->pedidos[maior])) {
            maior = esq;
        }
        if (dir < heap->quantidade && prioridadeMaior(&heap->pedidos[dir], &heap->pedidos[maior])) {
            maior = dir;
        }
        if (maior == idx) break;
        swapPedido(&heap->pedidos[idx], &heap->pedidos[maior]);
        idx = maior;
    }
}

static void construirHeap(Heap *heap)
{
    for (int i = heap->quantidade / 2 - 1; i >= 0; i--) {
        heapifyDown(heap, i);
    }
}

// Comparador para reclassificação/SLA (qsort): alta prioridade antes,
// entre altas: tempo desc; entre baixas: chegada asc
int cmpPedidosSLA(const void *pa, const void *pb)
{
    const Pedido *a = (const Pedido*)pa;
    const Pedido *b = (const Pedido*)pb;
    int ra = prioridadeRank(a), rb = prioridadeRank(b);
    if (ra != rb) return rb - ra; // alta primeiro
    if (ra == 0) { // ambos baixa: FIFO por chegada
        if (a->tempo_chegada != b->tempo_chegada)
            return a->tempo_chegada - b->tempo_chegada;
        return a->id - b->id;
    }
    // ambos alta: maior tempo primeiro
    if (a->tempo_preparo_total != b->tempo_preparo_total)
        return b->tempo_preparo_total - a->tempo_preparo_total;
    if (a->tempo_chegada != b->tempo_chegada)
        return a->tempo_chegada - b->tempo_chegada;
    return a->id - b->id;
}

static int garantirCapacidade(Heap *heap)
{
    if (heap->quantidade < heap->capacidade) return 1;
    int nova = heap->capacidade > 0 ? heap->capacidade * 2 : 8;
    Pedido *novo = (Pedido*)realloc(heap->pedidos, (size_t)nova * sizeof(Pedido));
    if (!novo) return 0;
    heap->pedidos = novo;
    heap->capacidade = nova;
    return 1;
}

// ---------------------- API do Heap ----------------------

int criarHeap(Heap *heap, int capacidade)
{
    if (capacidade < 1) capacidade = 8;
    heap->pedidos = (Pedido*)malloc((size_t)capacidade * sizeof(Pedido));
    if (!heap->pedidos) return 0;
    heap->quantidade = 0;
    heap->capacidade = capacidade;
    return 1;
}

void liberarHeap(Heap *heap)
{
    if (!heap) return;
    free(heap->pedidos);
    heap->pedidos = NULL;
    heap->quantidade = 0;
    heap->capacidade = 0;
}

int removerItemPrioritario(Heap *heap, Pedido *pedidoRemovido)
{
    if (!heap || heap->quantidade <= 0) return 0;
    if (pedidoRemovido) *pedidoRemovido = heap->pedidos[0];
    heap->pedidos[0] = heap->pedidos[heap->quantidade - 1];
    heap->quantidade--;
    if (heap->quantidade > 0) heapifyDown(heap, 0);
    return 1;
}

// ---------------------- Cálculo de Preparo ----------------------

int calcularPreparoItem(NomePedido nome)
{
    switch (nome) {
        case BATATA_FRITA: return 190;
        case SANDUICHE_SIMPLES: return 58;
        case SANDUICHE_MEDIO: return 88;
        case SANDUICHE_ELABORADO: return 105;
        case REFRIGERANTE: return 5;
        case SUCO: return 38;
        case MILK_SHAKE: return 60;
        case NADA: default: return 0;
    }
}

int calcularPreparoPedido(Pedido *pedido)
{
    // Estratégia simples: soma dos tempos dos itens (conservadora)
    int soma = 0;
    int cont = 0;
    for (int i = 0; i < 4; i++) {
        int t = pedido->itens[i].tempo_preparo_total;
        if (t <= 0) continue;
        soma += t;
        cont++;
    }
    pedido->num_itens = cont;
    pedido->tempo_preparo_total = soma;
    return soma;
}

int somarCargaAltaPrioridade(const Heap *heap)
{
    if (!heap) return 0;
    int soma = 0;
    for (int i = 0; i < heap->quantidade; i++) {
        if (!heap->pedidos[i].baixa_prioridade) {
            soma += heap->pedidos[i].tempo_preparo_total;
        }
    }
    return soma;
}

// ---------------------- SLA e Inserção ----------------------

static void definirPreparoSeNecessario(Pedido *pedido)
{
    if (pedido->tempo_preparo_total > 0) return;
    // Preenche tempos de itens, se vierem zerados
    for (int i = 0; i < 4; i++) {
        if (pedido->itens[i].tempo_preparo_total == 0) {
            pedido->itens[i].tempo_preparo_total = calcularPreparoItem(pedido->itens[i].nome);
            pedido->itens[i].tempo_restante_preparo = pedido->itens[i].tempo_preparo_total;
        }
    }
    calcularPreparoPedido(pedido);
}

int inserirPedidoComSLA(Heap *heap, Pedido pedido, int janelaSegundos)
{
    if (!heap) return 0;
    definirPreparoSeNecessario(&pedido);

    int cargaAlta = somarCargaAltaPrioridade(heap);
    if (cargaAlta + pedido.tempo_preparo_total > janelaSegundos) {
        pedido.baixa_prioridade = true; // envia para a cauda lógica
    } else {
        pedido.baixa_prioridade = false;
    }

    if (!garantirCapacidade(heap)) return 0;
    heap->pedidos[heap->quantidade] = pedido;
    heapifyUp(heap, heap->quantidade);
    heap->quantidade++;
    return 1;
}

// Reclassifica pedidos para respeitar a janela de 5 minutos entre os de alta prioridade
// Os que não cabem viram baixa prioridade e são reposicionados no heap
void reclassificarSLA(Heap *heap, int janelaSegundos)
{
    if (!heap || heap->quantidade == 0) return;

    // Copia atual para vetor auxiliar
    Pedido *tmp = (Pedido*)malloc((size_t)heap->quantidade * sizeof(Pedido));
    if (!tmp) return;
    memcpy(tmp, heap->pedidos, (size_t)heap->quantidade * sizeof(Pedido));

    // Ordena por prioridade atual (alta/baixa, tempo desc, chegada asc)
    // Para avaliar cumulativo somente dos de alta prioridade
    int n = heap->quantidade;
    // Comparador estático para qsort
    extern int cmpPedidosSLA(const void *pa, const void *pb);
    qsort(tmp, (size_t)n, sizeof(Pedido), cmpPedidosSLA);

    int acumulado = 0;
    for (int i = 0; i < n; i++) {
        if (!tmp[i].baixa_prioridade) {
            if (acumulado + tmp[i].tempo_preparo_total > janelaSegundos) {
                tmp[i].baixa_prioridade = true; // demover para baixa
            } else {
                acumulado += tmp[i].tempo_preparo_total;
            }
        }
    }

    // Reconstroi heap com flags atualizadas
    memcpy(heap->pedidos, tmp, (size_t)n * sizeof(Pedido));
    free(tmp);
    construirHeap(heap);
}

// ---------------------- Utilidades / Demo ----------------------

Pedido criarPedido(int id, int tempo_chegada,
                   NomePedido a, NomePedido b, NomePedido c, NomePedido d)
{
    Pedido p;
    memset(&p, 0, sizeof(Pedido));
    p.id = id;
    p.tempo_chegada = tempo_chegada;
    p.status = RECEBIDO;
    p.baixa_prioridade = false;

    NomePedido itens[4] = {a, b, c, d};
    for (int i = 0; i < 4; i++) {
        p.itens[i].nome = itens[i];
        p.itens[i].status = AGUARDANDO_PREPARADO;
        p.itens[i].tempo_preparo_total = calcularPreparoItem(itens[i]);
        p.itens[i].tempo_restante_preparo = p.itens[i].tempo_preparo_total;
    }
    calcularPreparoPedido(&p);
    return p;
}

void imprimirHeap(const Heap *heap)
{
    printf("\n--- HEAP (quantidade=%d, capacidade=%d) ---\n", heap->quantidade, heap->capacidade);
    for (int i = 0; i < heap->quantidade; i++) {
        const Pedido *p = &heap->pedidos[i];
        printf("[%02d] ID=%d T=%d P=%c chegada=%d\n", i, p->id, p->tempo_preparo_total,
               p->baixa_prioridade ? 'L' : 'H', p->tempo_chegada);
    }
}

