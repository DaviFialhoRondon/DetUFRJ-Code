#include "Definition.h"

#ifndef STRUCT_VIGA_H
#define STRUCT_VIGA_H

enum tipoRegiao {
balancoEsquerda        , //[0] Região de balanço à esquerda da viga
balancoDireita         , //[1] Região de balanço à direita da viga
negativaEsquerda       , //[2] Região de momento negativo com apoio final à esquerda
negativaDireita        , //[3] Região de momento negativo com apoio final à direita
negativaIntermediaria  , //[4] Região de momento negativo no meio da viga
positivaEsquerda       , //[5] Região de momento positivo com apoio final à esquerda
positivaDireita        , //[6] Região de momento positivo com apoio final à direita
positivaIntermediaria    //[7] Região de momento positivo no meio da viga
};

struct apoioRegiao
{
    int    secao    ; // (    ) Seção do apoio
    double largura  ; // (  m ) Largura do apoio
    double Cortante ; // ( kN ) Cortante do apoio
    double Momento  ; // ( kNm) Momento do apoio
};

// Regiões das viga
struct regiao_viga
{
    int numero        ; // (    ) Número de referência da regiao
    int limiteIni     ; // (    ) Seção em que a região se inicial
    int limiteFim     ; // (    ) Seção em que a região termina
    int secaoCritM     ; // (    ) Seção do momento máximo
    int secaoCritV     ; // (    ) Seção do momento máximo
    double maxMomentoCrit   ; // (kNm ) Valor do momento máximo
    double maxCortanteCrit  ; // (kNm ) Valor do momento máximo
    int   N_neg    ; //     Número de barras para armadura negativo      [N]
    int   N_const  ; //     Número de barras para armadura construtiva   [N]
    int   N_comp   ; //     Número de barras para armadura complementar  [N]
    int   N_Lmin   ; //     Número de barras para armadura mínima        [N]
    double Bt_neg   ; // (mm) Bitola das barras para armadura negativo    [N]
    double Bt_pos   ; // (mm) Bitola das barras para armadura mínima      [N]
    double Bt_const ; // (mm) Bitola das barras para armadura construtiva [N]

    enum tipoRegiao tipoRegiao ; // (    ) Tipo da região

    struct apoioRegiao apoioE ;
    struct apoioRegiao apoioD ;
};

struct malha
{
    int     N          ; //     Númerto de pontos na malha
    double  n          ; // (m) Espaçamento entre pontos da malha
    int     numRegioes ; //     Número de regiões no diagrama de momento fletor 
};

struct solicitacoes
{
    double Vd     ; // (kN)  Cortante ELU da análise [N]
    double Md_dec ; // (kNm) Momento ELU da análise  [N]
    double VFreq  ; // (kN)  Cortante frequente      [N]
    double MFreq  ; // (kNm) Momento frequente       [N]
};

struct geometria
{
    double altura      ; // (m) altura da viga
    double largura     ; // (m) largura da viga
    double comprimento ; // (m) comprimento da viga
    double cob         ; // (m) cobrimento da armadura
    double d           ; // (m) altura útil da viga
    double d2          ; // (m) distância face - armadura superior
};

struct propriedades
{
    double Ec        ; // (GPa) Módulo de elasticidade do concreto
    double Es        ; // (GPa) Módulo de elasticidade do aço
    double Ic        ; // (m^4) Momento de inércia da seção de concreto
    double fck       ; // (MPa) Resistência a compressão do concreto
    double fctk      ; // (MPa) Resistência a tração do concreto
    double fct_m     ; // (MPa) Resistência a tração média do concreto
    double fctk_inf  ; // (MPa) Resistência a tração inferior do concreto
    double fctk_sup  ; // (MPa) Resistência a tração superior do concreto
    double fyd       ; // (MPa) Resistência a tração/compressão das barras longitudinais
    double fywd      ; // (MPa) Resistência a tração/compressão das barras transversais
    double decalagem ; // ( m ) Decalagem do diagrama de momento fletor
    double dimAgreg  ; // ( m ) Dimensão máxima agregado graúdo
    double angBiela  ; // ( m ) Ângulo da biela de concreto

    int classeAgressividade ; // Classe de agressividade do meio
};

struct armaduras
{
    double A_neg    ; // (  m²   )   Armadura longitudinal de tração em cada seção             [N]
    double A_const  ; // (  m²   )   Armadura longitudinal construtiva em cada seção           [N]
    double A_comp   ; // (  m²   )   Armadura longitudinal positiva complementar em cada seção [N]
    double Asw      ; // (  m²/m )   Armadura transversal em cada seção                        [N]
};

struct apoio
{
    int     secaoApoios   ; // ( ) Seção em que estão os eixos dos apoios [NumApoios]
    double  larguraApoios ; // (m) Largura dos apoios                     [NumApoios]
};

struct exibir
{
    int divReg   ; // Exibir resultado da divisao das regioes
    int dimens   ; // Exibir resultado da divisao do dimensionamento
    int bUtil    ; // Exibir resultado das barras utilizadas
    int verElsw  ; // Exibir resultado da verificao ELSW
    int verSec   ; // Exibir resultado da verificao seção
    int detal    ; // Exibir resultado detalhamento
    int detalAdv ; // Exibir resultado de cada iteração melhor
};

struct viga
{
    int    numDiv    ; // (   ) Número de divisões realizados para cobrimento do diagrama
    int    numApoios ; // (   ) Número de apoios
    double Al_min    ; // ( m²) Armadura longitudinal mínima
    double Asw_min   ; // ( m²) Armadura transversal mínima
    double Md_min    ; // (kNm) Momento mínimo
    int btPos[7]     ; // (   ) Bitolas positivas habilitadas
    int btNeg[7]     ; // (   ) Bitolas negativas habilitadas
    int btEst[7]     ; // (   ) Bitolas estribos  habilitadas


    struct geometria    geo                          ;
    struct propriedades prop                         ;
    struct solicitacoes solicit [ NUM_MAX_SECOES  ]  ;
    struct armaduras    arm     [ NUM_MAX_REGIOES ]  ;
    struct regiao_viga  regiao  [ NUM_MAX_REGIOES ]  ;
    struct apoio        apoio   [ NUM_MAX_APOIOS  ]  ;
    struct malha        malha                        ;
    struct exibir       exibir                       ;
};

#endif