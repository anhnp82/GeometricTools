#include <cstdio>
#include <cmath>

using namespace std;

#include "Torus.h"
#include "util.h"
#include "Konstanten.h"


// Konstruktor:
SweepDist::Torus::Torus
( double RR, 
  double rr, 
  double dd
): R(RR), r(rr), d(dd), m(dd+rr), D(dd+2*rr)
{ if ( r < EPS_R ) throw Werkzeugfehler();

  Radius = sqrt(fabs(m)*fabs(m) + R*R) +r;

#ifdef TORUS
  fprintf(stderr,"Torus: d=%.10f D=%.10f m=%.10f\n", d, D, m);
  fprintf(stderr,"R=%.10f r=%.10f\n", R, r);
#endif
}


// Realisierung der Methode is_point_closest
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Torus::dist_point
( double p[], 
  double grad[],
  double wnp[]
)
{ 

#ifdef TORUS
  fprintf(stderr,"Torus:dist_point\n");
  fprintf(stderr,"Punkt: [%.10f,%.10f,%.10f]\n", p[0], p[1], p[2]);
#endif

  double radius = norm2(p);

  if ( radius <= R)
  { wnp[0] = p[0];
    wnp[1] = p[1];
    wnp[2] = m;
  }
  else
  { double faktor = R/radius;
    wnp[0] = faktor*p[0];
    wnp[1] = faktor*p[1];
    wnp[2] = m;
  }    

  vektoradd3(1.0, p, -1.0, wnp, grad);
  double abstand = norm3(grad);

#ifdef TORUS
  fprintf(stderr,"nahpunkt: [%.10f,%.10f,%.10f]\n", wnp[0], wnp[1], wnp[2]);
#endif

  if ( abstand > r)
  { 
    grad[0] /= abstand;
    grad[1] /= abstand;
    grad[2] /= abstand;

    if ( radius <= R)
    { wnp[2] = D;
      if ( p[2] < d)
        wnp[2] = d;
    }
    else
    { wnp[0] += r*grad[0];
      wnp[1] += r*grad[1];
      wnp[2] += r*grad[2];
    }
  }
  else
  { wnp[0] = p[0];
    wnp[1] = p[1];
    wnp[2] = p[2];
  }

  return abstand - r;  
}

// Realisierung der Methode get_Kreisscheibenradius
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Torus::get_Kreisscheibenradius
( double z
)
{ if ( z < d || z > D)
    return 0.0;

  double dtmp = r*r - (z-m)*(z-m);
  if ( dtmp < 0)
    dtmp = 0.0;

  return sqrt(dtmp) + R;
}


 
