#ifndef UTIL_SWEEPDIST_H
#define UTIL_SWEEPDIST_H

#include <cmath>

namespace SweepDist
{

class MathFehler
{};


/* Berechnet Norm des Vektors v im R^n: */
double norm(double *v, int n);
double norm2(double *v);
double norm3(double *v);


/* Berechnet Skalarprodukt der beiden Vektoren a und b im R^n */
double skalarprodukt(double *v, double *w, int n);
double skalarprodukt2(double *v, double *w);
double skalarprodukt3(double *v, double *w);


/* Berechnet a x b, Ergebnis nach c: */
void kreuzprodukt(double *a, double *b, double *c);


/* Matrix mal Vektor, Ergebnis nach y, im R^2 */
void matrix_mal_vektor2(double mat[2][2], double x[2], double y[2]);


/* Matrix mal Vektor, Ergebnis nach y, im R^3 */
void matrix_mal_vektor3(double mat[3][3], double x[3], double y[3]);


/* Matrix mal Matrix im R^3, Ergebnis nach y */
void matrix_mal_matrix3(double mat1[3][3], double mat2[3][3], double y[3][3]);


/* Linearkombination zweier Vektoren a x + b y im R^n, Ergebnis nach z */
void vektoradd(double a, double *x, double b, double *y, double *z, int n);
void vektoradd2(double a, double *x, double b, double *y, double *z);
void vektoradd3(double a, double *x, double b, double *y, double *z);


/* normiert den Vektor x auf euklidische Laenge 1, im R^n */
void normieren(double *x, int n);
void normieren2(double *x);
void normieren3(double *x);


/* Loesen der quadratischen Gleichung a x^2 + bx + c = 0 
  
   Rueckgabe: 
    3:    falls a=b=c=0 und somit die Gleichung allgemeingueltig ist
    2:    falls 2 verschiedene reelle Loesungen existieren, diese
          werden dann in loes[0] und loes[1] zurueckgegeben, wobei
          loes[0] < loes[1] gilt
    1:    es existiert genau eine reelle Nullstelle - diese wird 
          in loes[0] zurueckgegeben
    0:    es exisitiert keine reelle Loesung
*/
int quadsolve(double a, double b, double c, double loes[2]);


/* Loesen der Gleichung  4-ten Grades ax^4 + bx^3 + cx^2 + dx + e = 0
   mit a != 0.


   Nach

   Heitzinger/Troch/Valentin
   Praxis nichtlinearer Gleichungen
   Carl-Hanser-Verlag Muenchen 1984
   ISBN 3-446-14079-4

   Kapitel 3.4, Seite 101 - 103


   Zurueckgegeben wird die Anzahl der verschiedenen reellen Loesungen.
   Reelle Loesungen selbst werden soritiert in das Feld loes geschrieben
*/
int quartsolve(double a, double b, double c, double d, double e, double loes[]);


/* Loesen der kubischen Gleichung ax^3 + bx^2 + cx + d = 0
   mit a != 0.


   Nach

   Heitzinger/Troch/Valentin
   Praxis nichtlinearer Gleichungen
   Carl-Hanser-Verlag Muenchen 1984
   ISBN 3-446-14079-4

   Kapitel 3.3, Seite 89 - 94


   ACHTUNG: auf Seite 91 im Buch ist ein Fehler,
   im Fall D == 0 gilt z2 = z3 = -z1 / 2
   (die Division durch 2 fehlt im Buch!)


   Zurueckgegeben wird die Anzahl der verschiedenen reellen Loesungen
   Reelle Loesungen selbst werden soritiert in das Feld loes geschrieben.
*/
int kubsolve(double a, double b, double c, double d, double loes[]);


/* Nachiteration einer gefundenen Nullstelle x eines Polynoms 
   vom Grad maximal 4.

   anz ist die Anzahl der Nachiterationsschritte

   Es wird die verbesserte Loesung zurueckgegeben!
*/
double nachiteration(int grad, double pol[], double x, int anz);

}

#endif

