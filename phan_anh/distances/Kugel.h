#ifndef KUGEL_SWEEPDIST_H
#define KUGEL_SWEEPDIST_H

#include <cmath>

#include "Werkzeug.h"
#include "util.h"
#include "Konstanten.h"

namespace SweepDist
{

class Kugel: public Werkzeug
{ 
  protected:

    double r;  // Kugelradius
    double d;  // z-Koordinate der "linken" Kugelseite vom Tippunkt
               // kann positiv oder negativ sein    
    double D;  // "rechte" Kugelseite
    double m;  // z-Koordinate des Mittelpunktes (=d+r)
  public:
    

    // Konstruktor:
    Kugel
    ( double rr, 
      double dd = 0
    ); 


    // Realisierung der Methode is_point_closest 
    // der abstrakten Basisklasse Werkzeug:
    virtual double dist_point
    ( double p[], 
      double grad[],
      double wnp[]
    );


    // der abstrakten Basisklasse Werkzeug:
    virtual double get_Kreisscheibenradius
    ( double z
    );

};

}

#endif


