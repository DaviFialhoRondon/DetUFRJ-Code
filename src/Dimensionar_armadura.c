#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "Detalhamento.h"

void dimensionar_armadura(struct viga *vPtr)
{
    double ArmLong[2];
    double Asw;


//--------Armaduras Longitudinais-----------//

    for (int regiao = 0; regiao < (vPtr->malha.numRegioes); regiao++){

        struct regiao_viga * vReg = &vPtr->regiao[regiao] ; //Simplificando a notação

    // - Momento negativo
        if (vReg->maxMomentoCrit < 0.0) 
        {
            // Calculo armadura dupla/simples 
            arm_long(vReg->maxMomentoCrit,vPtr,ArmLong);      

            // Armadura negativa será resultado em tração ou armadura mínima
            vPtr->arm[regiao].A_neg   = fmax( ArmLong[0] , vPtr->Al_min );                      

            if (ArmLong[1] <= vPtr->Al_min) vPtr->arm[regiao].A_comp = 0.0;                          // Armadura mínima é suficiente para arm de compressão                                         
            else                            vPtr->arm[regiao].A_comp = (ArmLong[1]) - vPtr->Al_min;  // Haverá armadura complementar e armadura mínima                                                                                  
        }   
    
    // - Momento positivo
        else if (vReg->maxMomentoCrit > 0.0)
        {
            arm_long(vReg->maxMomentoCrit,vPtr,ArmLong); 

            if (ArmLong[0] > vPtr->Al_min) vPtr->arm[regiao].A_comp =  (ArmLong[0]) - vPtr->Al_min ;    // Armadura de tração é maior que armadura mínima -> haverá Arm comp + Arm min
            else                           vPtr->arm[regiao].A_comp  = 0.0;                             // Armadura de tração é menor que armadura mínima -> Não haverá Armadura complementar
            
            vPtr->arm[regiao].A_neg =  ArmLong[1];       // Armadura dupla (caso haja)
        }

    // - Momento nulo
        else
        {
            vPtr->arm[regiao].A_neg  = 0.0;
            vPtr->arm[regiao].A_comp = 0.0;
        }

        Asw = calc_arm_transv(vReg->maxCortanteCrit ,vPtr);

        // Cortante nulo será utilizado a armadura mínima
        if(Asw < vPtr->Asw_min) vPtr->arm[regiao].Asw = vPtr->Asw_min;
        else                    vPtr->arm[regiao].Asw = Asw;

        if (vPtr->exibir.dimens == 1)
        {
        printf("*******************************************************\n"   );
        printf("*********  CALCULO DA ARMADURA LONGITUDINAL  **********\n"   );
        printf("*******************************************************\n\n" );
        printf("--------------------REGIAO %d----------------------\n" , regiao );
        printf("Arm minima       = %f [m2]\n" , vPtr->Al_min             );
        printf("Arm Complementar = %f [m2]\n" , vPtr->arm[regiao].A_comp );
        printf("Arm Negativa     = %f [m2]\n" , vPtr->arm[regiao].A_neg  );
        printf("--------------------------------------------------\n");
        printf("\n");
        printf("******************************************************\n"     );
        printf("*********  CALCULO DA ARMADURA TRANSVERSAL  **********\n"     );
        printf("******************************************************\n\n"   );
        printf(" Armadura utilizada = %f [m2/m]\n" , vPtr->arm[regiao].Asw    );
        printf("--------------------------------------------------\n" );
        printf("\n");

    }    
    }

}
