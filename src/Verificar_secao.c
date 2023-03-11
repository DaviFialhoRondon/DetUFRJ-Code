#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "Detalhamento.h"
#include "Definition.h"

bool verificar_Secao( int regiao , struct viga *vPtr , int btEst)
{
    bool saida  ;   // Se TRUE - seção esta OK
    bool saida1 ;   // Se TRUE - seção tracao esta OK
    bool saida2 ;   // Se TRUE - seção compressao esta OK

    int    camada                 ; // (  ) Camada de barras na seção
    int    numeroBarrasFaltantes  ; // (  ) Número de barras faltantes na seção
    int    numBarrasSecao         ; // (  ) Número de barras calculadas na seção
    int    numBarrascalculadas    ; // (  ) Número de barras calculadas na camada
    double dReal                  ; // (m ) Altura útil calculada armadura de tracao
    double dReal2                 ; // (m ) Altura útil calculada armadura de compressao
    double bitolaSecao            ; // (mm) Bitola da seção
    double larguraEfetiva         ; // (m ) Largura Útil da viga
    double bitolaTransversal      ; // (mm) Bitola da Transversal
    double espacamentoVertical    ; // (m ) Espaçamento vertical entre face de barras
    double espacamentoHorizontal  ; // (m ) Espaçamento horizontal entre face de barras

    struct regiao_viga * vReg = &vPtr->regiao[regiao] ; //Simplificando a notação

    bitolaTransversal = bitolas_padrao[btEst] ;
    
    larguraEfetiva = vPtr->geo.largura - 2 * vPtr->geo.cob - 2 * (bitolaTransversal*0.001);

    if (vReg->maxMomentoCrit > 0) {
        numBarrasSecao = vReg->N_Lmin + vReg->N_comp;
        bitolaSecao    = vReg->Bt_pos;
    }
    else {
        numBarrasSecao = vReg->N_neg ;
        bitolaSecao    = vReg->Bt_neg;
    }

    numeroBarrasFaltantes = numBarrasSecao ;
    camada = 0 ;
    dReal  = 0 ;   

    // - Calculo do espaçamento Horizontal e Verical máximo
    espacamentoHorizontal = fmax(fmax(0.02,bitolaSecao * 0.001), 1.2 * vPtr->prop.dimAgreg) ;
    espacamentoVertical   = fmax(fmax(0.02,bitolaSecao * 0.001), 0.5 * vPtr->prop.dimAgreg) ;

    // - Calculo da altura util armadura de tracao
    while (numeroBarrasFaltantes > 0){
        if (camada == 0) numBarrascalculadas = fmin(maxbarras_camada(larguraEfetiva , espacamentoHorizontal , bitolaSecao*0.001),numBarrasSecao);
        else             numBarrascalculadas = 2 ;

        dReal = dReal + ( (double) numBarrascalculadas / ( (double) numBarrasSecao ) ) * ( ( (bitolaSecao*0.001)/2.0 ) + ( (double) camada * espacamentoVertical) ) ;

        numeroBarrasFaltantes = numeroBarrasFaltantes -  numBarrascalculadas ;
        camada++ ; 
    }
    dReal = vPtr->geo.altura - dReal - vPtr->geo.cob - (bitolaTransversal*0.001);

    if ((dReal) < vPtr->geo.d) saida1 = false ; 
    else                       saida1 = true ;
    
    // - Calculo da altura util armadura de compressao
    saida2 = 1;
    if ((vReg->maxMomentoCrit > 0) && (vReg->N_neg > 0)){

        numBarrasSecao        = vReg->N_neg;
        bitolaSecao           = vReg->Bt_neg;
        numeroBarrasFaltantes = numBarrasSecao;
        
        dReal2 = 0 ;
        camada = 0 ;

        while (numeroBarrasFaltantes >= 0){
            if (camada == 0) numBarrascalculadas = maxbarras_camada(larguraEfetiva , espacamentoHorizontal , bitolaSecao*0.001) ;
            else             numBarrascalculadas = 2 ;

            dReal2 = dReal2 + ( (double) numBarrascalculadas / ( (double) numBarrasSecao ) ) * ( ( (bitolaSecao*0.001)/2.0 ) + ( (double) camada * espacamentoVertical) ) ;

            numeroBarrasFaltantes = numeroBarrasFaltantes -  numBarrascalculadas ;
            camada++ ; 
        }

        dReal2 = dReal2 + vPtr->geo.cob + (bitolaTransversal*0.001);

        if (dReal2 < vPtr->geo.d2) saida2 = true ; 
        else                       saida2 = false  ;
    }

    else if ((vReg->maxMomentoCrit < 0) && (vReg->N_comp > 0)){

        numBarrasSecao        = vReg->N_comp;
        bitolaSecao           = vReg->Bt_pos;
        numeroBarrasFaltantes = numBarrasSecao;
     
        dReal2 = 0 ;
        camada = 0 ;

        while (numeroBarrasFaltantes >= 0){
            if (camada == 0) numBarrascalculadas = maxbarras_camada(larguraEfetiva , espacamentoHorizontal , bitolaSecao*0.001) ;
            else             numBarrascalculadas = 2 ;

            dReal2 = dReal2 + ( (double) numBarrascalculadas / ( (double) numBarrasSecao ) ) * ( ( (bitolaSecao*0.001)/2.0 ) + ( (double) camada * espacamentoVertical) ) ;

            numeroBarrasFaltantes = numeroBarrasFaltantes -  numBarrascalculadas ;
            camada++ ; 
        }

        dReal2 = dReal2 + vPtr->geo.cob + (bitolaTransversal*0.001);

        if (dReal2 < vPtr->geo.d2) saida2 = true ;
        else                       saida2 = false  ;
    }
       
    if ((saida1 == 0) ||(saida2 == 0)) saida = false  ;
    else                        saida = true ;

    if (vPtr->exibir.verSec == 1)
{   
    printf("**********  VERIFICACAO ALTURA UTIL REGIAO %d **********\n",regiao);  
    printf("Barras Secao critica: %d\n", numBarrasSecao);
    printf("Bitola: %.2f mm\n", bitolaSecao);
    printf("d Calculado Tracao = %.2f m\n", dReal);
    printf("d Projetado Tracao = %.2f m\n", vPtr->geo.d);
    printf("Ver Tracao = %d \n", saida);
    printf("d Calculado Comp = %.2f m\n", dReal2);
    printf("d Projetado Comp = %.2f m\n", vPtr->geo.d2);
    printf("Ver Comp = %d \n", saida2);
    if (saida == 1) printf("Altura util projetada - OK\n");
    else printf("Altura util projetada - NAO PASSA\n");
}
    return saida;
}