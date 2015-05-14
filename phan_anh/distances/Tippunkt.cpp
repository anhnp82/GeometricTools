#include <cstdio>

using namespace std;

#include "Tippunkt.h"
#include "util.h"
#include "Konstanten.h"

#pragma warning( disable : 4290 )
/* Konstruktor
  
   erzeug Tippunkt aus Punkt und Achse:
*/
SweepDist::Tippunkt::Tippunkt
( double p[], 
  double achse[]
)
{
  int i, j;
  double tmp;

  /* Basiswechselmatrix ins Tippunkt-KO-System,
   */

  /* so drehen, dass die Achse in (neue)
   * positive z-Richtung
   * zeigt: */
  tmp = norm2(achse);

 if ( tmp < EPS_R)
  { /* Basiswechsel ist Einheitsmatrix, keine Drehung erforderlich: */
    for ( i = 0; i < 3; ++i)
      for ( j = 0; j < 3; ++j)
        bw[i][j] = 0.0;

    for ( i = 0; i < 3; ++i)
      bw[i][i] = 1.0;

    /* ggf. Umdrehen, falls Achse in negative z-Richtung zeigt: */
    if ( achse[2] < 0.0)
    { bw[1][1] = -1.0;
      bw[2][2] = -1.0;
    }
  }
  else
  { /* 'wirkliche' Drehung: */
    bw[0][0] =   achse[1]/tmp;
    bw[0][1] = - achse[0]/tmp;
    bw[0][2] = 0.0;
    bw[1][0] = - bw[0][1]*achse[2];
    bw[1][1] =   bw[0][0]*achse[2];
    bw[1][2] = - tmp;
    bw[2][0] = achse[0];
    bw[2][1] = achse[1];
    bw[2][2] = achse[2];
  }

  /* invertieren = transponieren */
  for ( i = 0; i < 3; ++i)
    for ( j = 0; j < 3; ++j)
      inv_bw[i][j] = bw[j][i];	   

  /* inverse Verschiebung gleich p */ 
  for ( i = 0; i < 3; ++i)
    inv_versch[i] = p[i];

  /* originale Verschiebung: */
  matrix_mal_vektor3(bw, p, versch);
  for ( i = 0; i < 3; ++i)
    versch[i] *= -1;

  initialized = true;

#ifdef TIPPUNKT
  fprintf(stderr,"KO Welt -> Tippunkt: \n");
  for ( i = 0; i < 3; ++i )
    fprintf(stderr,"%8.5f  %8.5f  %8.5f     %8.5f\n", bw[i][0], bw[i][1], bw[i][2], versch[i]);
  fprintf(stderr,"\n");

  fprintf(stderr,"KO Tippunkt -> Welt: \n");
  for ( i = 0; i < 3; ++i )
    fprintf(stderr,"%8.5f  %8.5f  %8.5f     %8.5f\n", inv_bw[i][0], inv_bw[i][1], inv_bw[i][2], inv_versch[i]);
#endif

  fprintf(stderr,"\n");

}


// Punkt von Welt-Koordinaten in Tippunkt-Koordinaten umrechnen
void SweepDist::Tippunkt::punkt_welt_tippunkt
( double p[], 
  double q[]
) throw(Tippunktfehler)
{ if ( ! initialized) 
    throw Tippunktfehler();

  double tmpvek[3];
  matrix_mal_vektor3(bw, p, tmpvek);
  vektoradd3(1.0, tmpvek, 1.0, versch, q);
}


// Punkt von Tippunkt-Koordinaten in Welt-Koordinaten zurueckrechnen
void SweepDist::Tippunkt::punkt_tippunkt_welt
( double q[], 
  double p[]
) throw(Tippunktfehler)
{ if ( ! initialized) 
    throw Tippunktfehler();

  double tmpvek[3];
  matrix_mal_vektor3(inv_bw, q, tmpvek);
  vektoradd3(1.0, tmpvek, 1.0, inv_versch, p);
}


// Vektor von Weltkoordinaten in Tippunkt-Koordinaten umrechnen
// (keine Verschiebung)
void SweepDist::Tippunkt::vektor_welt_tippunkt
( double p[], 
  double q[]
) throw(Tippunktfehler)
{ if ( ! initialized) 
    throw Tippunktfehler();
  matrix_mal_vektor3(bw, p, q);
}


// Vektor von Tippunkt-Koordinaten in Welt-Koordinaten zurueckrechnen
// (keine Verschiebung)
void SweepDist::Tippunkt::vektor_tippunkt_welt
( double q[], 
  double p[]
) throw(Tippunktfehler)
{ if ( ! initialized) 
    throw Tippunktfehler();
  matrix_mal_vektor3(inv_bw, q, p);
}

#pragma warning( default : 4290 )


