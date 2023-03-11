#include "Detalhamento.h"

/*Alteração dia 22/01/23
    - Alterado o calculo das barras distribuidas para  (barrasMaxima / vPtr->numDiv) ao inveze de (vPtr->numDiv -1)
*/

int dist_neg_esq( int *barId , int regiao , struct barras_viga *bPtr , int *barraPtr , struct viga * vPtr )
{
    struct regiao_viga *vReg = &vPtr->regiao[regiao];
    int numBarra = *barraPtr;

    int result[2];
    double iniDiv[vPtr->numDiv];
    double fimDiv[vPtr->numDiv];

    double ancoragem ;  
    double momentoDivisao;  
    int barrasDistribuidas;  
    int barrasDivisao;  

    double larguraApoioE  = vReg->apoioE.largura ;
    double decalagem      = vPtr->prop.decalagem ;  
    double momentoCritico = vReg->maxMomentoCrit ;  
    double bitola         = vReg->Bt_neg         ;  // (mm) Bitola da região
    int limiteIni         = vReg->limiteIni      ;
    int limiteFim         = vReg->limiteFim      ;
    int barrasMaxima      = vReg->N_neg          ;  

    barrasDistribuidas = 0;

for (int divisao = 0 ; divisao < vPtr->numDiv ; divisao++)
{
    if (barrasDistribuidas < barrasMaxima){barrasDivisao = fmax( floor( barrasMaxima )/(vPtr->numDiv) , 1 ) ;}
    else{barrasDivisao = 0 ;}
    if (divisao == (vPtr->numDiv - 1)){barrasDivisao = barrasMaxima - barrasDistribuidas ;}
    if (barrasDivisao == 0){continue ;} 

    barrasDistribuidas = barrasDistribuidas + barrasDivisao ; //Atualizando o valor das barras distribuidas
    momentoDivisao     = barrasDistribuidas * ( fabs(momentoCritico) / barrasMaxima ) ; //Calculando o momento equivalente do número de barras distribuidas

    comprimento_divisoes (result, momentoDivisao, regiao , vPtr ) ;
    result[0] = limiteIni ; //Regiões negativas com apoio na esquerda serão limitadas pela seção limite da regiao

    iniDiv[divisao] = result[0] * vPtr->malha.n;
    fimDiv[divisao] = result[1] * vPtr->malha.n;

    ancoragem = ancoragem_necessaria( 1 , momentoCritico, bitola * 0.001 , vPtr->prop.fctk_inf, vPtr->prop.fyd ) ;

    if (divisao == 0) //O final + ancoragem + decalagem será comparadado com o limite da região
    {bPtr[numBarra].secaoFim = fmax(fimDiv[divisao]  + ancoragem + decalagem, (limiteFim * vPtr->malha.n) + decalagem + ancoragem) ;}
    
    else // O final + ancoragem + decalagem será comparadado com a seção final da divisão anterior
    { bPtr[numBarra].secaoFim = fmax(fimDiv[divisao]  + ancoragem + decalagem, fimDiv[divisao-1] + 10 * bitola * 0.0010 + decalagem) ;}
    
    bPtr[numBarra].secaoIni   = iniDiv[divisao] + vPtr->geo.cob - (larguraApoioE/2)  ;
    bPtr[numBarra].ganchoD    = 0                                                    ;
    bPtr[numBarra].ganchoE    = 1                                                    ;
    bPtr[numBarra].numBarras  = barrasDivisao                                        ;
    bPtr[numBarra].bitola     = bitola                                               ;
    bPtr[numBarra].numero     = *barId                                               ;
    bPtr[numBarra].camada     = 0                                                    ; //Camada superior
    bPtr[numBarra].tipoRegiao = 2                                                    ;
    strcpy(bPtr[numBarra].id,"Arm divisao (Esq) negativa"); //Identificador de arm

    numBarra++ ;
    (*barId)++ ;
}
    *barraPtr = numBarra;

    return 0;
}