#include <cstdio>
#include <cmath>

using namespace std;

#include "Kreisscheibe.h"
#include "util.h"
#include "Konstanten.h"



// Konstruktor:
SweepDist::Kreisscheibe::Kreisscheibe
( double RR, 
  double mm
) : R(RR), m(mm)
{ 
  Radius = sqrt(m*m+R*R);
#ifdef KS
  fprintf(stderr,"Kreisscheibe: R=%.10f  m=%.10f\n", R, m);
#endif
}


// Realisierung der Methode is_point_closest
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Kreisscheibe::dist_point
( double p[], 
  double grad[],
  double wnp[]
)
{ 
#ifdef KS
  fprintf(stderr,"Kreisscheibe: dist_point\n");
  fprintf(stderr,"Punkt [%.10f,%.10f,%.10f]\n", p[0], p[1], p[2]);
#endif

  double radius = norm2(p);
  double dist;

#ifdef KS
  fprintf(stderr,"Abstand zur z-Achse: %.10f\n", radius);
#endif

  if ( radius < R+EPS_R)
  { 
#ifdef KS
    fprintf(stderr,"Punkt im Kreisscheibenzylinder: ");
#endif
    wnp[0] = p[0];
    wnp[1] = p[1];
    wnp[2] = m;
    if ( fabs(p[2] - m) < EPS_R)
    {
#ifdef KS
      fprintf(stderr,"in Kreisscheibe!\n");
#endif
      return 0.0;
    }
    if ( p[2] > m)
    {
#ifdef KS
      fprintf(stderr,"rechts der Kreisscheibe\n");
#endif
      grad[0] = 0;
      grad[1] = 0;
      grad[2] = 1;
      return p[2] - m;
    }
    else
    {
#ifdef KS
      fprintf(stderr,"links der Kreisscheibe\n");
#endif
      grad[0] = 0;
      grad[1] = 0;
      grad[2] = -1;
      return m - p[2];
    }
  }

#ifdef KS
  fprintf(stderr,"Punkt ausserhalb Kreisscheibenzylinder!\n");
#endif

  double faktor = R/radius;
  wnp[0] = faktor*p[0];
  wnp[1] = faktor*p[1];
  wnp[2] = m;

  vektoradd3(1.0, p, -1.0, wnp, grad);
  dist = norm3(grad);

  if ( fabs(dist) > EPS_R)
  { 
#ifdef KS
    fprintf(stderr,"Punkt ausserhalb Kreisscheibe!\n");
#endif
    grad[0] /= dist;
    grad[1] /= dist;
    grad[2] /= dist;
  }
  else
    dist = 0;

  return dist;
}

// Realisierung der Methode get_Kreisscheibenradius
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Kreisscheibe::get_Kreisscheibenradius
( double z
)
{ throw Werkzeugfehler();

  return 0.0;
}

 
