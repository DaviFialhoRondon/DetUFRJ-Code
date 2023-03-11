#ifndef STRUCT_BARRA_H
#define STRUCT_BARRA_H

// Divisão da viga para o detalhamento longitudinal
struct barras_viga
{
    int    numero    ; // (    ) Número de referência da barras
    int    numBarras ; // (    ) Quantidade de barras na divisao
    double secaoIni  ; // (    ) Seção em que a barra se inicial
    double secaoFim  ; // (    ) Seção em que a barra terminada
    double bitola    ; // ( mm ) Bitola da barra
    int    camada    ; // (    ) Indicação -> superior | 1-> inferior
    bool   ganchoE   ; // (    ) Existencia de gancho na esquerda
    bool   ganchoD   ; // (    ) Existencia de gancho na direita
    char   id[50]    ; // (    ) Identificador

    enum tipoRegiao tipoRegiao ; // (    ) Tipo da região
};

struct estribos_viga
{
    int    numero ; // (    ) Número de referência da barras
    int    pernas ; // (    ) Número de pernas do estribo
    int    quant  ; // (    ) Quantidade de estribos na regiao
    double espac  ; // ( cm ) Espaçamento entre estribo na regiao
    double perim  ; // ( m  ) Perímetro de cada estribo da regiao
    double ini    ; // ( m  ) Seção em que a barra se inicial
    double fim    ; // ( m  ) Seção em que a barra terminada
    double bitola ; // ( mm ) Bitola do estribos

    enum tipoRegiao tipoRegiao ; // (    ) Tipo da região
};





#endif