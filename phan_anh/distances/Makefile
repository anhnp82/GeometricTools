
COPT=-c -Wall -O -Wuninitialized -pedantic
#COPT=-c -DTONNE -Wall -O -Wuninitialized -pedantic
LOPT=
ROPT=


OBJECTS= Konstanten.o Motion.o Translation.o util.o Tippunkt.o Bewegung.o Kugel.o Torus.o Zylinder.o Konus.o Kreisscheibe.o Werkzeugscheibe.o Linse.o Linse_flach.o Tonne.o


#OBJECTS= Konstanten.o Motion.o Translation.o util.o Tippunkt.o Bewegung.o Kugel.o Torus.o Zylinder_flach.o Zylinder_hoch.o Linse.o Konus_links_flach.o Konus_links_hoch.o Konus_rechts_flach.o Konus_rechts_hoch.o AsyTonne_hoch.o AsyTonne_flach.o Linse_flach.o Kreisscheibe.o Werkzeugscheibe.o 

all: $(OBJECTS) 

.cpp.o:
	g++ $(COPT) $<

	
clean:
	rm -f $(OBJECTS)
