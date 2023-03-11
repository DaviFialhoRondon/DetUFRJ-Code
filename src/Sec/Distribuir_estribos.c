#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "Detalhamento.h"
#include "Definition.h"

void distribuir_estribos(int regiao , double bitola , struct viga *vPtr , struct estribos_viga *estPtr ){

    int    quant;
    double espacamento;
    double areaBitola;
    double iniSecao;
    double fimSecao;
    double perim;

    struct estribos_viga *estReg = &estPtr[regiao];

// - Calculo do espacamento
    if (bitola ==  6.3) {areaBitola = 0.315;}
    if (bitola ==  8.0) {areaBitola = 0.500;}
    if (bitola == 10.0) {areaBitola = 0.800;}
    if (bitola == 12.5) {areaBitola = 1.250;}
    if (bitola == 16.0) {areaBitola = 2.000;}
    if (bitola == 20.0) {areaBitola = 3.140;}
    if (bitola == 25.0) {areaBitola = 5.000;}
    if (bitola == 32.0) {areaBitola = 8.000;}


    espacamento = floor(100 * areaBitola * NUM_PERNAS_ESTR)/(vPtr->arm[regiao].Asw * 10000) ;
    if ((fmod(espacamento,INTER_PAD_ESTR)) != 0) espacamento = floor(espacamento/INTER_PAD_ESTR)*INTER_PAD_ESTR ;
    espacamento = fmin(espacamento,20);
    
// - Calculo do perímetro do estribos
    perim = 2*(vPtr->geo.altura + vPtr->geo.largura - 4 * vPtr->geo.cob);

// - Calculo das seções iniciais e finais
    if (vPtr->regiao[regiao].tipoRegiao == 2){
        iniSecao = vPtr->regiao[regiao].limiteIni + vPtr->regiao[regiao].apoioE.largura/2;
        fimSecao = vPtr->regiao[regiao].limiteFim;
    }

    else if (vPtr->regiao[regiao].tipoRegiao == 3){
        iniSecao = vPtr->regiao[regiao].limiteIni;
        fimSecao = vPtr->regiao[regiao].limiteFim - vPtr->regiao[regiao].apoioD.largura/2;
    }

    else{
        iniSecao = vPtr->regiao[regiao].limiteIni;
        fimSecao = vPtr->regiao[regiao].limiteFim;
    }

    iniSecao = iniSecao * vPtr->malha.n;
    fimSecao = fimSecao * vPtr->malha.n;
    
// - Calculo das quantidades
    quant = ceil( (fimSecao - iniSecao)/(espacamento*0.01) );

// - Atribuição ao vetor de estribos

    estReg->numero     = regiao  ;
    estReg->pernas     = NUM_PERNAS_ESTR;
    estReg->quant      = quant;
    estReg->espac      = espacamento;
    estReg->perim      = perim;
    estReg->ini        = iniSecao;
    estReg->fim        = fimSecao;
    estReg->bitola     = bitola;
    estReg->tipoRegiao = vPtr->regiao[regiao].tipoRegiao;
    
}