#ifndef KONUS_LINKS_HOCH_SWEEPDIST_H
#define KONUS_LINKS_HOCH_SWEEPDIST_H

#include <cmath>

#include "Werkzeug.h"
#include "util.h"
#include "Konstanten.h"

namespace SweepDist
{

class Konus: public Werkzeug
{ 
  protected:

    double h;  // Hoehe des Konus
    double r;  // Radius vorne
    double R;  // Radius hinten
    double d; 
    double m;  // Mittelpunkt der Konusachse
    double D; 

    double fl_st;     // Steigung der Konusflanke
    double fl_nv[2];  // Normalenvektor zu Konusflanke, nach unten
    double fl_dist;   // Hesse-NF der Flanke: fl_nv*x+fl_dist = 0

  public:


    // Konstruktor:
    Konus
    ( double hh, 
      double rr, 
      double RR, 
      double dd=0.0
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


