#ifndef LINSE_SWEEPDIST_H
#define LINSE_SWEEPDIST_H

#include <cmath>

#include "Werkzeug.h"
#include "util.h"
#include "Konstanten.h"

namespace SweepDist
{

class Linse: public Werkzeug
{ 
  protected:

    double h;  // Linsendicke
    double H;  // Linsenhoehe
    double d;  // z-Koordinate der linken Kreisflaeche
    double m;  // z-Koordinate des Mittelpunktes
    double D;  // z-Koordinate der rechten Kreisflaeche

    double Ml;  // Mittelpunkt der rechten Kreisschale
    double Mr;  // Mittelpunkt der linken Kreisschale
    double R;  // Radius der Kugelschalen

    double nvl[2]; //Sektorgerade durch Ml
    double al;

    double nvr[2]; //Sektorgerade durch Mr
    double ar;


  public:

    // Konstruktor:
    Linse
    ( double hh, 
      double HH, 
      double dd = 0
    );


    // Realisierung der Methode is_point_closest
    // der abstrakten Basisklasse Werkzeug:
    virtual double dist_point
    ( double p[],
      double grad[], // Gradient vom Werkzeug in Richtung Punkt p in 
                     // diejenige Richtung, in der der Abstand groesser wird
      double wnp[]   // Punkt auf Werkzeug mit minimalem Abstand zum gegebenen Punkt
    );

    // Realisierung der Methode get_Kreisscheibenradius
    // der abstrakten Basisklasse Werkzeug:
    virtual double get_Kreisscheibenradius
    ( double z
    );
};

}

#endif


