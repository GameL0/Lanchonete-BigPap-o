#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "funcoes.h"

#define FIM_EXPEDIENTE_THRESHOLD 600

void main_loop(int timer_global, int tam_ciclo, Equipamento equipamentos[], Locais locais[], ListaItensArmazenados *estoque, Estatisticas *stats) {
    ListaFuncionarios reserva = criarListaFuncionarios();
    inicializarFuncionarios(&reserva); 

    ListaPedidos pedidos_entregues = criarLista();
    ListaPedidos pedidos_em_preparo = criarLista(); 
    bool res_auto = false;
    
    bool restaurante_aberto = true;
    int tempo_simulacao = 0;

    printf("Ligar modo automatico (1 - Sim, 2 - Nao)? ");
    int escolha = receberInput(false, 2, 2);
    if (escolha == 1) {
        res_auto = true;
        srand((unsigned) time(NULL));
    }
    
    while (true) {
        printf("\n\n###############################################################\n");
        printf(">>> TEMPO DE SIMULACAO: %d s <<<\n", tempo_simulacao);
        if (restaurante_aberto) {
            printf(">>> TEMPO RESTANTE PARA FECHAR: %d s <<<\n", timer_global);
        } else {
            printf(">>> EXPEDIENTE EXTRA: %d s <<<\n", -timer_global);
        }
        printf("###############################################################\n");
        
        if (timer_global <= 0 && restaurante_aberto) {
            restaurante_aberto = false;
            printf("\n\n###############################################################\n");
            printf("### RESTAURANTE FECHADO! FINALIZANDO PEDIDOS PENDENTES... ###\n");
            printf("###############################################################\n\n");
        }

        if (restaurante_aberto) {
            int op = fazerPedido(res_auto);
            switch (op) {
                case 1:
                    recepcao(&locais[RECEPCAO], res_auto, &reserva, estoque, timer_global, tempo_simulacao);
                    break;
                case 2:
                    printf("-- Sem novos pedidos neste ciclo --\n");
                    break;
                case 3:
                    sair();
                    break;
                default:
                    printf("Opcao invalida\n");
                    break;
            }
        } else {
            printf("-- O restaurante esta fechado para novos pedidos --\n");
        }
        
        printf("\n--- [FASE 1: ACONTECIMENTOS DO CICLO] ---\n");
        
        verificarEstoqueExpirado(estoque, tempo_simulacao);
        verificarPedidosProntos(&pedidos_em_preparo, &locais[MONTAR_BANDEJAS]);
        
        priorizarPedidosAntigos(locais, tempo_simulacao);

        if (timer_global <= FIM_EXPEDIENTE_THRESHOLD && restaurante_aberto) {
            printf("\n!!! FIM DE EXPEDIENTE SE APROXIMANDO - PRIORIZANDO PEDIDOS EM ESPERA !!!\n");
            for (int i = 0; i < 5; i++) {
                priorizarPedidosFimDeExpediente(&locais[i]);
            }
        }

        atenderFilaRecepcao(&locais[RECEPCAO], &reserva);
        separador(&locais[SEPARADOR], &reserva, estoque);
        gerenciarTodosEquipamentos(equipamentos, &reserva);
        montarBandeja(&locais[MONTAR_BANDEJAS], &reserva);

        printf("\n--- [FASE 2: RELATÓRIO DE ESTADO ATUAL] ---\n");
        
        imprimirStatusChapa(equipamentos[CHAPA]);

        printf("\n=== ESTOQUE DE ITENS PRONTOS ===\n");
        imprimirListaArmazenados(*estoque);
        printf("==================================\n");

        printf("\n=== PEDIDOS EM ANDAMENTO NOS LOCAIS ===\n");
        bool nenhum_em_andamento = true;
        const char* nomes_locais[] = {"RECEPCAO", "MONTAR_BANDEJAS", "SEPARADOR", "CAIXA", "RESERVA"};
        for (int i = 0; i < 5; i++) {
            if (locais[i].pedido_sendo_feitos.cabeca != NULL) {
                printf("  Local: %s\n", nomes_locais[i]);
                imprimirLista(locais[i].pedido_sendo_feitos);
                nenhum_em_andamento = false;
            }
        }
        if (nenhum_em_andamento) {
            printf("  (Nenhum pedido em andamento nos locais)\n");
        }
        printf("=======================================\n");

        printf("\n=== PEDIDOS EM PREPARO (AGUARDANDO ITENS) ===\n");
        imprimirLista(pedidos_em_preparo);
        printf("=============================================\n");

        for (int i = 0; i < 5; i++) {
            if (locais[i].heap.quantidade > 0) {
                printf("\n=== FILA DE PRIORIDADE - %s ===\n", nomes_locais[i]);
                imprimirHeap(&locais[i].heap);
                printf("=======================================\n");
            }
        }
        
        printf("\n--- PEDIDOS ENTREGUES ---\n");
        imprimirLista(pedidos_entregues);
        printf("-------------------------\n");

        printf("\n####################### FIM DO CICLO #######################\n");

        processarRecepcao(&locais[RECEPCAO], &locais[SEPARADOR], &reserva, tam_ciclo);
        processarSeparador(&locais[SEPARADOR], equipamentos, &reserva, &pedidos_em_preparo, tam_ciclo, estoque);
        processarEquipamentos(equipamentos, &reserva, tam_ciclo, estoque, tempo_simulacao);
        processarMontagem(&locais[MONTAR_BANDEJAS], &reserva, &pedidos_entregues, tam_ciclo, tempo_simulacao, stats);
        
        timer_global -= tam_ciclo;
        tempo_simulacao += tam_ciclo;

        if (!restaurante_aberto && !existemPedidosAtivos(locais, equipamentos, &pedidos_em_preparo)) {
            printf("\nTODOS OS PEDIDOS FORAM FINALIZADOS. ENCERRANDO O EXPEDIENTE.\n");
            liberarListaFuncionarios(&reserva);
            liberarListaPedidos(&pedidos_entregues);
            break;
        }
    }
}

int main() {
    int timer_global = 3600;
    int tam_ciclo = 10;

    Estatisticas stats = {0, 0};
    ListaItensArmazenados estoque = criarListaArmazenados();

    Locais locais[5];
    for (int i = 0; i < 5; ++i) {
        locais[i].nome = (NomeLocal) i;
        locais[i].fila_espera = criarLista();
        locais[i].funcionario = criarListaFuncionarios();
        locais[i].pedido_sendo_feitos = criarLista();
        criarHeap(&locais[i].heap, 20); // Aumentei a capacidade do heap
    }
    printf("Locais inicializados.\n");

    Equipamento equipamentos[4];
    int capacidades_maximas[] = {
        2, // PENEIRA
        3, // CHAPA
        2, // LIQUIDIFICADOR_MILK_SHAKE
        2  // LIQUIDIFICADOR_SUCO
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

    main_loop(timer_global, tam_ciclo, equipamentos, locais, &estoque, &stats);

    printf("\nLimpando memoria alocada...\n");
    for (int i = 0; i < 5; ++i) {
        liberarHeap(&locais[i].heap);
        liberarListaPedidos(&locais[i].fila_espera);
        liberarListaPedidos(&locais[i].pedido_sendo_feitos);
        liberarListaFuncionarios(&locais[i].funcionario);
    }
    
    liberarListaItensArmazenados(&estoque);

    printf("\n\n###############################################################\n");
    printf("###               RELATÓRIO FINAL DO EXPEDIENTE             ###\n");
    printf("###############################################################\n");
    printf("Total de pedidos entregues: %d\n", stats.total_pedidos_entregues);
    printf("Pedidos que levaram mais de 5 minutos: %d\n", stats.pedidos_mais_de_5_min);
    printf("###############################################################\n");

    printf("\nSimulacao finalizada com sucesso.\n");
    return 0;
}