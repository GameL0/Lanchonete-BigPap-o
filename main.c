#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "funcoes.h"

#define FIM_EXPEDIENTE_THRESHOLD 3600
// Limite N=5 agora está definido em funcoes.c

// <<< ALTERAÇÃO: Assinatura mudou >>>
void main_loop(FILE *arquivo_entrada, int timer_global, int tam_ciclo, Restaurante restaurantes[]) {
    
    bool restaurante_aberto = true;
    int tempo_simulacao = 0;
    bool continuar_lendo_arquivo = true; // <<< NOVO: Controla a leitura do arquivo
    
    while (true) {
        printf("\n\n###############################################################\n");
        printf(">>> TEMPO DE SIMULACAO: %d s (Ciclo %d) <<<\n", tempo_simulacao, tempo_simulacao / tam_ciclo);
        if (restaurante_aberto) {
            printf(">>> TEMPO RESTANTE PARA FECHAR: %d s <<<\n", timer_global);
        } else {
            printf(">>> EXPEDIENTE EXTRA: %d s <<<\n", -timer_global);
        }
        printf("###############################################################\n");
        
        if (timer_global <= 0 && restaurante_aberto) {
            restaurante_aberto = false;
            printf("\n\n###############################################################\n");
            printf("### RESTAURANTE FECHADO! PROCESSANDO PEDIDOS PENDENTES... ###\n");
            printf("###############################################################\n\n");
        }

        // <<< INÍCIO DA ALTERAÇÃO: Loop de leitura automático (CORREÇÃO DO BUG) >>>
        if (continuar_lendo_arquivo) {
            // Sempre tenta ler, mas passa o status 'restaurante_aberto'
            // A própria 'recepcao' vai rejeitar pedidos se o restaurante estiver fechado.
            continuar_lendo_arquivo = recepcao(arquivo_entrada, restaurantes, timer_global, tempo_simulacao, restaurante_aberto);
        } else if (restaurante_aberto) {
            printf("\n[SISTEMA] Fim do arquivo de entrada. Restaurante continua aberto.\n");
        } else {
             // Silencioso, pois esta é a condição normal de fechamento (arquivo acabou E restaurante fechou)
        }
        // <<< FIM DA ALTERAÇÃO >>>
        
        printf("\n--- PROCESSANDO CICLO %d PARA RESTAURANTES ---\n", tempo_simulacao / tam_ciclo);
        
        for (int i = 0; i < 2; i++) {
            Restaurante *res = &restaurantes[i];
            
            printf("\n\n--- [R%d] INICIO DO CICLO %d ---", res->id, tempo_simulacao / tam_ciclo);
            
            // --- ACONTECIMENTOS ---
            printf("\n[R%d] [EVENTOS] Verificando eventos do ciclo...\n", res->id);
            verificarEstoqueExpirado(res, tempo_simulacao);
            verificarPedidosProntos(res);
            priorizarPedidosAntigos(res, tempo_simulacao); // Corrigido de tempo_simulacao_total

            if (timer_global <= FIM_EXPEDIENTE_THRESHOLD && restaurante_aberto) {
                printf("\n[R%d] [ALERTA] FIM DE EXPEDIENTE SE APROXIMANDO! PRIORIZANDO FILAS.\n", res->id);
                priorizarPedidosFimDeExpediente(res);
            }

            // --- ALOCAÇÃO DE FUNCIONÁRIOS ---
            printf("[R%d] [ALOCACAO] Alocando funcionarios para tarefas pendentes...\n", res->id);
            atenderFilaRecepcao(res);
            separador(res);
            gerenciarTodosEquipamentos(res, restaurante_aberto);
            montarBandeja(res);

            // --- RELATÓRIOS (O que o usuário quer ver) ---
            printf("\n--- [R%d] RELATÓRIO DE ESTADO ---", res->id);
            
            // 1. "filas do equipamentos (Dos itens sendo feitos)" + Fila de espera de itens
            imprimirStatusEquipamentos(res);
            
            // 2. "fila de itens em separação" (Fila do local SEPARADOR) + "fila de prioridade"
            imprimirFilasLocais(res);

            // 3. "fila do estoque"
            imprimirListaArmazenados(res->estoque, res->id);
            
            // 4. "fila dos pedidos na fila de itens em preparo"
            printf("\n"); // Espaçador
            imprimirLista(res->pedidos_em_preparo, res->id, "PEDIDOS EM PREPARO (Aguardando Itens)");
            
            // 5. "filas do itens feitos"
            printf("\n"); // Espaçador
            imprimirLista(res->pedidos_entregues, res->id, "PEDIDOS ENTREGUES (Finalizados)");

            if (res->pedidos_postergados.cabeca != NULL) {
                printf("\n"); // Espaçador
                imprimirLista(res->pedidos_postergados, res->id, "PEDIDOS POSTERGADOS");
            }
            printf("--- [R%d] FIM DO RELATÓRIO ---\n", res->id);
            // --- FIM RELATÓRIOS ---
        }

        printf("\n\n####################### FIM DO CICLO %d #######################\n", tempo_simulacao / tam_ciclo);

        // Processamento (avançar o tempo) - feito DEPOIS para ambos
        for (int i = 0; i < 2; i++) {
            Restaurante *res = &restaurantes[i];
            processarRecepcao(res, tam_ciclo);
            processarSeparador(res, tam_ciclo);
            processarEquipamentos(res, tam_ciclo, tempo_simulacao);
            processarMontagem(res, tam_ciclo, tempo_simulacao);
            
            // Lógica de Postergados
            if (res->pedidos_postergados.cabeca != NULL && (!restaurante_aberto || !existemPedidosAtivos(res))) {
                NodePedido* pedido_postergado = removerListaPedidos_front(&res->pedidos_postergados);
                printf("\n[R%d] [REINICIANDO] Pedido postergado ID %d movido para a fila da recepcao.\n", res->id, pedido_postergado->pedido.id);
                // Adiciona na fila de espera única
                adicionarListaPedidos(&res->locais[RECEPCAO].fila_espera, pedido_postergado);
            }
        }
        
        timer_global -= tam_ciclo;
        tempo_simulacao += tam_ciclo;

        // <<< ALTERAÇÃO: Condição de parada atualizada >>>
        // Para quando o arquivo acabar E ambos os restaurantes estiverem inativos
        bool r1_inativo = !existemPedidosAtivos(&restaurantes[0]) && restaurantes[0].pedidos_postergados.cabeca == NULL;
        bool r2_inativo = !existemPedidosAtivos(&restaurantes[1]) && restaurantes[1].pedidos_postergados.cabeca == NULL;

        if (!continuar_lendo_arquivo && r1_inativo && r2_inativo) {
            printf("\n[SISTEMA] TODOS OS PEDIDOS DE AMBOS OS RESTAURANTES FORAM FINALIZADOS. ENCERRANDO O EXPEDIENTE.\n");
            break;
        }
    }
}

int main() {
    int timer_global = 3600;
    int tam_ciclo = 10;
    
    FILE *arquivo_entrada = fopen("entrada.txt", "r");
    if (arquivo_entrada == NULL) {
        printf("[SISTEMA] ERRO FATAL: Nao foi possivel abrir o arquivo 'entrada.txt'.\n");
        printf("[SISTEMA] Certifique-se de cria-lo (executando o 'gerar_entradas') antes de rodar a simulacao.\n");
        return 1;
    }
    printf("[SISTEMA] Arquivo 'entrada.txt' aberto com sucesso.\n");

    Restaurante restaurantes[2];

    for (int r_idx = 0; r_idx < 2; r_idx++) {
        restaurantes[r_idx].id = r_idx + 1; // R1 e R2
        restaurantes[r_idx].stats = (Estatisticas){0, 0};
        restaurantes[r_idx].estoque = criarListaArmazenados();
        restaurantes[r_idx].reserva = criarListaFuncionarios();
        restaurantes[r_idx].pedidos_entregues = criarLista();
        restaurantes[r_idx].pedidos_em_preparo = criarLista();
        restaurantes[r_idx].pedidos_postergados = criarLista();
        
        // Inicializa Locais
        for (int i = 0; i < 5; ++i) {
            restaurantes[r_idx].locais[i].nome = (NomeLocal) i;
            restaurantes[r_idx].locais[i].fila_espera = criarLista();
            
            restaurantes[r_idx].locais[i].funcionario = criarListaFuncionarios();
            restaurantes[r_idx].locais[i].pedido_sendo_feitos = criarLista();
            criarHeap(&restaurantes[r_idx].locais[i].heap, 20);
        }
        
        // Inicializa Equipamentos
        int capacidades_maximas[] = { 2, 3, 2, 2 }; // Peneira, Chapa, MilkShake, Suco
        for (int i = 0; i < 4; ++i) {
            restaurantes[r_idx].equipamentos[i].nome = (NomeEquipamento) i;
            restaurantes[r_idx].equipamentos[i].capacidade_maxima = capacidades_maximas[i];
            restaurantes[r_idx].equipamentos[i].capacidade_usada = 0;
            restaurantes[r_idx].equipamentos[i].fila_espera = criarListaItemPreparo();
            for (int j = 0; j < MAX_CAPACIDADE_EQUIPAMENTO; j++) {
                restaurantes[r_idx].equipamentos[i].itens_em_preparo[j].nome = NADA;
                restaurantes[r_idx].equipamentos[i].funcionarios_alocados[j] = NULL;
            }
        }
        
        // Inicializa Funcionários (cria 13 para cada restaurante)
        inicializarFuncionarios(&restaurantes[r_idx]);
    }
    printf("[SISTEMA] Ambos os restaurantes foram inicializados com sucesso.\n");


    main_loop(arquivo_entrada, timer_global, tam_ciclo, restaurantes);

    // Fechar o arquivo
    fclose(arquivo_entrada);
    printf("[SISTEMA] Arquivo 'entrada.txt' fechado.\n");

    printf("\n[SISTEMA] Limpando memoria alocada...\n");
    liberarRestaurante(&restaurantes[0]);
    liberarRestaurante(&restaurantes[1]);
    
    printf("\n\n###############################################################\n");
    printf("###           RELATÓRIO FINAL DO EXPEDIENTE                 ###\n");
    printf("###############################################################\n");
    printf("--- RESTAURANTE 1 ---\n");
    printf("Total de pedidos entregues (R1): %d\n", restaurantes[0].stats.total_pedidos_entregues);
    printf("Pedidos > 5 minutos (R1): %d\n", restaurantes[0].stats.pedidos_mais_de_5_min);
    printf("\n--- RESTAURANTE 2 ---\n");
    printf("Total de pedidos entregues (R2): %d\n", restaurantes[1].stats.total_pedidos_entregues);
    printf("Pedidos > 5 minutos (R2): %d\n", restaurantes[1].stats.pedidos_mais_de_5_min);
    printf("###############################################################\n");

    printf("\n[SISTEMA] Simulacao finalizada com sucesso.\n");
    return 0;
}