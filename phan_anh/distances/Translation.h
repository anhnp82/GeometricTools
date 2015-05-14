#ifndef TRANSLATION_SWEEPDIST_H
#define TRANSLATION_SWEEPDIST_H

#include "Werkzeug.h"
#include "Tippunkt.h"
#include "Motion.h"

namespace SweepDist
{

class Translationsfehler
{};


class Translation : public Motion
{ 
  protected:
	
    double P0[3];      /* Tip-Punkt, Anfang */
    double P1[3];      /* Tip-Punkt, Ende   */

    double Achse0[3];  /* Werkzeug-Achse, Anfang (normiert) */
    double BT_B[3][3]; /* Basiswechsel ins KO-System des Werkzeugs, 
  			 Tip-Punkt ist Nullpunkt,
		         z-Richtung ist Werkzeug-Achse am Anfang,
		         Kippen der Werkzeug-Achse in y-z-Ebene,
		         achse1 hat im neuen KO-System eine
		         NEGATIVE y-Komponente */
   double VBT_B[3];   /* Verschiebung ins KO-Systems des  Werkzeugs (Zeitpunkt 0) */
   double B_BT[3][3]; /* iniverser Basiswechsel */
   double VB_BT[3];   /* inverse Verschiebung */


  public:

    double aw[3];      /* Verschiebungsvektor im Welt-KO-System
 		          = P1 - P0  */
    double a[3];       /* Verschiebungsvektor der Bewegung
			  im KO-System des Werkzeugs!!! */
    double va;         /* Norm des Verschiebungsvektors, maximale
			  Verschiebungsgeschwindigkeit */


    // Konstruktor: erzeugt aus Werkzeug, den beiden Tippunkten und der Achse ein
    // Objekt der Klasse "Translation":
    Translation
    ( double p0[],      // Tippunkt Anfang der Bewegung
      double achse0[],  // (konstante) Achse
      double p1[]       // Tippunkt Ende der Bewegung
    );
    

    // erzeugt aus den Bewegungsdaten den Tippunkt zum Zeitpunkt t
    // macht aus den Bewegungsdaten den Tippunkt zum Zeitpunkt t
    void mach_tippunkt
    ( double t, 
      Tippunkt &tp
    );

    // Geschwindigkeit des Werkzeugpunktes point_wz zum Zeitpunkt t
    // Ergebnis: Betrag der Geschwinidkeit
    //           Geschwindigkeitsvektor in Welt-Koordinaten wird in speed_v 
    //           zurueckgegeben
    virtual double speed(double t, double point_wz[], double speed_v[]);
};

}

#endif
