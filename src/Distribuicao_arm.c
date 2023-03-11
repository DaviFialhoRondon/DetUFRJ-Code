#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "Detalhamento.h"

/**
 * @date 01/07/22
 * @author Davi Rondon
 * TODO:
 * - Calcular ancoragem nos apoios
 * - Calcular ancoragem das divisões
 * - Continuar divisões no negativo
 * - Armadura de compressão
 * 
 * @brief Calcula o comprimento de barra para cada divisão do digrama de cada região
 */

struct barras_viga * distribuicao_barras(struct viga * vPtr)
{
    struct barras_viga arrayBarrasAtual  [NUM_DIV_TOTAIS];
    struct barras_viga arrayBarrasFinal  [NUM_DIV_TOTAIS];

    struct estribos_viga arrayEstribosAtual  [NUM_MAX_REGIOES];
    struct estribos_viga arrayEstribosFinal  [NUM_MAX_REGIOES];

    int numBarAtual ; // (  ) Número de barras da iteração atual
    int numBarFinal ; // (  ) Número de barras fianl
    int numBarReg   ; // (  ) Número de barras na regiao anterior
    int numBarId    ; // (  ) Identificador de barras
    int validAnc    ; // (  ) ValidAncação se a ancoragem foi obdecida

    bool verELSW  ; //(  ) Resultado da verificação ELS-W
    bool verSecao ; //(  ) Resultado da verificação se a quantidade de barras é comportada na secao
    bool verSoluc ; //(  ) Verificação para nao haver regioes sem solucao

    double massaTot       ; //(kg) Massa total da armação da viga
    double massaTotMelhor ; //(kg) Massa total da armação mais leve

    massaTotMelhor = 99999999999999999 ;
    numBarFinal    = 0                 ;
    numBarReg      = 0                 ;
    verSoluc       = 0                 ;

for (int regiao = 0; regiao < vPtr->malha.numRegioes; regiao++){

    struct regiao_viga * vReg = &vPtr->regiao[regiao] ; //Simplificando a notação

    for (int btPos = 0; btPos < 7; btPos++){
        if (vPtr->btPos[btPos] == 0) continue ; //Caso bitola positiva não esteja habilitada

    for (int btNeg = 0; btNeg < 7; btNeg++){
        if (vPtr->btNeg[btNeg] == 0) continue ; //Caso bitola negativa não esteja habilitada

    for (int btEst = 0; btEst < 7; btEst++){
        if (vPtr->btEst[btEst] == 0) continue ; //Caso bitola do estribo não esteja habilitada

        distribuir_estribos  ( regiao , bitolas_padrao[btEst] , vPtr , arrayEstribosAtual );  
        calcular_barras_Long ( regiao , vPtr, btPos , btNeg);

        verELSW  = verificar_ELS_W ( regiao , vPtr , btEst);
        verSecao = verificar_Secao ( regiao , vPtr , btEst);

        if ((verELSW * verSecao ) == 0 ) continue;   

        numBarId = numBarReg;

        switch (vReg->tipoRegiao){
        case 0: validAnc = balanco_esquerda ( &numBarId , regiao , arrayBarrasAtual , &numBarAtual , vPtr); break;
        case 1: validAnc = balanco_direita  ( &numBarId , regiao , arrayBarrasAtual , &numBarAtual , vPtr); break;
        case 2: validAnc = dist_neg_esq     ( &numBarId , regiao , arrayBarrasAtual , &numBarAtual , vPtr); break;
        case 3: validAnc = dist_neg_dir     ( &numBarId , regiao , arrayBarrasAtual , &numBarAtual , vPtr); break;
        case 4: validAnc = dist_neg_interm  ( &numBarId , regiao , arrayBarrasAtual , &numBarAtual , vPtr); break;
        case 5: validAnc = dist_pos_esq     ( &numBarId , regiao , arrayBarrasAtual , &numBarAtual , vPtr); break;
        case 6: validAnc = dist_pos_dir     ( &numBarId , regiao , arrayBarrasAtual , &numBarAtual , vPtr); break;
        case 7: validAnc = dist_pos_interm  ( &numBarId , regiao , arrayBarrasAtual , &numBarAtual , vPtr); break;
        default: break;}
        dist_arm_comp ( &numBarId , regiao , arrayBarrasAtual , &numBarAtual , vPtr);


        if (validAnc == 1){
            numBarAtual = 0;
            continue;
        }
        
        massaTot = calc_peso_total( regiao , regiao , numBarAtual , arrayEstribosAtual , arrayBarrasAtual );


        if (massaTot < massaTotMelhor)  {

            massaTotMelhor  = massaTot ;

            barras_apend(numBarReg , numBarAtual , arrayBarrasAtual , arrayBarrasFinal);

            arrayEstribosFinal[regiao] = arrayEstribosAtual[regiao];

            numBarFinal  = numBarReg + numBarAtual ;

        if (vPtr->exibir.detalAdv == 1){
            for (int i = 0; i < numBarFinal; i++){
            printf("\nNB %d\n"                   , arrayBarrasFinal[i].numero     );
            printf("Inicio         = %3.2f m\n"  , arrayBarrasFinal[i].secaoIni   );
            printf("Final          = %3.2f m\n"  , arrayBarrasFinal[i].secaoFim   );
            printf("Qnt Barras     = %3d\n"      , arrayBarrasFinal[i].numBarras  );
            printf("Bitola         = %3.2f mm\n" , arrayBarrasFinal[i].bitola     ); 
            printf("Gancho D       = %3d\n"      , arrayBarrasFinal[i].ganchoD    ); 
            printf("Gancho E       = %3d\n"      , arrayBarrasFinal[i].ganchoE    ); 
            if (arrayBarrasFinal[i].camada == 0) printf("Camada: Superior\n" ); 
            else                                 printf("Camada: Inferior\n" ); 
            printf("Tipo Regiao    = %d\n"  , arrayBarrasFinal[i].tipoRegiao );
            }
            }
        }

    verSoluc    = 1 ;
    numBarAtual = 0 ;
    
}}}
    if (verSoluc == 0) {printf("\nImpossivel detalhar regiao %d\n\n",regiao); error_show("Nao foi possivel detalhar uma regiao da viga",201);}
    
    verSoluc       = 0;
    numBarReg      = numBarFinal;
    massaTotMelhor = 99999999999999999; //Próxima regiao

}

    massaTotMelhor = calc_peso_total(0,vPtr->malha.numRegioes-1,numBarFinal,arrayEstribosFinal,arrayBarrasFinal);



if (vPtr->exibir.detal == 1)
{
    for (int i = 0; i < numBarFinal; i++){
    printf("\nNB %d\n"                   , arrayBarrasFinal[i].numero     );
    printf("Inicio         = %3.2f m\n"  , arrayBarrasFinal[i].secaoIni   );
    printf("Final          = %3.2f m\n"  , arrayBarrasFinal[i].secaoFim   );
    printf("Qnt Barras     = %3d\n"      , arrayBarrasFinal[i].numBarras  );
    printf("Bitola         = %3.2f mm\n" , arrayBarrasFinal[i].bitola     ); 
    printf("Gancho D       = %3d\n"      , arrayBarrasFinal[i].ganchoD    ); 
    printf("Gancho E       = %3d\n"      , arrayBarrasFinal[i].ganchoE    ); 
    if (arrayBarrasFinal[i].camada == 0) printf("Camada: Superior\n" ); 
    else                                 printf("Camada: Inferior\n" ); 
    printf("Tipo Regiao    = %d\n"  , arrayBarrasFinal[i].tipoRegiao );
    printf("Id: %s\n"  , arrayBarrasFinal[i].id );
    }

    for (int i = 0; i < vPtr->malha.numRegioes; i++){
    printf("\nNE %d\n"                     , arrayEstribosFinal[i].numero     );
    printf("Espacamento      = %3.2f m\n"  , arrayEstribosFinal[i].espac      );
    printf("Inicio           = %3.2f m\n"  , arrayEstribosFinal[i].ini        );
    printf("Final            = %3.2f m\n"  , arrayEstribosFinal[i].fim        );
    printf("Qnt Estribos     = %3d\n"      , arrayEstribosFinal[i].quant      );
    printf("Bitola           = %3.2f mm\n" , arrayEstribosFinal[i].bitola     ); 
    printf("Perimetro cada   = %3.2f m\n"  , arrayEstribosFinal[i].perim      );  
    printf("Tipo Regiao      = %3d\n"      , arrayEstribosFinal[i].tipoRegiao ); }

    printf("\n\nMassa final = %.1f kg\n",massaTotMelhor);

}

    return 0;
}