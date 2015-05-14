#include <cstdio>

#include "Tonne.h"
#include "util.h"
#include "Konstanten.h"

using namespace std;


// Konstruktor:
SweepDist::Tonne::Tonne
( double Rl, 
  double Rr, 
  double hh, 
  double RR, 
  double dd
) : h(hh), rl(Rl), rr(Rr), R(RR), d(dd), m(dd +hh/2), D(dd+hh)
{ 
#ifdef TONNE
  fprintf(stderr,"Tonne\n\n\n");
  fprintf(stderr,"rl = %.15f\n", rl);
  fprintf(stderr,"rr = %.15f\n", rr);
  fprintf(stderr," h = %.15f\n", h);
  fprintf(stderr," R = %.15f\n", R);
  fprintf(stderr," d = %.15f\n", d);
#endif  
  if ( h < EPS_R || rl < EPS_R || rr < EPS_R)
    throw Werkzeugfehler();	
	
  double tmpvek[2] = { D-d, rr-rl };
  double len2 = norm2(tmpvek)/2.0;

  if ( R < len2)
    throw Werkzeugfehler();

  double lambda = sqrt(R*R-len2*len2);

  Mz = d + tmpvek[0]/2;
  My = rl + tmpvek[1]/2;

  normieren2(tmpvek);

  double tmp = tmpvek[0];
  tmpvek[0] = tmpvek[1];
  tmpvek[1] = -tmp;

  Mz += lambda * tmpvek[0];
  My += lambda * tmpvek[1];
  // (Mz,My) jetzt Mittelpunkt des Tonnenbauchkreises

#ifdef TONNE
    fprintf(stderr,"Mittelpunkt Tonnenbauchkreis: (%.10f, %.10f)\n", Mz, My);
    fprintf(stderr,"Radius: %.10f\n", R);
#endif

  if ( Mz < d-EPS_R || Mz > D+EPS_R)  // Keine Tonne, sondern Vase!
    throw Werkzeugfehler();
 
  if ( My > rl+EPS_R || My > rr+EPS_R )  // nicht konvex!
    throw Werkzeugfehler();
 
  alpha0 = atan2(rl-My, Mz - d );
  alpha1 = atan2(rr-My, Mz - D );

  double beta = atan2(My,Mz);

  if ( alpha0 <= beta && beta <= alpha1)
    Radius = sqrt(Mz*Mz+My*My) + R;
  else 
  { Radius = sqrt(rr*rr + D*D);
    double tmp_l = sqrt(rl*rl+d*d);
    if ( tmp_l > Radius)
      Radius = tmp_l;	    
  }

#ifdef TONNE
  fprintf(stderr,"groesster Abstand zum Nullpunkt: %.10f\n", Radius);
#endif

  /* rechte Begrenzungsgerade: */
  nvr[0] = -rr +My;
  nvr[1] = D-Mz;
  normieren2(nvr);

  tmpvek[0] = Mz;
  tmpvek[1] = My;
  
  ar = -skalarprodukt2(nvr, tmpvek);

  nvl[0] = rl-My;
  nvl[1] = Mz - d;
  normieren2(nvl);
  al = -skalarprodukt2(nvl, tmpvek);

#ifdef TONNE
  fprintf(stderr,"rechte Begrenzungsgerade:\n");
  fprintf(stderr,"Normalenvektor: (%.10f,%.10f)\n", nvr[0], nvr[1]);
  fprintf(stderr,"Abstand Nullpunkt: %.10f\n", ar);
  fprintf(stderr,"\nlinke Begrenzungsgerade:\n");
  fprintf(stderr,"Normalenvektor: (%.10f,%.10f)\n", nvl[0], nvl[1]);
  fprintf(stderr,"Abstand Nullpunkt: %.10f\n", al);
#endif

}  


// Realisierung der Methode dist_point
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Tonne::dist_point 
( double p[], 
  double grad[],
  double wnp[]
)
{ 
  double H = norm2(p);
  double dist;
#ifdef TONNE
  fprintf(stderr,"Tonne: dist_point\n");
  fprintf(stderr,"Punkt: (%.15f,%.15f,%.15f)\n", p[0], p[1], p[2]);
  fprintf(stderr,"H=%f\n", H);
#endif

  if ( p[2] >= D) // rechts
  { if ( H <= rr ) // neben Kreisscheibe
    { wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = D;
      grad[0] = 0;
      grad[1] = 0;
      grad[2] = 1;

      dist = p[2] - D;
#ifdef TONNE
      fprintf(stderr,"rechts neben Kreisscheibe, Abstand %f!\n", dist);      
#endif
      return dist;
    }
    else if ( nvr[0] * p[2] + nvr[1] * H + ar <= 0)
    { // nicht im Sektor
      wnp[0] = rr * p[0]/H;
      wnp[1] = rr * p[1]/H;
      wnp[2] = D;

      grad[0] = p[0] - wnp[0];
      grad[1] = p[1] - wnp[1];
      grad[2] = p[2] - wnp[2];

      dist = norm3(grad);
      if ( dist > 0)
      { grad[0] /= dist;
        grad[1] /= dist;
        grad[2] /= dist;
      }
#ifdef TONNE
      fprintf(stderr,"rechts nicht im Sektor, Abstand %f!\n", dist);      
#endif
      return dist;
    }
    else // im Sektor
    { 
      wnp[0] = My * p[0]/H;
      wnp[1] = My * p[1]/H;
      wnp[2] = Mz;

      grad[0] = p[0] - wnp[0];
      grad[1] = p[1] - wnp[1];
      grad[2] = p[2] - wnp[2];

      dist = norm3(grad);
      grad[0] /= dist;
      grad[1] /= dist;
      grad[2] /= dist;
     
      wnp[0] += R*grad[0];
      wnp[1] += R*grad[1];
      wnp[2] += R*grad[2];

      dist -= R;
#ifdef TONNE
      fprintf(stderr,"rechts im Sektor, Abstand %f!\n", dist);      
#endif
      return dist;
    }
  }
  else if ( p[2] <= d) // links
  { if ( H <= rl ) // neben Kreisscheibe
    { 
      wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = d;
      grad[0] = 0;
      grad[1] = 0;
      grad[2] = -1;

      dist = d - p[2];
#ifdef TONNE
      fprintf(stderr,"links neben Kreisscheibe, Abstand %f!\n", dist);      
#endif
      return dist;
    }
    else if ( nvl[0] * p[2] + nvl[1] * H + al <= 0)
    { // nicht im Sektor
      wnp[0] = rl * p[0]/H;
      wnp[1] = rl * p[1]/H;
      wnp[2] = d;

      grad[0] = p[0] - wnp[0];
      grad[1] = p[1] - wnp[1];
      grad[2] = p[2] - wnp[2];

      dist = norm3(grad);
      if ( dist > 0)
      { grad[0] /= dist;
        grad[1] /= dist;
        grad[2] /= dist;
      }
#ifdef TONNE
      fprintf(stderr,"links nicht im Sektor, Abstand %f!\n", dist);      
#endif
      return dist;
    }
    else // im Sektor
    { 
      wnp[0] = My * p[0]/H;
      wnp[1] = My * p[1]/H;
      wnp[2] = Mz;

      grad[0] = p[0] - wnp[0];
      grad[1] = p[1] - wnp[1];
      grad[2] = p[2] - wnp[2];

      dist = norm3(grad);
      grad[0] /= dist;
      grad[1] /= dist;
      grad[2] /= dist;
     
      wnp[0] += R*grad[0];
      wnp[1] += R*grad[1];
      wnp[2] += R*grad[2];

      dist -= R;
#ifdef TONNE
      fprintf(stderr,"links im Sektor, Abstand %f!\n", dist);      
#endif
      return dist;
    }
  }
  else // d < p[2] < D
  { 
#ifdef TONNE
    fprintf(stderr,"mittig\n");
#endif
    if ( nvr[0] * p[2] + nvr[1] * H + ar <= 0 ||
         nvl[0] * p[2] + nvl[1] * H + al <= 0)
    { // nicht im Sektor, also innerhalb Tonne
      wnp[0] = p[0];
      wnp[1] = p[1];
      wnp[2] = p[2];
      dist = p[2] - d;
      if ( D - p[2] < dist)
        dist = D - p[2];
 
      dist = -dist; 
#ifdef TONNE
    fprintf(stderr,"nicht im Sektor, also in Tonne, Abstand %f\n", dist);
#endif
      return dist;
    }
    else // im Sektor
    { 
#ifdef TONNE
    fprintf(stderr,"im Sektor\n");
#endif
      if ( H < EPS_R)  // auf Z-Achse, im Sektor
      { 
        wnp[0] = p[0];
        wnp[1] = p[1];
        wnp[2] = p[2];

        dist = R - sqrt((p[2] - Mz)*(p[2]-Mz)+My*My);

        if ( p[2] - d < dist)
          dist = p[2] - d;
        if ( D - p[2] < dist)
          dist = D - p[2];

        dist = -dist;
#ifdef TONNE
        fprintf(stderr,"auf z-Achse, Abstand %f\n", dist);
#endif
        return dist;
      }
      else // nicht auf Z-Achse
      { wnp[0] = My * p[0]/H;
        wnp[1] = My * p[1]/H;
        wnp[2] = Mz;

        grad[0] = p[0] - wnp[0];
        grad[1] = p[1] - wnp[1];
        grad[2] = p[2] - wnp[2];

        dist = norm3(grad);

        if ( dist > R) // nicht in Tonne
        { 
          grad[0] /= dist;
          grad[1] /= dist;
          grad[2] /= dist;

          wnp[0] += R * grad[0];
          wnp[1] += R * grad[1];
          wnp[2] += R * grad[2];
  
          dist -= R;       
#ifdef TONNE
          fprintf(stderr,"ausserhalb Tonne, Abstand %f\n", dist);
#endif 
          return dist;         
        }
        else // in Tonne
        { 
          wnp[0] = p[0];
          wnp[1] = p[1];
          wnp[2] = p[2];

          dist = R - dist;
          if ( p[2] - d < dist)
            dist = p[2] - dist;
          if ( D - p[2] < dist)
            dist = p[2] - dist;

          dist = -dist;
#ifdef TONNE
          fprintf(stderr,"in Tonne, Abstand %f\n", dist);
#endif 
          return dist;
        }
      } 
    }
  }
}	

// Realisierung der Methode get_Kreisscheibenradius
// der abstrakten Basisklasse Werkzeug:
double SweepDist::Tonne::get_Kreisscheibenradius
( double z
)
{ if ( z < d || z > D)
    return 0.0;

  double dtmp = R*R - (Mz-z)*(Mz-z);
  if ( dtmp < 0) 
    dtmp = 0.0;

  return sqrt(dtmp) - fabs(My);
}



