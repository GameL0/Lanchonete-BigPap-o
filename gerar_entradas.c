#include <stdio.h>
#include <stdlib.h>
#include <time.h>

/*
Este é um programa separado. Compile e execute-o primeiro.
Exemplo:
1. gcc gerar_entradas.c -o gerar_entradas
2. ./gerar_entradas
3. ./seu_programa_de_simulacao (que agora vai ler o 'entrada.txt')
*/

// Tempos de preparo (espelhados da simulação principal)
const int T_BATATA = 190;
const int T_SIMPLES = 58;
const int T_MEDIO = 88;
const int T_ELAB = 105;
const int T_REFRI = 5;
const int T_SUCO = 38;
const int T_MILK = 60;

const int LIMITE_TEMPO = 300; // 5 minutos

int main() {
    FILE *arquivo;
    int num_pedidos;
    int i, senha;
    int dist1, dist2;
    int qtd_batata, qtd_simples, qtd_medio, qtd_elaborado, qtd_refri, qtd_suco, qtd_milkshake;

    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    printf("Quantos eventos (pedidos ou pausas) deseja gerar no 'entrada.txt'? ");
    scanf("%d", &num_pedidos);

    arquivo = fopen("entrada.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar o arquivo 'entrada.txt'.\n");
        return 1;
    }

    // Loop para criar cada evento
    for (i = 0; i < num_pedidos; i++) {
        
        // <<< INÍCIO DA ALTERAÇÃO: Adicionar Pausa Aleatória (15% de chance) >>>
        int chance_pausa = rand() % 100;
        
        if (chance_pausa < 15) {
            // Gerar um evento de PAUSA
            fprintf(arquivo, "Pausa ............................\n");
            fprintf(arquivo, "0\n"); // Senha "0" indica pausa
            fprintf(arquivo, "Distancia 1.....................\n");
            fprintf(arquivo, "0\n");
            fprintf(arquivo, "Distancia 2.......................\n");
            fprintf(arquivo, "0\n");
            fprintf(arquivo, " batata frita.......................\n");
            fprintf(arquivo, "0\n");
            fprintf(arquivo, "sanduíche simples..................\n");
            fprintf(arquivo, "0\n");
            fprintf(arquivo, "sanduíche médio....................\n");
            fprintf(arquivo, "0\n");
            fprintf(arquivo, "sanduíche elaborado................\n");
            fprintf(arquivo, "0\n");
            fprintf(arquivo, "refrigerante.......................\n");
            fprintf(arquivo, "0\n");
            fprintf(arquivo, "suco...............................\n");
            fprintf(arquivo, "0\n");
            fprintf(arquivo, "milk shake.........................\n");
            fprintf(arquivo, "0\n");
            continue; // Pula para o próximo evento
        }
        // <<< FIM DA ALTERAÇÃO >>>
        
        // Se não for pausa, gera um pedido normal
        
        // 1. Decide Restaurante (0 = R1, 1 = R2)
        int restaurante_alvo = rand() % 2; 

        // 2. Decide Tipo (0 = Presencial, 1 = iFood)
        int tipo_pedido_num = rand() % 2;
        const char* tipo_pedido_str = (tipo_pedido_num == 1) ? "Ifood" : "Presencial";

        // 3. Gerar dados de senha e distância
        if (restaurante_alvo == 0) { // R1 (Senha Ímpar)
            senha = (i * 2) + 1;
            if (tipo_pedido_num == 0) { // Presencial R1
                dist1 = 0;
                dist2 = (rand() % 49) + 1; // > 0
            } else { // iFood R1
                dist1 = (rand() % 49) + 1; // > 0
                dist2 = (rand() % 49) + 1; // > 0
            }
        } else { // R2 (Senha Par)
            senha = (i * 2) + 2;
            if (tipo_pedido_num == 0) { // Presencial R2
                dist1 = (rand() % 49) + 1; // > 0
                dist2 = 0;
            } else { // iFood R2
                dist1 = (rand() % 49) + 1; // > 0
                dist2 = (rand() % 49) + 1; // > 0
            }
        }

        // --- LÓGICA DE GERAÇÃO CONTROLADA (para < 300s) ---
        int tempo_total = 0;

        int max_batata = (LIMITE_TEMPO - tempo_total) / T_BATATA;
        qtd_batata = rand() % (max_batata + 1);
        tempo_total += qtd_batata * T_BATATA;

        int max_elab = (LIMITE_TEMPO - tempo_total) / T_ELAB;
        qtd_elaborado = rand() % (max_elab + 1);
        tempo_total += qtd_elaborado * T_ELAB;

        int max_medio = (LIMITE_TEMPO - tempo_total) / T_MEDIO;
        qtd_medio = rand() % (max_medio + 1);
        tempo_total += qtd_medio * T_MEDIO;

        int max_milk = (LIMITE_TEMPO - tempo_total) / T_MILK;
        qtd_milkshake = rand() % (max_milk + 1);
        tempo_total += qtd_milkshake * T_MILK;

        int max_simples = (LIMITE_TEMPO - tempo_total) / T_SIMPLES;
        qtd_simples = rand() % (max_simples + 1);
        tempo_total += qtd_simples * T_SIMPLES;

        int max_suco = (LIMITE_TEMPO - tempo_total) / T_SUCO;
        qtd_suco = rand() % (max_suco + 1);
        tempo_total += qtd_suco * T_SUCO;
        
        int max_refri = (LIMITE_TEMPO - tempo_total) / T_REFRI;
        qtd_refri = rand() % (max_refri + 1);
        tempo_total += qtd_refri * T_REFRI;
        // --- FIM DA LÓGICA DE GERAÇÃO CONTROLADA ---


        // Escreve o bloco de 16 linhas no arquivo
        fprintf(arquivo, "%s ............................\n", tipo_pedido_str);
        fprintf(arquivo, "%d\n", senha);
        fprintf(arquivo, "Distancia 1.....................\n");
        fprintf(arquivo, "%d\n", dist1);
        fprintf(arquivo, "Distancia 2.......................\n");
        fprintf(arquivo, "%d\n", dist2);
        fprintf(arquivo, " batata frita.......................\n");
        fprintf(arquivo, "%d\n", qtd_batata);
        fprintf(arquivo, "sanduíche simples..................\n");
        fprintf(arquivo, "%d\n", qtd_simples);
        fprintf(arquivo, "sanduíche médio....................\n");
        fprintf(arquivo, "%d\n", qtd_medio);
        fprintf(arquivo, "sanduíche elaborado................\n");
        fprintf(arquivo, "%d\n", qtd_elaborado);
        fprintf(arquivo, "refrigerante.......................\n");
        fprintf(arquivo, "%d\n", qtd_refri);
        fprintf(arquivo, "suco...............................\n");
        fprintf(arquivo, "%d\n", qtd_suco);
        fprintf(arquivo, "milk shake.........................\n");
        fprintf(arquivo, "%d\n", qtd_milkshake);
    }

    fclose(arquivo);
    printf("Arquivo 'entrada.txt' com %d eventos (pedidos ou pausas) foi gerado com sucesso!\n", num_pedidos);

    return 0;
}