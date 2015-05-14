#include <cstdio>
#include <cmath>

using namespace std;

#include "util.h"
#include "Bewegung.h"
#include "Tippunkt.h"
#include "Werkzeug.h"
#include "Tippunkt.h"
#include "Konstanten.h"


/* Konstruktor: erzeugt Bewegungs-Objekt
                         p0: Tip-Punkt Anfang,
                         p1: Tip-Punkt Ende,
                         achse0: Richtungsvektor Anfang
                         achse1: Richtungsvektor Ende
*/
SweepDist::Bewegung::Bewegung
( double p0[3], 
  double achse0[3], 
  double p1[3], 
  double achse1[3]
)
{ int i, j;

  double tmp, tmpvek[3];

  double tmpmatrix1[3][3];
  double tmpmatrix2[3][3];

  /* Punkte und Achsen uebernehmen: */
  for ( i = 0; i < 3; ++i)
  { 
    P0[i] = p0[i];
    P1[i] = p1[i];
    Achse0[i] = achse0[i];
    Achse1[i] = achse1[i];
  }

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
        tmpmatrix1[i][j] = 0.0;

    for ( i = 0; i < 3; ++i)
      tmpmatrix1[i][i] = 1.0;

    /* ggf. Umdrehen, falls Achse in negative z-Richtung zeigt: */
    if ( achse0[2] < 0.0)
    { tmpmatrix1[1][1] = -1.0;	    
      tmpmatrix1[2][2] = -1.0;	    
    }
  }
  else
  { /* 'wirkliche' Drehung: */
    tmpmatrix1[0][0] =   Achse0[1]/tmp;
    tmpmatrix1[0][1] = - Achse0[0]/tmp;
    tmpmatrix1[0][2] = 0.0;

    tmpmatrix1[1][0] = - tmpmatrix1[0][1]*Achse0[2];
    tmpmatrix1[1][1] =   tmpmatrix1[0][0]*Achse0[2];
    tmpmatrix1[1][2] = - tmp;

    tmpmatrix1[2][0] = Achse0[0];
    tmpmatrix1[2][1] = Achse0[1];
    tmpmatrix1[2][2] = Achse0[2];
  }

#if 0
  fprintf(stderr,"1. Basiswechsel:\n");
  for ( i = 0; i < 3;++i)
    fprintf(stderr,"%10f %10f %10f\n", tmpmatrix1[i][0], 
		                       tmpmatrix1[i][1],
				       tmpmatrix1[i][2]);  
  fprintf(stderr,"\n");

  matrix_mal_vektor3(tmpmatrix1, Achse0, tmpvek);
  fprintf(stderr,"zur Kontrolle d0 bzgl. neuen KO-Systems," 
		  " muesste (0,0,1) sein:\n");
  fprintf(stderr,"(%10f,%10f,%10f)\n", tmpvek[0],tmpvek[1], tmpvek[2]);
#endif

  /* 2. Schritt: so um die z-Achse drehen, dass die Achse am Ende
   * der Torus-Bewegung in der y-z-Ebene liegt und eine negative 
   * y-Komponente hat: 
   */
  matrix_mal_vektor3(tmpmatrix1, Achse1, tmpvek);

#if 0
  fprintf(stderr,"d1 bzgl. neuen KO-Systems:\n");
  fprintf(stderr,"(%10f,%10f,%10f)\n", tmpvek[0],tmpvek[1], tmpvek[2]);
#endif

  tmp = norm2(tmpvek);
  if ( fabs(tmpvek[0]) < EPS_R)
  { /* keine Drehung erforderlich, Basiswechsel ist Einheitsmatrix */
    for ( i = 0; i < 3; ++i)
      for ( j = 0; j < 3; ++j)
        tmpmatrix2[i][j] = 0.0;
    for ( i = 0; i < 3; ++i)
      tmpmatrix2[i][i] = 1.0;
	  
    /* ggf. umorientieren, falls y-Komponente von (umgerechneter)
     * achse1 positiv ist:
     */
    if ( tmpvek[1] > 0)
    { tmpmatrix2[0][0] *= -1;
      tmpmatrix2[1][1] *= -1;
    }      
  }
  else
  { /* 'wirkliche' Drehung: */
    tmpmatrix2[0][0] = - tmpvek[1]/tmp;
    tmpmatrix2[0][1] = + tmpvek[0]/tmp;
    tmpmatrix2[0][2] = 0.0;

    tmpmatrix2[1][0] = - tmpvek[0]/tmp;
    tmpmatrix2[1][1] = - tmpvek[1]/tmp;
    tmpmatrix2[1][2] = 0.0;

    tmpmatrix2[2][0] = 0.0;
    tmpmatrix2[2][1] = 0.0;
    tmpmatrix2[2][2] = 1.0;
  }
#if 0
  fprintf(stderr,"2. Basiswechsel:\n");
  for ( i = 0; i < 3;++i)
    fprintf(stderr,"%10f %10f %10f\n", tmpmatrix2[i][0], 
		                       tmpmatrix2[i][1],
				       tmpmatrix2[i][2]);  
  fprintf(stderr,"\n");
#endif

  /* 1. und 2. Basiswechsel zusammenfassen: */
  
  matrix_mal_matrix3(tmpmatrix2,tmpmatrix1,BT_B);
  /* BT_B ist jetzt Basiswechselmatrx in das Torus-KO-System,
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
    
#if 0
  fprintf(stderr,"Basiswechsel ins Torus-KO-System:\nMatrix:\n");
  for ( i = 0; i < 3;++i)
    fprintf(stderr,"%10f %10f %10f\n", BT_B[i][0], 
		                       BT_B[i][1],
				       BT_B[i][2]);
  fprintf(stderr,"\nVerschiebungsvektor:\n");
  fprintf(stderr,"%10f %10f %10f\n", VBT_B[0], 
		                     VBT_B[1],
				     VBT_B[2]);
  fprintf(stderr,"inverse Matrix:\n");
  for ( i = 0; i < 3;++i)
    fprintf(stderr,"%10f %10f %10f\n", B_BT[i][0], 
		                       B_BT[i][1],
				       B_BT[i][2]);
  fprintf(stderr,"\ninv. Verschiebungsvektor:\n");
  fprintf(stderr,"%10f %10f %10f\n", VB_BT[0], 
		                     VB_BT[1],
				     VB_BT[2]);
#endif

#if 0
  matrix_mal_vektor3(BT_B, Achse1, tmpvek);
  fprintf(stderr,"zur Kontrolle d1 bzgl. neuen KO-Systems," 
		 " muesste (0,*,*) sein:\n");
  fprintf(stderr,"(%10f,%10f,%10f)\n", tmpvek[0],tmpvek[1], tmpvek[2]);
#endif

  /* Drehwinkel berechnen: */
  tmp = skalarprodukt3(achse0,achse1);

/* zur Sicherheit wegen Rundungsfehlern */  
  if ( tmp > 1.0) tmp = 1.0;
  if ( tmp < -1.0) tmp = -1.0;

  gamma = acos(tmp);

#if 0
  fprintf(stderr,"Gamma: %10f    Skalarprodukt: %.20f\n", gamma, tmp);
#endif  
  /* Bewegungsvektor im Welt-KO-System: */
  vektoradd3(1,P1,-1,P0,aw);

  /* Bewegungsvektor im Torus-KO-System: */
  matrix_mal_vektor3(BT_B,aw, a);

  /* maximale Geschwindigkeit aufgrund der Verschiebung: */
  va = norm3(a);

#ifdef BEWEGUNG
  fprintf(stderr,"Bewegung: gamma: %.10f  va = %.10f\n", gamma, va);
#endif  

  mach_tippunkt(0.0, TP0);
  return;
}

double SweepDist::Bewegung::speed(double t, double point_tool[], double speed_v[])
{
  double speed_0[3];
  speed_0[0] = a[0];
  speed_0[1] = - gamma * sin(gamma*t)*point_tool[1] - gamma * cos(gamma*t)*point_tool[2] + a[1];
  speed_0[2] =   gamma * cos(gamma*t)*point_tool[1] - gamma * sin(gamma*t)*point_tool[2] + a[2];

  TP0.vektor_tippunkt_welt(speed_0, speed_v);
  return norm3(speed_v);
}

// Erzeugung des Tippunktes (Koordinatensystem) zum Zeitpunkt t:
void SweepDist::Bewegung::mach_tippunkt
( double t, 
  Tippunkt &tp
)
{ 
  double tmpvek1[3], tmpvek2[3],tmpmat1[3][3];

  /* Drehwinkel ist  gamma * t : */
  double si = sin(gamma * t);
  double co = cos(gamma * t);

/*
  for ( int itmp  = 0; itmp < 9; ++itmp)
  {
    si = sin(gamma * t);
    co = cos(gamma * t);
  }	  
*/
  /* Uebergang vom Bewegungs-KO-System ins Tippunkt-KO-System
   * ist eine Drehung um die x-Achse um den Winkel  gamma * t :
   */ 
  tmpmat1[0][0] = 1.0;
  tmpmat1[0][1] = 0.0;
  tmpmat1[0][2] = 0.0;

  tmpmat1[1][0] = 0.0;
  tmpmat1[1][1] = co;
  tmpmat1[1][2] = si;

  tmpmat1[2][0] = 0.0;
  tmpmat1[2][1] = -si;
  tmpmat1[2][2] =  co;

  /* zusammengesetzte KO-Trafo berechnen: */
  matrix_mal_matrix3(tmpmat1, BT_B, tp.bw);

  /* Verschiebungsvektor berechnen: */
  matrix_mal_vektor3(tmpmat1, VBT_B,tmpvek2);

  tmpvek1[0] = -t*a[0];
  tmpvek1[1] = -t*(a[1] * co + a[2]*si);
  tmpvek1[2] =  t*(a[1] * si - a[2]*co); // - to->r;
  vektoradd3(1.0,tmpvek1,1.0,tmpvek2,tp.versch);

  /* inverse KO-Trafo berechnen: */
  tmpmat1[0][0] = 1.0;
  tmpmat1[0][1] = 0.0;
  tmpmat1[0][2] = 0.0;

  tmpmat1[1][0] = 0.0;
  tmpmat1[1][1] =   co;
  tmpmat1[1][2] = - si;

  tmpmat1[2][0] = 0.0;
  tmpmat1[2][1] =  si;
  tmpmat1[2][2] =  co;

  matrix_mal_matrix3(B_BT, tmpmat1, tp.inv_bw);

  /* Verschiebungsvektor der inversen KO-Traffo berechnen: */
  tmpvek1[0] =  t*a[0];
  tmpvek1[1] =  t*a[1];
  tmpvek1[2] =  t*a[2];

/*  
  tmpvek1[1] = -to->r*si + t*to->a[1];
  tmpvek1[2] =  to->r*co + t*to->a[2];
*/

  matrix_mal_vektor3(B_BT,tmpvek1,tmpvek2);
  vektoradd3(1.0,VB_BT,1.0,tmpvek2,tp.inv_versch);

  tp.initialized = true;

  return;
}






