#include "Detalhamento.h"

int dist_arm_comp(int *barId , int regiao ,struct barras_viga *bPtr , int *barraPtr, struct viga * vPtr )
{
    int numBarra = *barraPtr;

    struct regiao_viga * vReg = &vPtr->regiao[regiao];

    int barrasCompressao;
    double bitola;
    double ancoragem;
    double momentoDivisao;

    int camada;
    int resultDiv[2];

    if (vReg->maxMomentoCrit < 0)
    {
        barrasCompressao = vReg->N_comp ;
        bitola = vReg->Bt_pos ;
        camada = 1; //camada inferior
    }
    else
    {
        barrasCompressao = vReg->N_neg ;
        bitola = vReg->Bt_neg ;
        camada = 0; //camada superior
    }
    ancoragem = ancoragem_necessaria( 1 , vReg->maxMomentoCrit, bitola*0.001 , vPtr->prop.fctk_inf, vPtr->prop.fyd ) ;

    //M' = As' * (d-d'')*fyd
    momentoDivisao = (barrasCompressao * pow(bitola*0.001,2) * M_PI /4 ) * (vPtr->geo.d-vPtr->geo.d2) * (vPtr->prop.fyd * 1000) ;

    comprimento_divisoes (resultDiv , momentoDivisao , regiao , vPtr );

    if (barrasCompressao == 0) return 0;

    bPtr[numBarra].numero     = *barId                                                              ;
    bPtr[numBarra].secaoIni   = - vPtr->prop.decalagem - ancoragem + vPtr->malha.n * (resultDiv[0]) ;
    bPtr[numBarra].secaoFim   =   vPtr->prop.decalagem + ancoragem + vPtr->malha.n * (resultDiv[1]) ;
    bPtr[numBarra].numBarras  = barrasCompressao                                                    ;
    bPtr[numBarra].ganchoD    = 0                                                                   ; //Armadura de compressão nao terá gancho
    bPtr[numBarra].ganchoE    = 0                                                                   ; //Armadura de compressão nao terá gancho
    bPtr[numBarra].bitola     = bitola                                                              ;
    bPtr[numBarra].camada     = camada                                                              ; //Camada superior
    bPtr[numBarra].tipoRegiao = vReg->tipoRegiao                                                    ;
    strcpy(bPtr[numBarra].id,"Arm Compressao")                                                      ; //Identificador de arm de compressao

    numBarra++;
    (*barId)++;

    *barraPtr = numBarra;

    return 0;
}



