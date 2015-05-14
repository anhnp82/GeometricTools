#include <cstdio>
#include <cmath>

using namespace std;

#include "Linse.h"
#include "util.h"
#include "Konstanten.h"


// Konstruktor:
SweepDist::Linse::Linse
( double hh, 
  double HH, 
  double dd
)
: h(hh), H(HH), d(dd), m(dd+hh/2), D(dd+hh)
{ 

  if ( H < h ) throw Werkzeugfehler();   // Linse nicht konvex!
	                                 // wenn H==h gilt, handelt es sich um eine Kugel!
	
  if ( h < EPS_R) throw Werkzeugfehler();  // ausgeartet: zu schmal!

  double tmp = (H*H-h*h)/(4*h);          // Abstand der Kreismittelpunkte zu m

  Ml = m - tmp;  // linker Kreismittelpunkt
  Mr = m + tmp;  // rechter Kreismittelpunkt
  R = h/2 + tmp; // Radius beider Kreise

  
  // Radius = maximaler Abstand einen Linsenpunktes vom Nullpunkt
//  if ( Ml >= 0.0)   geaendert: 16.6.2009
  if ( Ml >= 0.0 || Mr <= 0.0)
    Radius = D;
  else
    Radius = sqrt(m*m + H*H/4.0);

  // Hesse-Normalform des Strahls von Ml
  // durch hoechsten Linsenpunkt, Normalenvektor zeigt
  // nach links/oben
  nvl[0] = -H/2;
  nvl[1] = m - Ml;
  normieren2(nvl);
  al = - Ml*nvl[0];

  // Hesse-Normalform des Strahls von Mr
  // durch hoechsten Linsenpunkt, Normalenvektor zeigt
  // nach rechts/ oben
  nvr[0] = H/2;
  nvr[1] = -m + Mr;
  normieren2(nvr);
  ar = - Mr*nvr[0];

#ifdef LINSE
  fprintf(stderr,"Linse:\n");
  fprintf(stderr,"d=%f, h=%f, H=%f, m=%f, D=%f\n", d, h, H, m, D);
  fprintf(stderr,"Ml=%f, Mr=%f\n", Ml, Mr);
  fprintf(stderr,"R=%f, Radius=%f\n", R, Radius);
  fprintf(stderr,"Gerade durch Ml: (%f,%f) * x + %f = 0\n", nvl[0], nvl[1], al);
  fprintf(stderr,"Gerade durch Mr: (%f,%f) * x + %f = 0\n", nvr[0], nvr[1], ar);
#endif

}


// Realisierung der Methode is_point_closest
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Linse::dist_point
( double p[], 
  double grad[], 
  double wnp[]  
)
{ 
#ifdef LINSE
  fprintf(stderr,"Linse: dist_point\n");
#endif

  double HH = norm2(p);
  double dist;

  if ( (nvl[0]*p[2] + nvl[1]*HH + al > EPS_R &&
        nvr[0]*p[2] + nvr[1]*HH + ar > EPS_R) )
  { // oberer Punkt ist Nahpunkt!
    wnp[0] = H/2*p[0]/HH;
    wnp[1] = H/2*p[1]/HH;
    wnp[2] = m;
    grad[0] = p[0] - wnp[0];
    grad[1] = p[1] - wnp[1];
    grad[2] = p[2] - wnp[2];

    dist = norm3(grad);
    if ( dist > 0)
    { grad[0] /= dist;
      grad[1] /= dist;
      grad[2] /= dist;
    }
  }    
  else if ( p[2] >= m )
  { grad[0] = p[0];
    grad[1] = p[1];
    grad[2] = p[2]-Ml;
    dist = norm3(grad);

    grad[0] /= dist;
    grad[1] /= dist;
    grad[2] /= dist;

    if ( dist >= R)
    { wnp[0] = R * grad[0];
      wnp[1] = R * grad[1];
      wnp[2] = Ml + R * grad[2];
    }
    else
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = p[2];
    }
    dist -= R;
  } 
  else // p[2] < m
  { grad[0] = p[0];
    grad[1] = p[1];
    grad[2] = p[2]-Mr;
    dist = norm3(grad);

    grad[0] /= dist;
    grad[1] /= dist;
    grad[2] /= dist;

    if ( dist >= R)
    { wnp[0] = R * grad[0];
      wnp[1] = R * grad[1];
      wnp[2] = Mr + R * grad[2];
    }
    else
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = p[2];
    }
    dist -= R;
  } 
  return dist;
}	

// Realisierung der Methode get_Kreisscheibenradius
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Linse::get_Kreisscheibenradius
( double z
)
{ 
  if ( z < d || z > D)
    return 0.0;

  double dtmp;

  if ( z == m)
    return H/2;

  if ( z < m)
  { dtmp = R*R - (Mr-z)*(Mr-z);
    if ( dtmp < 0) 
      dtmp = 0.0;
    return sqrt(dtmp);
  }
  else 
  { dtmp = R*R - (Ml-z)*(Ml-z);
    if ( dtmp < 0) 
      dtmp = 0.0;
    return sqrt(dtmp);
  }

  return 0.0;
}


