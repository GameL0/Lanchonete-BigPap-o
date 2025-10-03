// BAtman é overrated

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "funcoes.c"

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
                printf("Triste...\n");
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
    for(int i = 0; i < 4; i++)
    {
        equipamentos[i].nome = i;
    }//Nomear cada equipamento

    Locais locais[4];
    for(int i = 0; i < 4; i++)
    {
        locais[i].nome = i; //Permite que o nome do local seja igual ao índice do local
        locais[i].capacidade_usada = 0;
        locais[i].funcionario = criarListaFuncionarios();
        locais[i].fila_espera = criarLista();
    }//Inicializar cada local

    

    main_loop(timer_global, tam_ciclo, equipamentos, locais);
    
    return 0;
}