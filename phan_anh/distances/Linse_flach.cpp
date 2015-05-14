#include <cstdio>
#include <cmath>

using namespace std;

#include "Linse_flach.h"
#include "util.h"
#include "Konstanten.h"



// Konstruktor:
SweepDist::Linse_flach::Linse_flach
( double hh, 
  double HH, 
  double rkrk, 
  double dd
) : h(hh), H(HH), rk(rkrk), d(dd), m(dd+hh/2), D(dd+hh)
{ 

  if ( h < EPS_R || rk < -EPS_R || H < 2*rk+h - EPS_R)
    throw Werkzeugfehler();  

  double tmp = ((H-2*rk)*(H-2*rk)-h*h)/(4*h); // Abstand der Kreismittelpunkte zu m

  Ml = m - tmp;  // z-Koordinate linker Kreismittelpunkt
  Mr = m + tmp;  // z--Koordinate rechter Kreismittelpunkt
  R = h/2 + tmp; // Radius beider Kreise

  
  // Radius = maximaler Abstand eines Linsenpunktes vom Nullpunkt
  if ( m >= 0.0)
  { // Berechne Schnittpunkt der Geraden durch
    // linken Kreismittelpunkt und obersten Punkt der Linse
    // wenn dieser groesser gleich 0 ist, hat ein Punkt
    // auf dem rechten Kreis den weitesten Abstand von 0

    if ( Ml - rk *(m-Ml)/(H/2-rk) >= 0.0)
      Radius = sqrt(Ml*Ml+rk*rk)+R;
    else
    { Radius = sqrt(m*m+H*H/4);
      double Rtmp = sqrt(D*D+rk*rk);
      if ( Rtmp > Radius)
        Radius = Rtmp;
    }
  }
  else // m < 0
  { // Berechne Schnittpunkt der Geraden durch
    // rechten Kreismittelpunkt und obersten Punkt der Linse
    // wenn dieser kleiner gleich 0 ist, hat ein Punkt
    // auf dem rechten Kreis den weitesten Abstand von 0

    if ( Mr - rk *(m-Mr)/(H/2-rk) <= 0.0)
      Radius = sqrt(Mr*Mr+rk*rk)+R;
    else
    { Radius = sqrt(m*m+H*H/4);
      double Rtmp = sqrt(d*d+rk*rk);
      if ( Rtmp > Radius)
        Radius = Rtmp;
    }
  }


  // Hesse-Normalform des Strahls von Ml
  // durch hoechsten Linsenpunkt, Normalenvektor zeigt
  // nach links/oben
  nvl[0] = -(H-2*rk)/2;
  nvl[1] = m - Ml;
  normieren2(nvl);
  al = - (Ml*nvl[0] + rk * nvl[1]);

  // Hesse-Normalform des Strahls von Mr
  // durch hoechsten Linsenpunkt, Normalenvektor zeigt
  // nach rechts/ oben
  nvr[0] = (H-2*rk)/2;
  nvr[1] = -m + Mr;
  normieren2(nvr);
  ar = - (Mr*nvr[0] + rk * nvr[1]);

#ifdef LINSE_F
  fprintf(stderr,"Linse_flach:\n");
  fprintf(stderr,"d=%f, h=%f, H=%f, m=%f, D=%f, rk=%f\n", d, h, H, m, D, rk);
  fprintf(stderr,"Ml=%f, Mr=%f\n", Ml, Mr);
  fprintf(stderr,"R=%f, Radius=%f\n", R, Radius);
  fprintf(stderr,"Gerade durch Ml: (%f,%f) * x + %f = 0\n", nvl[0], nvl[1], al);
  fprintf(stderr,"Gerade durch Mr: (%f,%f) * x + %f = 0\n", nvr[0], nvr[1], ar);
#endif

}


// Realisierung der Methode dist_point
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Linse_flach::dist_point
( double p[], 
  double grad[],
  double wnp[]
)
{ 
#ifdef LINSE_F
  fprintf(stderr,"Linse_flach: dist_point\n");
#endif

  double dist;
  double HH = norm2(p);

  if ( HH <= rk + EPS_R)
  { 
#ifdef LINSE_F
    fprintf(stderr,"im zentraler Zylinder, rk = %.10f ", rk);
#endif
    if ( p[2] > D)  // rechts
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = D;
      grad[0] = 0;
      grad[1] = 0;
      grad[2] = 1;
      
      dist = p[2] - D; 
      return dist;
    }
    else if ( p[2] < d) // links
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = d;
      grad[0] = 0;
      grad[1] = 0;
      grad[2] = -1;
      
      dist = d - p[2]; 
      return dist;
    }
    else // in Linse
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = p[2];

      if ( p[2] >= m )
        dist = p[2]-D;
      else
        dist = d - p[2];
      return dist;
    }
  }
  else if ( ( nvl[0]*p[2] + nvl[1]*HH + al > EPS_R &&
              nvr[0]*p[2] + nvr[1]*HH + ar > EPS_R) )
  { // oberer Punkt ist Nahpunkt!
#ifdef LINSE_F
    fprintf(stderr,"im oberen Sektor!\n");
#endif
    wnp[0] = H/2 * p[0]/HH;
    wnp[1] = H/2 * p[1]/HH;
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
    return dist;
  } 

  if ( p[2] >= m)
  { wnp[0] = rk * p[0] / HH;
    wnp[1] = rk * p[1] / HH;
    wnp[2] = Ml;

    grad[0] = p[0] - wnp[0];
    grad[1] = p[1] - wnp[1];
    grad[2] = p[2] - wnp[2];

    dist = norm3(grad);
    if ( dist > R)
    { grad[0] /= dist;
      grad[1] /= dist;
      grad[2] /= dist;
  
      wnp[0] += R*grad[0];
      wnp[1] += R*grad[1];
      wnp[2] += R*grad[2];

      dist -= R;
    }
    else // dist < R
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = p[2];

      dist -= R;
    }
  }
  else
  { wnp[0] = rk * p[0] / HH;
    wnp[1] = rk * p[1] / HH;
    wnp[2] = Mr;

    grad[0] = p[0] - wnp[0];
    grad[1] = p[1] - wnp[1];
    grad[2] = p[2] - wnp[2];

    dist = norm3(grad);
    if ( dist > R)
    { grad[0] /= dist;
      grad[1] /= dist;
      grad[2] /= dist;
  
      wnp[0] += R*grad[0];
      wnp[1] += R*grad[1];
      wnp[2] += R*grad[2];

      dist -= R;
    }
    else // dist < R
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = p[2];

      dist -= R;
    }
  }
  return dist;
}	

// Realisierung der Methode get_Kreisscheibenradius
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Linse_flach::get_Kreisscheibenradius
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


 

