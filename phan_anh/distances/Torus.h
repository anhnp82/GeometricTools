#ifndef TORUS_H
#define TORUS_H

#include <cmath>

#include "Werkzeug.h"
#include "util.h"
#include "Konstanten.h"

namespace SweepDist
{

class Torus: public Werkzeug
{ 
  protected:

    double R;  // Seelenkreisradius
    double r;  // Wulstradius
    double d;  // z-Koordinate der linken Kreisflaeche
    double m;  // z-Koordinate des Mittelpunktes
    double D;  // z-Koordinate der rechten Kreisflaeche

  public:

    // Konstruktor
    Torus
    ( double RR, 
      double rr, 
      double dd = 0
    );


    // Realisierung der Methode is_point_closest
    // der abstrakten Basisklasse Werkzeug:
    virtual double dist_point
    ( double p[],
      double grad[],
      double wnp[]
    );


    // Realisierung der Methode get_Kreisscheibenradius
    // der abstrakten Basisklasse Werkzeug:
    virtual double get_Kreisscheibenradius
    ( double z
    );

};

}

#endif


