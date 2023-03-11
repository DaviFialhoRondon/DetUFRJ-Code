#include "Detalhamento.h"


int dist_pos_interm(int *barId , int regiao , struct barras_viga *bPtr , int *barraPtr , struct viga * vPtr )
{
    struct regiao_viga *vReg = &vPtr->regiao[regiao];
    int numBarra = *barraPtr;

    int result[2];
    double iniDiv[vPtr->numDiv];
    double fimDiv[vPtr->numDiv];

    double ancoragem ;  
    double armaduraVao;
    double momentoDivisao;  
    double armaduraApoio ;
    int barrasDistribuidas;  
    int barrasDivisao;  
    int barrasApoio ;

    int secaoApoioE       = vReg->apoioE.secao    ;
    int secaoApoioD       = vReg->apoioD.secao    ;
    int limiteIni         = vReg->limiteIni       ;
    int limiteFim         = vReg->limiteFim       ;
    int barrasMaxima      = vReg->N_neg           ;  
    double bitola         = vReg->Bt_pos          ;  
    double larguraApoioE  = vReg->apoioE.largura  ;
    double larguraApoioD  = vReg->apoioD.largura  ;
    double decalagem      = vPtr->prop.decalagem  ;  
    double momentoCritico = vReg->maxMomentoCrit  ;  

    armaduraVao   = vPtr->Al_min  + vPtr->arm[regiao].A_comp  ;
    barrasMaxima  = vReg->N_Lmin + vReg->N_comp ;

    // Calculando o número de barras estabelcido por norma que deve ser ancorado no apoio 
    armaduraApoio  = armadura_apoio( regiao , armaduraVao , vPtr);
    barrasApoio    = ceil(armaduraApoio/(0.25*M_PI*pow(bitola*0.001,2)));

    // As barras ancoradas no apoio serão ou o estabelecido por norma ou a armadura mínima
    barrasApoio        = fmax (armaduraApoio , vReg->N_Lmin )  ;
    barrasDistribuidas = barrasApoio ;

    // Caso tenha-se somente uma divisão, a armadura inicial deve ser a total
    if (vPtr->numDiv == 1) barrasDistribuidas = barrasMaxima;

    //Nas regiões extremas, calcular ancoragem sem gancho e verificar se é suficiente 
    ancoragem = ancoragem_necessaria( 0 , momentoCritico, bitola*0.001 , vPtr->prop.fctk_inf, vPtr->prop.fyd ) ;

    bPtr[numBarra].numero     = *barId                                                    ;
    bPtr[numBarra].secaoIni   = secaoApoioE * vPtr->malha.n - ancoragem + (larguraApoioE/2) ;  
    bPtr[numBarra].secaoFim   = secaoApoioD * vPtr->malha.n + ancoragem - (larguraApoioD/2) ;
    bPtr[numBarra].numBarras  = barrasDistribuidas                                          ;
    bPtr[numBarra].ganchoE    = 0                                                           ;
    bPtr[numBarra].ganchoD    = 0                                                           ;
    bPtr[numBarra].bitola     = bitola                                                      ;
    bPtr[numBarra].camada     = 1                                                           ; //Camada inferior   
    bPtr[numBarra].tipoRegiao = 7                                                           ; 
    strcpy(bPtr[numBarra].id,"Arm inicial (Interm) positiva"); //Identificador de arm

    numBarra++;
    (*barId)++;
    
// - CALCULO DAS DIVISÕES
for (int divisao = 0 ; divisao < vPtr->numDiv-1 ; divisao++)
{
    if (barrasDistribuidas < barrasMaxima){barrasDivisao = fmax( floor( barrasMaxima )/( vPtr->numDiv - 1) , 1 ) ;}
    else{barrasDivisao = 0 ;}
    if (divisao == (vPtr->numDiv - 1)){barrasDivisao = barrasMaxima - barrasDistribuidas ;}
    if (barrasDivisao == 0){continue ;} 

    barrasDistribuidas = barrasDistribuidas + barrasDivisao ; //Atualizando o valor das barras distribuidas
    momentoDivisao = barrasDistribuidas * ( fabs(momentoCritico) / barrasMaxima ) ; //Calculando o momento equivalente do número de barras distribuidas

    comprimento_divisoes (result, momentoDivisao, regiao , vPtr ) ;

    iniDiv[divisao] = result[0] * vPtr->malha.n;
    fimDiv[divisao] = result[1] * vPtr->malha.n;

    ancoragem = ancoragem_necessaria( 0 , momentoCritico, bitola*0.001 , vPtr->prop.fctk_inf, vPtr->prop.fyd ) ;

    if (divisao == 0) //O fimDiv  Inicio + ancoragem + decalagem será comparadado com o limite da região
    {
        bPtr[numBarra].secaoIni = fmin(iniDiv[divisao]  - ancoragem - decalagem, (limiteIni * vPtr->malha.n) - 10 * bitola * 0.001) ;
        bPtr[numBarra].secaoFim = fmax(fimDiv[divisao]  + ancoragem + decalagem, (limiteFim * vPtr->malha.n) + 10 * bitola * 0.001) ;
    }
    else // O fimDiv+ ancoragem e Inicio + decalagem será comparadado com a seção fimDivda divisão anterior
    {
        bPtr[numBarra].secaoIni = fmin(iniDiv[divisao]  - ancoragem - decalagem, iniDiv[divisao-1] - 10 * bitola * 0.001) ;
        bPtr[numBarra].secaoFim = fmax(fimDiv[divisao]  + ancoragem + decalagem, fimDiv[divisao-1] + 10 * bitola * 0.001) ;
    }
    

    bPtr[numBarra].numero     = *barId        ;
    bPtr[numBarra].numBarras  = barrasDivisao ;
    bPtr[numBarra].ganchoD    = 0             ; 
    bPtr[numBarra].ganchoE    = 0             ; 
    bPtr[numBarra].bitola     = bitola        ;
    bPtr[numBarra].camada     = 1             ; //Camada inferior
    bPtr[numBarra].tipoRegiao = 7             ;
    strcpy(bPtr[numBarra].id,"Arm divisao (Interm) positiva"); //Identificador de arm

    numBarra++;
    (*barId)++;
}
    *barraPtr = numBarra;

    return 0;
}