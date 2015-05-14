#ifndef MOTION_MACHER_SWEEPDIST_H
#define MOTION_MACHER_SWEEPDIST_H


#include "Translation.h"
#include "Bewegung.h"
#include "Konstanten.h"

namespace SweepDist
{

class Motion_macher
{
   public:

   Motion* mach_Motion( double p0[], double achse0[],
                        double p1[], double achse1[])
   { double tmpvek[3];
     vektoradd3(1, achse1, -1, achse0, tmpvek);

     if ( norm3(tmpvek) < EPS_R)
     { // Motion ist Translation:
       return new Translation( p0, achse0, p1);
     }
     else
     { // Motion ist funef-Achs-Bewegung:
       return new Bewegung( p0, achse0, p1, achse1);
     }
   }     

   Motion* mach_Bewegung( double p0[], double achse0[],
                          double p1[], double achse1[])
   { 
     return new Bewegung( p0, achse0, p1, achse1);
   }     

   Motion* mach_Translation( double p0[], double achse0[],
                             double p1[], double achse1[])
   { 
     return new Translation( p0, achse0, p1);
   }     
};

}

#endif


