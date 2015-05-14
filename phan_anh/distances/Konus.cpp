#include <cstdio>
#include <cmath>

using namespace std;

#include "Konus.h"
#include "util.h"
#include "Konstanten.h"

// Konstruktor:
SweepDist::Konus::Konus
( double hh, 
  double rr, 
  double RR, 
  double dd
) : h(hh), r(rr), R(RR) , d(dd), m(dd + hh/2), D(dd+hh)
{
#ifdef KONUS
    fprintf(stderr,"erzeuge Konus\n");
    fprintf(stderr,"Konusdaten: d=%f D=%f r=%f R=%f\n",d,D,r,R);
#endif    
  if ( r < EPS_R || R < EPS_R || h < EPS_R )
    throw Werkzeugfehler();


  double radius1 = sqrt( D*D + R*R);
  double radius2 = sqrt(d*d+r*r);

  Radius = radius1;
  if ( radius2 > radius1)
    Radius = radius2;

  fl_st = (R - r)/h;

  fl_nv[0] = fl_st;
  fl_nv[1] = -1.0;
  normieren2(fl_nv);
  fl_dist = -fl_nv[0]*d -fl_nv[1] *r; 

#ifdef KONUS
    fprintf(stderr,"Gleichung Flanke: (%f,%f)^T x + %f = 0\n", fl_nv[0], fl_nv[1], fl_dist);
#endif    
}	


// Realisierung der Methode dist_point
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Konus::dist_point 
( double p[],
  double grad[],
  double wnp[]
)			
{
  double H = norm2(p);
  double dist;
#ifdef KONUS
  fprintf(stderr,"Konus::dist_point Punkt=(%f,%f,%f) H=%f\n", p[0], p[1], p[2], H);
#endif
  if ( p[2] < d && H <= r) // links des Konus
  { 
#ifdef KONUS
    fprintf(stderr,"Punkt links des Konus!\n");
#endif
    wnp[0] = p[0];
    wnp[1] = p[1];
    wnp[2] = d;

    grad[0] = 0;
    grad[1] = 0;
    grad[2] = -1;
    return d - p[2];
  }
  else if ( p[2] > D && H < R)
  { 
#ifdef KONUS
    fprintf(stderr,"Punkt rechts des Konus!\n");
#endif

    wnp[0] = p[0];
    wnp[1] = p[1];
    wnp[2] = D;

    grad[0] = 0;
    grad[1] = 0;
    grad[2] = 1;
    return p[2] - D;
  }
  else
  {
    double fl_abst = p[2] * fl_nv[0] + H * fl_nv[1] + fl_dist; // Abstand zur Flanke
#ifdef KONUS
    fprintf(stderr,"Flankenabstand: %f\n", fl_abst);
#endif
    if ( H < r && H < R)
    { // => Punkt im Innern des Zylinders
#ifdef KONUS
      fprintf(stderr,"Punkt im Innern!\n");
#endif
      wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = p[2];

      dist = fl_abst;
      if ( p[2] - d  < dist)
        dist = p[2] - d;
      if ( D - p[2] < dist)
        dist = D - p[2];
      return -dist;
    }
    double nv[3] = {fl_nv[1] * p[0]/H, fl_nv[1] * p[1]/H, fl_nv[0] }; // umgerechneter Normalenvektor
#ifdef KONUS
    fprintf(stderr,"Normalenvektor: (%f,%f,%f)\n", nv[0], nv[1], nv[2]);
    fprintf(stderr,"Norm umgerechneter NV: %f (muesste 1 sein!)\n", norm3(nv));
#endif
    double fl_pkt[3]; // Projektion auf Flanke
    vektoradd3(1.0, p, -fl_abst, nv, fl_pkt);

#ifdef KONUS
    fprintf(stderr,"Flankenpunkt: (%f,%f,%f)\n", fl_pkt[0],fl_pkt[1], fl_pkt[2]);
#endif

    if ( fl_pkt[2] < d)
    { 
#ifdef KONUS
      fprintf(stderr,"Punkt links ueber Konus!\n");
#endif
      wnp[0] = r*p[0]/H;
      wnp[1] = r*p[1]/H;
      wnp[2] = d;
      vektoradd3(1,p,-1,wnp,grad);
      dist = norm3(grad);
      if ( dist > 0)
      { grad[0] /= dist;
        grad[1] /= dist;
        grad[2] /= dist;
      }

      return dist;
    }
    else if ( fl_pkt[2] > D)
    { 
#ifdef KONUS
      fprintf(stderr,"Punkt rechts ueber Konus!\n");
#endif
      wnp[0] = R*p[0]/H;
      wnp[1] = R*p[1]/H;
      wnp[2] = D;
      vektoradd3(1,p,-1,wnp,grad);
      dist = norm3(grad);
      if ( dist > 0)
      {
        grad[0] /= dist;
        grad[1] /= dist;
        grad[2] /= dist;
      }

      return dist;
    }
    else if ( fl_abst >= 0)
    { 
#ifdef KONUS
      fprintf(stderr,"Punkt im Innern!\n");
#endif
      wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = p[2];

      dist = fl_abst;
      if ( p[2] - d  < dist)
        dist = p[2] - d;
      if ( D - p[2] < dist)
        dist = D - p[2];
      return -dist;
    }
    else
    { 
#ifdef KONUS
      fprintf(stderr,"Punkt ueber Flanke!\n");
#endif
      wnp[0] = fl_pkt[0];
      wnp[1] = fl_pkt[1];
      wnp[2] = fl_pkt[2];
      vektoradd3(1,p,-1,wnp,grad);
      dist = norm3(grad);
      if ( dist > 0)
      {
        grad[0] /= dist;
        grad[1] /= dist;
        grad[2] /= dist;
      }

      return dist;
    }
  }
}	


// Realisierung der Methode get_Kreisscheibenradius
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Konus::get_Kreisscheibenradius
( double z
)
{ if ( z < d || z > D)
    return 0.0;
  else
    return (z-d)/h * (R-r) + r;
}


