#ifndef LINSE_FLACH_SWEEPDIST_H
#define LINSE_FLACH_SWEEPDIST_H

#include <cmath>

#include "Werkzeug.h"
#include "util.h"
#include "Konstanten.h"

namespace SweepDist
{

class Linse_flach: public Werkzeug
{ 
  protected:

    double h;  // Linsendicke
    double H;  // Linsenhoehe
    double rk; // Radius der rechten und linken (flachen) Kreisscheibe
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
    Linse_flach 
    ( double hh, 
      double HH, 
      double rkrk, 
      double dd = 0
    );


    // Realisierung der Methode dist_point
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


