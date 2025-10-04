#ifndef ENUMS_H
#define ENUMS_H

// Enum para os tipos de itens do pedido
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

// Enum para o status geral de um pedido
typedef enum {
    RECEBIDO,
    PREPARANDO_ITENS,
    AGUARDANDO_MONTAGEM,
    EM_MONTAGEM,
    ENTREGUE
} StatusPedido;

// Enum para o status de um item individual dentro de um pedido
typedef enum {
    AGUARDANDO_PREPARADO,
    EM_PREPARO,
    PRONTO
} StatusItem;

// Enum para as habilidades dos funcionários
typedef enum {
    HABILIDADE_CAIXA,
    HABILIDADE_SEPARACAO,
    HABILIDADE_SANDUICHE,
    HABILIDADE_BATATA,
    HABILIDADE_SUCO,
    HABILIDADE_BEBIDA, // Para Milk-shake
    HABILIDADE_MONTAGEM
} Habilidade;

// Enum para o status de um funcionário
typedef enum {
    LIVRE,
    OCUPADO
} StatusFuncionario;

// Enum para os locais da lanchonete
typedef enum {
    RECEPCAO,
    MONTAR_BANDEJAS,
    SEPARADOR,
    CAIXA, // Pode ser usado se o caixa for um local físico
    RESERVA
} NomeLocal;

// Enum para os equipamentos de preparo
typedef enum {
    PENEIRA,
    CHAPA,
    LIQUIDIFICADOR_MILK_SHAKE,
    LIQUIDIFICADOR_SUCO
} NomeEquipamento;

#endif // ENUMS_H