#include "Detalhamento.h"

int ler_analise(char *fMomentoElu, char *fCortanteElu ,char *fMomentoFreq, struct viga *vPtr) {
 
  FILE * fEntDiag = NULL;

  int nPonto1 = 0;
  int nPonto2 = 0;
  int nPonto3 = 0;

  double i;

// Diagrama M-ELU
  fEntDiag = fopen(fMomentoElu, "r");

  if (fEntDiag == NULL) printf("%s - nao encontrado\n", fMomentoElu);
  else{
    i = 0;
    while (i != EOF){
      i = fscanf(fEntDiag,"%lf",&vPtr->solicit[nPonto1].Md_dec);
      nPonto1++;
    }
    printf("%s - OK\n", fMomentoElu);
  }
  fclose(fEntDiag);

// Diagrama V-ELU
  fEntDiag = fopen(fCortanteElu, "r");

  if (fEntDiag == NULL) printf("%s - nao encontrado\n", fCortanteElu);
  else{
    i = 0;
    while (i != EOF){
      i = fscanf(fEntDiag,"%lf",&vPtr->solicit[nPonto2].Vd);
      nPonto2++;
    }
    printf("%s - OK\n", fCortanteElu);
  }
  fclose(fEntDiag);

// Diagrama M-FREQ
  fEntDiag = fopen(fMomentoFreq, "r");

  if (fEntDiag == NULL) printf("%s - nao encontrado\n", fMomentoFreq);
  else{
    i = 0;
    while (i != EOF){
      i = fscanf(fEntDiag,"%lf",&vPtr->solicit[nPonto3].MFreq);
      nPonto3++;
    }
    printf("%s - OK\n", fMomentoFreq);
  }
  fclose(fEntDiag);

  if (nPonto1 == nPonto2 && nPonto2 == nPonto3) printf("Todos os diagramas foram carregados com sucesso\n\n");
  else error_show("Quantidade de pontos diferentes entre os diagramas",1);

  vPtr->malha.N = nPonto1 - 1;

  return 0;
}

int ler_entrada(struct viga *vPtr) {

char line[100]; 
char strLixo[100];

double largApoio,posApoio;

FILE * fEntGer = fopen("Entrada_dados.txt", "r");

if (fEntGer == NULL) printf("Arquivo de entrada nao encontrado");

while (fgets(line, sizeof(line),fEntGer)){

    if (strncmp(line, "Altura (m):"                                ,11) == 0) fscanf(fEntGer, "%lf " , &vPtr->geo.altura);
    if (strncmp(line, "Largura (m):"                               ,12) == 0) fscanf(fEntGer, "%lf " , &vPtr->geo.largura);
    if (strncmp(line, "Cobrimento (m):"                            ,15) == 0) fscanf(fEntGer, "%lf " , &vPtr->geo.cob);
    if (strncmp(line, "Comprimento (m):"                           ,16) == 0) fscanf(fEntGer, "%lf " , &vPtr->geo.comprimento);
    if (strncmp(line, "fck (MPa):"                                 ,10) == 0) fscanf(fEntGer, "%lf " , &vPtr->prop.fck);
    if (strncmp(line, "Classe de Agressividade Ambiental (1 a 4):" ,42) == 0) fscanf(fEntGer, "%d  " , &vPtr->prop.classeAgressividade);

    if (strncmp(line, "| BITOLAS POSITIVAS HABILITADAS (mm) |"  ,38) == 0){
      for (int bitola1 = 0; bitola1 < 7; bitola1++) {
        fscanf(fEntGer, "%s %d  " ,strLixo, &vPtr->btPos[bitola1]);
        if ((vPtr->btPos[bitola1] != 0) && (vPtr->btPos[bitola1] != 1)) error_show("Valor habilitado as bitolas fora de 0 e 1",12); 
      }
    }

    if (strncmp(line, "| BITOLAS NEGATIVAS HABILITADAS (mm) |"  ,38) == 0){
      for (int bitola2 = 0; bitola2 < 7; bitola2++){
        fscanf(fEntGer, "%s %d  " ,strLixo, &vPtr->btNeg[bitola2]);
        if ((vPtr->btNeg[bitola2] != 0) && (vPtr->btNeg[bitola2] != 1)) error_show("Valor habilitado as bitolas fora de 0 e 1",12);
      }
    }

    if (strncmp(line, "| BITOLAS ESTRIBO HABILITADAS (mm) |"  ,36) == 0){
      for (int bitola3 = 0; bitola3 < 7; bitola3++){
        fscanf(fEntGer, "%s %d  " ,strLixo, &vPtr->btEst[bitola3]);
        if ((vPtr->btEst[bitola3] != 0) && (vPtr->btEst[bitola3] != 1)) error_show("Valor habilitado as bitolas fora de 0 e 1",12);
    }
    }

    if (strncmp(line, "Numero de apoios:"                          ,17) == 0) fscanf(fEntGer, "%d  " , &vPtr->numApoios);

    if (strncmp(line, "| INFORMACOES DOS APOIOS (Posicao (m) / Largura (m)) |"  ,53) == 0){
        
        for (int apoio = 0; apoio < vPtr->numApoios; apoio++)
        {
            fscanf(fEntGer, "%s %lf %s %s %lf " ,strLixo,&posApoio,strLixo,strLixo,&largApoio);

            vPtr->apoio[apoio].secaoApoios   = posApoio / (vPtr->geo.comprimento / (vPtr->malha.N - 1 ) );
            vPtr->apoio[apoio].larguraApoios = largApoio; 
        }
    }
    }
    fclose(fEntGer);


  FILE * fEntAvan = fopen("AdvConfig.txt", "r");
  if (fEntAvan == NULL) printf("Arquivo de entrada nao encontrado");

  while (fgets(line, sizeof(line),fEntAvan)){

    if (strncmp(line, "Dimensao do agregado graudo (m):"                ,32) == 0) fscanf(fEntAvan, "%lf " , &vPtr->prop.dimAgreg);
    if (strncmp(line, "Numero de divisoes:"                             ,19) == 0) fscanf(fEntAvan, "%d " , &vPtr->numDiv);
    if (strncmp(line, "Angulo da biela de concreto (Rad):"              ,34) == 0) fscanf(fEntAvan, "%lf " , &vPtr->prop.angBiela);
    if (strncmp(line, "Alpha E NBR6118 item 8.2.8:"                     ,27) == 0) fscanf(fEntAvan, "%lf " , &vPtr->prop.alphaE);
    if (strncmp(line, "Exibir Divisao regioes (0 ou 1):"                ,32) == 0) fscanf(fEntAvan, "%d " , &vPtr->exibir.divReg);
    if (strncmp(line, "Exibir Dimensionamento (0 ou 1):"                ,32) == 0) fscanf(fEntAvan, "%d " , &vPtr->exibir.dimens);
    if (strncmp(line, "Exibir barras utilizadas (0 ou 1):"              ,34) == 0) fscanf(fEntAvan, "%d " , &vPtr->exibir.bUtil);
    if (strncmp(line, "Exibir verificao ELSW (0 ou 1):"                 ,31) == 0) fscanf(fEntAvan, "%d " , &vPtr->exibir.verElsw);
    if (strncmp(line, "Exibir verificao Secao (0 ou 1):"                ,32) == 0) fscanf(fEntAvan, "%d " , &vPtr->exibir.verSec);
    if (strncmp(line, "Exibir resultados detalhamento (0 ou 1):"        ,40) == 0) fscanf(fEntAvan, "%d " , &vPtr->exibir.detal);
    if (strncmp(line, "Exibir melhor resultado cada iteracao (0 ou 1):" ,47) == 0) fscanf(fEntAvan, "%d " , &vPtr->exibir.detalAdv);
  }
  fclose(fEntAvan);


    printf("Altura      : %.2f m   \n" , vPtr->geo.altura               );
    printf("Largura     : %.2f m   \n" , vPtr->geo.largura              );
    printf("Cobrimento  : %.2f m   \n" , vPtr->geo.cob                  );
    printf("Comprimento : %.2f m   \n" , vPtr->geo.comprimento          );
    printf("Fck         : %.0f MPa \n" , vPtr->prop.fck                 );
    printf("CAA         : %d       \n" , vPtr->prop.classeAgressividade );
    printf("Num Apoio   : %d       \n" , vPtr->numApoios                );

    for (int i = 0; i < vPtr->numApoios; i++)
    printf("Apoio %d - Pos = %.2f Larg = %.2f \n",i,vPtr->apoio[i].secaoApoios * (vPtr->geo.comprimento / (vPtr->malha.N - 1 )), vPtr->apoio[i].larguraApoios);
    
    printf("Bt Pos-> 6.3 = %d | 8.0 = %d | 10.0 = %d  | 12.5 = %d | 16.0 = %d | 20.0 = %d | 25.0 = %d\n" , vPtr->btPos[0],vPtr->btPos[1],vPtr->btPos[2],vPtr->btPos[3],vPtr->btPos[4],vPtr->btPos[5],vPtr->btPos[6]);
    printf("Bt Neg-> 6.3 = %d | 8.0 = %d | 10.0 = %d  | 12.5 = %d | 16.0 = %d | 20.0 = %d | 25.0 = %d\n" , vPtr->btNeg[0],vPtr->btNeg[1],vPtr->btNeg[2],vPtr->btNeg[3],vPtr->btNeg[4],vPtr->btNeg[5],vPtr->btNeg[6]);
    printf("Bt Est-> 6.3 = %d | 8.0 = %d | 10.0 = %d  | 12.5 = %d | 16.0 = %d | 20.0 = %d | 25.0 = %d\n" , vPtr->btEst[0],vPtr->btEst[1],vPtr->btEst[2],vPtr->btEst[3],vPtr->btEst[4],vPtr->btEst[5],vPtr->btEst[6]);
    
    return 0;

}

void Valid_entrada(struct viga * vPtr)
{
    double cobMin;

    if (vPtr->malha.N                  <= 0 ) error_show("Numero de pontos menor ou igual a zero",2);
    if (vPtr->geo.altura               <= 0 ) error_show("Altura menor ou igual a zero",3) ;
    if (vPtr->geo.largura              <= 0 ) error_show("Largura menor ou igual a zero",4);
    if (vPtr->geo.comprimento          <= 0 ) error_show("Comprimento da viga menor ou igual a zero",5);
    if (vPtr->numApoios                <= 0 ) error_show("Numero de apoios menor ou igual a zero",6);
    if (vPtr->prop.fck                 <= 0 ) error_show("Fck menor ou igual a zero",7);
    if (vPtr->prop.classeAgressividade <= 0 ) error_show("CAA menor ou igual a zero",8);
    if (vPtr->prop.classeAgressividade >  4 ) error_show("CAA maior que 4",9);
    if (vPtr->prop.dimAgreg            <= 0 ) error_show("Dimensao do agregado menor ou igual a zero (configuracoes avancadas)",14);
    if (vPtr->numDiv                   <= 0 ) error_show("Numero de divisoes menor ou igual a zero (configuracoes avancadas)",15);
    if (vPtr->prop.angBiela            <= 0 ) error_show("Angulo da biela de concreto menor ou igual a zero (configuracoes avancadas)",16);
    if (vPtr->prop.alphaE              <= 0 ) error_show("Parametro alphaE menor ou igual a zero (configuracoes avancadas)",17);

    switch (vPtr->prop.classeAgressividade){
    case 1: cobMin = 0.025; break;
    case 2: cobMin = 0.030; break;
    case 3: cobMin = 0.040; break;
    case 4: cobMin = 0.050; break;
    }

    for (int apoio = 0; apoio < vPtr->numApoios; apoio++){
      if ((vPtr->apoio[apoio].secaoApoios < 0) || (vPtr->apoio[apoio].secaoApoios > vPtr->malha.N)) error_show("Apoio fora dos limites",10);
      if (vPtr->apoio[apoio].larguraApoios <= 0) error_show("Largura do apoio menor ou iguala a zero",11);
    }
    
    int verPos,verNeg,verEst;
    verPos = 0;
    verNeg = 0;
    verPos = 0;

    for (int i = 0 ; i < 7 ; i++) {
      verPos = verPos + vPtr->btPos[i];
      verNeg = verNeg + vPtr->btNeg[i];
      verEst = verEst + vPtr->btEst[i];
    }

    if (verPos == 0 || verNeg == 0 || verEst == 0) error_show("Nenhuma bitola habilitada",13);

    if (vPtr->geo.cob < cobMin){
      printf("Cobrimento de entrada (%.3f m) menor que cobrimento mínimo. Sera utilizado cobrimento = %.3f m",vPtr->geo.cob , cobMin);
      vPtr->geo.cob = cobMin;
    }   
}

void calculo_propriedades(struct viga * vPtr)
{
    vPtr->malha.n = vPtr->geo.comprimento / (vPtr->malha.N-1) ;  // Espaçamento entre pontos da malha

    vPtr->prop.Es  = 210 ; // módulo de elasticidade do aço (GPa)

    // Vigas - definições gerais
    vPtr->geo.d    = 0.9 * vPtr->geo.altura                        ; // (m ) Altura útil para armadura de tração(m)
    vPtr->geo.d2   = 0.1 * vPtr->geo.altura                        ; // (m ) Altura util para armadura de compressão (m)
    vPtr->prop.Ic  = vPtr->geo.largura * pow(vPtr->geo.altura,3)/12.0 ; // (m4) Momento de inércia

    vPtr->prop.fyd  = 500.0 / 1.15                       ; // Resistência de calculo das barras longitudinais do aço CA-50 (MPa)
    vPtr->prop.fywd = 500.0 / 1.15                       ; // Resistência de calculo das barras transversais do aço CA-50 (MPa)

    // Fct_m - Resistência a tração média do concreto (MPa) e Ecs - Módulo de elasticidade (GPa)
    if (vPtr->prop.fck <= 50){
        vPtr->prop.fct_m = 0.3 * pow(vPtr->prop.fck, (2.0 / 3.0));
        vPtr->prop.Ec  = 0.001 * vPtr->prop.alphaE * 5600 * sqrt(vPtr->prop.fck) * fmin( (0.8 + (0.2 * (vPtr->prop.fck)/80)) , 1 ) ; // módulo de elasticidade do concreto (GPa)
    }
    else{
        vPtr->prop.fct_m = 2.12 * log(1 + 0.11 * vPtr->prop.fck);
        vPtr->prop.Ec  = 0.001 * 21500 * vPtr->prop.alphaE * pow( ((vPtr->prop.fck/10) + 1.25) , (1/3) ) * fmin( (0.8 + (0.2 * (vPtr->prop.fck)/80)) , 1 ) ;
    }

    vPtr->prop.fctk_sup = 1.3 * vPtr->prop.fct_m; // Resistência à tração superior (MPa)
    vPtr->prop.fctk_inf = 0.7 * vPtr->prop.fct_m; // Resistência à tração inferior (MPa)

    double cortanteMaximo = 0 ;

    for (int ponto = 0 ; ponto < vPtr->malha.N ; ponto++){
        if ( cortanteMaximo < fabs(vPtr->solicit[ponto].Vd) ) cortanteMaximo = fabs(vPtr->solicit[ponto].Vd) ;
    }

    vPtr->prop.decalagem = comprimento_decalagem( vPtr->geo.d , vPtr->geo.largura , vPtr->prop.fctk_inf , cortanteMaximo ) ;
}