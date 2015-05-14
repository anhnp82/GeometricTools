#include <cstdio>
#include <cmath>

using namespace std;

#include "Kugel.h"
#include "util.h"
#include "Konstanten.h"


// Konstruktor
SweepDist::Kugel::Kugel
( double rr, 
  double dd
) : r(rr), d(dd), D(dd+2*rr), m(dd+rr)
{ if ( r < EPS_R) throw Werkzeugfehler();

  Radius = fabs(m) + r;

#ifdef KUGEL
  fprintf(stderr,"Kugel: r=%.10f  m=%.10f\n", r,m);
#endif
}


// Realisierung der Methode dist_point
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Kugel::dist_point
( double point[], 
  double grad[],
  double wnp[]
)
{ double dist;
  grad[0] = point[0];
  grad[1] = point[1];
  grad[2] = point[2] - m;

  dist = norm3(grad); 

  if ( dist < EPS_R)  // Punkt ist Kugelmittelpunt => fertig
  {
    wnp[0] = 0;
    wnp[1] = 0;
    wnp[2] = m;
    return -r;
  }
  

  if ( dist >= r)
  {
    grad[0] /= dist;
    grad[1] /= dist;
    grad[2] /= dist;

    wnp[0] = r * grad[0];
    wnp[1] = r * grad[1];
    wnp[2] = m+r * grad[2];
  }
  else
  {
    wnp[0] = point[0];
    wnp[1] = point[1];
    wnp[2] = point[2];
  }
  
  return dist - r;  

}  


// der abstrakten Basisklasse Werkzeug:
double SweepDist::Kugel::get_Kreisscheibenradius
( double z
)
{ 
  if ( z < d || z > d + 2*r )
    return 0.0;

  double dtmp = r*r - (z-m)*(z-m);
  if ( dtmp < 0.0)
    dtmp = 0;

  return sqrt(dtmp);
}



