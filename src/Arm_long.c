#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "Detalhamento.h"

void arm_long(double MomentoEntrada , struct viga *vPtr , double * result)
{
    double Mcalc     ; // (kNm) Momento calculado
    double alpha_c   ; // (adm) Parâmetro de calculo do concreto
    double lambda    ; // (adm) Parâmetro de calculo do concreto
    double x_max     ; // ( m ) Posição máxima da linha neuytra
    double x, x1, x2 ; // ( m ) Posição da linha neutra e seus resultados após a solução da eq de 2º grau
    double Md_max    ; // (kNm) Momento máximo para armadura simples
    double Delta_M   ; // (kNm) Diferença entre o momento máximo e o momento solicitante para o caso de armadura dupla

    double gama_m = 1.4 ; // fator de segurança do concreto

    double Arm_T ; // (cm²) Armadura de tração calculada
    double Arm_C ; // (cm²) Armadura de compressão calculada

    //Transformação quando momento é negativo
    if (MomentoEntrada < 0) Mcalc = -MomentoEntrada;

    else Mcalc = MomentoEntrada;
    
    // Calculo de lamba , alpha_c e Md_max
    if (vPtr->prop.fck <= 50)
    {
        alpha_c = 0.85 ;
        lambda  = 0.8  ;

        x_max   = 0.45 * vPtr->geo.d;

        Md_max  = (lambda) * (x_max) * (alpha_c) * ((vPtr->prop.fck * pow(10, 3)) / gama_m) * (vPtr->geo.largura) * (vPtr->geo.d - (0.5) * (lambda) * (x_max));
    }

    else
    {
        alpha_c = 0.85 * (1 - ((vPtr->prop.fck - 50) / 200)) ;
        lambda  = 0.8  - ((vPtr->prop.fck - 50) / 400)       ;

        x_max   = 0.35 * vPtr->geo.d ;

        Md_max  = (lambda) * (x_max) * (alpha_c) * ((vPtr->prop.fck * pow(10, 3)) / gama_m) * (vPtr->geo.largura) * (vPtr->geo.d - (0.5) * (lambda) * (x_max));
    }

    /*---------Verificação da armadura dupla / simples------------*/

    if (Mcalc <= Md_max) // Armadura simples
    {
        // Posição da linha neutra
        x1 = (vPtr->geo.d / lambda) * (1.0 + (sqrt(1.0 - ((Mcalc) / ((0.5) * (alpha_c) * (vPtr->geo.largura) * (pow(vPtr->geo.d, 2)) * ((vPtr->prop.fck * pow(10, 3)) / gama_m))))));
        x2 = (vPtr->geo.d / lambda) * (1.0 - (sqrt(1.0 - ((Mcalc) / ((0.5) * (alpha_c) * (vPtr->geo.largura) * (pow(vPtr->geo.d, 2)) * ((vPtr->prop.fck * pow(10, 3)) / gama_m))))));

        if (x1 <= vPtr->geo.altura) x = x1;
        if (x2 <= vPtr->geo.altura) x = x2;
        else error_show("Nao foi possivel calcular a linha neutra no dimensionamento",101);

        Arm_T = Mcalc / ((vPtr->prop.fyd * pow(10, 3)) * ((vPtr->geo.d) - (0.5) * (lambda) * (x)));
        Arm_C = 0;
    }

    if (Mcalc > Md_max) // Armadura dupla
    {
        Delta_M = Mcalc - Md_max;
        x = x_max;

        Arm_C = Delta_M / ((vPtr->geo.d - vPtr->geo.d2) * (vPtr->prop.fyd * pow(10, 3)));
        Arm_T = Md_max  / ((vPtr->prop.fyd * pow(10, 3)) * ((vPtr->geo.d) - (0.5) * (lambda) * (x_max))) + Arm_C;
    }

    result[0] = Arm_T;
    result[1] = Arm_C;


    if (vPtr->exibir.dimens == 1)
        {
            printf("Momento calculado     = %f [kNm] \n" ,  MomentoEntrada    );
            printf("Parametro Lambda      = %f       \n" , lambda             );
            printf("Parametro Alpha_c     = %f       \n" , alpha_c            );
            printf("Altura util elemento  = %f [m]   \n" , vPtr->geo.d           );
            printf("Largura elemento      = %f [m]   \n" , vPtr->geo.largura     );
            printf("fcd do concreto       = %f [MPa] \n" , vPtr->prop.fck/gama_m );
            printf("Momento maximo        = %f [kNm] \n" , Md_max             );
            printf("Momento minimo        = %f [kNm] \n" , vPtr->Md_min         );
            printf("Altura linha neutra   = %f [m]   \n" , x                  );

            printf("\n");
            
            printf("Armadura minima     = %f [m2] \n" , vPtr->Al_min       );
            printf("Armadura Tracao     = %f [m2] \n" , Arm_T               );
            printf("Armadura Compressao = %f [m2] \n" , Arm_C               );
            printf("--------------------------------------------------\n\n" );
        }

};


/*-------------------------------------------------------------------------------------------------------------------------------------------------------*/


void arm_long_min (struct viga *vPtr )
{
    double alpha_c      ; // ( ) Parâmetro de calculo do concreto
    double lambda       ; // ( ) Parâmetro de calculo do concreto
    double x1,x2,x      ; // (m) Posição da linha neutra e seus resultados após a solução da eq de 2º grau
    double gama_m = 1.4 ; // fator de segurança do concreto

    double W0 = (vPtr->geo.largura * pow(vPtr->geo.altura, 2)) / 6.0; // (m³) Módulo de resistência da seção 


    // Calculo de lamba e alpha_c
    if (vPtr->prop.fck <= 50)
    {
        alpha_c = 0.85;
        lambda = 0.8;
    }
    else
    {
        alpha_c = 0.85 * (1 - ((vPtr->prop.fck - 50) / 200));
        lambda = 0.8 - ((vPtr->prop.fck - 50) / 400);
    }


    vPtr->Md_min = 0.8 * W0 * pow(10, 3) * vPtr->prop.fctk_sup;   // (kNm) Momento mínimo de cálculo
    
    // Posição da linha neutra
    x1 = (vPtr->geo.d / lambda) * (1.0 + (sqrt(1.0 - ((vPtr->Md_min) / ((0.5) * (alpha_c) * (vPtr->geo.largura) * (pow(vPtr->geo.d, 2)) * ((vPtr->prop.fck * pow(10, 3)) / gama_m))))));
    x2 = (vPtr->geo.d / lambda) * (1.0 - (sqrt(1.0 - ((vPtr->Md_min) / ((0.5) * (alpha_c) * (vPtr->geo.largura) * (pow(vPtr->geo.d, 2)) * ((vPtr->prop.fck * pow(10, 3)) / gama_m))))));

    if (x1 < vPtr->geo.altura) x = x1;
    if (x2 < vPtr->geo.altura) x = x2;

    vPtr->Al_min = vPtr->Md_min / ((vPtr->prop.fyd * pow(10, 3)) * ((vPtr->geo.altura * 0.9) - (0.5) * (lambda) * (x)));

    if (vPtr->Al_min< ( 0.15/100 * vPtr->geo.largura * vPtr->geo.altura ) ) vPtr->Al_min = 0.15/100 * vPtr->geo.largura * vPtr->geo.altura ;
    
}