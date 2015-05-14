#include <cstdio>
#include <cmath>

using namespace std;

#include "Zylinder.h"
#include "util.h"
#include "Konstanten.h"


// Konstruktor
SweepDist::Zylinder::Zylinder
( double hh, 
  double rr, 
  double dd
): h(hh), r(rr), d(dd), m(dd +hh/2), D(dd+hh)
{ if ( h < EPS_R || r < EPS_R)
    throw Werkzeugfehler();	
	
  if ( d >= -h/2)  
    Radius = sqrt(r*r + (d+h)*(d+h));
  else    
    Radius = sqrt(d*d+r*r);

#ifdef ZYL
  fprintf(stderr,"Zylinder: d=%.10f D=%.10f\n", d, D);
  fprintf(stderr,"h=%.10f r=%.10f\n", h, r);
#endif
}  


// Realisierung der Methode dist_point
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Zylinder::dist_point
( double p[], 
  double grad[],
  double wnp[]
)
{ 
#ifdef ZYL
  fprintf(stderr,"Zylinder: dist_point\n");
  fprintf(stderr,"Zylinder: d=%.15f h=%.15f r=%.15f D=%.15f\n", d, h, r, D);
#endif

  double H = norm2(p);
  double dist;

  if ( p[2] < d) // Punkt links des Zylinders
  { if ( H <= r)
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = d;

      grad[0] = 0;
      grad[1] = 0;
      grad[2] = -1;
      dist = d - p[2];
    }
    else
    { wnp[0] = r/H*p[0];
      wnp[1] = r/H*p[1];
      wnp[2] = d;
      vektoradd3(1.0, p, -1.0, wnp, grad);
      dist = norm3(grad);

      if ( dist > 0)
      { grad[0] /= dist;
        grad[1] /= dist;
        grad[2] /= dist;
      }
    }
    return dist;
  }
  else if ( p[2] > D) // Punkt rechts des Zylinders
  { if ( H <= r)
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = D;

      grad[0] = 0;
      grad[1] = 0;
      grad[2] = 1;

      dist = p[2] - D;
    }
    else
    { wnp[0] = r/H*p[0];
      wnp[1] = r/H*p[1];
      wnp[2] = D;
      vektoradd3(1.0, p, -1.0, wnp, grad);
      dist = norm3(grad);

      if ( dist > 0)
      { grad[0] /= dist;
        grad[1] /= dist;
        grad[2] /= dist;
      }
    }
    return dist;
  }
  else  
  { if ( H <= r) // Punkt im Innern
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = p[2];

      dist = r - H;
      if ( (p[2] - d) < dist) 
        dist = p[2] - d;
      if ( (D - p[2]) < dist)
        dist = D - p[2];
      return -dist;
    }
    else // Punkt ueber Zylindermantel
    { wnp[0] = 0;
      wnp[1] = 0;
      wnp[2] = p[2];

      vektoradd3(1.0,p,-1.0,wnp,grad);
      dist = norm3(grad);
      grad[0] /= dist;
      grad[1] /= dist;
      grad[2] /= dist;
      dist -= r;
      wnp[0] = r/H*p[0];
      wnp[1] = r/H*p[1];

      return dist;
    }  
  }
}


// Realisierung der Methode get_Kreisscheibenradius
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Zylinder::get_Kreisscheibenradius
( double z
)
{ if ( z < d || z > D)
    return 0.0;
  else
    return r;
}



