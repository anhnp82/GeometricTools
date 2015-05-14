#ifndef TIPPUNKT_SWEEPDIST_H
#define TIPPUNKT_SWEEPDIST_H

#include "Werkzeug.h"
#include "Konstanten.h"

#pragma warning( disable : 4290 )

namespace SweepDist
{

//class Bewegung;

/* Fehlerklasse */
struct Tippunktfehler
{};

class Tippunkt
{ friend class Bewegung;
  friend class Translation;
 
  protected:
    double bw[3][3];      // Basiswechselmatrix
    double versch[3];     // Verschiebungsvektor
    double inv_bw[3][3];  // inverse Basiswechselmatrix
    double inv_versch[3]; // inverse Verschiebung
    bool initialized;

  public:

    // Umrechnung eines Punktes vom Welt-Ko-System ins Tippunkt-Ko-System
    void punkt_welt_tippunkt
    ( double p[],  // gegebener Punkt in Welt-Koordinaten
      double q[]   // berechnete Tippunkt-Koordinaten des Punkes
    ) throw(Tippunktfehler);
    
    // Umrechnung eines Punktes vom Tippunkt-Ko-System ins Welt-Ko-System
    void punkt_tippunkt_welt
    ( double q[],  // gegebener Punkt in Tippunkt-Koordinaten
      double p[]   // berechnete Welt-Koordinaten des Punkes
    ) throw(Tippunktfehler);

    // Umrechnung eines Vektors vom Welt-Ko-System ins Tippunkt-Ko-System
    void vektor_welt_tippunkt
    ( double p[],  // gegebener Vektor in Welt-Koordinaten
      double q[]   // berechnete Tippunkt-Koordinaten des Vektors
    ) throw(Tippunktfehler);
    
    // Umrechnung eines Vektors vom Tippunkt-Ko-System ins Welt-Ko-System
    void vektor_tippunkt_welt
    ( double q[],  // gegebener Vektor in Tippunkt-Koordinaten
      double p[]   // berechnete Welt-Koordinaten des Vektors
    ) throw(Tippunktfehler);

    // leerer Konstruktor, Tippunkten werden von 
    // der Funktion Bewegung::mach_tippunkt erzeugt
    Tippunkt() : initialized(false) {};

    // weiterer Konstruktor zur direkten Berechnung eines
    // Tippunktes aus den Weltkoordinaten des Tippunktes
    // und der Rotationsachse:
    Tippunkt
    ( double p[], 
      double achse[]
    );

};

}

#pragma warning( default : 4290 )

#endif

