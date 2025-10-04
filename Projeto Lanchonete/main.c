#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "funcoes.h"

int fazerPedido(bool res_auto) {
    printf("===============================================================\n");
    printf("BEM VINDO AO BIG PAPÃO, O SANDUICHE MAIS RAPIDO DO NORDESTE!!\n");
    printf("Queres fazer your pedido ?\n");
    printf(" 1 - Fazer Pedido\n 2 - Nao fazer pedido\n 3 - Fechar o programa\n");
    printf("Resposta: ");
    return receberInput(res_auto, 2, 3);
}

void main_loop(int timer_global, int tam_ciclo, Equipamento equipamentos[], Locais locais[]) {
    ListaFuncionarios reserva = criarListaFuncionarios();
    inicializarFuncionarios(&reserva); 

    ListaPedidos pedidos_entregues = criarLista();
    ListaPedidos pedidos_em_preparo = criarLista(); 
    bool res_auto = false;

    printf("Ligar modo automatico (1 - Sim, 2 - Nao)? ");
    int escolha = receberInput(false, 2, 2);
    if (escolha == 1) {
        res_auto = true;
        srand((unsigned) time(NULL));
    }
    
    do {
        int op = fazerPedido(res_auto);
        switch (op) {
            case 1:
                recepcao(&locais[RECEPCAO], res_auto, &reserva);
                break;
            case 2:
                printf("-- sem pedido neste ciclo --\n");
                break;
            case 3:
                sair();
                break;
            default:
                printf("Opcao invalida\n");
                break;
        }

        separador(&locais[SEPARADOR], &reserva);
        
        processarMontagem(&locais[MONTAR_BANDEJAS], &reserva, &pedidos_entregues, tam_ciclo);
        processarEquipamentos(equipamentos, &reserva, tam_ciclo);
        processarSeparador(&locais[SEPARADOR], equipamentos, &reserva, &pedidos_em_preparo, tam_ciclo);
        processarRecepcao(&locais[RECEPCAO], &locais[SEPARADOR], &reserva, tam_ciclo);


        printf("\n=== FILA DE ESPERA DA RECEPÇÃO ===\n");
        imprimirLista(locais[RECEPCAO].fila_espera);
        printf("======================================\n\n");

        printf("Tempo global atual: %d s\n", timer_global);
        
        for (int i = 0; i < 5; i++) {
            verificarEPriorizarPedidos(&locais[i], timer_global);
        }
        
        for (int i = 0; i < 5; i++) {
            if (locais[i].heap.quantidade > 0) {
                printf("\n=== FILA DE PRIORIDADE - %s ===\n", 
                    i == 0 ? "RECEPCAO" : 
                    i == 1 ? "MONTAR_BANDEJAS" :
                    i == 2 ? "SEPARADOR" :
                    i == 3 ? "CAIXA" : "RESERVA");
                imprimirHeap(&locais[i].heap);
            }
        }
        verificarPedidosProntos(&pedidos_em_preparo, &locais[MONTAR_BANDEJAS]);

        montarBandeja(&locais[MONTAR_BANDEJAS], &reserva);

        printf("\n--- PEDIDOS ENTREGUES ---\n");
        imprimirLista(pedidos_entregues);
        printf("-------------------------\n");

        timer_global -= tam_ciclo;
    } while (timer_global >= 0);
}

int main() {
    int timer_global = 300;
    int tam_ciclo = 10;

    Locais locais[5];
    for (int i = 0; i < 5; ++i) {
        locais[i].nome = (NomeLocal) i;
        locais[i].fila_espera = criarLista();
        locais[i].funcionario = criarListaFuncionarios();
        locais[i].pedido_sendo_feitos = criarLista();
        criarHeap(&locais[i].heap, 10);
    }
    printf("Locais inicializados.\n");

    Equipamento equipamentos[4];
    int capacidades_maximas[] = {
        2, // PENEIRA
        4, // CHAPA
        1, // LIQUIDIFICADOR_MILK_SHAKE
        1  // LIQUIDIFICADOR_SUCO
    };
    for (int i = 0; i < 4; ++i) {
        equipamentos[i].nome = (NomeEquipamento) i;
        equipamentos[i].capacidade_maxima = capacidades_maximas[i];
        equipamentos[i].capacidade_usada = 0;
        equipamentos[i].fila_espera = criarListaItemPreparo();
        
        for (int j = 0; j < MAX_CAPACIDADE_EQUIPAMENTO; j++) {
            equipamentos[i].itens_em_preparo[j].nome = NADA;
            equipamentos[i].funcionarios_alocados[j] = NULL;
        }
    }
    printf("Equipamentos inicializados.\n");

    main_loop(timer_global, tam_ciclo, equipamentos, locais);

    for (int i = 0; i < 5; ++i) liberarHeap(&locais[i].heap);

    return 0;
}