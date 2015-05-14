#ifndef WERKZEUGSCHEIBE_SWEEPDIST_H
#define WERKZEUGSCHEIBE_SWEEPDIST_H

#include <cmath>

#include "util.h"
#include "Konstanten.h"
#include "Kreisscheibe.h"
#include "Werkzeug.h"

namespace SweepDist
{

class Werkzeugscheibe
{
  public:

    //Destruktor:
    virtual ~Werkzeugscheibe();


    // Konstruktor:
    Werkzeugscheibe
    ( Werkzeug &wz,  // zugrundeliegendes Werkzeug
      double ll,     // z-Koordinate linker Rand
      double rr      // z-Koordinate rechter Rand
    );


    // gibt z-Koordinate des linkes Randes zurueck:
    double get_Links();


    // gibt z-Koordinate des rechten Randes zurueck:
    double get_Rechts();


    // gibt Adresse der linken Kreisscheibe zurueck:
    Kreisscheibe *get_KsLinks();


    // gibt Adresse der rechtsn Kreisscheibe zurueck:
    Kreisscheibe *get_KsRechts();


    // gibt Adresse des Werkzeugs zurueck:
    Werkzeug *get_Wz();

   virtual double dist_point
    ( double p[],    // Punkt in Werkzeug-Koordinaten
      double grad[], // Gradient vom Werkzeug in Richtung Punkt p in 
                     // diejenige Richtung, in der der Abstand groesser wird
      double wnp[]   // Punkt auf Werkzeug mit minimalem Abstand zum gegebenen Punkt
    );

  private:

   Werkzeug *Wz;           // zugrundeliegendes Werkzeug
   Kreisscheibe *KsLinks;  // linke Kreisscheibe
   Kreisscheibe *KsRechts; // rechte Kreisscheibe

   double Links;           // z-Koordinate linker Rand
   double RadiusLinks;     // Radius linke Kreisscheibe
   double Rechts;          // z-Koordinate rechter Rand
   double RadiusRechts;    // Radius rechte Kreisscheibe


   // Copy-Konstruktor verbieten:
   Werkzeugscheibe(const Werkzeugscheibe &wzs)
   { }

   // Zuweisung verbieten:
   const Werkzeugscheibe & operator= (const Werkzeugscheibe &wzs)
   { return *this;
   }

};

}

#endif


