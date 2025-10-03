#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include "funcoes.h"
// #include "structs.h"

/*############# FUNCAO SAIR ##################*/
void sair(){
    printf("\nPrograma finalizado!");
    exit(0);
}


/*##################### FUNCAO MAIN LOOP ########################*/
int receberInput(bool res_auto, int opc_auto, int opc_manual)
{
    int var_control;
    if(res_auto)
    {
        var_control = rand() % opc_auto + 1;
        printf("%d\n", var_control);
        return var_control;
    }
    else
    {
        scanf("%d", &var_control);
        
        //Tratamento de erro
        if(var_control > 0 && var_control <= opc_manual)
            return var_control;
        printf("\nResposta inválida!\n");
        printf("Tente Novamente\n");
        return receberInput(res_auto, opc_auto, opc_manual);
    }
}

/* ####################### FUNÇÕES HEAP #######################################*/

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
        swap(&heap->pedidos[indice], &heap->pedidos[(indice - 1) / 2]);
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
    printf("(ID: %d T:%d)\n", heap->pedidos[indice].id, heap->pedidos[indice].tempo_preparo_total);

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
        printf("(ID: %d T:%d) | ", heap->pedidos[i].id, heap->pedidos[i].tempo_preparo_total);
    }
    printf("]\n\n");

    // Visão 2: Como Árvore (Estrutura Lógica)
    printf("Visao Arvore (Raiz a esquerda):\n");
    imprimirNoHeap(heap, 0, 0); // Chama a função recursiva a partir da raiz
    printf("--------------------------\n");
}


/*################# FUNCOES LISTA DE PEDIDOS ##################################*/

ListaPedidos criarLista()
{
    ListaPedidos lista;
    lista.quantidade = 0;
    lista.cabeca = NULL;
    lista.cauda = NULL;
    return lista;
}

ListaPedidos removerLista(ListaPedidos lista)
{
    NodePedido *proxNode = lista.cabeca->prox;
    proxNode->ante = NULL;
    lista.cabeca->prox = NULL;
    lista.cabeca = proxNode;
    lista.quantidade--;
    return lista;
}

NodePedido* criarNodePedido(Pedido pedido)
{
    NodePedido *novo_node = (NodePedido*) malloc(sizeof(NodePedido)); 
    novo_node->pedido = pedido;
    novo_node->ante = NULL;
    novo_node->prox = NULL;
    return novo_node;
}

ListaPedidos adicionarLista(ListaPedidos lista, NodePedido *novo_node)
{
    novo_node->prox = lista.cauda;
    novo_node->ante = NULL;
    lista.cauda->ante = novo_node;
    lista.cauda = novo_node;
    lista.quantidade++;
    return lista;
}

/*################# FUNCOES LISTA DE PEDIDOS ##################################*/

ListaFuncionarios criarListaFuncionarios()
{
    ListaFuncionarios lista;
    lista.quantidade = 0;
    lista.cabeca = NULL;
    lista.cauda = NULL;
    return lista;
}

ListaFuncionarios removerListaFuncionario(ListaFuncionarios lista, NodeFuncionario *funcionario)
{
    if(lista.cabeca == funcionario)
    {
        lista.cabeca = funcionario->prox;
    }
    else if(lista.cauda == funcionario)
    {
        lista.cauda = funcionario->ante;
    }
    else
    {
        funcionario->ante->prox = funcionario->prox;
        funcionario->prox->ante = funcionario->ante;
    }

    funcionario->prox = NULL;
    funcionario->ante = NULL; 
    lista.quantidade--;
    return lista;
}

void adicionarListaFuncionario(ListaFuncionarios *lista, NodeFuncionario *funcionario)
{
    if(lista->cabeca == NULL)
    {
        lista->cabeca = funcionario;
        lista->cauda = funcionario;
    }
    else
    {
        lista->cauda->prox = funcionario;
        funcionario->ante = lista->cauda;
        lista->cauda = funcionario;
    }
    lista->quantidade++;
}

/*################# FUNCOES IMPRIMIR #####################*/

void imprimirStatusPedido(StatusPedido status)
{
    switch(status) {
        case RECEBIDO:
            printf("Status: Recebido\n");
            break;
        case PREPARANDO_ITENS:
            printf("Status: Preparando itens\n");
            break;
        case AGUARDANDO_MONTAGEM:
            printf("Status: Aguardando montagem\n");
            break;
        case ENTREGUE:
            printf("Status: Entregue\n");
            break;
        case ATRASADO:
            printf("Status: Atrasado\n");
            break;
        case ARMAZENADO:
            printf("Status: Armazenado\n");
            break;
        default:
            printf("Status desconhecido\n");
            break;
    }
}

void imprimirStatusItem(StatusItem status)
{
    switch (status)
    {
    case AGUARDANDO_PREPARADO:
        printf("Aguardando preparo.\n");
        break;
    
    case EM_PREPARO:
        printf("Em preparo.\n");
        break;

    case PRONTO:
        printf("Pronto.\n");
        break;

    default:
        break;
    }
}

void imprimirNomeDoItem(NomePedido nome)
{
    switch(nome) {
        case SANDUICHE_SIMPLES:
            printf("Sanduíche Simples:\n");
            break;
        case SANDUICHE_MEDIO:
            printf("Sanduíche Médio:\n");
            break;
        case SANDUICHE_ELABORADO:
            printf("Sanduíche Elaborado:\n");
            break;
        case BATATA_FRITA:
            printf("Batata Frita:\n");
            break;
        case REFRIGERANTE:
            printf("Refrigerante:\n");
            break;
        case MILK_SHAKE:
            printf("Milk Shake:\n");
            break;
        case SUCO:
            printf("Suco:\n");
            break;
        default:
            printf("Pedido desconhecido:\n");
            break;
    }
}

void imprimirItensPedido(ItemPedido item)
{
    printf("\n");
    imprimirNomeDoItem(item.nome);
    imprimirStatusItem(item.status);
}

void imprimirPedido(Pedido pedido)
{
    printf("\nPEDIDO %d\n", pedido.id);
    for(int i = 0; i<pedido.num_itens; i++)
    {
        imprimirItensPedido(pedido.itens[i]);
    }
    printf("Tempo de chegada: %ds.\n", pedido.tempo_chegada);
    printf("Tempo de preparo total: %ds.\n", pedido.tempo_preparo_total);
}

void imprimirLista(ListaPedidos lista)
{
    if(lista.cabeca == NULL)
    {
        printf("\nA fila está vazia!\n");
        return;
    }
    NodePedido *pivo = lista.cabeca;
    while(pivo != NULL)
    {
        imprimirPedido(pivo->pedido);
    }
    printf("Quantidade de pedidos: %d\n", lista.quantidade);
}

void diminuirCicloPedidos(int ciclo, ListaPedidos *lista)
{
    NodePedido *pivo = lista->cauda;
    while(pivo != NULL)
    {
        pivo->pedido.tempo_preparo_local-=ciclo;
        pivo->pedido.tempo_preparo_total-=ciclo; 
        pivo = pivo->prox;
    }
    return;
}

ItemPedido receberItem(NomePedido nome)
{
    ItemPedido item;
    
    if(nome == NADA)
    {
        item.status = PRONTO;
        item.tempo_preparo_total = 0;
        item.tempo_restante_preparo = 0;
    }
    else
    {
        item.status = AGUARDANDO_PREPARADO;
        item.tempo_preparo_total = calcularPreparo(nome);
        item.tempo_restante_preparo = item.tempo_preparo_total;
    
    }
    item.nome = nome;
    return item;
}


NodePedido *receberPedido(bool res_auto)
{
    Pedido novo_pedido;
    int item_comer, item_beber;

    for(int max_itens = 2; max_itens > 0; max_itens--){
        printf("\nEscolha ate 4 itens (2 de comer e 2 de beber)\n");
        printf("\n\n==== Itens de Comer ====\n");
        printf(" 1 - Sanduiche Simples\n 2 - Sanduiche Medio\n 3 - Sanduiche Elaborado\n 4 - Batata Frita\n 5 - Nao quero nada\n");
        printf("Resposta: ");
        item_comer = receberInput(res_auto, 5, 5);
        item_comer--; //Tornar resposta compatível com a lista

        printf("\n\n==== Itens de Beber ====\n");
        printf("1 - Refrigerante\n 2 - Milk-Shake\n 3 - Suco\n 4 - Nao quero nada\n");
        printf("Resposta: ");
        
        item_beber = receberInput(res_auto, 4, 4);
        item_beber += 4;//Tornar resposta compatível com a lista     
        int i = 4 / max_itens - 1;
        novo_pedido.itens[i] = receberItem(item_comer);
        novo_pedido.itens[i-1] = receberItem(item_beber);
    }
    NodePedido *node = criarNodePedido(novo_pedido);
    return node; //Retorna o ponteiro para o node do pedido
} 


/*#################### SEM DEFINICAO ########################*/

int calcularPreparo(NomePedido pedido) //Retorna o tempo de preparo em segundos
{
    switch(pedido)
    {
        case BATATA_FRITA:
            return 190; // 3 min e 10 seg

        case SANDUICHE_SIMPLES:
            return 58;

        case SANDUICHE_MEDIO:
            return 88; // 1 min e 28 seg

        case SANDUICHE_ELABORADO:
            return 105; // 1 min e 45 seg

        case REFRIGERANTE:
            return 5;

        case SUCO:
            return 38;

        case MILK_SHAKE:
            return 60; // 1 min
        case NADA:
            return 0;
        default:
            return 0;
    }
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

NodeFuncionario* encontrarFuncionario(ListaFuncionarios *lista, Habilidade habilidade_requerida, Locais *local)
{
    if(lista->cabeca == NULL)
    {
        return NULL;
    }

    NodeFuncionario *pivo;
    
    while(pivo != NULL)
    {
        for(int i = 1; i <= 3; i++)
        {
            if(pivo->funcionario.num_habilidade == i) //Escolhe funcionário capaz de executar menos funções
            {
                for(int j = 0; j < pivo->funcionario.num_habilidade; j++)
                {
                    if(pivo->funcionario.habilidades[j] == habilidade_requerida)
                    {
                        pivo->funcionario.status = OCUPADO;
                        printarLocalFuncionario(pivo->funcionario, local->nome);
                        pivo->funcionario.local_atual = local->nome;
                        return pivo;
                    }
                }
            }
        }
        pivo = pivo->prox;
    }

    return NULL;
}

/*
int tempo_restante(Pedido *pedido, Funcionario funcionario[], Equipamento equipamentos[])
{
    int tempo_preparo = pedido->itens[0].tempo_preparo_total;
    int local_vago;
    int tempo_funcionario;


    - tempo_preparo = tempo que ele demora para ser preparado
    - local_vago = tempo onde o local onde ele será preparado estará vazio 
    - tempo_funcionario = menor tempo em que teremos um funcionário apto a preparar;

    if (tempo_funcionario > local_vago)
    {
        tempo_restante = tempo_preparo + local_vago; 
    }
    else
    {
        tempo_restante = tempo_preparo + tempo_funcionario;
    }

    - Faça isso com todos os outros itens da lista:
    - Puxa um item do pedido;
    - tempo_preparo = tempo que ele demora para ser preparado
    - local_vago = tempo onde o local onde ele será preparado estará vazio 
    - tempo_funcionario = menor tempo em que teremos um funcionário apto a

    if (tempo_funcionario > local_vago)
    {
        if(tempo_restante < tempo_preparo + local_vago) tempo_restante = tempo_preparo + local_vago; 
        
    }
    else
    {
        if(tempo_restante < tempo_preparo + tempo_funcionario) tempo_restante = tempo_preparo + tempo_funcionario;
    }

    return 0;
    }
*/



// Funções relacionados a bandeja


void liberarFuncionario(ListaFuncionarios *reserva, NodeFuncionario *node_funcionario)
{
    node_funcionario->funcionario.status = LIVRE;
    printarLocalFuncionario(node_funcionario->funcionario, RESERVA);
    node_funcionario->funcionario.local_atual = RESERVA;
    adicionarListaFuncionario(reserva, node_funcionario);
    node_funcionario->funcionario.pedido_trabalhado = NULL;
}


bool todosItensProntos(Pedido pedido) {
    for(int i = 0; i < pedido.num_itens; i++) {
        if(pedido.itens[i].status != PRONTO) {
            return false;
        }
    }
    return true;
}


void atualizarPedidosParaMontagem(ListaPedidos *lista, ListaPedidos *fila_montagem) {
    NodePedido *pivo = lista->cabeca;
    while(pivo != NULL) {
        if(todosItensProntos(pivo->pedido) && pivo->pedido.status != AGUARDANDO_MONTAGEM) {
            pivo->pedido.status = AGUARDANDO_MONTAGEM;
            *fila_montagem = adicionarLista(*fila_montagem, pivo);
            printf("pedido %d pronto para montagem!\n", pivo->pedido.id);
        }
        pivo = pivo->prox;
    }
}

void printarLocal(NomeLocal local)
{
    switch(local)
    {
        case RECEPCAO:
            printf("Recepção");
            return;
        case MONTAR_BANDEJAS:
            printf("Montador de bandejas");
            return;
        case SEPARADOR:
            printf("Separador");
            return;
        case CAIXA:
            printf("Caixa");
            return;
        case RESERVA:
            printf("Reserva");
            return;
    }
}

void printarLocalFuncionario(Funcionario funcionario, NomeLocal novo_local)
{
    
    printf("Funcionario %d saiu de: ", funcionario.id);
    printarLocal(funcionario.local_atual);
    printf("e foi para: ");
    printarLocal(novo_local);
    printf(".\n");
}

void montarBandeja(Locais *m, ListaPedidos *reserva, int ciclo) {
    if(m->fila_espera.cabeca == NULL) return;

    NodeFuncionario *pivo = m->funcionario.cauda;
    NodePedido *pedidoNode = pivo->funcionario.pedido_trabalhado;
    Pedido *pedido = &pedidoNode->pedido;

    
    while(pivo != NULL)
    {
        pedido->tempo_preparo_local-=ciclo;
        if(pedido->tempo_preparo_local == 0)
        {
            printf("pedido %d foi ENTREGUE!\n", pedido->id);
            m->funcionario = removerListaFuncionario(m->funcionario, pivo);                    
        }
        pivo = pivo->prox;
    }

    pedidoNode = m->fila_espera.cabeca;
    pedido = &pedidoNode->pedido;

    NodeFuncionario *montador = encontrarFuncionario(reserva, HABILIDADE_MONTAGEM, m);
    while(montador != NULL && pedidoNode != NULL)
    {
        montador = encontrarFuncionario(reserva, HABILIDADE_CAIXA, m);
        printf("Funcionario %d iniciou montagem do pedido %d.\n", montador->funcionario.id, pedido->id);
        montador->funcionario.pedido_trabalhado = pedido;
        pedido->tempo_preparo_local = 30; // "tempo de preparo"
    }   
    pedido->status = ENTREGUE; //pedido montado
}

void recepcao(Locais *r, int ciclo, bool res_auto, ListaFuncionarios *reserva)
{
    NodeFuncionario *recepcionista = encontrarFuncionario(reserva, HABILIDADE_CAIXA, RECEPCAO);
    if(recepcionista == NULL)
    {
        printf("\nNão há recepcionístas disponíveis no momento!\n");
    }
    else
    {
        NodePedido *novo_pedido = receberPedido(res_auto); 
        recepcionista->funcionario.pedido_trabalhado = novo_pedido;
        adicionarListaFuncionario(&r->funcionario, recepcionista);
        novo_pedido->pedido.tempo_preparo_local = 30; //30 segundos para a recepção
        r->pedido_sendo_feitos = adicionarLista(r->pedido_sendo_feitos, novo_pedido);
    }
    diminuirCicloPedidos(ciclo, &r->pedido_sendo_feitos); //Como é a fila de espera, considera-se que ele já vai ser atendido.
    if(r->funcionario.cabeca->funcionario.pedido_trabalhado->pedido.tempo_preparo_local == 0)
    {
        NodeFuncionario *funcionario_livre;
        r->funcionario = removerListaFuncionario(r->funcionario, funcionario_livre);
        liberarFuncionario(reserva, funcionario_livre);
    } //Entende-se que a cabeça da fila sempre será preparada primeiro
}