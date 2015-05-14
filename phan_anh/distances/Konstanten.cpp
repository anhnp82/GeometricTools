#include "Konstanten.h"


/* EPS_R: minimale raeumliche Distanz

   Falls die Distanz eines Werkzeuges von einem Werkstueck
   kleienr als EPS_R ist, wird dies als Kollision gewertet.
*/
const double SweepDist::EPS_R = 1.0E-9;


/* EPS_GRAD: minimale Gradientenlaenge

   nur bei Abstaenden groesser als EPS_GRAD 
   werden Skalarprodukte mit Gradienten 
   benutzt, um Kollisionen auszuschliessen.

   EPS_GRAD sollte mindestens  10 x EPS_R  sein!
*/
const double SweepDist::EPS_GRAD = 1.0E-8;

/* EPS_T: minimale zeitliche Distanz

   Zeitpunkte mit geringerem Abstand werden 
   als identisch angesehen.

   EPS_T sollte so sein, dass bei der Geschwindigkeit V
   einer "ueblichen" Bewegung ungefaehr

     V x EPS_T = EPS_R

  gilt.
*/
const double SweepDist::EPS_T = 1.0E-12;


/* EPS_SP: minimales Skalarprodukt zweier (normierter) Vektoren

   bei kleinerem Betrag eines Skalarproduktes werden die 
   beteiligten Vektoren als aufeinander senkrecht angesehen.
*/
const double SweepDist::EPS_SP = 1.0E-8;


/* EPS_FKT: minimaler Betrag eines Funktionswertes

   (betrags-) kleinere Funktionswerte werden als 0 angesehen.
*/
const double SweepDist::EPS_FKT = 1.0E-10;


/* EPS_NULLST: minimale Differenz fuer Nullstellen 

  absolute Fehlerschranke bei Nullstellenberechnungen 
*/
const double SweepDist::EPS_NULLST = 1.0E-9;


/* EPS_KOEFF: minimaler Betrag eines Polynomkoeffizienten 

  Koeffizienten mit kleinerem Betrag werden als 0 angesehen.
*/
const double SweepDist::EPS_KOEFF = 1.0E-9;


/* EPS_PARA, BOUND_PARA0:

  anhand dieser Konstanten wird entschieden, ob eine
  Hyperbel ("parabula") zu einem Strahl ausgeartet ist 
  oder nicht.

  Die Werte  EPS_PARA = 1.0E-6  und BOUBD_PARA = 1.0E9
  haben sich bewaehrt.
*/
const double SweepDist::EPS_PARA = 1.0E-6;
const double SweepDist::BOUND_PARA0 = 1.0E9;


/* PI: Naeherungswert fuer Kreiszahl.
*/
const double SweepDist::PI = 3.14159265359;



