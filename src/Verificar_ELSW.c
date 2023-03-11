#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "Detalhamento.h"
#include "Definition.h"

bool verificar_ELS_W( int regiao , struct viga * vPtr , int btEst )
{
    bool saida; //retorna se a verificação esta ok ou não

    int    numBarrasTracao     ; // (   ) Numero de barras em tração da seção
    int    numBarrasCompressao ; // (   ) Numero de runbarras em compressão da seção
    int    maxBarrasCamada     ; // (   ) Numero de barras máximo na camada inferior
    int    secaoCritica        ; // (   ) Seções em que há momento máximo em cada região

    float  wkMax              ; // ( mm ) Abertura máxima de fissura
    float  bitolaLongitudinal ; // (m  ) Bitola longitudinal da seção
    float  bitolaTransversal  ; // (m  ) Bitola trasnversal da seção

    double wk1                      ; // ( mm ) Parâmetro para calculo da expressão da fissuração 
    double wk2                      ; // ( mm ) Parâmetro para calculo da expressão da fissuração
    double wk                       ; // ( mm ) Abertura de fissura
    double taxaArmadura             ; // (m²/m) Taxa de armadura no tirante de armadura na seção NBR6118:2014 Seção 17.3.3.2
    double sigmaS                   ; // (MPa) Tensão na armadura de um tirante
    double linhaNeutra2             ; // (m  ) Linha neutra no estádio II
    double inercia2                 ; // (m4 ) Inercia no estádio II
    double momentoMaximo            ; // (kNm) Valor do momento máximo em cada região para combinação frequente
    double armaduraTracao           ; // (m² ) Armadura efetiva Longidudinal na parte tracionada
    double armaduraCompressao       ; // (m² ) Armadura efetiva Longidudinal na parte comprimida
    double larguraEfetiva           ; // (m  ) Largura Útil da viga
    double espacamentoHorizontalMin ; // (m  ) Espaçamento horizonalt máximo NBR6118:2014 Seção 18.3.2.2
    double espacamentoHorizontal    ; // (m  ) Espaçamento horizonalt calculado entre as barras
    double espacamentoVertical      ; // (m  ) Espaçamento vertical NBR6118:2014 Seção 18.3.2.2


    float  alphaE = 15  ; // (   ) Parâmetro divisão do modulo de young do aço contra o modulo secante do concreto (Norma estabelce como 15)
    float  neta1 = 2.25 ; // (   ) Coeficiente de aderência (Tabela 8.3 NBR6118:2014)


    // Abertura de dissura máximo
    switch (vPtr->prop.classeAgressividade)
    {
    case 1 : wkMax = 0.4 ; break;
    case 2 : wkMax = 0.3 ; break;    
    case 3 : wkMax = 0.3 ; break; 
    case 4 : wkMax = 0.2 ; break;           
    }

    struct regiao_viga *vReg = &vPtr->regiao[regiao];

        bitolaTransversal = bitolas_padrao[btEst] * 0.001 ; 
        secaoCritica      = vReg->secaoCritM    ;
        momentoMaximo     = fabs(vPtr->solicit[secaoCritica].MFreq) ;
        larguraEfetiva    = vPtr->geo.largura - 2 * vPtr->geo.cob - 2 * bitolaTransversal ;
  
    // - número de barras, armadura de tracao e compressao na seção crítica
        if (vReg->maxMomentoCrit > 0)
        {
            bitolaLongitudinal  = vReg->Bt_pos  * 0.001 ;
            numBarrasCompressao = vReg->N_neg  ;
            numBarrasTracao     = vReg->N_comp + vReg->N_Lmin ;

            armaduraTracao     = numBarrasTracao      * pow ( vReg->Bt_pos * 0.001 , 2 ) * 0.25 * M_PI ;
            armaduraCompressao = numBarrasCompressao  * pow ( vReg->Bt_neg * 0.001 , 2 ) * 0.25 * M_PI ;
        }
        else
        {
            bitolaLongitudinal  = vReg->Bt_neg  * 0.001  ;
            numBarrasTracao     = vReg->N_neg  ;
            numBarrasCompressao = vReg->N_comp + vReg->N_Lmin ;

            armaduraTracao     = numBarrasTracao      * pow ( vReg->Bt_neg * 0.001 , 2 ) * 0.25 * M_PI ;
            armaduraCompressao = numBarrasCompressao  * pow ( vReg->Bt_pos * 0.001 , 2 ) * 0.25 * M_PI ;    
        }

    // - Espaçamento para cada seção
        espacamentoHorizontalMin = fmax(fmax(0.02,bitolaLongitudinal * 0.001), 1.2 * vPtr->prop.dimAgreg) ;
        
        // Máximo de barras na primeira camada
        maxBarrasCamada = maxbarras_camada( larguraEfetiva , espacamentoHorizontalMin , bitolaLongitudinal ) ;

        // Espaçamento entre barras na primeira camada
        espacamentoHorizontal    = fmax ( espacamentoHorizontalMin , espacamento_camada ( larguraEfetiva , maxBarrasCamada , bitolaLongitudinal ) ) ;
        espacamentoVertical      = fmax ( fmax(0.02,bitolaLongitudinal), 0.5 * vPtr->prop.dimAgreg ) ;

    // - Taxa de armadura na seção 
        taxaArmadura = calc_taxa_armadura ( vPtr->geo.cob, bitolaLongitudinal , bitolaTransversal , espacamentoHorizontal , espacamentoVertical ) ;

    // - Linha neutra e Inercia no estádio 2 
        linhaNeutra2 = linha_neutra_2 ( alphaE , armaduraTracao , armaduraCompressao , vPtr->geo.largura , vPtr->geo.d , vPtr->geo.d2 ) ;
        inercia2     = inercia_2      ( alphaE , armaduraTracao , armaduraCompressao , vPtr->geo.largura , vPtr->geo.d , vPtr->geo.d2 ) ;


    // - Calcular a tensão máxima nas BARRAS
        sigmaS = 0.001 * ( momentoMaximo * fabs( linhaNeutra2 - vPtr->geo.d ) ) / inercia2  ; // Verificando somente a armadura inferior     

    // - Abertura de fissura
        wk1 = ( bitolaLongitudinal * sigmaS * 3 * sigmaS ) / ( 12.5 * neta1 * vPtr->prop.Es * vPtr->prop.fct_m ) ;

        wk2 = ( ( bitolaLongitudinal * sigmaS ) / ( 12.5 * neta1 * vPtr->prop.Es ) ) * ( ( 4 / taxaArmadura) + 45.0 ) ;

        wk = fmin( wk1, wk2 ) ;

        if (wk > wkMax) saida = false ;
        else saida = true ;
        
    if (vPtr->exibir.verElsw == 1)
    {
        printf("********** VERIFICACAO ELSW REGIAO %d **********\n",regiao);
        printf("\n");
        printf("Secao critica       = %d             \n " , secaoCritica             );
        printf("Momento maximo      = %f     (kNm)   \n " , momentoMaximo            );
        printf("Bitola Longitudinal = %f     ( m )   \n " , bitolaLongitudinal       );
        printf("NumBarrasTracao     = %d             \n " , numBarrasTracao          );   
        printf("NumBarrasCompressao = %d             \n " , numBarrasCompressao      );   
        printf("Max Barras camada   = %d             \n " , maxBarrasCamada          );   
        printf("Espacamento min     = %f     ( m )   \n " , espacamentoHorizontalMin );
        printf("Espacamento Horiz   = %f     ( m )   \n " , espacamentoHorizontal    );   
        printf("Espacamento Vert    = %f     ( m )   \n " , espacamentoVertical      );   
        printf("Taxa armadura       = %f    (m2/m2)  \n " , taxaArmadura             ); 
        printf("Armadura Tracao     = %f     (m2)    \n " , armaduraTracao           ); 
        printf("Armadura Compr      = %f     (m2)    \n " , armaduraCompressao       ); 
        printf("Linha Neutra 2      = %f     (m )    \n " , linhaNeutra2             ); 
        printf("Inercia 2           = %f     (m4)    \n " , inercia2                 ); 
        printf("SigmaS              = %f     (MPa)   \n " , sigmaS                   );        
        printf("Es                  = %f     (GPa)   \n " , vPtr->prop.Es            );        
        printf("fct_m               = %f     (MPa)   \n " , vPtr->prop.fct_m         );        
        printf("wk1                 = %0.10f (mm)    \n " , wk1                      ); 
        printf("wk2                 = %0.10f (mm)    \n " , wk2                      ); 
        printf("wk                  = %0.10f (mm)    \n " , wk                       ); 
        printf("wk Max              = %0.10f (mm)    \n " , wkMax                    ); 
        printf("\n\n");
    }
              
    return saida;

}
