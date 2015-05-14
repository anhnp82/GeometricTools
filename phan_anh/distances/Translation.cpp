#include <cstdio>
#include <cmath>

using namespace std;

#include "util.h"
#include "Translation.h"
#include "Tippunkt.h"
#include "Werkzeug.h"
#include "Tippunkt.h"
#include "Konstanten.h"


/*******************************************************************
 * 
 *  Konstruktor: erzeugt Translations-Objekt
 *                       p0: Tip-Punkt Anfang,
 *                       p1: Tip-Punkt Ende,
 *                       achse0: Richtungsvektor Anfang
 *
 *******************************************************************/
SweepDist::Translation::Translation
( double p0[3], 
  double achse0[3], 
  double p1[3]
)
{ int i, j;

  double tmp;
  double tmpvek[3];
  /* Punkte und Achsen uebernehmen: */
  for ( i = 0; i < 3; ++i)
  { 
    P0[i] = p0[i];
    P1[i] = p1[i];
    Achse0[i] = achse0[i];
    tmpvek[i] = P1[i] - P0[i];
  }

  
  if ( norm3(tmpvek) < 1.E-7)
    throw Translationsfehler(); // keine wirkliche Translation!

  /* Basiswechselmatrix ins Tippunkt-KO-System,
   * Beginn der Bewegung
   */

  /* 1. Schritt: so drehen, dass die Achse am Anfang in (neue) 
   * positive z-Richtung
   * zeigt: */
  tmp = norm2(Achse0);

  if ( tmp < EPS_R)
  { /* Basiswechsel ist Einheitsmatrix, keine Drehung erforderlich: */
    for ( i = 0; i < 3; ++i)
      for ( j = 0; j < 3; ++j)
        BT_B[i][j] = 0.0;

    for ( i = 0; i < 3; ++i)
      BT_B[i][i] = 1.0;

    /* ggf. Umdrehen, falls Achse in negative z-Richtung zeigt: */
    if ( achse0[2] < 0.0)
    { BT_B[1][1] = -1.0;	    
      BT_B[2][2] = -1.0;	    
    }
  }
  else
  { /* 'wirkliche' Drehung: */
    BT_B[0][0] =   Achse0[1]/tmp;
    BT_B[0][1] = - Achse0[0]/tmp;
    BT_B[0][2] = 0.0;

    BT_B[1][0] = - BT_B[0][1]*Achse0[2];
    BT_B[1][1] =   BT_B[0][0]*Achse0[2];
    BT_B[1][2] = - tmp;

    BT_B[2][0] = Achse0[0];
    BT_B[2][1] = Achse0[1];
    BT_B[2][2] = Achse0[2];
  }

  /* BT_B ist jetzt Basiswechselmatrx in das Werkzeug-KO-System,
   * Torus zu Beginn der Bewegung!
   */

  /* inversen Basiswechsel berechnen: Matrix invertieren,
   * also transponieren, da orthogonal: */
  for ( i = 0; i < 3; ++i)
    for ( j = 0; j < 3; ++j)
      B_BT[i][j] = BT_B[j][i];	    

  /* Verschiebungsvektoren berechnen: */
  matrix_mal_vektor3(BT_B,P0, VBT_B);
  for ( i = 0; i < 3; ++i)
  { VBT_B[i] *= -1;
    VB_BT[i] = P0[i];
  }  
    
  /* Bewegungsvektor im Welt-KO-System: */
  vektoradd3(1,P1,-1,P0,aw);

  /* Bewegungsvektor im Torus-KO-System: */
  matrix_mal_vektor3(BT_B,aw, a);

  /* maximale Geschwindigkeit aufgrund der Verschiebung: */
  va = norm3(a);

  return ;
}


// Erzeuge Tippunkt-Ko-System:
void SweepDist::Translation::mach_tippunkt
( double t, 
  Tippunkt &tp
)
{ 
  double tmpvek1[3], tmpvek2[3],tmpmat1[3][3];

  for ( int i = 0; i < 3; ++i)
    for ( int j = 0; j < 3; ++j)
    { tp.bw[i][j] = BT_B[i][j];
      tp.inv_bw[i][j] = B_BT[i][j];
    }


  /* Verschiebungsvektor berechnen: */
  matrix_mal_vektor3(tmpmat1, VBT_B,tmpvek2);

  tmpvek1[0] = -t*a[0];
  tmpvek1[1] = -t*a[1];
  tmpvek1[2] = -t*a[2]; 
  vektoradd3(1.0,tmpvek1,1.0,VBT_B,tp.versch);

  /* Verschiebungsvektor der inversen KO-Traffo berechnen: */
  tmpvek1[0] =  t*a[0];
  tmpvek1[1] =  t*a[1];
  tmpvek1[2] =  t*a[2];

  matrix_mal_vektor3(B_BT,tmpvek1,tmpvek2);
  vektoradd3(1.0,VB_BT,1.0,tmpvek2,tp.inv_versch);

  tp.initialized = true;

  return;
}

double SweepDist::Translation::speed(double t, double point_wz[], double speed_v[])
{ speed_v[0] = aw[0];
  speed_v[1] = aw[1];
  speed_v[2] = aw[2];

  return va;
}


