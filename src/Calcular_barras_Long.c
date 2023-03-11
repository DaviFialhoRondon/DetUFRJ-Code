#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>

#include "Detalhamento.h"
#include "Definition.h"

void calcular_barras_Long(int regiao, struct viga *vPtr , int numBtPos , int numBtNeg )
{
//--------Número de barras-----------//

double areaMinimaEfetiva ; // (m²) Área mínima efetiva

    struct regiao_viga *vReg = &vPtr->regiao[regiao];

    // - Armadura positiva
    if (vPtr->arm[regiao].A_comp == 0)     // * Somente armadura mínima -> Min
    {
        vReg->Bt_pos = bitolas_padrao[numBtPos] ;
        vReg->N_Lmin = ceil( vPtr->Al_min / (M_PI * pow(vReg->Bt_pos/1000,2) / 4.0 )) ;
        vReg->N_comp = 0 ;
    }

    else  // * Caso haja armadura complementar, deve-se verificar se essa será maior que a armadura mínima efetiva calculada                                        
    {
        vReg->Bt_pos = bitolas_padrao[numBtPos] ;
        
        vReg->N_Lmin = ceil( vPtr->Al_min / ( M_PI * pow( vReg->Bt_pos / 1000 , 2 ) / 4.0 )) ;
        areaMinimaEfetiva = vReg->N_Lmin *  ( M_PI * pow( vReg->Bt_pos / 1000 , 2 ) / 4.0 ) ;

        if (vPtr->arm[regiao].A_comp >= (areaMinimaEfetiva - vPtr->Al_min))       // * Armadura complementar maior que a mínima efetiva -> Min + Comp
        {
            vReg->N_comp = ceil( (vPtr->arm[regiao].A_comp - (areaMinimaEfetiva - vPtr->Al_min)) / ( M_PI * pow( ( vReg->Bt_pos/ 1000 ) , 2 ) / 4.0 )) ; 
        }
        else                   // * Armadura mínima efetiva maior que complementar -> Min
        {
            vReg->N_comp  = 0 ;   
        }
    }

    // - Arm negativa

        vReg->Bt_neg = bitolas_padrao[numBtNeg] ;
        vReg->N_neg  = ceil( vPtr->arm[regiao].A_neg / (M_PI * pow((vReg->Bt_neg/1000),2) / 4.0 )) ;
    
       
    if (vPtr->exibir.bUtil == 1)
    {

    printf("*****************************************************\n"   );
    printf("******************  BARRAS REGIOES ******************\n\n" );
    printf("*****************************************************\n"   );

    printf("------------------REGIAO %d------------------------\n",regiao); 
    printf("Arm_min   -- %d fi %.1f mm \n" , vReg->N_Lmin  , vReg->Bt_pos   );
    printf("Arm_comp  -- %d fi %.1f mm \n" , vReg->N_comp  , vReg->Bt_pos   );
    printf("Arm_neg   -- %d fi %.1f mm \n" , vReg->N_neg   , vReg->Bt_neg   );
    printf("--------------------------------------------------\n");
    printf("\n\n");
    }


}
