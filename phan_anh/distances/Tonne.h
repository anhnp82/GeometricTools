#ifndef TONNE_SWEEPDIST_H
#define TONNE_SWEEPDIST_H

#include <cmath>

#include "Werkzeug.h"
#include "util.h"
#include "Konstanten.h"

namespace SweepDist
{

class Tonne: public Werkzeug
{ 
  protected:

    double h;  // Hoehe der Tonne
    double rl;  // Radius Seitenkreisflaeche links
    double rr;  // Radius Seitenkreisflaeche rechts
    double R;  // Radius des "Tonnenbauchkreises"
    double d;  // z-Koordinate des linken Randes
    double m;  // z-Koordinate des Mittelpunktes
    double D;  // z-Koordinate des rechten Randes

//    double mittelhoehe; // Hoehe des Schnittpunktes der beiden Schnittparabeln

    double Mz;  // z-Koordinate Mittelpunkt des "Tonnenbauchkreises"
    double My;  // y-Koordinate Mittelpunkt des "Tonnenbauchkreises"

    double alpha0, alpha1; // Begrenzungswinkel des "Tonnenbauchkreises"

    double nvr[2], nvl[2], ar, al;
    

  public:

    // Konstruktor:
    Tonne
    ( double Rl, 
      double Rr, 
      double hh, 
      double RR, 
      double dd=0
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


