#include <stdio.h>
#include <stdbool.h>

typedef struct 
{
    int comer1;
    int comer2;
    int beber1;
    int beber2;
    int itens_restantes; 
    int tempo_restante;
} pedido;

typedef struct 
{
    pedido p;
    node_pedido *ant;
    node_pedido *prox;
} node_pedido;

typedef struct 
{
    //Funções que o funcionário pode desenvolver:
    char habilidade1[10];
    char habilidade2[10];
    char habilidade3[10];

    int tempo_ocupado; //Segundos que o funcionário demora em uma ocupação
} funcionario;

pedido pegar_pedido()
{
    pedido p;
    
    do
    {
        printf("\nComidas:\n 00-Nada \n 01-Batata-frita; \n 02-Sanduíche Simples; \n 03-Sanduíche Médio; \n 04-Sanduíche Elaborado;");
        printf("\nDigite o número da comida 1: ");
        scanf("%d", &p.comer1);
        printf("\nDigite o número da comida 2: ");
        scanf("%d", &p.comer2);
        printf("\nBebidas:\n 00-Nada \n 01-Refrigerante; \n 02-Suco; \n 03-Milk Shake;");
        printf("\nDigite o número da bebida 1: ");
        scanf("%d", &p.beber1);
        printf("\nDigite o número da bebida 2: ");
        scanf("%d", &p.beber2);
        p.itens_restantes = 4;
        p.tempo_restante = 300;
    } while (p.comer1 >= 0, p.comer1 < 5, p.comer2 >= 0, p.comer2 < 5, p.beber1 >= 0, p.beber1 < 4, p.beber2 >= 0, p.beber2 < 4); // Tratamento de erro 

    return p;
}

bool separa_funcionarios(char tipo[10])
{

}

void prepara_batata()
{

}

void sanduicheira()
{
    // 
}

void separa_pedidos(pedido p)
{
    // Se não puder puxar funcionários, jogar para a lista de espera
    
    int separadores = 0; // Quantidade de separadores trabalhando

    while(separa_funcionarios("separador"))
    {
        separadores++;

        // Pega um pedido da lista de espera
        // Prioriza os pedidos mais urgentes
        // Se vermos que não é possível entregar o pedido em 5 minutos, ele não é urgente
        // Enche lista de pedidos a serem separados agora 
    }
    while(separadores > 0)
    {
        //pega o pedido da lista 
        separadores--;
    }
    // Incrementa 30 segundos no tempo do pedido separado e nos da lista de espera 
}

void recepcao()
{
    // A cada 30 segundos verifica se tem alguém 
    // Verifica se algum funcionário pode receber o pedido
    // Se receber algum pedido, manda ele ser separado
    // Decrementa 30 segundos de todos os setores
    // Se o estabelecimento fechar, não recebe mais pedidos
    // Espera todos os pedidos serem atendidos
}

int main()
{

}