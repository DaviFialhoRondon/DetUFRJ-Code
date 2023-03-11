#include "Detalhamento.h"

int balanco_esquerda( int *barId , int regiao , struct barras_viga *bPtr , int *barraPtr , struct viga * vPtr )
{
    struct regiao_viga *vReg = &vPtr->regiao[regiao];
    int numBarra = *barraPtr;

    double ancoragem;
    
    double momentoCritico = vReg->maxMomentoCrit ;
    int    limiteFinal    = vReg->limiteFim      ;
    double bitola         = vReg->Bt_neg         ;
    double decalagem      = vPtr->prop.decalagem ;
    
    int    secaoApoioE   = vReg->apoioE.secao;
    double larguraApoioE = vReg->apoioE.largura;

// * Armadura mínima
    ancoragem = ancoragem_necessaria ( 0 , momentoCritico , bitola*0.001  , vPtr->prop.fctk_inf , vPtr->prop.fyd ) ;

    bPtr[numBarra].numero     =  *barId                                                                       ;
    bPtr[numBarra].secaoIni   =  0                                                                            ; //Seção inicial será o inicio da viga  
    bPtr[numBarra].secaoFim   =  secaoApoioE * vPtr->malha.n + decalagem + ancoragem - larguraApoioE / 2      ; //Seção final será no apoio acresentando a ancoragem
    bPtr[numBarra].numBarras  =  vPtr->regiao[regiao].N_Lmin                                                  ; //Número de barras da armadura mínima
    bPtr[numBarra].bitola     =  vPtr->regiao[regiao].Bt_pos                                                  ; //Bitola da armadura positiva
    bPtr[numBarra].ganchoD    =  0                                                                            ; //Gancho inexistente
    bPtr[numBarra].ganchoE    =  0                                                                            ; //Gancho inexistente
    bPtr[numBarra].camada     =  1                                                                            ; //Camada inferior
    bPtr[numBarra].tipoRegiao =  0                                                                            ;
    strcpy(bPtr[numBarra].id,"Arm minima Balanco")                                                            ; //Identificador de arm

    numBarra++ ;
    (*barId)++   ;

// * Armadura negativa
    bitola = vReg->Bt_neg ;
    ancoragem = ancoragem_necessaria ( 1 , momentoCritico , bitola*0.001  , vPtr->prop.fctk_inf , vPtr->prop.fyd ) ;

    bPtr[numBarra].numero    =   *barId                                               ;
    bPtr[numBarra].secaoIni  =   0                                                    ; //Seção inicial será o inicio da viga  
    bPtr[numBarra].secaoFim  = ( limiteFinal * vPtr->malha.n) + decalagem + ancoragem ; //Seção final será no apoio acresentando a ancoragem
    bPtr[numBarra].numBarras =   vReg->N_neg                                          ; //Número de barras da armadura mínima
    bPtr[numBarra].bitola    =   bitola                                               ; //Bitola da armadura positiva
    bPtr[numBarra].ganchoD   =   1                                                    ; //Gancho inexistente
    bPtr[numBarra].ganchoE   =   0                                                    ; //Gancho inexistente
    bPtr[numBarra].camada    =   0                                                    ; //Camada superior
    bPtr[numBarra].tipoRegiao =  0                                                    ;
    strcpy(bPtr[numBarra].id,"Arm Negativa Balanco")                                  ; //Identificador de arm
    
    numBarra++ ;
    (*barId)++   ;
    *barraPtr = numBarra;

    return 0;
}

int balanco_direita( int *barId , int regiao , struct barras_viga *bPtr , int *barraPtr , struct viga * vPtr )
{
    struct regiao_viga *vReg = &vPtr->regiao[regiao];

    double ancoragem;

    int numBarra = *barraPtr;

    double momentoCritico = vReg->maxMomentoCrit ;
    int    limiteInicial  = vReg->limiteIni    ;
    double bitola         = vReg->Bt_neg       ;
    double decalagem      = vPtr->prop.decalagem ;
    
    int    secaoApoioD   = vReg->apoioD.secao;
    double larguraApoioD = vReg->apoioD.largura;


    // * Armadura mínima
   
    ancoragem = ancoragem_necessaria ( 0 , momentoCritico , bitola*0.001  , vPtr->prop.fctk_inf , vPtr->prop.fyd ) ;


    bPtr[numBarra].numero     =   *barId                                                                        ;
    bPtr[numBarra].secaoIni   =   secaoApoioD * vPtr->malha.n - decalagem - ancoragem + larguraApoioD / 2       ; // Seção inicial será o último apoio  
    bPtr[numBarra].secaoFim   = ( vPtr->malha.N - 1) * vPtr->malha.n                                            ; // Seção final será o fim da viga
    bPtr[numBarra].numBarras  =   vPtr->regiao[regiao].N_Lmin                                                   ; // Número de barras da armadura mínima
    bPtr[numBarra].bitola     =   vPtr->regiao[regiao].Bt_pos                                                   ; // Bitola da armadura positiva
    bPtr[numBarra].ganchoD    =   0                                                                             ; // Gancho existente
    bPtr[numBarra].ganchoE    =   0                                                                             ; // Gancho inexistente
    bPtr[numBarra].camada     =   1                                                                             ; //Camada inferior
    bPtr[numBarra].tipoRegiao =   1                                                                             ;
    strcpy(bPtr[numBarra].id,"Arm minima Balanco")                                                              ; //Identificador de arm

    // * Armadura negativa
    bitola = vReg->Bt_neg ;
    
    ancoragem = ancoragem_necessaria ( 1 , momentoCritico , bitola*0.001  , vPtr->prop.fctk_inf , vPtr->prop.fyd ) ;

    numBarra++ ;
    (*barId)++   ;

    bPtr[numBarra].numero     =   *barId                                                  ;
    bPtr[numBarra].secaoIni   = ( limiteInicial * vPtr->malha.n ) - decalagem - ancoragem ; // Seção inicial será o último apoio  
    bPtr[numBarra].secaoFim   = ( vPtr->malha.N - 1) * vPtr->malha.n                      ; // Seção final será o fim da viga
    bPtr[numBarra].numBarras  =   vReg->N_neg                                             ; // Número de barras da armadura mínima
    bPtr[numBarra].bitola     =   bitola                                                  ; // Bitola da armadura positiva
    bPtr[numBarra].ganchoD    =   0                                                       ; // Gancho existente
    bPtr[numBarra].ganchoE    =   1                                                       ; // Gancho inexistente
    bPtr[numBarra].camada     =   0                                                       ; //Camada superior
    bPtr[numBarra].tipoRegiao =   1                                                       ;
    strcpy(bPtr[numBarra].id,"Arm Negativa Balanco")                                      ; //Identificador de arm


    numBarra++ ;
    (*barId)++   ;
    *barraPtr = numBarra;

    return 0;
}