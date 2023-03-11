#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "Detalhamento.h"

void div_reg(struct viga *vPtr  )
{
    int pontoMalha;

    int raizes [NUM_MAX_SECOES] ; // Vetor com as raízes do diagrama de momento decalado    

    int numRaizes = 0          ; // Número de raízes totais do diagrama de momento decalado

    double momentoAntes = vPtr->solicit[0].Md_dec  ;
    double momentoAtual = vPtr->solicit[1].Md_dec  ;

    int apoioSaida[2] ;

    raizes[numRaizes++] = 0 ;

    for (pontoMalha = 1; pontoMalha < (vPtr->malha.N-1); pontoMalha++)
    {
        if ( (momentoAtual >= 0 && momentoAntes < 0) || ( momentoAtual <= 0 &&  momentoAntes > 0 )) 
        {
            raizes[numRaizes++] = pontoMalha ;
        } 
        momentoAntes = vPtr->solicit[pontoMalha -1 ].Md_dec ;
        momentoAtual = vPtr->solicit[pontoMalha    ].Md_dec ;
    }
    
    raizes[numRaizes++] = pontoMalha;

    vPtr->malha.numRegioes = numRaizes - 1                      ; // Número de regiões do diagrama de momento decalado

    for (int regiao = 0; regiao < vPtr->malha.numRegioes; regiao++)
    {
        struct regiao_viga * vReg   = &vPtr->regiao[regiao] ;

        vReg->numero    = regiao;
        vReg->limiteIni = raizes[regiao    ];
        vReg->limiteFim = raizes[regiao + 1];

    // - MOMENTO CRÍTICO
        vReg->maxMomentoCrit = 0;

        for (int ponto = vReg->limiteIni ; ponto <= vReg->limiteFim; ponto++)
        {
            if ( fabs ( vPtr->solicit[ ponto ].Md_dec  ) > vReg->maxMomentoCrit )
            {
                vReg->maxMomentoCrit = fabs ( vPtr->solicit[ ponto ].Md_dec  ) ; 
                vReg->secaoCritM  = ponto ; 
            }
        }

        vReg->maxMomentoCrit = vPtr->solicit[ vReg->secaoCritM ].Md_dec  ;

    // - CORTANTE CRÍTICO
        vReg->maxCortanteCrit = 0;

        for (int ponto = vReg->limiteIni ; ponto < vReg->limiteFim; ponto++)
        {
            if ( fabs ( vPtr->solicit[ ponto ].Vd  ) > vReg->maxCortanteCrit )
            {
                vReg->maxCortanteCrit = fabs ( vPtr->solicit[ ponto ].Vd  ) ; 
                vReg->secaoCritV  = ponto ; 
            }
        }

        vReg->maxCortanteCrit = vPtr->solicit[ vReg->secaoCritV ].Vd  ;

    // - DESCOBRIR OS APOIOS 

        descobrir_apoio(vReg->secaoCritM , vPtr, apoioSaida) ;

        vReg->apoioE.secao    = vPtr->apoio   [ apoioSaida[0]      ].secaoApoios    ;
        vReg->apoioE.largura  = vPtr->apoio   [ apoioSaida[0]      ].larguraApoios  ;
        vReg->apoioE.Momento  = vPtr->solicit [ vReg->apoioE.secao ].Md_dec  ;
        vReg->apoioE.Cortante = vPtr->solicit [ vReg->apoioE.secao ].Vd      ;

        vReg->apoioD.secao    = vPtr->apoio   [ apoioSaida[1]      ].secaoApoios    ;
        vReg->apoioD.largura  = vPtr->apoio   [ apoioSaida[1]      ].larguraApoios  ;
        vReg->apoioD.Momento  = vPtr->solicit [ vReg->apoioD.secao ].Md_dec       ;
        vReg->apoioD.Cortante = vPtr->solicit [ vReg->apoioD.secao ].Vd           ;


        // * Balanços
        if (  (vReg->apoioE.secao > 0 )  &&  ( vReg->numero == 0 )  )                                                      { vReg->tipoRegiao = balancoEsquerda      ;}
        else if (  (vReg->apoioD.secao < (vPtr->malha.N - 1) )  &&  ( vReg->numero == (vPtr->malha.numRegioes - 1) )  )          { vReg->tipoRegiao = balancoDireita       ;}

        // * Região negativa
        else if (vReg->maxMomentoCrit < 0 && (vReg->apoioE.secao == 0) )                                                   { vReg->tipoRegiao = negativaEsquerda      ;} 
        else if (vReg->maxMomentoCrit < 0 && (vReg->apoioD.secao == (vPtr->malha.N - 1)) )                                 { vReg->tipoRegiao = negativaDireita       ;}  
        else if (vReg->maxMomentoCrit < 0 && ( (vReg->apoioE.secao != 0) && (vReg->apoioD.secao != (vPtr->malha.N - 1))) ) { vReg->tipoRegiao = negativaIntermediaria ;} 
            
        // * Região positiva
        else if (vReg->maxMomentoCrit > 0 && (  vReg->apoioE.secao == 0) )                                                 {vReg->tipoRegiao = positivaEsquerda ;} 
        else if (vReg->maxMomentoCrit > 0 && (  vReg->apoioD.secao == (vPtr->malha.N - 1)) )                               {vReg->tipoRegiao = positivaDireita ;}  
        else if (vReg->maxMomentoCrit > 0 && ( (vReg->apoioE.secao != 0) && (vReg->apoioD.secao != (vPtr->malha.N - 1))) ) {vReg->tipoRegiao = positivaIntermediaria ;}  
        
    }

    if (vPtr->exibir.divReg == 1)
    {
        printf("--------------------------------------------------\n");
        printf("-----------------DIVISAO_REGIOES------------------\n");
        printf("Numero de Regioes : %d\n\n",vPtr->malha.numRegioes);

        for (int regiao = 0; regiao < vPtr->malha.numRegioes ; regiao++){
        struct regiao_viga * vReg   = &vPtr->regiao[regiao] ;

        printf("Regiao = %d  [%d : %d] \n",vReg->numero,vReg->limiteIni,vReg->limiteFim);
        printf("Secao Critica Momento  = %d  e Momento  critico = %f \n",vReg->secaoCritM , vReg->maxMomentoCrit);
        printf("Secao Critica Cortante = %d  e Cortante critico = %f \n",vReg->secaoCritV , vReg->maxCortanteCrit);
        printf("Apoio E: Secao = %d | Largura = %f | Momento = %f | Cortante = %f \n",vReg->apoioE.secao,vReg->apoioE.largura,vReg->apoioE.Momento,vReg->apoioE.Cortante);    
        printf("Apoio D: Secao = %d | Largura = %f | Momento = %f | Cortante = %f \n",vReg->apoioD.secao,vReg->apoioD.largura,vReg->apoioD.Momento,vReg->apoioD.Cortante);    
        printf("Tipo de regiao = %d\n",vReg->tipoRegiao);

        printf("--------------------------------------------------\n\n");
        }
    }


}