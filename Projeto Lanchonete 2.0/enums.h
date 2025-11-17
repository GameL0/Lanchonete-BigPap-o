#ifndef ENUMS_H
#define ENUMS_H

typedef enum {
    PRESENCIAL,
    IFOOD
} TipoPedido;

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

typedef enum {
    RECEBIDO,
    PREPARANDO_ITENS,
    AGUARDANDO_MONTAGEM,
    EM_MONTAGEM,
    ENTREGUE
} StatusPedido;

typedef enum {
    AGUARDANDO_PREPARADO,
    EM_PREPARO,
    PRONTO
} StatusItem;

typedef enum {
    HABILIDADE_CAIXA,
    HABILIDADE_SEPARACAO,
    HABILIDADE_SANDUICHE,
    HABILIDADE_BATATA,
    HABILIDADE_SUCO,
    HABILIDADE_BEBIDA,
    HABILIDADE_MONTAGEM
} Habilidade;

typedef enum {
    LIVRE,
    OCUPADO
} StatusFuncionario;

typedef enum {
    RECEPCAO,
    MONTAR_BANDEJAS,
    SEPARADOR,
    CAIXA, 
    RESERVA
} NomeLocal;

typedef enum {
    PENEIRA,
    CHAPA,
    LIQUIDIFICADOR_MILK_SHAKE,
    LIQUIDIFICADOR_SUCO
} NomeEquipamento;

#endif