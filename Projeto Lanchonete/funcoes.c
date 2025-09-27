#include <stdio.h>
#include "structs.h"

void sair(){
    printf("\nPrograma finalizado!");
    exit(0);
}

int criarHeap(Heap *heap, int capacidade){
    heap->pedidos = (Pedido*) malloc(capacidade * sizeof(Pedido));

    if(heap->pedidos == NULL){
        return 0;
    }

    heap->quantidade = 0;
    heap->capacidade = capacidade;
    return 1;
}

void liberarHeap(Heap* heap){
    free(heap->pedidos);
    heap->pedidos = NULL;
    heap->quantidade = 0;
    heap->capacidade = 0;
}

void swap(Pedido *a, Pedido *b){
    Pedido temp = *a;
    *a = *b;
    *b = temp;
}

void subirNoHeap(Heap* heap, int indice){
    while(indice > 0 && heap->pedidos[indice].tempo_preparo_total < heap->pedidos[(indice - 1) / 2].tempo_preparo_total){
        swap(&heap->pedidos[indice].tempo_preparo_total, &heap->pedidos[(indice - 1) / 2].tempo_preparo_total);
        indice = (indice - 1) / 2;
    }
}

void inserirPedido(Heap* heap, Pedido novoPedido){
    if(heap->quantidade == heap->capacidade){
        printf("A lista está cheia, não conseguimos fazer mais pedidos.\n");
        return;
    }

    heap->quantidade++;
    int indice = heap->quantidade - 1;
    heap->pedidos[indice] = novoPedido;
    subirNoHeap(heap, indice);
}


void descerNoHeap(Heap* heap, int indice){
    int menor = indice;
    int esquerda = 2 * indice + 1;
    int direita = 2 * indice + 2;

    if(esquerda < heap->quantidade && heap->pedidos[esquerda].tempo_preparo_total < heap->pedidos[indice].tempo_preparo_total){
        menor = esquerda;
    }


    if(direita < heap->quantidade && heap->pedidos[direita].tempo_preparo_total < heap->pedidos[menor].tempo_preparo_total){
        menor = direita;
    }

    if(menor != indice){
        swap(&heap->pedidos[indice], &heap->pedidos[menor]);
        descerNoHeap(heap, menor);
    }
}

int removerItemPrioritario(Heap* heap, Pedido* pedidoRemovido){
    if(heap->quantidade <= 0) return 0;

    *pedidoRemovido = heap->pedidos[0];

    if(heap->quantidade > 1){
        heap->pedidos[0] = heap->pedidos[heap->quantidade - 1];
        descerNoHeap(heap, 0);
    }
    heap->quantidade--;

    return 1;
}

// --- Funções de Impressão ---

/**
 * @brief Função auxiliar recursiva para imprimir o heap como uma árvore.
 * Não precisa ser declarada no .h, pois é usada apenas por imprimirHeap.
 * @param heap Ponteiro para o heap.
 * @param indice Índice do nó atual a ser impresso.
 * @param nivel Nível de profundidade na árvore (para indentação).
 */
void imprimirNoHeap(Heap* heap, int indice, int nivel) {
    if (indice >= heap->quantidade) {
        return;
    }

    // Visita o filho da direita primeiro
    imprimirNoHeap(heap, 2 * indice + 2, nivel + 1);

    // Imprime o nó atual com indentação
    for (int i = 0; i < nivel; i++) {
        printf("    ");
    }
    printf("(ID:%d, T:%d)\n", heap->pedidos[indice].id, heap->pedidos[indice].tempo_preparo_total);

    // Visita o filho da esquerda
    imprimirNoHeap(heap, 2 * indice + 1, nivel + 1);
}

/**
 * @brief Imprime o estado atual do heap em dois formatos: vetor e árvore.
 * @param heap Ponteiro para o heap a ser impresso.
 */
void imprimirHeap(Heap* heap) {
    printf("\n--- ESTADO ATUAL DO HEAP ---\n");
    if (heap->quantidade == 0) {
        printf("Heap Vazio.\n");
        printf("--------------------------\n");
        return;
    }

    // Visão 1: Como Vetor (Layout da Memória)
    printf("Visao Vetor [Quantidade: %d, Capacidade: %d]\n", heap->quantidade, heap->capacidade);
    printf("[ ");
    for (int i = 0; i < heap->quantidade; i++) {
        printf("ID %d (T:%d) | ", heap->pedidos[i].id, heap->pedidos[i].tempo_preparo_total);
    }
    printf("]\n\n");

    // Visão 2: Como Árvore (Estrutura Lógica)
    printf("Visao Arvore (Raiz a esquerda):\n");
    imprimirNoHeap(heap, 0, 0); // Chama a função recursiva a partir da raiz
    printf("--------------------------\n");
}




// int verifica_chapa()
// {
//     return 0;
// }

// int verifica_batata()
// {
//     return 0;
// }

// int verifica_batata()
// {
//     return 0;
// }

// int tempo_restante(Pedido *pedido, Funcionario funcionario[], Equipamento equipamentos[])
// {
//     int tempo_preparo = pedido->itens[0].tempo_preparo_total;
//     int local_vago;
//     int tempo_funcionario;

    
    
//     /*
//     - tempo_preparo = tempo que ele demora para ser preparado
//     - local_vago = tempo onde o local onde ele será preparado estará vazio 
//     - tempo_funcionario = menor tempo em que teremos um funcionário apto a preparar;

//     if (tempo_funcionario > local_vago)
//     {
//         tempo_restante = tempo_preparo + local_vago; 
//     }
//     else
//     {
//         tempo_restante = tempo_preparo + tempo_funcionario;
//     }

//     - Faça isso com todos os outros itens da lista:
//         - Puxa um item do pedido;
//         - tempo_preparo = tempo que ele demora para ser preparado
//         - local_vago = tempo onde o local onde ele será preparado estará vazio 
//         - tempo_funcionario = menor tempo em que teremos um funcionário apto a

//     if (tempo_funcionario > local_vago)
//     {
//         if(tempo_restante < tempo_preparo + local_vago) 
//             tempo_restante = tempo_preparo + local_vago; 
        
//     }
//     else
//     {
//         if(tempo_restante < tempo_preparo + tempo_funcionario)
//             tempo_restante = tempo_preparo + tempo_funcionario;
//     }
//     */
//     return 0;
// }


