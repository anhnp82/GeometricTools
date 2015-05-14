#ifndef ZYLINDER_SWEEPDIST_H
#define ZYLINDER_SWEEPDIST_H

#include <cmath>

#include "Werkzeug.h"
#include "util.h"
#include "Konstanten.h"

namespace SweepDist
{

class Zylinder: public Werkzeug
{ 
  protected:

    double h;  // Hoehe des Zylinders
    double r;  // Radius
    double d;  // z-Koordinate des linken Randes
    double m;  // z-Koordinate des Mittelpunktes
    double D;  // z-Koordinate des rechten Randes

  public:

    // Konstruktor
    Zylinder
    ( double hh, 
      double rr, 
      double dd=0
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


