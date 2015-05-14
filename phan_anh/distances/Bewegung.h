#ifndef BEWEGUNG_SWEEPDIST_H
#define BEWEGUNG_SWEEPDIST_H

#include "Werkzeug.h"
#include "Tippunkt.h"
#include "Motion.h"

namespace SweepDist
{

class Bewegung: public Motion
{ 
  protected:
	
    double P0[3];      /* Tip-Punkt, Anfang */
    double P1[3];      /* Tip-Punkt, Ende   */

    double Achse0[3];  /* Werkzeug-Achse, Anfang (normiert) */
    double Achse1[3];  /* Werkzeug-Achse, Ende   (normiert) */
    double BT_B[3][3]; /* Basiswechsel ins KO-System des Werkzeugs, 
                          Tip-Punkt ist Nullpunkt,
                          z-Richtung ist Werkzeug-Achse am Anfang,
                          Kippen der Werkzeug-Achse in y-z-Ebene,
                          achse1 hat im neuen KO-System eine
                          NEGATIVE y-Komponente */
    double VBT_B[3];   /* Verschiebung ins KO-Systems des  Werkzeugs (Zeitpunkt 0) */
    double B_BT[3][3]; /* iniverser Basiswechsel */
    double VB_BT[3];   /* inverse Verschiebung */

    Tippunkt TP0;        /* Tippunkt zum Zeitpunkt t=0 */

  public:

    double gamma;      /* Gesamt-Kippwinkel des Werkzeugs */
    double aw[3];      /* Verschiebungsvektor im Welt-KO-System = P1 - P0  */
    double a[3];       /* Verschiebungsvektor der Bewegung
                          im KO-System des Werkzeugs!!! */
    double va;         /* Norm des Verschiebungsvektors, maximale
                          Verschiebungsgeschwindigkeit */

    // Konstruktor: erzeugt aus den beiden Tippunkten und den Achsen ein
    // Objekt der Klasse "Bewegung":
    Bewegung
    ( double p0[],     // 1. Tippunkt
      double achse0[], // 1. Achse
      double p1[],     // 2. Tippunkt
      double achse1[]  // 2. Achse
    );
    

    // erzeugt aus den Bewegungsdaten den Tippunkt zum Zeitpunkt t
    void mach_tippunkt
    ( double t, 
      Tippunkt &tp
    );

    // berechnet Geschwindigkeit des Werkzeugpunktes mit (Werkzeug-) Koordinaten point_tool
    // zum Zeitpunkt t.
    // gibt den Geschwindikeitsvektor (in Welt-Koordinaten) in speed_v zurueck und 
    // den Betrag der Geschwindigkeit im Funktionsergebnis:
    virtual double speed(double t, double point_tool[], double speed_v[]);

  protected:

};

}

#endif
