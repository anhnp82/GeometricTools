#ifndef WERKZEUG_SWEEPDIST_H
#define WERKZEUG_SWEEPDIST_H

#include <cmath>

#include "util.h"
#include "Konstanten.h"

namespace SweepDist
{

struct Werkzeugfehler
{};

class Werkzeug
{
  public:


    // virtueller Destruktor:
    virtual ~Werkzeug() {}

    /* dist_point:
     
       berechnet den Abstand des Punktes p vom Werkzeug

       Rueckgabe:

         im Fall eines positiven Abstandes:

           grad[3]: (normierter) Richtungsvektor vom Werkzeug in Richtung p
           wnp[3]:  Punkt auf Werkzeugoberflaeche mit minimalem Abstand zum Punkt

           (jeweils in Werkzeug-Koodrinaten)

         im Fall eines negativen Abstandes:       

           grad[3]: bedeutungslos
           wnp[3]:  Punkt p selbst
    */
    virtual double dist_point
    ( double p[],    // Punkt in Werkzeug-Koordinaten
      double grad[], // Gradient vom Werkzeug in Richtung Punkt p in 
                     // diejenige Richtung, in der der Abstand groesser wird
      double wnp[]   // Punkt auf Werkzeug mit minimalem Abstand zum gegebenen Punkt
    ) = 0;

    /* get_Kreisscheibenradius:

       berechnet den Radius der mit der als Argument gegebenen z-Koordinate
       aus dem Werkzeug herausgeschnittenen Kreisscheibe.

       Diese Radius kann 0 sein, falls die Schnittebene ausserhalb des Werkzeuges bleibt.
    */
    virtual double get_Kreisscheibenradius
    ( double z
    ) = 0;

    /* getRadius:

       liefert den maximalen Abstand eines Werkzeugpunktes 
       vom Nullpunkt (= Tippunkt)
    */
    double getRadius()
    { return Radius; 
    }

  
  protected:

    // maximaler Abstand eines Werkzeugpunktes vom Nullpunkt:
    double Radius;
};

}

#endif


