#include <cmath>
#include <cstdio>

using namespace std;

#include "util.h"

/* berechnet Norm eines Vektors */
double SweepDist::norm(double *v, int n)
{ double sum=0.0;
 
  for ( --n; n >= 0; --n)
    sum += v[n]*v[n];

  return sqrt(sum);
}

double SweepDist::norm2(double *v)
{ return sqrt(v[0]*v[0] + v[1]*v[1]);
}

double SweepDist::norm3(double *v)
{ return sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
}


/* berechnet Skalarprodukt von v mit w */
double SweepDist::skalarprodukt(double *v, double *w, int n)
{ double sum=0.0;
 
  for ( --n; n >= 0; --n)
    sum += v[n]*w[n];

  return sum;
}

double SweepDist::skalarprodukt2(double *v, double *w)
{ return v[0]*w[0] + v[1]*w[1];
}

double SweepDist::skalarprodukt3(double *v, double *w)
{ return v[0]*w[0] + v[1]*w[1] + v[2]*w[2];
}


/* c = a x b */
void SweepDist::kreuzprodukt(double *a, double *b, double *c)
{ c[0] = a[1]*b[2] - b[1]*a[2];
  c[1] = a[2]*b[0] - a[0]*b[2];
  c[2] = a[0]*b[1] - b[0]*a[1];
}


void SweepDist::matrix_mal_vektor2(double A[2][2], double x[2], double y[2])
{ 
	
  y[0] = A[0][0]*x[0] + A[0][1] * x[1];
  y[1] = A[1][0]*x[0] + A[1][1] * x[1];
}


void SweepDist::matrix_mal_vektor3(double A[3][3], double x[3], double y[3])
{ 
  y[0] = A[0][0]*x[0] + A[0][1] * x[1] + A[0][2] * x[2];
  y[1] = A[1][0]*x[0] + A[1][1] * x[1] + A[1][2] * x[2];
  y[2] = A[2][0]*x[0] + A[2][1] * x[1] + A[2][2] * x[2];
} 


void SweepDist::matrix_mal_matrix3(double A[3][3], double B[3][3], double C[3][3])
{ int i,j,k;

  for ( i = 0; i < 3; ++i)
    for ( j = 0; j < 3; ++j)
    { C[i][j] = 0.0;
      for ( k = 0; k < 3; ++k)
        C[i][j] += A[i][k] * B[k][j];
    }
}


/* berechnet z = a*x + b*y in dim Dimensionen */
void SweepDist::vektoradd( double a, double *x, double b, double *y, double *z, int dim)
{ int i;

  for ( i = 0; i < dim; ++i)
    z[i] = a*x[i] + b*y[i];
}

void SweepDist::vektoradd2( double a, double *x, double b, double *y, double *z)
{ z[0] = a * x[0] + b * y[0]; 
  z[1] = a * x[1] + b * y[1]; 
}	

void SweepDist::vektoradd3( double a, double *x, double b, double *y, double *z)
{ z[0] = a * x[0] + b * y[0]; 
  z[1] = a * x[1] + b * y[1]; 
  z[2] = a * x[2] + b * y[2]; 
}	


/* normiert den Vektor */
void SweepDist::normieren(double *x, int dim)
{ double dtmp = norm(x,dim);
  int i;
  for ( i = 0; i < dim; ++i)
    x[i] /= dtmp;
}

void SweepDist::normieren2(double *x)
{ double dtmp = norm2(x);

  x[0] /= dtmp;
  x[1] /= dtmp;
}

void SweepDist::normieren3(double *x)
{ double dtmp = norm3(x);

  x[0] /= dtmp;
  x[1] /= dtmp;
  x[2] /= dtmp;
}


int SweepDist::quadsolve( double a, double b, double c, double loes[])
{ double p, q, diskr;

  if ( a == 0 )
  { // lineare Gleichung
    if ( b == 0)
    {	    
      if ( c == 0)
        return 3;
      else
	return 0;    
    }
    else // b != 0
    { loes[0] = -c/b;
      return 1;
    }
  }
  else  // quadratische Gleichung
  { p = b/a/2.;
    q = c/a;
        
    diskr = p*p - q;

    if ( diskr < 0)
      return 0;
    if ( diskr == 0)
    { loes[0] = -p;
      return 1;
    }
    else
    { diskr = sqrt(diskr);
      loes[0] = -p - diskr;
      loes[1] = -p + diskr;
      return 2;
    }
  }
}


int SweepDist::kubsolve(double a, double b, double c, double d, double loes[])
{ 
  /* Loesen einer kubischen Gleichung nach  

     Heitzinger/Troch/Valentin
     Praxis nichtlinearer Gleichungen
     Carl-Hanser-Verlag Muenchen 1984
     ISBN 3-446-14079-4

     Kapitel 3.3, Seite 89 - 94


     ACHTUNG: auf Seite 91 im Buch ist ein Fehler:
     im Fall D == 0 gilt z2 = z3 = -z1 / 2
     (die Division durch 2 fehlt im Buch!)
  */
#ifdef POL_SOLVE
  fprintf(stderr,"kub.: a=%.15e b=%.15e c=%.15e %.15e\n", a ,b , c, d);
#endif
  if ( a == 0)
  { fprintf(stderr,"kubsolve: a=0\n");
    throw MathFehler();
  }

  // normieren:
  b /= a;
  c /= a;
  d /= a;

  // neue form: y^3 + ay^2 + by + c = 0:
  a = b;
  b = c;
  c = d;

/* Substitution: x  = y - a/3 fuehrt auf
     y^3 + py + q = 0
*/
  double p = b - a*a/3;
  double q = 2*a*a*a/27 -a*b/3 + c;

#ifdef POL_SOLVE
  fprintf(stderr,"kub.: p=%.15e q=%.15e\n", p ,q );
#endif

//  if ( fabs(p) < 1.E-12) p = 0;
//  if ( fabs(q) < 1.E-12) q = 0;
 
  // Diskriminante:
  double D = q*q/4 + p*p*p/27;

#ifdef POL_SOLVE
  fprintf(stderr,"kub.: p=%.15e q=%.15e D=%.15e\n", p ,q , D);
#endif

  if ( p == 0)
  { if ( q == 0)
    { loes[0] = -a/3;
      return 1;
    }
    double faktor = 1;
    if ( q > 0)
    { q = -q;
      faktor = -1;
    }
    loes[0] = faktor*pow(-q, 1./3.) - a/3;
    return 1;
  }
  if ( q == 0)
  { if ( p < 0)
    { loes[0] = -sqrt(-p) - a/3;
      loes[1] = -a/3;
      loes[2] = sqrt(-p) - a/3;
      return 3;
    }
    loes[0] = -a/3;
    return 1;
  }

  if ( D > 0)
  { 
    D = sqrt(D);
    double w1 = -q/2 - D;
    double w2 = -q/2 + D;

    double u, v;
    double faktor = 1;
    if ( w1 < 0)
    { w1 = -w1;
      faktor = -1;
    }
    u = faktor * pow(w1, 1./3.);

    faktor = 1; 
    if ( w2 < 0)
    { w2 = -w2;
      faktor = -1;
    }
    v = faktor * pow(w2, 1./3.);
    loes[0] = u+v - a/3;
    return 1;
  }
  if ( D == 0)
  { double faktor = 1;
    if ( q < 0)
    { faktor = -1;
      q = -q;
    }
    double y = -faktor * pow(4*q, 1./3.);
    if ( y < 0)
    {
      loes[0] =  y - a/3;
      loes[1] = -y/2 - a/3;  // y durch 2 !!!! Fehler im Buch!
      return 2;
    }
    loes[0] = -y/2 - a/3;   // y durch 2 !!!!  Fehler im Buch!
    loes[1] =  y - a/3;
    return 2;
  }

  double r = sqrt(-p*p*p/27);
  double phi = acos(-q/(2*r));
  
  r = 2*pow(r, 1./3.);
  loes[0] = r * cos(phi/3) -a/3;
  loes[1] = r * cos((phi + 2*3.14159265358979)/3) -a/3;
  loes[2] = r * cos((phi - 2*3.14159265358979)/3) -a/3;   

  double tmp;
  if ( loes[0] > loes[1])
  { tmp = loes[0];
    loes[0] = loes[1];
    loes[1] = tmp;
  }
  if ( loes[1] > loes[2])
  { tmp = loes[1];
    loes[1] = loes[2];
    loes[2] = tmp;
  }
  if ( loes[0] > loes[1])
  { tmp = loes[0];
    loes[0] = loes[1];
    loes[1] = tmp;
  }
  return 3;
}

int SweepDist::quartsolve(double a, double b, double c, double d, double e,
double loes[])
{ 
  /* Loesen einer quartischen Gleichung nach  

     Heitzinger/Troch/Valentin
     Praxis nichtlinearer Gleichungen
     Carl-Hanser-Verlag Muenchen 1984
     ISBN 3-446-14079-4

     Kapitel 3.4, Seite 101 - 103
  */

  if ( a == 0)
  { fprintf(stderr,"quartsolve: a=0\n");
    throw MathFehler();
  }

  // normieren
  b /= a;
  c /= a;
  d /= a;
  e /= a;

/* Substitution: x  = z - b/4 fuehrt auf

   z^3 + alpha z^2 + beta z + gamma = 0
*/
  double alpha = -3*b*b/8 + c;
  double beta = b*b*b/8 - b*c/2 + d;
  double gamma = -3*b*b*b*b/256 + b*b*c/16 - b*d/4 + e;

#ifdef POL_SOLVE
  fprintf(stderr,"alpha=%.10e beta=%.10e gamma=%.10e\n", alpha, beta, gamma);
#endif

  if ( fabs(alpha) < 1.E-12) alpha = 0;
  if ( fabs(beta) < 1.E-12) beta = 0;
  if ( fabs(gamma) < 1.E-12) gamma = 0;

#ifdef POL_SOLVE
  fprintf(stderr,"alpha=%.10f beta=%.10f gamma=%.10f\n", alpha, beta, gamma);
#endif

  if ( beta == 0)
  {
    /* Sonderfall: beta == 0:   
       man hat ein biquadratisches Polynom

       z^4 + alpha z^2 + gamma = 0

       welches hier traditionell geloest wird:
    */
    double D = alpha*alpha/4-gamma;  // Diskriminante!

#ifdef POL_SOLVE
    fprintf(stderr,"beta=0 Diskriminante: %.10e\n", D);
#endif
    if ( fabs(D) < 1.E-12)
      D = 0;
    
    if ( D < 0)  // keine reellen Loesungen!
      return 0;
  
    if ( D == 0)
    { if ( alpha > 0) // keine reelle Loesung
        return 0;
      if ( alpha == 0) // 4-fache Loesung 0
      { loes[0] = -b/4;
        return 1;
      }

      // jetzt: alpha < 0
      loes[1] =  sqrt(-alpha/2);
      loes[0] = - loes[1];
      loes[0] -= b/4;
      loes[1] -= b/4;
      return 2;
    }

    // jetzt: D > 0:

    if ( gamma == 0)
    { loes[0] = -b/4;    // 0 ist schon mal doppelte Nullstelle!
      if ( alpha > 0)    // keine weiteren Nullstellen!
        return 1;

      loes[1] = loes[0];
      loes[2] = sqrt(-alpha);
      loes[0] = -loes[2];
      loes[0] -= b/4;
      loes[2] -= b/4;
      return 3;
    }

    // jetzt: gamma != 0:
    D = sqrt(D);
    double u1 = -alpha/2 - D;
    double u2 = -alpha/2 + D;
  
    // weder u1 noch u2 koennen 0 sein, da gamma != 0
    // u1 < u2 da D > 0
    if ( u2 < 0)
      return 0;
    if ( u1 < 0)
    { loes[1] = sqrt(u2);
      loes[0] = - loes[1];
      loes[0] -= b/4;
      loes[1] -= b/4;
      return 2;
    }
    // jetzt: 0 < u1 < u2:
    loes[2] = sqrt(u1);
    loes[1] = - loes[2];
    loes[3] = sqrt(u2);
    loes[0] = - loes[3];

    loes[0] -= b/4;
    loes[1] -= b/4;
    loes[2] -= b/4;
    loes[3] -= b/4;

    return 4;
  }

  // jetzt: beta != 0, KEINE biquadratische Gleichung!
  // Loeusng nach der Methode von Descartes und Ferrari:
  
  // zunaechst: positive reelle Nullstelle der Resolventen bestimmen:
  double koeff[4];
  koeff[3] = 1.0;
  koeff[2] = 2*alpha;
  koeff[1] = alpha*alpha-4*gamma;
  koeff[0] = -beta*beta;

#ifdef POL_SOLVE
  fprintf(stderr,"Resolvente: %.15e %.10e %.15e %.15e\n", koeff[3], koeff[2], koeff[1], koeff[0]);
#endif
 
  int erg = kubsolve(koeff[3], koeff[2], koeff[1], koeff[0], loes);
 
#ifdef POL_SOLVE
  fprintf(stderr,"Nullstellen: %d\n", erg);
  for ( int itmp = 0; itmp < erg; ++itmp)
    fprintf(stderr,"%.15f ", loes[itmp]);
  fprintf(stderr,"\n");
#endif
 
  if ( erg == 0)
  { fprintf(stderr,"quartsolve: kubische Gleichung hat keine Loesung!\n");
    throw MathFehler();
  }
  loes[erg-1] = nachiteration(3, koeff, loes[erg-1],5);

  double y = loes[erg-1];

#ifdef POL_SOLVE
  fprintf(stderr,"Resolvente: Anzahl Nullstellen: %d\n", erg);
  fprintf(stderr,"groesste Nullstelle: %.10e\n", y);
  double tmpwert = y*y*y + 2*alpha*y*y + (alpha*alpha-4*gamma)*y -beta*beta;
  fprintf(stderr,"Wert: %.10f (sollte ungefaehr 0 sein!)\n", tmpwert);
#endif
  if ( y <= 0)
  { fprintf(stderr," quartsolve: kubische Gleichung hat keine positive Loesung!");
    throw MathFehler();
  }
  
  double r = -sqrt(y);
  double s = (alpha + y + beta/(-r))/2;
  double u = (alpha + y - beta/(-r))/2;

  /* Nullstellen sind die Nullstellen der beiden quadratischen
     Polynome:

     z^2 + rz + s = 0     und   z^2 -rz + u = 0

  */
  int anzloes = 0;
  double disk = r*r/4-s;
#ifdef POL_SOLVE
  fprintf(stderr,"disk1: %.10f\n", disk);
#endif
  if ( disk == 0)
  { loes[anzloes++] = -r/2 - b/4;;
  }
  else if (disk > 0)
  { disk = sqrt(disk);
    loes[anzloes++] = -r/2 - disk - b/4;
    loes[anzloes++] = -r/2 + disk - b/4;
  }

  disk = r*r/4 - u;
#ifdef POL_SOLVE
  fprintf(stderr,"disk2: %.10f\n", disk);
#endif

  if ( disk == 0)
  { loes[anzloes++] = r/2 - b/4;;
  }
  else if ( disk > 0)
  { disk = sqrt(disk);
    loes[anzloes++] = r/2 - disk - b/4;
    loes[anzloes++] = r/2 + disk - b/4;
  }

  // sortieren der Nullstellen:
  for ( int i = anzloes-1; i > 0; --i)
  { double max = loes[0];
    int imax = 0;
    for ( int j = 1; j <= i; ++j)
    { if ( loes[j] > max)
      { max = loes[j];
        imax = j;
      }
    }
    if ( imax != i)
    { loes[imax] = loes[i];
      loes[i] = max;
    }
  }
  return anzloes;
}

double SweepDist::nachiteration(int grad, double pol[], double x_start, int anz)
{ double q[5];
  double x =x_start;
  double vfh = 1;
  double kwert;

#ifdef POL_SOLVE
  fprintf(stderr,"Starte Nachiteration:\n");
  for ( int i = 0; i <= grad; ++i)
    fprintf(stderr,"p[%d] = %.10f\n", i, pol[i]);
  fprintf(stderr,"x=%.10f\n", x_start);
#endif

  for (int it = 0; it < anz; ++it) 
  { for ( int i = 0; i <= grad; ++i)
      q[i] = pol[i];
    for ( int i = 0; i < 3; ++i)
      for ( int j = grad-1; j >= i; --j)
        q[j] += q[j+1]*x; 

#ifdef POL_SOLVE
    fprintf(stderr,"x: %.10e\n", x);
    fprintf(stderr,"Wert: %.10e\n", q[0]);
    fprintf(stderr,"Ableitung: %.10e\n", q[1]);   
    fprintf(stderr,"2-te Ableitung: %.10e\n", q[2]*2);
#endif

    if ( fabs(q[0]) < 1.E-30)
      break;

    if ( fabs(q[1]) < 1.E-30)
    { //fprintf(stderr,"it=%d: q[0]=%.10e  q[1]=%.10e\n", it, q[0], q[1]);
      throw MathFehler();
    }

    if ( q[1] * q[1] != 0 && q[2] * q[0] != 0)
      vfh = 1/(1-q[2]*2*q[0]/(q[1]*q[1]));
    
    kwert = vfh*q[0]/q[1];
#ifdef POL_SOLVE
    fprintf(stderr,"Faktor: %.10f\n", vfh);   
#endif

    if ( fabs(kwert/x) < 1.E-15)
      break;
    x -= kwert;
  }
#ifdef POL_SOLVE
  fprintf(stderr,"verbessertes x: %.10e\n", x);
#endif

  return x;
}

