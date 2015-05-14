#ifndef KREISSCHEIBE_SWEEPDIST_H
#define KREISSCHEIBE_SWEEPDIST_H

#include <cmath>

#include "Werkzeug.h"
#include "util.h"
#include "Konstanten.h"

namespace SweepDist
{

class Kreisscheibe : public Werkzeug
{ 

  protected:

    double R;  // Kreisradius
    double m;  // z-Koordinate des Mittelpunktes


  public:

    // Konstruktor:
    Kreisscheibe
    ( double RR, 
      double mm = 0
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
    virtual double get_Kreisscheibenradius(double z);


};

}

#endif


