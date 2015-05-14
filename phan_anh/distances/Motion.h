#ifndef MOTION_SWEEPDIST_H
#define MOTION_SWEEPDIST_H

#include "Werkzeug.h"
#include "Werkzeugscheibe.h"
#include "Tippunkt.h"

namespace SweepDist
{

class Motion
{ 
  public:

    // Destruktor, virtuell da abstrakte Klasse 
    virtual ~Motion()
    {}

    // Abstand des Werkzeugs bei Bewegung zum Zeitpunkt t
    double dist_statisch
    ( double t,          // Zeitpunkt (Eingabe)
      Werkzeug &wz,      // Werkzeug  (Eingabe)
      double point_w[],  // Welt-Koordinaten des Punktes  (Eingabe)
      double grad_w[],   // Abstandsvektor vom Werkzeug zum Punkt (in Welt-Koordinaten, Rueckgabe)
      double wnp_w[],    // Punkt auf Werkzeug mit minimalem Abstand (in Welt-Koordinaten, Rueckgabe)
      double *abl        // Abstandsaenderung zum Zeitpunkt t (Ableitung des Abstandes, Rueckgabe)
    );                   // Rueckgabe: Abstand zum Zeitpunkt t

    // Abstand der Werkzeugscheibe bei Bewegung zum Zeitpunkt t
    double dist_statisch
    ( double t, 
      Werkzeugscheibe &wz, 
      double point_w[],
      double grad_w[],
      double wnp_w[],
      double *abl
    );

    // Geschwindigkeit des Werkzeugpunktes point_wz zum Zeitpunkt t
    // Ergebnis: Betrag der Geschwinidkeit
    //           Geschwindigkeitsvektor in Welt-Koordinaten wird in speed_w 
    //           zurueckgegeben
    virtual double speed(double t, double point_wz[], double speed_v[]) = 0;

    //  berechnet minimalen Abstand vom bewegten Werkzeug zum Punkt
    //  im Intervall [t0, t1] bei vorliegender Bewegung 
    //
    // Funktionsergebnis:
    //   -1:  Werkzeug kollidiert mit Punkt (KEINE Abstandsberechnung!)
    //    1:  minimaler Abstand ausserhalb von [t0,t1], d.h.
    //        - Ableitung des Abstandes bei t0 > 0, berechnet werden Abstand und Gradient bei t0
    //        oder
    //        - Ableitung des Abstandes bei t1 < 0, berechnet werden Abstand und Gradient bei t1
    //    0:  minimaler Abstand im Intervall [t0, t1] (Abstand wird berechnet)
    // 
    //  Rueckgabe: (nur, wenn Funktionsergebnis 0 oder 1)
    //       t_min: Zeitpunkt der minimalen Distanz 
    //    dist_min: minimale Distanz
    //      grad[]: (normierter) Distanzvektor vom Werkzeug zum Punkt
    virtual int min_dist
    ( Werkzeug &wz, 
      double point[], 
      double *t_min,
      double *dist_min,
      double grad[],
      double t0 = 0.0, 
      double t1 = 1.0
    );
    // Dito fuer Werkzeugscheiben
    virtual int min_dist
    ( Werkzeugscheibe &wz, 
      double point[], 
      double *t_min,
      double *dist_min,
      double grad[],
      double t0 = 0.0, 
      double t1 = 1.0
    );

    // macht aus den Bewegungsdaten den Tippunkt zum Zeitpunkt t
    virtual void mach_tippunkt
    ( double t, 
      Tippunkt &tp
    ) = 0;

};

}

#endif
