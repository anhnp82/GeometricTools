#ifndef KONSTANTEN_SWEEPDIST_H
#define KONSTANTEN_SWEEPDIST_H

namespace SweepDist
{

/* EPS_R: minimale raeumliche Distanz

   Falls die Distanz eines Werkzeuges von einem Werkstueck
   kleienr als EPS_R ist, wird dies als Kollision gewertet.
*/
extern const double EPS_R ; // = 1.0E-9


/* EPS_GRAD: minimale Gradientenlaenge

   nur bei Abstaenden groesser als EPS_GRAD 
   werden Skalarprodukte mit Gradienten 
   benutzt, um Kollisionen auszuschliessen.

   EPS_GRAD sollte mindestens  10 x EPS_R  sein!
*/
extern const double EPS_GRAD ; // = (10*EPS_R)

/* EPS_T: minimale zeitliche Distanz

   Zeitpunkte mit geringerem Abstand werden 
   als identisch angesehen.

   EPS_T sollte so sein, dass bei der Geschwindigkeit V
   einer "ueblichen" Bewegung ungefaehr

     V x EPS_T ; // = EPS_R

  gilt.
*/
extern const double EPS_T ; // = 1.0E-12


/* EPS_SP: minimales Skalarprodukt zweier (normierter) Vektoren

   bei kleinerem Betrag eines Skalarproduktes werden die 
   beteiligten Vektoren als aufeinander senkrecht angesehen.
*/
extern const double EPS_SP ; // = 1.0E-8


/* EPS_FKT: minimaler Betrag eines Funktionswertes

   (betrags-) kleinere Funktionswerte werden als 0 angesehen.
*/
extern const double EPS_FKT ; // = 1.0E-10


/* EPS_NULLST: minimale Differenz fuer Nullstellen 

  absolute Fehlerschranke bei Nullstellenberechnungen 
*/
extern const double EPS_NULLST ; // = 1.0E-9


/* EPS_KOEFF: minimaler Betrag eines Polynomkoeffizienten 

  Koeffizienten mit kleinerem Betrag werden als 0 angesehen.
*/
extern const double EPS_KOEFF ; // = 1.0E-9


/* EPS_PARA, BOUND_PARA0:

  anhand dieser Konstanten wird entschieden, ob eine
  Hyperbel ("parabula") zu einem Strahl ausgeartet ist 
  oder nicht.

  Die Werte  EPS_PARA = 1.0E-6  und BOUBD_PARA = 1.0E9
  haben sich bewaehrt.
*/
extern const double EPS_PARA ; // = 1.0E-6
extern const double BOUND_PARA0 ; // = 1.0E9


/* PI: Naeherungswert fuer Kreiszahl.
*/
extern const double PI ; // = 3.14159265359

}

#endif

