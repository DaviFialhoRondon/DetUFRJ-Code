#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "Detalhamento.h"

double calc_arm_transv (double Vcalc , struct viga * vPtr  )
{
    //Será utilizado o modelo de cálculo II
    //Transformação quando momento é negativo
    if (Vcalc < 0) Vcalc = -Vcalc;
    
    double A_sw;
    double vrd2;
    double alpha_v2;

    alpha_v2 = 1 - (vPtr->prop.fck/250);
  
    // Angulo da biela
    double teta = vPtr->prop.angBiela;

    // Ângulo da diagonal tracionada (estribos = 90º)
    double alpha = M_PI / 2; // rad

    //double alphaC = 1 - ( vPtr->prop.fck / 250 ) ;

    // Força resistente do concreto
    double Vc = 0.6 * ( vPtr->prop.fctk_inf * pow(10, 3) / 1.4 ) * vPtr->geo.largura * vPtr->geo.d;

    //Verificação da resistência do concreto
    vrd2 =  1000 * 0.54 * alpha_v2 * vPtr->prop.fck * 1.4 * vPtr->geo.largura * vPtr->geo.d * pow(sin(teta),2) * ((1/tan(alpha)) + (1/tan(teta)));

    if (Vcalc > vrd2) error_show("Vrd2 menor que cortante de calculo",102);

    // Valor da armadura em (m²/m)
    A_sw = (Vcalc - Vc) / (0.9 * vPtr->geo.d * (vPtr->prop.fywd * pow(10, 3)) * ((1.0 / tan(teta)) + (1.0 / tan(alpha))) * sin(alpha));

    if (A_sw < 0.0) A_sw = 0.0;
    
    if (CALCULO_ARMADURAS == 1)
    {
        printf("Cortante calculado = %f  [kN]   \n" ,  Vcalc            );
        printf("Parametro teta     = %f  [rad]  \n" , teta              );
        printf("Parametro alpha    = %f  [rad]  \n" , alpha             );
        printf("Armadura estribos  = %f  [m2/m] \n" , A_sw              );
        printf("Armadura minima    = %f  [m2/m] \n" , vPtr->Asw_min    );
        printf("--------------------------------------------------\n"   );
    }

    return A_sw;
}

void arm_transv_min(struct viga *vPtr){

    double gama_s = 1.15; // Fator de redução de resistência do aço

    // Ângulo da diagonal tracionada (estribos = 90º)
    double alpha = M_PI / 2; // rad

    vPtr->Asw_min = 0.2 * (vPtr->prop.fct_m / (vPtr->prop.fywd * gama_s)) * vPtr->geo.largura * sin(alpha);
}