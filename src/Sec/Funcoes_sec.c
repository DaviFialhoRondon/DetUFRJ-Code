#include "Detalhamento.h"

// Achar elemento máximo de um array int
double elemento_max(double array[], int arraySize)
{

    int Max      = 0 ;
    int valorMax = 0 ;

    for (int elemento = 0; elemento < arraySize; elemento++)
    {
        if (array[elemento]> valorMax)
        {
            valorMax = array[elemento];
            Max      = elemento;
        }
    }
    return Max;
}

// Calcular a linha Neutra de uma seção no estádio II
double linha_neutra_2(double alphaE , double armaduraTracao , double armaduraCompressao , double largura , double alturaTracao , double alturaCompressao )
{

    double x     ; // (m) Linha neutra no estádio 2
    double A , B ; //     Parâmetros para cálculo

    double alphaE2 = alphaE - 1 ; // Parâmetro para calculo

    A = ( alphaE * armaduraTracao + alphaE2 * armaduraCompressao ) / largura ;

    B = 2 * ( alphaE * armaduraTracao * alturaTracao + alphaE2 * armaduraCompressao * alturaCompressao ) / largura ;

    x = - A + sqrt( pow(A,2) + B ) ;

    return x ;
}

// Calcular a Inércia de uma seção no estádio II
double inercia_2( double alphaE , double armaduraTracao , double armaduraCompressao , double largura , double alturaTracao , double alturaCompressao )
{

    double x ; // (m ) Linha neutra no estádio 2
    double I ; // (m4) Inercia no estágio 2
    double A ; //      Parâmetros para cálculo
    double B ; //      Parâmetros para cálculo
    double C ; //      Parâmetros para cálculo

    double alphaE2 = alphaE - 1 ; // Parâmetro para calculo

    x = linha_neutra_2( alphaE , armaduraTracao , armaduraCompressao , largura , alturaTracao , alturaCompressao ) ;

    A = ( largura * pow( x , 3 ) ) / 3.0 ;

    B = alphaE * armaduraTracao * pow( (x - alturaTracao) , 2 ) ;

    C = alphaE2 * armaduraCompressao * pow( (x - alturaCompressao) , 2 ) ;

    I = A + B + C ;

    return I ;
}

// Calcular o número máximo de barras para primeira camada
int maxbarras_camada (double larguraEfetiva , double espacamentoHorizontalmaximo , double bitola)
{
    int numeroBarras;

    numeroBarras = floor( (larguraEfetiva + espacamentoHorizontalmaximo) / (bitola + espacamentoHorizontalmaximo) ) ;

    return numeroBarras;
}

// Calcular o espaçamento dado um número de barras na camada
double espacamento_camada (double larguraEfetiva , int numeroBarrasCamada , double bitola)
{
    double espacamento;

    espacamento = ( larguraEfetiva - (numeroBarrasCamada * bitola) ) / ( numeroBarrasCamada - 1 ) ;

    return espacamento;
}

// Calcular a menor taxa de armadura nos tirante de concreto armado
double calc_taxa_armadura ( double cobrimento , float bitolaLongitudinal , float bitolaTransversal , double espacamentoHorizontal , double espacamentoVertical )
{

    double areaCriticaBordas    ; // (  m² ) Área critica do tirante posicionado nas bordas da seção
    double areaCriticaCentro    ; // (  m² ) Área critica do tirante posicionado no centro da seção
    double taxaArmadura         ; // (m²/m²) Área de armadura por área de tirante

    double padrao = 7.5 * bitolaLongitudinal ;

    double b1 ; // Parâmetro para cálculo das áreas dos tirantes de concreto
    double b2 ; // Parâmetro para cálculo das áreas dos tirantes de concreto
    double h1 ; // Parâmetro para cálculo das áreas dos tirantes de concreto
    double h2 ; // Parâmetro para cálculo das áreas dos tirantes de concreto 

//* Utilizando somente a camada inferior

// Área crítica nas bordas
    b1 = fmin( 0.5 * bitolaLongitudinal + bitolaTransversal + cobrimento , padrao ) ;
    b2 = fmin( 0.5 * espacamentoHorizontal + 0.5 * bitolaLongitudinal , padrao ) ;
    h1 = fmin( 0.5 * bitolaLongitudinal + bitolaTransversal + cobrimento , padrao ) ;
    h2 = fmin( 0.5 * espacamentoVertical , padrao ) ;

    areaCriticaBordas = (b1 + b2) * (h1 + h2) ;

    // Área crítica no centro
    b1 = fmin( 0.5 * espacamentoHorizontal + 0.5 * bitolaLongitudinal , padrao ) ;
    b2 = fmin( 0.5 * espacamentoHorizontal + 0.5 * bitolaLongitudinal , padrao ) ;
    h1 = fmin( 0.5 * bitolaLongitudinal + bitolaTransversal + cobrimento , padrao ) ;
    h2 = fmin( 0.5 * espacamentoVertical , padrao ) ;

    areaCriticaCentro = (b1 + b2) * (h1 + h2) ;

    taxaArmadura      = ( M_PI * 0.25 * pow( bitolaLongitudinal , 2 ) ) / fmax( areaCriticaBordas , areaCriticaCentro ) ;

    return taxaArmadura ;
}

// Calcular o comprimento de decalagem
double comprimento_decalagem (double alturaUtil , double largura , double fctk_inf , double cortanteMaximo )
{
    double alpha     ; // (rad) Ângulo de inclinação da armadura transversal 
    double decalagem ; // ( m ) Comprimento de decalagem
    double Vc        ; // (kN ) Força resistente do concreto
    double cotgA     ; // (kN ) Cotangente de alpha

    alpha = M_PI / 2 ;  
    cotgA =  1 / tan( alpha ) ;

    Vc = 0.6 * ( (fctk_inf * pow(10, 3) ) /1.4 ) * largura * alturaUtil ;

    if (Vc >= cortanteMaximo) decalagem = alturaUtil ;

    else decalagem =  alturaUtil * ( ( ( cortanteMaximo ) / ( 2 * ( cortanteMaximo - Vc ) ) )  * ( ( 1 + cotgA ) - ( cotgA ) ) ) ;

    decalagem = fmin(decalagem,alturaUtil);
    decalagem = fmax(decalagem,0.5*alturaUtil);
    
    return decalagem ;
}

// Calcular a ancoragem necessária
//Não foi considerado (As calculado)/(As efetiva)
double ancoragem_necessaria ( bool gancho , double momento , double bitola  , double fctk_inf , double fyd )
{
    //Bitola em metros

    double n1    ; // (   ) Influência da superfície das barras na aderência
    double n2    ; // (   ) Influência da região das barras na aderência
    double n3    ; // (   ) Influência Diâmetro das barras na aderência
    double fbd   ; // (MPa) Resistencia de aderência de cálculo
    double lb    ; // ( m ) Comprimento básico de aderência
    double lbMin ; // ( m ) Comprimento mínimo de aderência
    double lbNec ; // ( m ) Comprimento necessário de aderência
    double a1    ; // ( m ) Coeficiente pela presença de gancho
    
////////////////////////////////////////////////////////////////////////////

// - Cálculo da resistencia de aderência (fbd)

    n1 = 2.25 ; // Barras nervuradas
    n3 = 1.0  ; // Barras menores que 32mm

    if (momento < 0)
    {
        n2 = 0.7 ;
    }
    else
    {
        n2 = 1.0 ;
    }
    
    fbd = n1 * n2 * n3 * fctk_inf/1.4 ;

// - Cálculo da ancoragem básica

    lb = fmax ( ( bitola / 4 ) * ( fyd / fbd ) , ( 25 * bitola ) ) ;

// - Cálculo da ancoragem mínima

    lbMin = fmax ( fmax ( 0.3 * lb , 10 * bitola ) , 0.1 ) ;

// - Cálculo da ancoragem necessária

    if (gancho) a1 = 0.7 ;
    else        a1 = 1.0 ;

    lbNec = fmax ( a1 * lb  , lbMin ) ;

    return lbNec ;

}

void descobrir_apoio (int secaoCritica , struct viga * structViga , int *result ){

    //Apoio à esquerda
    for (int i = 0; i < structViga->numApoios; i++)
    {
        if (structViga->apoio[i].secaoApoios <= secaoCritica )
        {
        result[0] = i ;

        }
    }
    //Apoio à direita
    for (int i = structViga->numApoios - 1; i >= 0; i--)
    {
        if (structViga->apoio[i].secaoApoios > secaoCritica )
        {
         result[1] = i ;

        }
    }
}

void  comprimento_divisoes  (int * resultado ,double momentoDivisao,int regiao, struct viga * vPtr )
{

    int limiteInicial = vPtr->regiao[regiao].limiteIni  ;
    int limiteFinal   = vPtr->regiao[regiao].limiteFim  ;
    int secaoCritica  = vPtr->regiao[regiao].secaoCritM ;

    // - ACHANDO O PONTO DE ENCONTRO DA BARRA COM O DIAGRAMA DE MOMENTO PELA ESQUERDA
    for (int secao = limiteInicial ; secao < secaoCritica; secao++)
    {   
        if  (   (fabs(vPtr->solicit[secao].Md_dec     ) <= fabs(momentoDivisao))  
                                            &&
                (fabs(vPtr->solicit[secao + 1].Md_dec ) >= fabs(momentoDivisao)) )
            {
                resultado[0] = secao -1 ; 
            }
    }

    // - ACHANDO O PONTO DE ENCONTRO DA BARRA COM O DIAGRAMA DE MOMENTO PELA DIREITA
    for (int secao = secaoCritica ; secao < limiteFinal; secao++)
    {
        if  (
                (fabs(vPtr->solicit[secao].Md_dec  ) >= fabs(momentoDivisao)) 
                                        &&
                (fabs(vPtr->solicit[secao+ 1].Md_dec) <= fabs(momentoDivisao))
            )
            {
                resultado[1] = secao + 1 ; 
            }
    }
}
double armadura_apoio( int regiao , double armaduraVao , struct viga * vPtr)
{

    double armaduraApoioA ; // (m²) Armadura calculada no apoio caso A
    double armaduraApoioB ; // (m²) Armadura calculada no apoio caso B
    double armaduraApoioC ; // (m²) Armadura calculada no apoio caso C
    double armaduraE      ; // (m²) Armadura pelo critério com maior armadura para o apoio Esquerdo
    double armaduraD      ; // (m²) Armadura pelo critério com maior armadura para o apoio Direito
    double armaduraFinal  ; // (m²) Armadura pelo critério com maior armadura
    double resultArm[2]   ; // (m²) Resultado do cálculo da armadura para o momento positivo

    int secaoApoio  ;

    double momentoApoio  ;
    double cortanteApoio ;

    int numNos = vPtr->malha.N ;

    double decalagem  = vPtr->prop.decalagem ;
    double alturaUtil = vPtr->geo.d          ;
    double momentoVao = vPtr->regiao[regiao].maxMomentoCrit ;

// - Apoio ESQUERDO

    secaoApoio    = vPtr->regiao[regiao].apoioE.secao    ;
    momentoApoio  = vPtr->regiao[regiao].apoioE.Momento  ;
    cortanteApoio = vPtr->regiao[regiao].apoioE.Cortante ;

    armaduraApoioA = 0;
    armaduraApoioB = 0;
    armaduraApoioC = 0;

    //Apoio de extremidade ou intermediário
    if ( fabs(momentoApoio) <= (0.5 * momentoVao) ) armaduraApoioC = (1/3) *  armaduraVao ; //Critério C1
    else                                          armaduraApoioC = (1/4) *  armaduraVao ; //Critério C2

    //Apoio de extremidade
    if ( ( secaoApoio == 0) || (secaoApoio == (numNos - 1)) ) armaduraApoioB = (decalagem * cortanteApoio) / (alturaUtil * 1000 * vPtr->prop.fyd) ; //Critério B

    //Momento positivo
    if ( momentoApoio > 0 ) {

        arm_long(momentoApoio , vPtr , resultArm);
        if (resultArm[1] != 0) printf("Armadura ancorada no apoio para caso A (18.3.2.4) apresenta armadura de compressao");
        
        armaduraApoioA = resultArm[0];
    }

    armaduraE = fmax ( fmax (armaduraApoioA , armaduraApoioB ) , armaduraApoioC ) ;

// - Apoio DIREITO

    secaoApoio    = vPtr->regiao[regiao].apoioD.secao    ;
    momentoApoio  = vPtr->regiao[regiao].apoioD.Momento  ;
    cortanteApoio = vPtr->regiao[regiao].apoioD.Cortante ;

    armaduraApoioA = 0;
    armaduraApoioB = 0;
    armaduraApoioC = 0;

    //Apoio de extremidade ou intermediário
    if ( fabs(momentoApoio) <= (0.5 * momentoVao) ) armaduraApoioC = 1/3 *  armaduraVao ; //Critério C1
    else                                          armaduraApoioC = 1/4 *  armaduraVao ; //Critério C2

    //Apoio de extremidade
    if ( ( secaoApoio == 0) || secaoApoio == (numNos - 1)) armaduraApoioB = (decalagem * cortanteApoio) / (alturaUtil * vPtr->prop.fyd) ; //Critério B

    //Momento positivo
    if ( momentoApoio > 0 ) {

        arm_long(momentoApoio , vPtr , resultArm);
        if (resultArm[1] != 0) printf("Armadura ancorada no apoio para caso A (NBR 6118 - 18.3.2.4) com armadura de compressão");
        
        armaduraApoioA = resultArm[0];
    }

    armaduraD = fmax ( fmax (armaduraApoioA , armaduraApoioB ) , armaduraApoioC ) ;

    armaduraFinal = fmax(armaduraE , armaduraD);

    return armaduraFinal ;
}

void barras_apend(int ini , int qntBarras , struct barras_viga *insertBarras , struct barras_viga *arrayBarras )
{
    for (int i = 0; i < qntBarras ; i++) arrayBarras[ini + i] = insertBarras[i]   ;  
}

double calc_peso_total( int regIni, int regFim  , int numBarras , struct estribos_viga * estPtr , struct barras_viga * bPtr )
{
    double compDiv;
    double massaLinear;
    double massaTotBarra;
    double massaTotEst;
    double massaTot;
    double bitola  ;

    massaTot = 0;
    massaTotEst = 0;
    massaTotBarra = 0;

    for (int i = 0; i < numBarras; i++)
    {
        compDiv = (bPtr[i].secaoFim - bPtr[i].secaoIni) * bPtr[i].numBarras ;
        bitola  =  bPtr[i].bitola;

        if (bitola ==  6.3) {massaLinear = 0.25;}
        if (bitola ==  8.0) {massaLinear = 0.40;}
        if (bitola == 10.0) {massaLinear = 0.63;}
        if (bitola == 12.5) {massaLinear = 1.00;}
        if (bitola == 16.0) {massaLinear = 1.60;}
        if (bitola == 20.0) {massaLinear = 2.50;}
        if (bitola == 25.0) {massaLinear = 4.00;}

        massaTotBarra = massaTotBarra + (massaLinear * compDiv) ;

    }
    
    for (int i = regIni; i <= regFim; i++){

        if (estPtr[i].bitola ==  6.3) {massaLinear = 0.25;}
        if (estPtr[i].bitola ==  8.0) {massaLinear = 0.40;}
        if (estPtr[i].bitola == 10.0) {massaLinear = 0.63;}
        if (estPtr[i].bitola == 12.5) {massaLinear = 1.00;}
        if (estPtr[i].bitola == 16.0) {massaLinear = 1.60;}
        if (estPtr[i].bitola == 20.0) {massaLinear = 2.50;}
        if (estPtr[i].bitola == 25.0) {massaLinear = 4.00;}

        massaTotEst = massaTotEst + (estPtr[i].quant * estPtr[i].perim  * massaLinear);

    }

    massaTot = massaTotBarra + massaTotEst;

    return massaTot;
}

void error_show(char * msg , int erroNum){

    int entrada;

    printf("\n\n\n\n\n#####################################################################################\n");
    printf("Error %d\n", erroNum);
    printf("%s\n", msg);

    printf("\nDigite e enter para sair\n");
    scanf("%d",&entrada);
    
    exit(erroNum);
}