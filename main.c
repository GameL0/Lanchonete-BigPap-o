#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "funcoes.h"

int main(void)
{
    Heap heap;
    const int janelaSLA = 300; // 5 minutos em segundos

    if (!criarHeap(&heap, 8)) {
        printf("Falha ao criar heap.\n");
        return 1;
    }

    // Cria alguns pedidos de teste com diferentes cargas
    Pedido p1 = criarPedido(1, 0, SANDUICHE_ELABORADO, BATATA_FRITA, MILK_SHAKE, NADA); // 105+190+60 = 355 (baixa)
    Pedido p2 = criarPedido(2, 10, SANDUICHE_MEDIO, SANDUICHE_SIMPLES, MILK_SHAKE, NADA); // 88+58+60 = 206 (alta)
    Pedido p3 = criarPedido(3, 20, BATATA_FRITA, REFRIGERANTE, NADA, NADA); // 190+5 = 195 (alta)
    Pedido p4 = criarPedido(4, 30, SANDUICHE_ELABORADO, MILK_SHAKE, NADA, NADA); // 105+60 = 165 (alta)
    Pedido p5 = criarPedido(5, 40, SUCO, REFRIGERANTE, NADA, NADA); // 38+5 = 43 (alta)

    inserirPedidoComSLA(&heap, p1, janelaSLA);
    inserirPedidoComSLA(&heap, p2, janelaSLA);
    inserirPedidoComSLA(&heap, p3, janelaSLA);
    inserirPedidoComSLA(&heap, p4, janelaSLA);
    inserirPedidoComSLA(&heap, p5, janelaSLA);

    printf("\nEstado inicial do heap (H=alta, L=baixa):\n");
    imprimirHeap(&heap);

    // Reclassificar (simula ajuste periódico)
    reclassificarSLA(&heap, janelaSLA);
    printf("\nDepois de reclassificar por SLA:\n");
    imprimirHeap(&heap);

    // Remove em ordem de prioridade
    printf("\nRemovendo em ordem de prioridade:\n");
    Pedido r;
    while (removerItemPrioritario(&heap, &r)) {
        printf("POP -> ID=%d T=%d P=%c\n", r.id, r.tempo_preparo_total, r.baixa_prioridade ? 'L' : 'H');
    }

    liberarHeap(&heap);
    return 0;
}

