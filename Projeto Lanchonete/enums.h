#ifndef ENUMS_H
#define ENUMS_H

// Enum para o status de um item individual do pedido
typedef enum {
    AGUARDANDO_PREPARADO,
    EM_PREPARO,
    PRONTO
} StatusItem;

// Enum para o status geral de um pedido completo
typedef enum {
    RECEBIDO,
    PREPARANDO_ITENS,
    AGUARDANDO_MONTAGEM,
    ENTREGUE,
    ATRASADO,
    ARMAZENADO
} StatusPedido;

typedef enum {
    PENEIRA,
    CHAPA,
    LIQUIDIFICADOR_MILK_SHAKE,
    LIQUIDIFICADOR_SUCO
} NomeEquipamento;

typedef enum {
    RECEPCAO,
    MONTAR_BANDEJAS,
    SEPARADOR,
    CAIXA,
    RESERVA
} NomeLocal;

// Enum para o status de um funcionário
typedef enum {
    LIVRE,
    OCUPADO
} StatusFuncionario;

typedef enum {
    SANDUICHE_SIMPLES,
    SANDUICHE_MEDIO,
    SANDUICHE_ELABORADO,
    BATATA_FRITA,
    REFRIGERANTE,
    MILK_SHAKE,
    SUCO,
    NADA
} NomePedido;

// Enum para as diferentes habilidades que um funcionário pode ter
typedef enum {
    HABILIDADE_CAIXA,
    HABILIDADE_SEPARACAO,
    HABILIDADE_SANDUICHE,
    HABILIDADE_BATATA,
    HABILIDADE_SUCO,
    HABILIDADE_BEBIDA, // Refrigerantes e Milk-shakes
    HABILIDADE_MONTAGEM
} Habilidade;

#endif // ENUMS_H
