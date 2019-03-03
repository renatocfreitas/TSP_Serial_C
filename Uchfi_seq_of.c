/*---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#pragma hdrstop
/*CONVEX HULL*/
#define PRIMEIRA_METADE 0
#define SEGUNDA_METADE 1
#define VERTICE_MAIS_A_DIREITA 0
#define VERTICE_MAIS_A_ESQUERDA 1
#define INFERIOR 0
#define SUPERIOR 1
#define ESQDIRINF 0
#define ESQDIRSUP 1

/*FARTHEST INSERTION*/
#define INFINITO 100000000
/*---------------------------------------------------------------------------*/
typedef struct {
   int id;
   float x;
   float y;
   } city;

typedef struct {
    int idx;
    int idy;
    float dist;
} adj;
typedef struct {
   int id;
   float dist_from_tour;
} vdist_from_tour;

void atribui (city * Out, city In)
{
Out->id = In.id;
Out->x  = In.x;
Out->y  = In.y;
}
int equal_city (city C1, city C2)
{
   if ((C1.id == C2.id) && (C1.x == C2.x) && (C1.y == C2.y)) return 1;
   return 0;   
}
/**************************************************************************/
/**********************FUNÇÕES UTILIZADAS NO CONVEX HULL*******************/
/****************************************************************/
/* distance recebe duas cidades como entrada e retorna a distancia entre elas*/
/* esta funcao sera usada apenas uma vez para a construcao da matriz de adjacencias*/
float distance (city c1, city c2)
{
return ( sqrt((c1.x-c2.x)*(c1.x-c2.x)+(c1.y-c2.y)*(c1.y-c2.y)) );
}
/*angulo recebe duas retas e retorna o angulo entre elas em graus*/
float angulo (city reta1[2], city reta2[2])
{
   float m1, m2, a, tga, angrad, anggrau;
   m1 = (reta1[0].y - reta1[1].y)/(reta1[0].x - reta1[1].x);
   if (reta2[0].x == reta2[1].x)
      a = M_PI/2;
   else {
      m2 = (reta2[0].y - reta2[1].y)/(reta2[0].x - reta2[1].x);
      tga = (m2 - m1) / (1 + (m1*m2));
      a = atan(tga);
      if (tga < 0)
         a += M_PI; /*atan trabalha so entre -pi/2 e +pi/2. Somo pi pra jogar o angulo alem de 90 graus*/
   }
   anggrau = (a*180)/M_PI;
   return (anggrau);
}

/*retorna o fecho convexo de um triangulo (tam=3) ou de um segmento de reta (tam=2)*/
/*e executado na base da recursao*/
city * convexo_base (city * S, int tam)
{
   int j, i, ib;
   city * O;
   city * Z;
   city aux, r1[2], r2[2], rh[2];
   float d1, d2, teta1, teta2;
switch (tam) {
case 0:
return (NULL);
break;
case 1:
O = (city *)malloc(1*sizeof(city));
Z = (city *)malloc(1*sizeof(city));/*nao usado*/
atribui (&O[0], S[0]);
break;
case 2:
O = (city *)malloc(2*sizeof(city));
Z = (city *)malloc(2*sizeof(city));/*nao usado*/
if (S[0].y <= S[1].y) {
atribui (&O[0], S[0]);
atribui (&O[1], S[1]);
}
else {
atribui (&O[0], S[1]);
atribui (&O[1], S[0]);
}
break;
case 3:
      O = (city *)malloc(3*sizeof(city));
    Z = (city *)malloc(2*sizeof(city));

    atribui (&aux, S[0]);

         ib = 0;
    for (i=1; i<3; i++)
       if (S[i].y < aux.y) {
          ib = i;
          atribui (&aux, S[i]);
       }

    atribui (&O[0], S[ib]);

         /*Z contem os dois pontos restantes*/
    j = 0;
    for (i=0; i<3; i++) {
       if (i == ib)
          continue;
       atribui (&Z[j], S[i]);
       j++;
    }

/*rh: reta horizontal com origem no ponto mais baixo*/
         for (i=0; i<2; i++)
            atribui (&rh[i], O[0]);
         rh[1].x++;

         /*as retas r1 e r2 iniciam pelo mesmo ponto: O[0]*/
         atribui (&r1[0], O[0]);
         atribui (&r2[0], O[0]);

         /*dos pontos restantes: Z[0] e Z[1], o mais próx. da origem vai para r1,
         /*o outro para r2*/
         d1 = distance (O[0], Z[0]);
         d2 = distance (O[0], Z[1]);
         if (d1 < d2) { atribui(&r1[1], Z[0]); atribui(&r2[1], Z[1]); }
         else         { atribui(&r1[1], Z[1]); atribui(&r2[1], Z[0]); }

         /*teta1 e´ o angulo entre rh e r1, teta2 e´ o angulo entre rh e r2*/
         teta1 = angulo (rh, r1);
         teta2 = angulo (rh, r2);

         /*ordena o fecho convexo acrescentando inicialmente o primeiro ponto*/
         /* que aparece no sentido anti-horario*/
         if (teta1 > teta2) { atribui(&O[1], r2[1]); atribui(&O[2], r1[1]); }
         else               { atribui(&O[1], r1[1]); atribui(&O[2], r2[1]); }

break;
default:
O = (city *)malloc(2*sizeof(city));
Z = (city *)malloc(2*sizeof(city));
printf ("\n**Erro convexo_base: parametro errado -> tam = %d**\n",tam);
   }
   free (Z);
   return O;
}
/****************************************************************/
/*se metade = PRIMEIRA_METADE, retorna a primeira metade de S*/
/*se metade = SEGUNDA_METADE, retorna a segunda metade de S*/
/* nc ==> comprimento do vetor S
tamanho ==> comprimento do vetor resultante da divisao
*/
city * dividir (city * S, int nc, int * tamanho, int metade)
{
   city * O;
   int i, cont=0, tam=0;
   float fx=0, compara_x=0, x_medio=0;

   *tamanho = 0;
   cont = 1;
   compara_x = S[0].x;
   fx = compara_x;
   for (i = 0; i < nc; i++)
      if (compara_x != S[i].x) {
         compara_x = S[i].x;
         fx += compara_x;
         cont++;
      }
   x_medio = fx/cont;
   tam = 0;
   for (i = 0; i < nc; i++) {
      if (S[i].x > x_medio)
         break;
      tam++;
   }
   if (metade == PRIMEIRA_METADE) {
      *tamanho = tam;
      O = (city *) malloc ((*tamanho) * sizeof (city));
      for (i = 0; i < tam; i++)
         atribui (&O[i], S[i]);
   } else {
      *tamanho = nc - tam;
      O = (city *) malloc ((*tamanho) * sizeof (city));
      for (i = tam; i < nc; i++)
         atribui (&O[i - tam], S[i]);
   }
   return O;
}
/****************************************************************/
/* retorna 1 se o ponto Sij estiver SOMENTE acima da reta r */
int acima (city r[2], city Sij)
{
float coef, y2;
   if (r[0].x != r[1].x) {
   coef = (r[0].y - r[1].y)/(r[0].x - r[1].x);
   y2 = (coef * (Sij.x - r[1].x)) + r[1].y;
   if (y2 >= Sij.y)
   return 0;
   } else {
      if (r[0].x >= Sij.x)
         return 0;
   }
   return 1;
}
/****************************************************************/
int colinear (city r[2], city Sij)
{
   float coef_reta, coef2, y2;
   if (r[0].x != r[1].x) {
   y2 = (((r[0].y - r[1].y)/(r[0].x - r[1].x)) * (Sij.x - r[1].x)) + r[1].y;
   if (y2 == Sij.y)
   return 1;
   } else {
      if (r[0].x == Sij.x)
         return 1;
   }
   return 0;
}
/****************************************************************/
/* S ==>
   tam ==> tamanho de S
retorna o indice do ponto mais a direita de S ou mais a esquerda
dependendo do valor de esq_dir
*/
int vertice_esq_dir (city * S, int tam, int esq_dir)
{
int i, indice = 0;

if (esq_dir == VERTICE_MAIS_A_DIREITA) {
for (i=1; i<tam; i++)
if (S[i].x > S[indice].x)
indice = i;
}
else {
    for (i=1; i<tam; i++)
if (S[i].x < S[indice].x)
indice = i;
}
return indice;
}
/****************************************************************/
int * tangente (city * S1, int tamS1, city * S2, int tamS2, int supinf)
{
int iv1, iv2, i, j, temp1, temp2, corrigei, corrigej;
   int emcima, ecolinear, icont, jcont, ireta, jreta, sair_i = 0, sair_j = 0;
int * resinf;
   int * ressup;
city reta[2];

   resinf = (int*) malloc (2 * sizeof (int));
   ressup = (int*) malloc (2 * sizeof (int));
   
iv1 = vertice_esq_dir (S1, tamS1, VERTICE_MAIS_A_DIREITA);
iv2 = vertice_esq_dir (S2, tamS2, VERTICE_MAIS_A_ESQUERDA);

atribui (&reta[0], S1[iv1]);
atribui (&reta[1], S2[iv2]);

switch (supinf) {
case INFERIOR:
      i = iv1 - 1; if (i == -1)    i = 0;
      j = iv2 + 1; if (j == tamS2) j = tamS2 - 1;

      ireta = iv1;
      jreta = iv2;

      do {
         sair_j = 0;
         emcima = acima (reta, S2[j]);
         ecolinear = colinear (reta, S2[j]);
         while (!emcima || (ecolinear && S2[j].y <= S2[jreta].y)) {
            if ((S2[j].y == S2[jreta].y) && (S2[j].y == reta[0].y)) { /*se a reta estiver na horizontal e S2[j] seja colinear a ela*/
               if ((S2[j].x - reta[0].x) < (S2[jreta].x - reta[0].x)) { /*se 2 ou + pts estiverem no mesmo y, isto evita o loop infinito escolhendo o que forma a reta mais curta*/
                  atribui (&reta[1], S2[j]);
                  jreta = j;
               }
               j++; if (j == tamS2) j = 0;
               break;
            }   
            /*reta vertical com S2[j] localizado entre os dois pontos---------------------------------------*/
            if ((reta[0].x == reta[1].x) && (S2[j].x == reta[0].x))
               if (((reta[0].y > reta[1].y) && (S2[j].y > reta[1].y)) ||
                   ((reta[0].y < reta[1].y) && (S2[j].y > reta[0].y))) {
                   sair_j = 1;
                   break;
               }
            /*----------------------------------------------------------------------------*/
            atribui (&reta[1], S2[j]);
            jreta = j;
            j++;
            if (j == tamS2) j = 0;
            if (tamS2 == 1) /*caso S2 seja um único ponto, este loop será infinito*/
               break;
            emcima = acima (reta, S2[j]);
            ecolinear = colinear (reta, S2[j]);
         }
         sair_i = 0;
         emcima = acima (reta, S1[i]);
         ecolinear = colinear (reta, S1[i]);
         while (!emcima || (ecolinear && S1[i].y <= S1[ireta].y)) {
            if ((S1[i].y == S1[ireta].y) && (S1[i].y == reta[1].y)) {
               if ((reta[1].x - S1[i].x) < (reta[1].x - S1[ireta].x)) {
                  atribui (&reta[0], S1[i]);
                  ireta = i;
               }
               i--; if (i == -1) i = tamS1 - 1;
               break;
            }
            /*reta vertical com S1[i] localizado entre os dois pontos---------------------------------------*/
            if ((reta[0].x == reta[1].x) && (S1[i].x == reta[0].x))
               if (((reta[0].y > reta[1].y) && (S1[i].y > reta[1].y)) ||
                   ((reta[0].y < reta[1].y) && (S1[i].y > reta[0].y))){
                   sair_i = 1;
                   break;
               }
            /*----------------------------------------------------------------------------*/
            atribui (&reta[0], S1[i]);
            ireta = i;
            i--;
            if (i == -1) i = tamS1 - 1;
            if (tamS1 == 1) /*caso S1 seja um único ponto, este loop será infinito*/
               break;
            emcima = acima (reta, S1[i]);
            ecolinear = colinear (reta, S1[i]);
         }
         emcima    = acima (reta, S2[j]);
         ecolinear = colinear (reta, S2[j]);
         jcont     = (!emcima || (ecolinear && (S2[j].y <= S2[jreta].y) )) ;
         if ((S2[j].y == S2[jreta].y) && (tamS2 == 1))
            jcont = 0;
         emcima    = acima (reta, S1[i]);
         ecolinear = colinear (reta, S1[i]);
         icont     = (!emcima || (ecolinear && (S1[i].y <= S1[ireta].y) )) ;
         if ((S1[i].y == S1[ireta].y) && (tamS1 == 1)) /*caso particular: S1 tem apenas 1 ponto, neste caso ocorre loop infinito*/
            icont = 0;

         if (jcont)
            /*S2[j] está + distante de reta[0], logo permanece reta[1] == S2[jreta]*/
            if (((S2[j].y == S2[jreta].y) && (S2[j].y == reta[0].y)) &&
                ((S2[j].x - reta[0].x) > (S2[jreta].x - reta[0].x)))
                     jcont = 0;
         if (icont)
            /*S1[i] está + distante de reta[1], logo permanece reta[0] == S1[ireta]*/
            if (((S1[i].y == S1[ireta].y) && (S1[i].y == reta[1].y)) &&
                ((reta[1].x - S1[i].x) > (reta[1].x - S1[ireta].x)))
                     icont = 0;
         /*se a reta não se alterou.. é pq é ela mesma que deve ser retornada*/
         if (equal_city (S1[i], S1[ireta]) && equal_city (S2[j], S2[jreta]))
            break;
         if (sair_i && sair_j)  /*problema da reta vertical*/
            break;
      } while (jcont || icont);
break;
case SUPERIOR:
      i = iv1 + 1; if (i == tamS1) i = tamS1 - 1;
      j = iv2 - 1; if (j == -1)    j = 0;

      ireta = iv1;
      jreta = iv2;

      do {
         emcima = acima (reta, S2[j]);
         ecolinear = colinear (reta, S2[j]);
         while (emcima || (ecolinear && S2[j].y >= S2[jreta].y)) {
            if ((S2[j].y == S2[jreta].y) && (S2[j].y == reta[0].y)) {
               if ((S2[j].x - reta[0].x) > (S2[jreta].x - reta[0].x)) {
                  atribui (&reta[1], S2[j]);
                  jreta = j;
               }
               j--; if (j == -1) j = tamS2 - 1;
               break;
            }
            atribui (&reta[1], S2[j]);
            jreta = j;
            j--;
            if (j == -1) j = tamS2 - 1;
            if (tamS2 == 1) /*caso S2 seja um único ponto, este loop será infinito*/
               break;
            emcima = acima (reta, S2[j]);
            ecolinear = colinear (reta, S2[j]);
         }
         emcima = acima (reta, S1[i]);
         ecolinear = colinear (reta, S1[i]);
         while (emcima || (ecolinear && S1[i].y >= S1[ireta].y)) {
            if ((S1[i].y == S1[ireta].y) && (S1[i].y == reta[1].y)) {
               if ((reta[1].x - S1[i].x) > (reta[1].x - S1[ireta].x)) {
                  atribui (&reta[0], S1[i]);
                  ireta = i;
               }
               i++; if (i == tamS1) i = 0;
               break;
            }
            atribui (&reta[0], S1[i]);
            ireta = i;
            i++;
            if (i == tamS1) i = 0;
            if (tamS1 == 1) /*caso S1 seja um único ponto, este loop será infinito*/
               break;
            emcima = acima (reta, S1[i]);
            ecolinear = colinear (reta, S1[i]);
         }
         emcima    = acima (reta, S2[j]);
         ecolinear = colinear (reta, S2[j]);
         jcont     = (emcima || (ecolinear && S2[j].y >= S2[jreta].y));
         /*se S2 é apenas um ponto, S2[jreta] é o segundo ponto da tangente*/
         if ((S2[j].y == S2[jreta].y) && (tamS2 == 1))
            jcont = 0;
         emcima    = acima (reta, S1[i]);
         ecolinear = colinear (reta, S1[i]);
         icont     = (emcima || (ecolinear && S1[i].y >= S1[ireta].y));
         /*se S1 é apenas um ponto, S1[ireta] é o primeiro ponto da tangente*/
         if ((S1[i].y == S1[ireta].y) && (tamS1 == 1))
            icont = 0;

         if (jcont)
            /*S2[j] está + próx. de reta[0], logo permanece reta[1] == S2[jreta]*/
            if (((S2[j].y == S2[jreta].y) && (S2[j].y == reta[0].y)) &&
                ((S2[j].x - reta[0].x) < (S2[jreta].x - reta[0].x)))
                  jcont = 0;
         if (icont)
            /*S1[i] está + próx. de reta[1], logo permanece reta[0] == S1[ireta]*/
            if (((S1[i].y == S1[ireta].y) && (S1[i].y == reta[1].y)) &&
                ((reta[1].x - S1[i].x) < (reta[1].x - S1[ireta].x)))
                  icont = 0;

         /*se a reta não se alterou.. é pq é ela mesma que deve ser retornada*/
         if (equal_city (S1[i], S1[ireta]) && equal_city (S2[j], S2[jreta]))
            break;
      } while (jcont || icont);
break;
}
   if (supinf == INFERIOR) {
      free (ressup);
   resinf[0] = ireta; resinf[1] = jreta;
      return resinf;
   } else {
      free (resinf);
      ressup[0] = ireta; ressup[1] = jreta;
      return ressup;
   }
}
int compare_mais_alto (const void *C1, const void *C2)
{
   int r = 0;
   float y1, y2;
   y1 = ((city*)C1)->y;
   y2 = ((city*)C2)->y;
   if (y1 < y2) r = 1;
   if (y1 > y2) r = -1;
   return r;
}
int compare_mais_baixo (const void *C1, const void *C2)
{
   int r = 0;
   float y1, y2;
   y1 = ((city*)C1)->y;
   y2 = ((city*)C2)->y;
   if (y1 < y2) r = -1;
   if (y1 > y2) r = 1;
   return r;
}
/*retorna o ponto mais alto de S*/
city * ponto_mais_alto (city * S, int tamS)
{
   int i;
   city * Saux, *P;
   P    = (city *) malloc (1*sizeof(city));
   Saux = (city *) malloc (tamS*sizeof(city));
   for (i = 0; i < tamS; i++) atribui(&Saux[i], S[i]);
   qsort (Saux, tamS, sizeof(city), compare_mais_alto);
   atribui (&P[0], Saux[0]);
   free (Saux);
   return (P);
}
/*retorna o ponto mais baixo de S*/
city * ponto_mais_baixo (city * S, int tamS)
{
   int i;
   city * Saux, *P;
   P    = (city *) malloc (1*sizeof(city));
   Saux = (city *) malloc (tamS*sizeof(city));
   for (i = 0; i < tamS; i++) atribui(&Saux[i], S[i]);
   qsort (Saux, tamS, sizeof(city), compare_mais_baixo);
   atribui (&P[0], Saux[0]);
   free (Saux);
   return (P);
}

int conjunto_colinear_horizontal (city * S, int tamS)
{
   int i;
   if (tamS > 1) {
      for (i = 1; i < tamS; i++)
         if (S[i].y != S[0].y)
            return 0;
   }
   return 1;
}
/*assume que S1 já é um conjunto colinear horizontal*/
int conjunto_colinear_horizontal_esqinf (city * S1, int tamS1, city * S2, int tamS2)
{
   int r = 0;
   city * P2;
   P2 = ponto_mais_baixo (S2, tamS2);
   if (conjunto_colinear_horizontal (S1, tamS1))
      if (S1[0].y <= P2->y)
         r = 1;
   free (P2);
   return r;
}
/*assume que S2 já é um conjunto colinear horizontal*/
int conjunto_colinear_horizontal_dirinf (city * S1, int tamS1, city * S2, int tamS2)
{
   int r = 0;
   city * P1;
   P1 = ponto_mais_baixo (S1, tamS1);
   if (conjunto_colinear_horizontal (S2, tamS2))
      if (P1->y >= S2[0].y)
         r = 1;
   return r;
}
/*assume que S1 já é um conjunto colinear horizontal*/
int conjunto_colinear_horizontal_esqsup (city * S1, int tamS1, city * S2, int tamS2)
{
   int r = 0;
   city * P2;
   P2 = ponto_mais_alto (S2, tamS2);
   if (conjunto_colinear_horizontal (S1, tamS1))
      if (S1[0].y >= P2->y)
         r = 1;
   free (P2);
   return r;
}
/*assume que S1 já é um conjunto colinear horizontal*/
int conjunto_colinear_horizontal_dirsup (city * S1, int tamS1, city * S2, int tamS2)
{
   int r = 0;
   city * P1;
   P1 = ponto_mais_alto (S1, tamS1);
   if (conjunto_colinear_horizontal (S2, tamS2))
      if (P1->y <= S2[0].y)
         r = 1;
   free (P1);
   return r;
}
/**************************************************************************/
int compare_colinear_horizontal_esqdirsup (const void *C1, const void *C2)
{
   int r = 0;
   float x1, x2;
   x1 = ((city*)C1)->x;
   x2 = ((city*)C2)->x;
   if (x1 < x2) r = 1;
   if (x1 > x2) r = -1;
   return r;
}
int compare_colinear_horizontal_esqdirinf (const void *C1, const void *C2)
{
   int r = 0;
   float x1, x2;
   x1 = ((city*)C1)->x;
   x2 = ((city*)C2)->x;
   if (x1 < x2) r = -1;
   if (x1 > x2) r = 1;
   return r;
} 

city * convex_hull_colinear_horizontal (city * S, int tamS, int *tamSaida, int side)
{
   int i, e = 0;
   city * O;

   *tamSaida = tamS;
   O = (city*) malloc ((*tamSaida) * sizeof (city));
   e = conjunto_colinear_horizontal (S, tamS);
   if (e) {
      for (i = 0; i < tamS; i++)
         atribui(&O[i], S[i]);
      if (side == ESQDIRSUP)
         qsort (O, tamS, sizeof(city), compare_colinear_horizontal_esqdirsup);
      else if (side == ESQDIRINF)
         qsort (O, tamS, sizeof(city), compare_colinear_horizontal_esqdirinf);
      return O;
   }
   else {
      free (O);
      return S;
   }
}

int conjunto_colinear_vertical (city * S, int tamS)
{
   int i;
   if (tamS > 1) {
      for (i = 1; i < tamS; i++)
         if (S[i].x != S[0].x)
            return 0;
   }
   return 1;
}

int compare_colinear_vertical (const void *C1, const void *C2)
{
   int r = 0;
   float y1, y2;
   y1 = ((city*)C1)->y;
   y2 = ((city*)C2)->y;
   if (y1 < y2) r = -1;
   if (y1 > y2) r = 1;
   return r;
}
city * convex_hull_colinear_vertical_dir (city * S, int tamS, int *tamSaida)
{
   int i, e = 0;
   city * O;

   *tamSaida = tamS;
   O = (city*) malloc ((*tamSaida) * sizeof (city));
   e = conjunto_colinear_vertical(S, tamS);
   if (e) {
      for (i = 0; i < tamS; i++)
         atribui(&O[i], S[i]);
      qsort (O, tamS, sizeof(city), compare_colinear_vertical);
      return O;
   }
   else {
      free (O);
      return S;
   }
}
city * convex_hull_colinear_vertical_esq (city * S, int tamS, int *tamSaida)
{
   int i, j, tamO;
   city *O, *Inv;

   *tamSaida = tamS;
   O = convex_hull_colinear_vertical_dir (S, tamS, &tamO);
   Inv = (city*) malloc ((tamO - 1)*sizeof(city));

   /*preservo o primeiro elemento de O e inverto o resto*/
   j = 0;
   for (i = tamO - 1; i > 0; i--) {
      atribui (&Inv[j], O[i]);
      j++;
   }
   j = 1;
   for (i = 0; i < tamO - 1; i++) {
      atribui (&O[j], Inv[i]);
      j++;
   }   
   free (Inv);
   return O;
}
/***********************************************************************************/
/*merge recebe dois fechos convexos S1 e S2 e realiza um merge deles*/
city * merge (city * S1, int tamS1, city * S2, int tamS2, int tginf[2], int tgsup[2], int * lengthOut)
{
   city * Out, *newOut, * Resto, *OrdResto;
   int i, j, k, tamOut, newtamOut, iOut, tamResto, tamOrdResto, marcador, achouSO = 0, achouSE = 0, achouNE = 0, achouNO = 0;
   int SO, SE, NE, NO;

   SO = tginf[0]; SE = tginf[1]; NE = tgsup[1]; NO = tgsup[0];
   tamOut = 0;
   j = SE;
   if ((j == tamS2 - 1) && (j != NE)) { /*se j ficasse igual a NE, nem entra no for abaixo...*/
      tamOut++;
      j = 0;
   }
   if (tamS2 >= 1) {
      if (j > NE) {
         for (i = j; i < tamS2; i++)
            tamOut++;
         j = 0;   
      }
      for (i = j; i <= NE; i++) {
         if ((i == tamS2 - 1) && (i != NE))/*se durante o percurso de SE a NE o i passar pelo tamanho max do vetor*/
            i = 0;
         tamOut++;
      }
   }
   j = SO;
   if ((j == 0) && (j != NO)) {
      tamOut++;
      j = tamS1 - 1;
   }
   if (tamS1 >= 1) {
      if (j < NO) { /*i vai passar pelo zero...*/
         for (i = j; i >= 0; i--)
            tamOut++;
         j = tamS1 - 1;
      }
      for (i = j; i >= NO; i--) {
         if ((i == 0) && (i != NO))
            i = tamS1 - 1;
         tamOut++;
      }
   }
   /*if (tamS1 >= 1) {
      for (i = NO; i < tamS1; i++) {
         if (S1[i].id == S1[SO].id) { /*se durante o percurso encontrar S1[SO], pára*/
   /*         achouSO = 1;
            break;
         }
         tamOut++;
      }
      /*se SO == NO? neste caso nem executou o for acima: pontos nao seriam contabilizados*/
   /*   if (SO == NO)
         achouSO = 0;
   } */
   /*se "i" não chegou em SO, existem pontos entre S1[0] e S1[SO] que não foram contados*/
   /*if (!achouSO)
      for (i = 0; i < SO; i++)
         tamOut++;*/
   /*desativando o flag para uso posterior*/
   achouSO = 0;
   
   Out = (city *) malloc (tamOut * sizeof (city));
   iOut = 0;
   if (S1[SO].y <= S2[SE].y) {   /*se S1[SO] e mais baixo ou igual que S2[SE]*/
      for (i = 0; i <= SO; i++) {
         if (iOut < tamOut) { /*evitar access overrun*/
            atribui (&Out[iOut], S1[i]);
            iOut++;
         }   
      }
      for (i = SE; i < tamS2; i++) {
         if (iOut < tamOut) { /*evitar access overrun*/
            atribui (&Out[iOut], S2[i]);
            iOut++;
            if (S2[i].id == S2[NE].id) {
               achouNE = 1;
               break;
            }
         }   
      }
      if (!achouNE)
         for (i = 0; i <= NE; i++) 
            if (iOut < tamOut) { /*evitar access overrun*/
               atribui (&Out[iOut], S2[i]);
               iOut++;
            }

      achouNE = 0;
      if (S1[SO].id != S1[NO].id)
         for (i = NO; i < tamS1; i++) {
            if (iOut < tamOut) { /*evitar access overrun*/
               atribui (&Out[iOut], S1[i]);
               iOut++;
               if (S1[i].id == S1[SO].id) {
                  achouSO = 1;
                  break;
               }
            }
         }
      achouSO = 0;
   } else {
      iOut = 0;
      /*começa inserindo do ponto mais baixo de S2 (S2[0]) até S[NE]*/
      for (i = 0; i <= NE; i++) {
         if (iOut < tamOut) { /*evitar access overrun*/
            atribui (&Out[iOut], S2[i]);
            iOut++;
         }   
      }
      if (S1[NO].id == S1[SO].id){ /*se S1[SO] e S1[NO] coincidem, sai do for e insere este ponto apenas uma vez...*/
         if (iOut < tamOut) { /*evitar access overrun*/
            atribui (&Out[iOut], S1[NO]);
            iOut++;
         }   
      }   
      else {
         for (i = NO; i < tamS1; i++) {
            if (iOut < tamOut) { /*evitar access overrun*/
               atribui (&Out[iOut], S1[i]);
               iOut++;
               if (S1[i].id == S1[SO].id){ /*se chegar a SO, pára*/
                  achouSO = 1;
                  break;
               }
            }
         }
         if (!achouSO)
            if (iOut < tamOut) { /*evitar access overrun*/
               for (i = 0; i <= SO; i++){
                  atribui (&Out[iOut], S1[i]);
                  iOut++;
               }
            }   
         achouSO = 0;
      }
      /*fecha o ciclo em S2*/
      if (SE > NE) /*se SE está mais prox do fim do vetor que NE, de SE ate tamS2-1 é o pedaço que falta inserir*/
         if (iOut < tamOut) { /*evitar access overrun*/
            for (i = SE; i < tamS2; i++) {
               atribui (&Out[iOut], S2[i]);
               iOut++;
            }
         }   
   }
   /*se entre os pontos S2[NE] e S1[NO] houver pontos a serem acrescidos ao merge de chs, eles serão acrescentados */
   tamResto = 0;
   if (S1[NO].y == S2[NE].y) {
      for (j = NE; j < tamS2; j++)
         if ((j != NE) && (S2[j].y == S2[NE].y))
            tamResto++;
      for (i = 0; i < NO; i++)
         if (S1[i].y == S1[NO].y)
            tamResto++;
   }         
   if (tamResto > 0) {
      Resto = (city*) malloc (tamResto*sizeof(city));
      k = 0;
      for (j = NE; j < tamS2; j++)
         if (k < tamResto)
            if ((j != NE) && (S2[j].y == S2[NE].y)) {
               atribui (&Resto[k], S2[j]);
               k++;
            }
      for (i = 0; i < NO; i++)
         if (k < tamResto)
            if (S1[i].y == S1[NO].y) {
               atribui (&Resto[k], S1[i]);
               k++;
            }
      for (i = 0; i < tamOut; i++)
         if (Out[i].id == S2[NE].id) {
            marcador = i;
            break;
         }
      OrdResto = convex_hull_colinear_horizontal (Resto, tamResto, &tamOrdResto, ESQDIRSUP);
      newtamOut = tamOut + tamResto;
      newOut = (city*)malloc(newtamOut*sizeof(city));
      for (i = 0; i <= marcador; i++)
         if (i < newtamOut)
            atribui (&newOut[i], Out[i]);
      j = marcador + 1;
      for (i = 0; i < tamResto; i++)
         if (j < newtamOut) {
            atribui (&newOut[j], Resto[i]);
            j++;
         }
      k = marcador + 1;
      for (i = k; i < tamOut; i++)
         if (j < newtamOut) {
            atribui (&newOut[j], Out[i]);
            j++;
         }
      if (Resto != NULL)
         free (Resto);
      if (OrdResto != NULL)
         free (OrdResto);
      if (Out != NULL)
         free (Out);
      *lengthOut = newtamOut;
      return newOut;
   }
   *lengthOut = tamOut; /*tamanho do fecho convexo gerado pelo merge*/
   return Out;
}
/****************************************************************/
city * mergehull (city * E1, int tamE1, city * E2, int tamE2, int * tamch)
{
   city * R1; int tamR1;    city * R2; int tamR2;
   city * R3; int tamR3;    city * R4; int tamR4;
   city * S1; int tamS1;    city * S2; int tamS2;
   city * S; int tamS;
   city * S1aux; int tamS1aux;
   city * S2aux; int tamS2aux;
   int * tgsup; int ts[2];
   int * tginf; int ti[2];
   int i, ccv1 = 0, ccv2 = 0, cch1inf = 0, cch2inf = 0, cch1sup = 0, cch2sup = 0, ccv1aux = 0, ccv2aux = 0;
   /*printf ("\n**************************************************************************");
   printf ("\nE1[%d]: ", tamE1);
   for (i = 0; i < tamE1; i++) printf ("[%d](%4.0f;%4.0f) ", E1[i].id, E1[i].x, E1[i].y);
   printf ("\n**************************************************************************");
   printf ("\nE2[%d]: ", tamE2);
   for (i = 0; i < tamE2; i++) printf ("[%d](%4.0f;%4.0f) ", E2[i].id, E2[i].x, E2[i].y);*/

   ccv1 = conjunto_colinear_vertical (E1, tamE1);
   ccv2 = conjunto_colinear_vertical (E2, tamE2);
   cch1inf = conjunto_colinear_horizontal_esqinf (E1, tamE1, E2, tamE2);
   cch2inf = conjunto_colinear_horizontal_dirinf (E1, tamE1, E2, tamE2);
   cch1sup = conjunto_colinear_horizontal_esqsup (E1, tamE1, E2, tamE2);
   cch2sup = conjunto_colinear_horizontal_dirsup (E1, tamE1, E2, tamE2);

   if (tamE1 == 0 && tamE2 == 0) {
      S = NULL;
      tamS = 0;
   } else {
      if (tamE1 <= 3 && tamE2 <= 3) {
         if (tamE1 == 0)
S = convexo_base (E2, tamE2);
else if (tamE2 == 0)
S = convexo_base (E1, tamE1);
else {
            if      (cch1inf) S1 = convex_hull_colinear_horizontal (E1, tamE1, &tamS1, ESQDIRINF);
            else if (cch1sup) S1 = convex_hull_colinear_horizontal (E1, tamE1, &tamS1, ESQDIRSUP);
            else if (ccv1) S1 = convex_hull_colinear_vertical_esq (E1, tamE1, &tamS1);
            else S1 = convexo_base (E1, tamE1);

            if      (cch2inf) S2 = convex_hull_colinear_horizontal (E2, tamE2, &tamS2, ESQDIRINF);
            else if (cch2sup) S2 = convex_hull_colinear_horizontal (E2, tamE2, &tamS2, ESQDIRSUP);
            else if (ccv2) S2 = convex_hull_colinear_vertical_dir (E2, tamE2, &tamS2);
            else S2 = convexo_base (E2, tamE2);

          tginf = tangente (S1, tamE1, S2, tamE2, INFERIOR);
          tgsup = tangente (S1, tamE1, S2, tamE2, SUPERIOR);
          for (i = 0; i <= 1; i++) { ti[i] = tginf[i]; ts[i] = tgsup[i]; }
          S = merge (S1, tamE1, S2, tamE2, ti, ts, &tamS);
            if (tginf != NULL)
               free (tginf);
            if (tgsup != NULL)
               free (tgsup);
            if (S1 != NULL)
               free (S1);
            if (S2 != NULL)
               free (S2);
}
      } else {
         if (tamE1 <= 3) {
            if (tamE1 == 0) {
               if      (cch2inf) S = convex_hull_colinear_horizontal (E2, tamE2, &tamS, ESQDIRINF);
               else if (cch2sup) S = convex_hull_colinear_horizontal (E2, tamE2, &tamS, ESQDIRSUP);
               else if (ccv2) S = convex_hull_colinear_vertical_dir (E2, tamE2, &tamS);
               else {
                  R1 = dividir (E2, tamE2, &tamR1, PRIMEIRA_METADE);
                  R2 = dividir (E2, tamE2, &tamR2, SEGUNDA_METADE);
                  S = mergehull (R1, tamR1, R2, tamR2, &tamS);
                  if (R1 != NULL)
                     free (R1);
                  if (R2 != NULL)
                     free (R2);
               }
            } else {
               if      (cch1inf) S1 = convex_hull_colinear_horizontal (E1, tamE1, &tamS1, ESQDIRINF);
               else if (cch1sup) S1 = convex_hull_colinear_horizontal (E1, tamE1, &tamS1, ESQDIRSUP);
               else if (ccv1) S1 = convex_hull_colinear_vertical_esq (E1, tamE1, &tamS1);
               else S1 = convexo_base (E1, tamE1);

               if      (cch2inf) S2 = convex_hull_colinear_horizontal (E2, tamE2, &tamS2, ESQDIRINF);
               else if (cch2sup) S2 = convex_hull_colinear_horizontal (E2, tamE2, &tamS2, ESQDIRSUP);
               else if (ccv2) S2 = convex_hull_colinear_vertical_dir (E2, tamE2, &tamS2);
               else {
                  R1 = dividir (E2, tamE2, &tamR1, PRIMEIRA_METADE);
                  R2 = dividir (E2, tamE2, &tamR2, SEGUNDA_METADE);
                  S2 = mergehull (R1, tamR1, R2, tamR2, &tamS2);
                  if (R1 != NULL)
                     free (R1);
                  if (R2 != NULL)
                     free (R2);
               }

               tginf = tangente (S1, tamE1, S2, tamS2, INFERIOR);
               tgsup = tangente (S1, tamE1, S2, tamS2, SUPERIOR);
               for (i = 0; i <= 1; i++) { ti[i] = tginf[i]; ts[i] = tgsup[i]; }

               S = merge (S1, tamE1, S2, tamS2, ti, ts, &tamS);
               if (tginf != NULL)
                  free (tginf);
               if (tgsup != NULL)
                  free (tgsup);
               if (S1 != NULL)
                  free (S1);
               if (S2 != NULL)
                  free (S2);
            }
         } else {
            if (tamE2 <= 3) {
               if (tamE2 == 0) {
                  if      (cch1inf) S = convex_hull_colinear_horizontal (E1, tamE1, &tamS, ESQDIRINF);
                  else if (cch1sup) S = convex_hull_colinear_horizontal (E1, tamE1, &tamS, ESQDIRSUP);
                  else if (ccv1) S = convex_hull_colinear_vertical_esq (E1, tamE1, &tamS);
                  else {
                     R3 = dividir (E1, tamE1, &tamR3, PRIMEIRA_METADE);
                     R4 = dividir (E1, tamE1, &tamR4, SEGUNDA_METADE);
                     S  = mergehull (R3, tamR3, R4, tamR4, &tamS);
                     if (R3 != NULL)
                        free (R3);
                     if (R4 != NULL)
                        free (R4);
                  }
               } else {
                  if      (cch1inf) S1 = convex_hull_colinear_horizontal (E1, tamE1, &tamS1, ESQDIRINF);
                  else if (cch1sup) S1 = convex_hull_colinear_horizontal (E1, tamE1, &tamS1, ESQDIRSUP);
                  else if (ccv1) S1 = convex_hull_colinear_vertical_esq (E1, tamE1, &tamS1);
                  else {
                     R3 = dividir (E1, tamE1, &tamR3, PRIMEIRA_METADE);
                     R4 = dividir (E1, tamE1, &tamR4, SEGUNDA_METADE);
                     S1 = mergehull (R3, tamR3, R4, tamR4, &tamS1);
                     if (R3 != NULL)
                        free (R3);
                     if (R4 != NULL)
                        free (R4);
                  }

                  if      (cch2inf) S2 = convex_hull_colinear_horizontal (E2, tamE2, &tamS2, ESQDIRINF);
                  else if (cch2sup) S2 = convex_hull_colinear_horizontal (E2, tamE2, &tamS2, ESQDIRSUP);
                  else if (ccv2) S2 = convex_hull_colinear_vertical_dir (E2, tamE2, &tamS2);
                  else S2 = convexo_base (E2, tamE2);

                  tginf = tangente (S1, tamS1, S2, tamE2, INFERIOR);
                  tgsup = tangente (S1, tamS1, S2, tamE2, SUPERIOR);
                  for (i = 0; i <= 1; i++) { ti[i] = tginf[i]; ts[i] = tgsup[i]; }

                  S = merge (S1, tamS1, S2, tamE2, ti, ts, &tamS);
                  if (tginf != NULL)
                     free (tginf);
                  if (tgsup != NULL)
                     free (tgsup);
                  if (S1 != NULL)
                     free (S1);
                  if (S2 != NULL)
                     free (S2);
               }
            } else {
               if      (cch1inf) S1 = convex_hull_colinear_horizontal (E1, tamE1, &tamS1, ESQDIRINF);
               else if (cch1sup) S1 = convex_hull_colinear_horizontal (E1, tamE1, &tamS1, ESQDIRSUP);
               else if (ccv1) S1 = convex_hull_colinear_vertical_esq (E1, tamE1, &tamS1);
               else {
                  R1 = dividir (E1, tamE1, &tamR1, PRIMEIRA_METADE);
                  R2 = dividir (E1, tamE1, &tamR2, SEGUNDA_METADE);
                  S1 = mergehull (R1, tamR1, R2, tamR2, &tamS1);
                  ccv1aux = conjunto_colinear_vertical (S1, tamS1);
                  if (ccv1aux) {
                     S1aux = convex_hull_colinear_vertical_esq (S1, tamS1, &tamS1aux);
                     free (S1);
                     tamS1 = tamS1aux;
                     S1 = (city*) malloc (tamS1*sizeof(city));
                     for (i = 0; i < tamS1; i++) atribui (&S1[i], S1aux[i]);
                     free (S1aux);
                  }
                  if (R1 != NULL)
                     free (R1);
                  if (R2 != NULL)
                     free (R2);
               }

               if      (cch2inf) S2 = convex_hull_colinear_horizontal (E2, tamE2, &tamS2, ESQDIRINF);
               else if (cch2sup) S2 = convex_hull_colinear_horizontal (E2, tamE2, &tamS2, ESQDIRSUP);
               else if (ccv2) S2 = convex_hull_colinear_vertical_dir (E2, tamE2, &tamS2);
               else {
                  R3 = dividir (E2, tamE2, &tamR3, PRIMEIRA_METADE);
                  R4 = dividir (E2, tamE2, &tamR4, SEGUNDA_METADE);
                  S2 = mergehull (R3, tamR3, R4, tamR4, &tamS2);
                  ccv2aux = conjunto_colinear_vertical (S2, tamS2);
                  if (ccv2aux) {
                     S2aux = convex_hull_colinear_vertical_dir (S2, tamS2, &tamS2aux);
                     free (S2);
                     tamS2 = tamS2aux;
                     S2 = (city*) malloc (tamS2*sizeof(city));
                     for (i = 0; i < tamS2; i++) atribui (&S2[i], S2aux[i]);
                     free (S2aux);
                  }
                  if (R3 != NULL)
                     free (R3);
                  if (R4 != NULL)
                     free (R4);
               }

               tginf = tangente (S1, tamS1, S2, tamS2, INFERIOR);
               tgsup = tangente (S1, tamS1, S2, tamS2, SUPERIOR);
               for (i = 0; i <= 1; i++) { ti[i] = tginf[i]; ts[i] = tgsup[i]; }

               S = merge (S1, tamS1, S2, tamS2, ti, ts, &tamS);
               if (tginf != NULL)
                  free (tginf);
               if (tgsup != NULL)
                  free (tgsup);
               if (S1 != NULL)
                  free (S1);
               if (S2 != NULL)
                  free (S2);
            }
         }
      }
   }
   *tamch = tamS;
   printf ("\nS[%d]: ", tamS);
   /*for (i = 0; i < tamS; i++) printf ("[%d](%4.0f;%4.0f) ", S[i].id, S[i].x, S[i].y);*/
   printf ("\n**************************************************************************");
   return S;
}

/**************************************************************/
int compare (const void *C1, const void *C2)
{
   int r = 0;
   float x1, x2, y1, y2;
   x1 = ((city*)C1)->x;
   x2 = ((city*)C2)->x;
   if (x1 < x2) r = -1;
   if (x1 > x2) r = 1;
   if (x1 == x2) {
      y1 = ((city*)C1)->y;
      y2 = ((city*)C2)->y;
      if (y1 < y2) r = 1;
      if (y1 > y2) r = -1;
   }
   return r;
}

/**************************************************************/
city * convexhull (city * E, int tamE, int * tamS)
{
   int i;
   city * Esq; int tamEsq;
   city * Dir; int tamDir;
   city * S;   int tamO;
   city * Ecopia;

   Ecopia = (city*) malloc (tamE * sizeof (city));
   for (i = 0; i < tamE; i++)
      atribui (&Ecopia[i], E[i]);
   qsort(Ecopia, tamE, sizeof (city), compare);

if (tamE < 4) {
S = convexo_base (Ecopia, tamE);
*tamS = tamE;
} else {
Esq = dividir (Ecopia, tamE, &tamEsq, PRIMEIRA_METADE);
    Dir = dividir (Ecopia, tamE, &tamDir, SEGUNDA_METADE);
    S = mergehull (Esq, tamEsq, Dir, tamDir, &tamO);
    *tamS = tamO;
    free (Ecopia);
    free (Esq);
    free (Dir);
   }
return S;
}
/**************************************************************/
/* converte um vetor de ids em vetor de city correspondente */
city * mesgtocity (int * message, int tamesg, city * Ecopia, int tamEcopia)
{
   int i, j, cont=0;
   city * S;

   S = (city*) malloc (tamesg * sizeof (city));
   for (j = 0; j < tamesg; j++)
      for (i = 0; i < tamEcopia; i++)
         if (message[j] == Ecopia[i].id) {
            atribui (&S[j], Ecopia[i]); cont++;
            break;
         }
   return S;

}
/* converte um vetor de city em vetor de ids correspondente */
int * citytomesg (city * E, int tamE)
{
   int i;
   int * mesg;

   mesg = (int*) malloc (tamE * sizeof (int));
   for (i = 0; i < tamE; i++)
      mesg[i] = E[i].id;
   return mesg;
}
/**************************************************************/
/**************************************************************/
/**************************************************************/
/**********FUNÇÕES UTILIZADAS NO FARTHEST INSERTION************/
/*************************************************************************/
/*recebe o arquivo de ids de cidades e retorna o numero de cidades existentes*/
int number_cities (FILE * arq)
{
	int count = 0;
	char s[6];

	while (!feof(arq)) {
		fscanf(arq, "%s", s);
		if (atoi(s) == -1)
		break;
		count++;
	}
	return (count);
}
/*recebe o numero de cidades, o arquivo de ids e o de coordenadas*/
/*retorna o vetor de cidades*/
city * load_cities (int num_cities, FILE * fp_ids, FILE * fp_coord)
{
    char s[6];
    city * v_map;
    int i = 0;
    s[0] = '0'; s[1] = '0'; s[2] = '0'; s[3] = '0';
    v_map = (city *) malloc (num_cities * sizeof (city));
    while (!feof(fp_coord))
    {
       fscanf(fp_ids, "%s", s);
       if (atoi(s) == -1) break;
       v_map[i].id = atoi(s);
       fscanf(fp_coord, "%s", s); v_map[i].x = atof(s);
       fscanf(fp_coord, "%s", s); v_map[i].y = atof(s);
       i++;
    }
    return v_map;

}
/*recebe o vetor de cidades com o respectivo numero de cidades*/
/*retorna uma matriz de adjacencias*/
adj ** popula_v_adj (city * v_map, int num_cities)
{
   int i, j;
   adj ** v_adj;

   v_adj = (adj **) malloc (num_cities * sizeof (adj*));
   for (j = 0; j < num_cities; j++)
       v_adj[j] = (adj *) malloc (num_cities * sizeof (adj));

   for (j = 0; j < num_cities; j++)
       for (i = 0; i < num_cities; i++)
       {
           v_adj[j][i].idx = v_map[i].id;
           v_adj[j][i].idy = v_map[j].id;
           v_adj[j][i].dist = distance(v_map[j], v_map[i]);
       }
   return (v_adj);
}
/*busca na matriz de adjacencias pela distancia entre duas cidades id1 e id2*/
float distance_city (adj ** v_adj, int num_cities, int id1, int id2 )
{
    int i, j;
    for (j = 0; j < num_cities; j++)
        for (i = 0; i < num_cities; i++)
            if ((v_adj[j][i].idx == id1) && (v_adj[j][i].idy == id2))
                return v_adj[j][i].dist;
    return -1; /*erro: nao foi encontrada distancia*/
}
/*recebe: v_adj [num_cities X num_cities], cidade "id", um subtour IdTour [num_citiesIdTour]*/
/*retorna: a soma das distancias de todos os ids de IdTour a cidade "id"*/
float distance_city_from_tour (adj ** v_adjacente, int num_cities, int id, int * IdTour, int num_citiesIdTour) {
	int i;
	float dist = 0;
	for (i = 0; i < num_citiesIdTour; i++)
		dist += distance_city (v_adjacente, num_cities, id, IdTour[i]);
	return (dist);
}
/*fitness da rota*/
float custo_rota (city * rota, adj ** v_adjacente, int num_cities)
{
	int i;
	float custo = 0;
	for (i = 0; i < num_cities - 1; i++)
		custo += distance_city (v_adjacente, num_cities, rota[i].id, rota[i+1].id);
	return (custo);
}
/*retorna o id da cidade mais distante da cidade "id"*/
int farthest_point_from_id (city * E, adj ** v_adj, int num_cities, int id)
{
	int i, farthest_id;
	float dist, dist_max = 0;
	for (i = 0; i < num_cities; i++) {
		dist = distance_city (v_adj, num_cities, E[i].id, id);
		if (dist > dist_max) {
			farthest_id = E[i].id;
			dist_max = dist;
		}
	}
	return (farthest_id);
}
int IsInTour (int * E, int num_cities, int id)
{
	int i;
	for (i = 0; i < num_cities; i++)
		if (E[i] == id)
			return 1;
	return 0;
}
int IsInTourCity (city * E, int num_cities, int id)
{
	int i;
	for (i = 0; i < num_cities; i++)
	if (E[i].id == id)
		return 1;
	return 0;
}
int compare_farthest_point (const void *C1, const void *C2)
{
   int r = 0;
   float dist1 = ((vdist_from_tour*)C1)->dist_from_tour;
   float dist2 = ((vdist_from_tour*)C2)->dist_from_tour;
   if (dist1 < dist2) r = -1;
   if (dist1 > dist2) r = 1;
   return r;
}
/*retorna o id da cidade mais distante do subtour STour */
/*esta funcao é muito lenta, estou tentando substitui-la por algo mais rapido*/
int farthest_point_from_subtour (city *E, int num_citiesE, adj **v_adj, int *STour, int num_citiesSTour)
{
	int i, j, tamD, esta, farthest_id;
   vdist_from_tour * Ddist;
	float dist;

   /*inicia o vetor de cidades não inseridas no tour*/
	tamD = num_citiesE - num_citiesSTour;
   Ddist = (vdist_from_tour*) malloc (tamD * sizeof (vdist_from_tour));

   /*popula este vetor*/
	j = 0;
	for (i = 0; i < num_citiesE; i++) {
		esta = IsInTour (STour, num_citiesSTour, E[i].id);
		if (!esta) {
         Ddist[j].id = E[i].id;
         Ddist[j].dist_from_tour = distance_city_from_tour (v_adj, num_citiesE, Ddist[j].id, STour, num_citiesSTour);
			j++;
		}
	}
	/*D eh um vetor de ids que estao fora do subtour STour*/
   qsort (Ddist, tamD, sizeof (vdist_from_tour), compare_farthest_point);
   farthest_id = Ddist[tamD-1].id;

   free (Ddist);
	return (farthest_id);
}
city * busca (city * E, int num_cities, int id)
{
	int i;
	city * cidade;
	cidade = (city *) malloc (1 * sizeof (city));
	for (i = 0; i < num_cities; i++)
	if (E[i].id == id) {
		cidade->id = E[i].id;
		cidade->x  = E[i].x;
		cidade->y  = E[i].y;
	}
	return (cidade);
}

void insert_id (int * STour, int * num_citiesSTour, int id, int indice_i, int indice_j)
{
	int i;

	for (i = *num_citiesSTour - 1; i >= indice_j; i--)
		STour[i+1] = STour[i];
	STour[indice_j] = id;
	(*num_citiesSTour)++;
}

int * farthest_insertion (city * E, adj ** v_adj, int num_cities, int initial_node)
{
	int i, c, indice_i, indice_j;
	int numinscities = 0; /*numero de cidades ja inseridas em S*/
	city * cidade;
	int * S;
	float dist, dist_min = INFINITO, dist_ir, dist_rj, dist_ij;

	S = (int *) malloc (num_cities * sizeof (int));

	for (i = 0; i < num_cities; i++)
		if (E[i].id == initial_node)
			S[0] = E[i].id;
	numinscities++;

	c = farthest_point_from_id (E, v_adj, num_cities, S[0]);
	S[1] = c;
	numinscities++;

	while (numinscities < num_cities) {

		c = farthest_point_from_subtour (E, num_cities, v_adj, S, numinscities);
		for (i = 0; i < numinscities - 1; i++) {
			dist_ir = distance_city (v_adj, num_cities, S[i],      c);
			dist_rj = distance_city (v_adj, num_cities,    c, S[i+1]);
			dist_ij = distance_city (v_adj, num_cities, S[i], S[i+1]);
			dist    = dist_ir + dist_rj - dist_ij;
			if (dist < dist_min) {
				dist_min = dist;
				indice_i = i;
				indice_j = i+1;
			}
		}
		dist_min = INFINITO;
		insert_id (S, &numinscities, c, indice_i, indice_j);
	}
	return (S);
}
/*recebe o conjunto de cidades e seu tamanho e o fecho convexo e seu tamanho*/
/*retorna um vetor de cidades que estao fora do fecho convexo com suas respectivas distancias do mesmo*/
vdist_from_tour * inicia_vcity_N (city * In1, int num_cities, city * Ch, int numinscities, int * tamN)
{
	int i, j, esta;
	vdist_from_tour * N;

	*tamN = num_cities - numinscities; /*tamanho do vetor N*/
	/*printf ("\n|||||||||||||||||tamN = %d", *tamN);*/
	N = (vdist_from_tour*) malloc (*tamN * sizeof (vdist_from_tour));
   /*for (i = 0; i < *tamN; i++) {
      N[i].id = 0; N[i].dist_from_tour = 0;
   } */
   j = 0;
	for (i = 0; i < num_cities; i++) {
		esta = IsInTourCity (Ch, numinscities, In1[i].id);
		if (!esta) {
		N[j].id = In1[i].id;
		j++;
	}
}
   return N;
}
int * farthest_insertion_from_ch (city * E, adj ** v_adj, int num_cities, city * Ch, int * numinscities)
{
   int c, i, j, indice_i, indice_j, tamN, esta = 0;
   float dist, dist_min = INFINITO, dist_ir, dist_rj, dist_ij, dist_tour;
   vdist_from_tour *N;
   int * S;
   int * cCh;

   /*N contem os pontos ainda não inseridos*/
	N = inicia_vcity_N (E, num_cities, Ch, *numinscities, &tamN);

   cCh = (int *) malloc ((*numinscities) * sizeof(int)); /*versão de int do Ch*/
   for (i = 0; i < *numinscities; i++)
      cCh[i] = Ch[i].id;
   for (i = 0; i < tamN; i++) {
      N[i].dist_from_tour = distance_city_from_tour (v_adj, num_cities, N[i].id, cCh, *numinscities);
      printf ("\nN[%i].dist_from_tour = %5.2f",i, N[i].dist_from_tour);
   }

   S = (int *) malloc (num_cities * sizeof(int));
   for (i = 0; i < *numinscities; i++)
      S[i] = Ch[i].id;

	while (*numinscities < num_cities) {

		/*c = farthest_point_from_subtour (E, num_cities, v_adj, S, *numinscities);*/
      qsort (N, tamN, sizeof(vdist_from_tour), compare_farthest_point);
      c = N[tamN-1].id;

		for (i = 0; i < *numinscities - 1; i++) {
			dist_ir = distance_city (v_adj, num_cities, S[i],      c);
			dist_rj = distance_city (v_adj, num_cities,    c, S[i+1]);
			dist_ij = distance_city (v_adj, num_cities, S[i], S[i+1]);
			dist    = dist_ir + dist_rj - dist_ij;
			if (dist < dist_min) {
				dist_min = dist;
				indice_i = i;
				indice_j = i+1;
			}
		}
		dist_min = INFINITO;
		printf ("\n*numinscities = %d ", *numinscities);
      /*o ponto c será eliminado de N e inserido em S*/
      tamN--;
      N = (vdist_from_tour*)realloc(N, tamN*sizeof(vdist_from_tour));
      /*o tour conterá c, logo a distancia do ponto nao inserido ao tour deve ser atualizado*/
      for (i = 0; i < tamN; i++)
         N[i].dist_from_tour += distance_city (v_adj, num_cities, N[i].id, c);
      /*inserção de c em S*/
		insert_id (S, numinscities, c, indice_i, indice_j);
	}
   free(N);
   free(cCh);
	return (S);
}
adj ** free_matrix_adj (adj ** v_adj, int tam)
{
   int i;
   if (v_adj == NULL) return (NULL);
   if (tam < 1) {  /* verifica parametros recebidos */
     printf ("** Erro: Parametro invalido **\n");
     return (v_adj);
   }
   for (i=0; i<tam; i++)
      free (v_adj[i]); /* libera as linhas da matriz */
   free (v_adj);      /* libera a matriz (vetor de ponteiros) */
   return (NULL); /* retorna um ponteiro nulo */
}
/*************************************************************************/
#pragma argsused
int main(int argc, char* argv[])
{
   FILE *fp_ids, *fp_coord;
	adj ** v_adj;
	int tam=0, i=0, tamCh=0, tamSubTour=0;
	city * In, * Ch, *cOut;
	int * Out;
	float custoIn, custoOut;

	fp_ids   = fopen (argv[1], "r");
	fp_coord = fopen (argv[2], "r");
	if ( !fp_ids || !fp_coord ) {
		printf("Erro ao abrir arquivos de entrada.\n");
   	exit(1);
   }

	tam = number_cities(fp_ids);
   rewind(fp_ids);
	In = load_cities (tam, fp_ids, fp_coord);
	v_adj = popula_v_adj (In, tam);
	Ch = convexhull (In, tam, &tamCh);
	tamSubTour = tamCh;
	Out = farthest_insertion_from_ch (In, v_adj, tam, Ch, &tamSubTour);
	/*invariante: tamSubTour == tam*/
	cOut = mesgtocity (Out, tamSubTour, In, tam);
	custoIn = custo_rota (In, v_adj, tam);
	custoOut = custo_rota (cOut, v_adj, tamSubTour);

	printf("\n Entrada In: ");	for (i = 0; i < tam; i++)	printf ("[%d] ", In[i].id);
	printf ("custo: %f", custoIn);
	printf("\n Saída  Out: ");	for (i = 0; i < tam; i++)	printf ("[%d] ", cOut[i].id);
	printf ("custo: %f", custoOut);

	free(In);
	free(Ch);
	free(cOut);
   free(Out);
   free_matrix_adj(v_adj, tam);
   fclose (fp_ids);
   fclose (fp_coord);
	return 0;
}
