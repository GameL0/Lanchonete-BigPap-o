// SuperMan é overrated

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "funcoes.h"

int fazerPedido(bool res_auto)
{
    //Adicionar verificador do Heap
    //Se o Heap estiver cheio, não fazer pedido
    printf("===============================================================\n");
    printf("BEM VINDO AO BIG PAPÃO, O SANDUICHE MAIS RAPIDO DO NORDESTE!!\n");
    printf("Queres fazer you pedido ?\n");
    printf(" 1 - Fazer Pedido\n 2 - Não fazer pedido... Vagabundo\n 3 - Fechar o código\n");
    printf("Resposta: ");
    return receberInput(res_auto, 2, 3);
}


void main_loop(int timer_global, int tam_ciclo, Equipamento equipamento[], Locais locais[])
{
    ListaPedidos pedidos_em_espera = criarLista();

    Heap fila_pedidos;
    
    bool res_auto;
    ListaFuncionarios reserva = criarListaFuncionarios();
    
    if(!criarHeap(&fila_pedidos, 10))
    {
        printf("Erro ao criar o heap.\n");
        return;
    }

    printf("Ligar modo automático (1 - Sim, 2 - Não)?");
    if(receberInput(false, 2, 2) == 1)
    {
        res_auto = true;
        srand(time(NULL));
    }

    do{
        switch(fazerPedido(res_auto))
        {
            case 1:
                recepcao(&locais[RECEPCAO], tam_ciclo, res_auto, &reserva);
                break;
            case 2:
                printf("--\n");
                break;
            case 3:
                sair();
                return;
            default:
                printf("Escreveu, nada com nada");
                break;
        }

        printf("%d seg\n", timer_global);
        timer_global = timer_global - tam_ciclo;
    } while(timer_global >= 0);
}


int main(){
    int timer_global = 300;
    int tam_ciclo = 100;

    Equipamento equipamentos[4];
    // Define a capacidade máxima de cada equipamento 
    int capacidades_maximas[] = {
        2, // PENEIRA (Fritadeira)
        4, // CHAPA
        1, // LIQUIDIFICADOR_MILK_SHAKE
        1  // LIQUIDIFICADOR_SUCO
    };

    printf("Inicializando equipamentos...\n");
    for(int i = 0; i < 4; i++)
    {
        equipamentos[i].nome = i; // Atribui PENEIRA, CHAPA, etc.
        equipamentos[i].capacidade_maxima = capacidades_maximas[i];
        equipamentos[i].capacidade_usada = 0;

        // Inicializa as listas de funcionários e a fila de espera de itens como vazias
        equipamentos[i].funcionarios = criarListaFuncionarios();
        equipamentos[i].fila_espera.cabeca = NULL;
        equipamentos[i].fila_espera.cauda = NULL;

        // Zera os arrays de preparo e armazenamento para evitar lixo de memória
        for (int j = 0; j < 6; j++) {
            equipamentos[i].itens_preparo[j].nome = NADA;
        }
        for (int j = 0; j < 4; j++) {
            equipamentos[i].armazenamento[j].nome = NADA;
        }
    }
    printf("Equipamentos inicializados com sucesso.\n");


    Locais locais[4];
    for(int i = 0; i < 4; i++)
    {
        locais[i].nome = i; //Permite que o nome do local seja igual ao índice do local
        locais[i].capacidade_usada = 0;
        locais[i].funcionario = criarListaFuncionarios();
        locais[i].fila_espera = criarLista();
    }//Inicializar cada local

    int capacidade_heap_locais = 10; 

    printf("Inicializando locais...\n");
    for(int i = 0; i < 5; i++)
    {
        locais[i].nome = i; // Atribui RECEPCAO, MONTAR_BANDEJAS, etc.
        locais[i].capacidade_usada = 0;
        
        // Inicializa as listas de funcionários e pedidos como vazias
        locais[i].funcionario = criarListaFuncionarios();
        locais[i].fila_espera = criarLista();
        locais[i].pedido_sendo_feitos = criarLista();

        // Inicializa o heap de prioridade para cada local
        if (!criarHeap(&locais[i].heap, capacidade_heap_locais)) {
            printf("Erro ao criar heap para o local %d\n", i);
            // Tratar erro aqui, talvez saindo do programa
        }
    }
    printf("Locais inicializados com sucesso.\n");

    main_loop(timer_global, tam_ciclo, equipamentos, locais);
    
    return 0;
}