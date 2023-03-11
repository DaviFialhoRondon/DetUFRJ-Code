#ifndef HEADERFILE_H
#define HEADERFILE_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "Struct_viga.h"
#include "Struct_armacao.h"
#include "Definition.h"

#define DETALHAR_DIVISAO_REGIOES   0
#define CALCULO_ARMADURAS          0
#define DETALHAR_SECOES_ARMADURAS  0
#define CALCULO_ELS_W              0
#define CALCULO_SECOES             0
#define DISTRIBUIR_BARRAS          1
#define DETALHAR_SOLUCAO_MELHOR    0

int ler_entrada(struct viga *vPtr);

void   arm_long_min    (struct viga *vPtr ) ;
void   arm_transv_min  (struct viga *vPtr ) ;
double calc_arm_transv (double Vcalc , struct viga * vPtr  ) ;
void   arm_long        (double MomentoEntrada , struct viga *vPtr , double * result) ;

bool    verificar_ELS_W      (int regiao , struct viga * vPtr , int btEst ) ;
bool    verificar_Secao      (int regiao , struct viga * vPtr , int btEst ) ;
void    div_reg              (struct viga * vPtr                       ) ;
void    dimensionar_armadura (struct viga * vPtr                       ) ;
void    calcular_barras_Long (int regiao, struct viga *vPtr , int numBtPos , int numBtNeg ) ;
void    Valid_entrada        (struct viga * vPtr                       ) ;
void    calculo_propriedades (struct viga * vPtr                       ) ;

struct  barras_viga * distribuicao_barras(struct viga * vPtr);
struct  barras_viga * getPtrBarrasMelhor();

int    maxbarras_camada      (double larguraEfetiva , double espacamentoHorizontalmaximo , double bitola) ;
void   descobrir_apoio       (int secaoCritica , struct viga * structViga , int * result ) ;
void   comprimento_divisoes  (int * resultado ,double momentoDivisao,int regiao, struct viga * vPtr ) ;
double elemento_max          (double array[], int arraySize) ;
double linha_neutra_2        (double alphaE , double armaduraTracao , double armaduraCompressao , double largura , double alturaTracao , double alturaCompressao ) ;
double inercia_2             (double alphaE , double armaduraTracao , double armaduraCompressao , double largura , double alturaTracao , double alturaCompressao     ) ;
double espacamento_camada    (double larguraEfetiva , int numeroBarrasCamada , double bitola) ;
double calc_taxa_armadura    (double cobrimento , float bitolaLongitudinal , float bitolaTransversal , double espacamentoHorizontal , double espacamentoVertical ) ;
double comprimento_decalagem (double alturaUtil , double largura , double fctk_inf , double cortanteMaximo ) ;
double ancoragem_necessaria  (bool gancho , double momento , double bitola  , double fctk_inf , double fyd ) ;
double armadura_apoio        (int regiao , double armaduraVao , struct viga * vPtr) ;
double calc_peso_total       (int regFim , int regIni , int numBarras , struct estribos_viga * estPtr , struct barras_viga * bPtr ) ;
void   barras_apend          (int ini , int qntBarras , struct barras_viga *insertBarras , struct barras_viga *arrayBarras );

int balanco_esquerda (int *barId , int regiao , struct barras_viga *bPtr , int *numBarra , struct viga * vPtr ) ;
int balanco_direita  (int *barId , int regiao , struct barras_viga *bPtr , int *numBarra , struct viga * vPtr ) ;
int dist_neg_esq     (int *barId , int regiao , struct barras_viga *bPtr , int *numBarra , struct viga * vPtr ) ; 
int dist_neg_dir     (int *barId , int regiao , struct barras_viga *bPtr , int *numBarra , struct viga * vPtr ) ;
int dist_neg_interm  (int *barId , int regiao , struct barras_viga *bPtr , int *numBarra , struct viga * vPtr ) ;
int dist_pos_esq     (int *barId , int regiao , struct barras_viga *bPtr , int *numBarra , struct viga * vPtr ) ;
int dist_pos_dir     (int *barId , int regiao , struct barras_viga *bPtr , int *numBarra , struct viga * vPtr ) ;
int dist_pos_interm  (int *barId , int regiao , struct barras_viga *bPtr , int *numBarra , struct viga * vPtr ) ;
int dist_arm_comp    (int *barId , int regiao , struct barras_viga *bPtr , int *barraPtr , struct viga * vPtr ) ;

void distribuir_estribos(int regiao , double bitola , struct viga *vPtr , struct estribos_viga *estPtr );

int ler_analise(char *fMomentoElu, char *fCortanteElu ,char *fMomentoFreq, struct viga *vPtr);

void error_show(char * msg , int erroNum);

#endif