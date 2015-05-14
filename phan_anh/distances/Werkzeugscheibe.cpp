#include <cstdio>
#include <cmath>

using namespace std;

#include "Werkzeugscheibe.h"
#include "util.h"
#include "Konstanten.h"



// Destruktor:
SweepDist::Werkzeugscheibe::~Werkzeugscheibe()
{ if (KsLinks) delete KsLinks;
  if (KsRechts) delete KsRechts;
  
  KsLinks = 0;
  KsRechts = 0;
}


// Konstruktor:
SweepDist::Werkzeugscheibe::Werkzeugscheibe
( Werkzeug &wz,   // zugrundeliegendes Werkzeug
  double ll,      // z-Koordinate linker Rand
  double rr       // z-Koordinate rechter Rand
) : Wz(&wz), Links(ll), Rechts(rr)
{

  if ( Links >= Rechts) throw Werkzeugfehler();

  KsLinks = 0;
  KsRechts = 0;

  if ( (RadiusLinks = Wz->get_Kreisscheibenradius(Links)) > 0)
    KsLinks = new Kreisscheibe(RadiusLinks, Links);

  if ( (RadiusRechts = Wz->get_Kreisscheibenradius(Rechts)) > 0)
    KsRechts = new Kreisscheibe(RadiusRechts, Rechts);
}   


// gibt z-Koordinate des linkes Randes zurueck:
double SweepDist::Werkzeugscheibe::get_Links()
{ return Links;
}


// gibt z-Koordinate des rechten Randes zurueck:
double SweepDist::Werkzeugscheibe::get_Rechts()
{ return Rechts;
}


// gibt Adresse der linken Kreisscheibe zurueck:
SweepDist::Kreisscheibe * SweepDist::Werkzeugscheibe::get_KsLinks()
{ return KsLinks;
}


// gibt Adresse der rechten Kreisscheibe zurueck:
SweepDist::Kreisscheibe * SweepDist::Werkzeugscheibe::get_KsRechts()
{ return KsRechts;
}


// gibt Adresse des Werkzeugs zurueck:
SweepDist::Werkzeug * SweepDist::Werkzeugscheibe::get_Wz()
{ return Wz;
}

double SweepDist::Werkzeugscheibe::dist_point
( double p[],    // Punkt in Werkzeug-Koordinaten
  double grad[], // Gradient vom Werkzeug in Richtung Punkt p in 
                 // diejenige Richtung, in der der Abstand groesser wird
  double wnp[]   // Punkt auf Werkzeug mit minimalem Abstand zum gegebenen Punkt
)
{ double dist;

  dist = Wz->dist_point(p, grad, wnp);

  if (  KsLinks == 0 && KsRechts == 0)
    return dist;

  if ( KsLinks != 0 && wnp[2] < Links)
  {
#ifdef WERKZEUGSCHEIBE
    fprintf(stderr,"linke Kreisscheibe!\n");
#endif
    dist = KsLinks->dist_point(p, grad, wnp);
    return dist;
  }

  if ( KsRechts != 0 && wnp[2] > Rechts)
  {
#ifdef WERKZEUGSCHEIBE
    fprintf(stderr,"rechte Kreisscheibe!\n");
#endif
    dist = KsRechts->dist_point(p, grad, wnp);
    return dist;
  }

  if ( dist < 0)
  {
    if ( KsLinks != 0)
      if ( Links - wnp[2]  > dist)
        dist = Links- wnp[2];
    if ( KsRechts != 0)
      if ( wnp[2] - Rechts > dist)
        dist = wnp[2] - Rechts;
  }

  return dist;

}


  

