#include "Detalhamento.h" 
#include "Struct_viga.h" 
#include "Struct_armacao.h" 
#include "Definition.h" 

/*definições*/
double bitolas_padrao[] = {6.3 , 8.0 , 10.0 , 12.5 , 16.0 , 20.0 , 25.0 }; // (mm) bitolas utilizadas

/** 
 * ! Notas
 * * Verificar dimensões do agregado
 * * Verificar o pré-dimensionamento da altura d2
 * * Desenvolver ELS-DEF
 * * Rever cálculo do momento de inercia para estádio 2 (ELS-W e ELS-DEF)
 * * Rever a expressão da abertura de fissura (só é necessário verificar a camada inferior)
 * * Para as seções máximas na abertura de fissura, verificar se o num de raizes para combinação frequente será o mesmo
 * * Adicionar altura das armaduras longitudinais (alterar struct barras_vigas e funções geradoras de barras)
 * * Rever cálculo do peso das armaduras para otimização
 * * Adicionar bitolas utilizadas para iteração na otimização
 * * Adicionar verificação do VRd2 no cálculo da armadura Transversal
 * * Ver verificação da ancoragem das barras longitudinais nos apoios e adicionar como restrição à armadura otimizada (18.3.2.4.1)
 * 
 * ! Premissas de projeto:
 * * Vigas em flexão simples
 * * Seções retangulares
 * * É necessário a ultização de uma discretização da viga tão menor possivel pois as raízes das funções das solicitações será aproximada para os pontos da malha
*/

clock_t startTot,endTot;
clock_t startEntr,endEntr;
clock_t startDimen,endDimen;
clock_t startDet,endDet;
double timeTot,timeEntr,timeDim,timeDet;

int main()
{
    startTot = clock();


    struct viga vigaStru ;
    struct viga * vPtr = &vigaStru ;

    /*-----------------------------------------------*/
    /*--------------------ENTRADA--------------------*/
    /*-----------------------------------------------*/
    startEntr = clock();

printf("----------------ENTRADA DE DADOS-----------------\n");

    ler_analise("diagramas\\M-ELU.txt","diagramas\\V-ELU.txt","diagramas\\M-FREQ.txt",vPtr);

    ler_entrada(vPtr); 

    Valid_entrada(vPtr) ;

    endEntr = clock();

    /*--------------------------------------------------*/
    /*------------------DIMENSIONAMENTO----------------*/
    /*--------------------------------------------------*/

    startDimen = clock();

    calculo_propriedades ( vPtr ) ;

    div_reg ( vPtr ) ;

    arm_long_min   ( vPtr ) ; // Armaduras mínima flexão

    arm_transv_min ( vPtr ) ; // Armaduras mínima cortante

    dimensionar_armadura ( vPtr ) ;
 
    endDimen = clock();

    /*----------------------------------------------*/
    /*------------------DETALHAMENTO----------------*/
    /*----------------------------------------------*/

    startDet = clock();

    distribuicao_barras  ( vPtr ) ;

    endDet = clock();

    /*----------------------------------------------*/
    /*----------------FIM PROCESSAMENTO-------------*/
    /*----------------------------------------------*/

    endTot = clock();

    timeEntr = ((double) (endEntr  - startEntr  )) / CLOCKS_PER_SEC;
    timeDim  = ((double) (endDimen - startDimen )) / CLOCKS_PER_SEC;
    timeDet  = ((double) (endDet   - startDet   )) / CLOCKS_PER_SEC;
    timeTot  = ((double) (endTot   - startTot   )) / CLOCKS_PER_SEC;


    printf("----------Tempos de execucao--------------\n");
    printf("Tempo Entrada         - %2.4f\n",timeEntr) ;
    printf("Tempo Dimensionamento - %2.4f\n",timeDim ) ;
    printf("Tempo Detalhamento    - %2.4f\n",timeDet ) ;
    printf("Tempo total           - %2.4f\n",timeTot ) ;
    printf("------------------------------------------\n");

    int a = 0;
    printf("\n\nDigite e enter para sair\n");
    scanf("%d", &a); 

    return 0;
}