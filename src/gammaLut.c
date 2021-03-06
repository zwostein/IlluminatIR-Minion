#include "gammaLut.h"

#include <stdint.h>
#include <avr/pgmspace.h>

#define GAMMALUT_MAX( bits ) ((1UL<<(bits))-1)

#define GAMMALUT_2P4( bits, type ) \
const type gammaLut_2p4_##bits[] PROGMEM = { \
	0.000000E+00*GAMMALUT_MAX(bits)+0.5, 1.676114E-06*GAMMALUT_MAX(bits)+0.5, 8.846583E-06*GAMMALUT_MAX(bits)+0.5, 2.340963E-05*GAMMALUT_MAX(bits)+0.5,  \
	4.669255E-05*GAMMALUT_MAX(bits)+0.5, 7.976853E-05*GAMMALUT_MAX(bits)+0.5, 1.235568E-04*GAMMALUT_MAX(bits)+0.5, 1.788706E-04*GAMMALUT_MAX(bits)+0.5,  \
	2.464447E-04*GAMMALUT_MAX(bits)+0.5, 3.269532E-04*GAMMALUT_MAX(bits)+0.5, 4.210208E-04*GAMMALUT_MAX(bits)+0.5, 5.292319E-04*GAMMALUT_MAX(bits)+0.5,  \
	6.521366E-04*GAMMALUT_MAX(bits)+0.5, 7.902557E-04*GAMMALUT_MAX(bits)+0.5, 9.440845E-04*GAMMALUT_MAX(bits)+0.5, 1.114096E-03*GAMMALUT_MAX(bits)+0.5,  \
	1.300743E-03*GAMMALUT_MAX(bits)+0.5, 1.504461E-03*GAMMALUT_MAX(bits)+0.5, 1.725669E-03*GAMMALUT_MAX(bits)+0.5, 1.964772E-03*GAMMALUT_MAX(bits)+0.5,  \
	2.222161E-03*GAMMALUT_MAX(bits)+0.5, 2.498215E-03*GAMMALUT_MAX(bits)+0.5, 2.793303E-03*GAMMALUT_MAX(bits)+0.5, 3.107781E-03*GAMMALUT_MAX(bits)+0.5,  \
	3.441997E-03*GAMMALUT_MAX(bits)+0.5, 3.796292E-03*GAMMALUT_MAX(bits)+0.5, 4.170994E-03*GAMMALUT_MAX(bits)+0.5, 4.566428E-03*GAMMALUT_MAX(bits)+0.5,  \
	4.982908E-03*GAMMALUT_MAX(bits)+0.5, 5.420742E-03*GAMMALUT_MAX(bits)+0.5, 5.880234E-03*GAMMALUT_MAX(bits)+0.5, 6.361677E-03*GAMMALUT_MAX(bits)+0.5,  \
	6.865363E-03*GAMMALUT_MAX(bits)+0.5, 7.391576E-03*GAMMALUT_MAX(bits)+0.5, 7.940594E-03*GAMMALUT_MAX(bits)+0.5, 8.512691E-03*GAMMALUT_MAX(bits)+0.5,  \
	9.108137E-03*GAMMALUT_MAX(bits)+0.5, 9.727197E-03*GAMMALUT_MAX(bits)+0.5, 1.037013E-02*GAMMALUT_MAX(bits)+0.5, 1.103719E-02*GAMMALUT_MAX(bits)+0.5,  \
	1.172864E-02*GAMMALUT_MAX(bits)+0.5, 1.244471E-02*GAMMALUT_MAX(bits)+0.5, 1.318566E-02*GAMMALUT_MAX(bits)+0.5, 1.395172E-02*GAMMALUT_MAX(bits)+0.5,  \
	1.474314E-02*GAMMALUT_MAX(bits)+0.5, 1.556014E-02*GAMMALUT_MAX(bits)+0.5, 1.640297E-02*GAMMALUT_MAX(bits)+0.5, 1.727183E-02*GAMMALUT_MAX(bits)+0.5,  \
	1.816697E-02*GAMMALUT_MAX(bits)+0.5, 1.908860E-02*GAMMALUT_MAX(bits)+0.5, 2.003695E-02*GAMMALUT_MAX(bits)+0.5, 2.101222E-02*GAMMALUT_MAX(bits)+0.5,  \
	2.201464E-02*GAMMALUT_MAX(bits)+0.5, 2.304441E-02*GAMMALUT_MAX(bits)+0.5, 2.410175E-02*GAMMALUT_MAX(bits)+0.5, 2.518686E-02*GAMMALUT_MAX(bits)+0.5,  \
	2.629994E-02*GAMMALUT_MAX(bits)+0.5, 2.744121E-02*GAMMALUT_MAX(bits)+0.5, 2.861085E-02*GAMMALUT_MAX(bits)+0.5, 2.980907E-02*GAMMALUT_MAX(bits)+0.5,  \
	3.103606E-02*GAMMALUT_MAX(bits)+0.5, 3.229202E-02*GAMMALUT_MAX(bits)+0.5, 3.357713E-02*GAMMALUT_MAX(bits)+0.5, 3.489160E-02*GAMMALUT_MAX(bits)+0.5,  \
	3.623560E-02*GAMMALUT_MAX(bits)+0.5, 3.760933E-02*GAMMALUT_MAX(bits)+0.5, 3.901297E-02*GAMMALUT_MAX(bits)+0.5, 4.044670E-02*GAMMALUT_MAX(bits)+0.5,  \
	4.191070E-02*GAMMALUT_MAX(bits)+0.5, 4.340516E-02*GAMMALUT_MAX(bits)+0.5, 4.493025E-02*GAMMALUT_MAX(bits)+0.5, 4.648615E-02*GAMMALUT_MAX(bits)+0.5,  \
	4.807304E-02*GAMMALUT_MAX(bits)+0.5, 4.969108E-02*GAMMALUT_MAX(bits)+0.5, 5.134045E-02*GAMMALUT_MAX(bits)+0.5, 5.302133E-02*GAMMALUT_MAX(bits)+0.5,  \
	5.473387E-02*GAMMALUT_MAX(bits)+0.5, 5.647826E-02*GAMMALUT_MAX(bits)+0.5, 5.825465E-02*GAMMALUT_MAX(bits)+0.5, 6.006322E-02*GAMMALUT_MAX(bits)+0.5,  \
	6.190412E-02*GAMMALUT_MAX(bits)+0.5, 6.377752E-02*GAMMALUT_MAX(bits)+0.5, 6.568358E-02*GAMMALUT_MAX(bits)+0.5, 6.762246E-02*GAMMALUT_MAX(bits)+0.5,  \
	6.959433E-02*GAMMALUT_MAX(bits)+0.5, 7.159933E-02*GAMMALUT_MAX(bits)+0.5, 7.363764E-02*GAMMALUT_MAX(bits)+0.5, 7.570939E-02*GAMMALUT_MAX(bits)+0.5,  \
	7.781476E-02*GAMMALUT_MAX(bits)+0.5, 7.995389E-02*GAMMALUT_MAX(bits)+0.5, 8.212693E-02*GAMMALUT_MAX(bits)+0.5, 8.433404E-02*GAMMALUT_MAX(bits)+0.5,  \
	8.657537E-02*GAMMALUT_MAX(bits)+0.5, 8.885107E-02*GAMMALUT_MAX(bits)+0.5, 9.116128E-02*GAMMALUT_MAX(bits)+0.5, 9.350616E-02*GAMMALUT_MAX(bits)+0.5,  \
	9.588585E-02*GAMMALUT_MAX(bits)+0.5, 9.830050E-02*GAMMALUT_MAX(bits)+0.5, 1.007503E-01*GAMMALUT_MAX(bits)+0.5, 1.032353E-01*GAMMALUT_MAX(bits)+0.5,  \
	1.057556E-01*GAMMALUT_MAX(bits)+0.5, 1.083116E-01*GAMMALUT_MAX(bits)+0.5, 1.109032E-01*GAMMALUT_MAX(bits)+0.5, 1.135306E-01*GAMMALUT_MAX(bits)+0.5,  \
	1.161940E-01*GAMMALUT_MAX(bits)+0.5, 1.188934E-01*GAMMALUT_MAX(bits)+0.5, 1.216291E-01*GAMMALUT_MAX(bits)+0.5, 1.244012E-01*GAMMALUT_MAX(bits)+0.5,  \
	1.272098E-01*GAMMALUT_MAX(bits)+0.5, 1.300550E-01*GAMMALUT_MAX(bits)+0.5, 1.329370E-01*GAMMALUT_MAX(bits)+0.5, 1.358560E-01*GAMMALUT_MAX(bits)+0.5,  \
	1.388119E-01*GAMMALUT_MAX(bits)+0.5, 1.418051E-01*GAMMALUT_MAX(bits)+0.5, 1.448356E-01*GAMMALUT_MAX(bits)+0.5, 1.479035E-01*GAMMALUT_MAX(bits)+0.5,  \
	1.510090E-01*GAMMALUT_MAX(bits)+0.5, 1.541522E-01*GAMMALUT_MAX(bits)+0.5, 1.573332E-01*GAMMALUT_MAX(bits)+0.5, 1.605522E-01*GAMMALUT_MAX(bits)+0.5,  \
	1.638093E-01*GAMMALUT_MAX(bits)+0.5, 1.671046E-01*GAMMALUT_MAX(bits)+0.5, 1.704383E-01*GAMMALUT_MAX(bits)+0.5, 1.738104E-01*GAMMALUT_MAX(bits)+0.5,  \
	1.772212E-01*GAMMALUT_MAX(bits)+0.5, 1.806706E-01*GAMMALUT_MAX(bits)+0.5, 1.841590E-01*GAMMALUT_MAX(bits)+0.5, 1.876863E-01*GAMMALUT_MAX(bits)+0.5,  \
	1.912527E-01*GAMMALUT_MAX(bits)+0.5, 1.948583E-01*GAMMALUT_MAX(bits)+0.5, 1.985032E-01*GAMMALUT_MAX(bits)+0.5, 2.021877E-01*GAMMALUT_MAX(bits)+0.5,  \
	2.059117E-01*GAMMALUT_MAX(bits)+0.5, 2.096754E-01*GAMMALUT_MAX(bits)+0.5, 2.134790E-01*GAMMALUT_MAX(bits)+0.5, 2.173225E-01*GAMMALUT_MAX(bits)+0.5,  \
	2.212060E-01*GAMMALUT_MAX(bits)+0.5, 2.251298E-01*GAMMALUT_MAX(bits)+0.5, 2.290938E-01*GAMMALUT_MAX(bits)+0.5, 2.330983E-01*GAMMALUT_MAX(bits)+0.5,  \
	2.371433E-01*GAMMALUT_MAX(bits)+0.5, 2.412290E-01*GAMMALUT_MAX(bits)+0.5, 2.453554E-01*GAMMALUT_MAX(bits)+0.5, 2.495227E-01*GAMMALUT_MAX(bits)+0.5,  \
	2.537310E-01*GAMMALUT_MAX(bits)+0.5, 2.579804E-01*GAMMALUT_MAX(bits)+0.5, 2.622711E-01*GAMMALUT_MAX(bits)+0.5, 2.666031E-01*GAMMALUT_MAX(bits)+0.5,  \
	2.709765E-01*GAMMALUT_MAX(bits)+0.5, 2.753916E-01*GAMMALUT_MAX(bits)+0.5, 2.798482E-01*GAMMALUT_MAX(bits)+0.5, 2.843467E-01*GAMMALUT_MAX(bits)+0.5,  \
	2.888871E-01*GAMMALUT_MAX(bits)+0.5, 2.934695E-01*GAMMALUT_MAX(bits)+0.5, 2.980940E-01*GAMMALUT_MAX(bits)+0.5, 3.027608E-01*GAMMALUT_MAX(bits)+0.5,  \
	3.074699E-01*GAMMALUT_MAX(bits)+0.5, 3.122214E-01*GAMMALUT_MAX(bits)+0.5, 3.170156E-01*GAMMALUT_MAX(bits)+0.5, 3.218523E-01*GAMMALUT_MAX(bits)+0.5,  \
	3.267319E-01*GAMMALUT_MAX(bits)+0.5, 3.316543E-01*GAMMALUT_MAX(bits)+0.5, 3.366198E-01*GAMMALUT_MAX(bits)+0.5, 3.416283E-01*GAMMALUT_MAX(bits)+0.5,  \
	3.466800E-01*GAMMALUT_MAX(bits)+0.5, 3.517750E-01*GAMMALUT_MAX(bits)+0.5, 3.569135E-01*GAMMALUT_MAX(bits)+0.5, 3.620955E-01*GAMMALUT_MAX(bits)+0.5,  \
	3.673211E-01*GAMMALUT_MAX(bits)+0.5, 3.725904E-01*GAMMALUT_MAX(bits)+0.5, 3.779036E-01*GAMMALUT_MAX(bits)+0.5, 3.832607E-01*GAMMALUT_MAX(bits)+0.5,  \
	3.886618E-01*GAMMALUT_MAX(bits)+0.5, 3.941071E-01*GAMMALUT_MAX(bits)+0.5, 3.995966E-01*GAMMALUT_MAX(bits)+0.5, 4.051304E-01*GAMMALUT_MAX(bits)+0.5,  \
	4.107088E-01*GAMMALUT_MAX(bits)+0.5, 4.163316E-01*GAMMALUT_MAX(bits)+0.5, 4.219991E-01*GAMMALUT_MAX(bits)+0.5, 4.277114E-01*GAMMALUT_MAX(bits)+0.5,  \
	4.334685E-01*GAMMALUT_MAX(bits)+0.5, 4.392706E-01*GAMMALUT_MAX(bits)+0.5, 4.451177E-01*GAMMALUT_MAX(bits)+0.5, 4.510100E-01*GAMMALUT_MAX(bits)+0.5,  \
	4.569475E-01*GAMMALUT_MAX(bits)+0.5, 4.629304E-01*GAMMALUT_MAX(bits)+0.5, 4.689587E-01*GAMMALUT_MAX(bits)+0.5, 4.750326E-01*GAMMALUT_MAX(bits)+0.5,  \
	4.811521E-01*GAMMALUT_MAX(bits)+0.5, 4.873174E-01*GAMMALUT_MAX(bits)+0.5, 4.935285E-01*GAMMALUT_MAX(bits)+0.5, 4.997855E-01*GAMMALUT_MAX(bits)+0.5,  \
	5.060886E-01*GAMMALUT_MAX(bits)+0.5, 5.124377E-01*GAMMALUT_MAX(bits)+0.5, 5.188332E-01*GAMMALUT_MAX(bits)+0.5, 5.252749E-01*GAMMALUT_MAX(bits)+0.5,  \
	5.317630E-01*GAMMALUT_MAX(bits)+0.5, 5.382977E-01*GAMMALUT_MAX(bits)+0.5, 5.448789E-01*GAMMALUT_MAX(bits)+0.5, 5.515069E-01*GAMMALUT_MAX(bits)+0.5,  \
	5.581817E-01*GAMMALUT_MAX(bits)+0.5, 5.649033E-01*GAMMALUT_MAX(bits)+0.5, 5.716719E-01*GAMMALUT_MAX(bits)+0.5, 5.784876E-01*GAMMALUT_MAX(bits)+0.5,  \
	5.853505E-01*GAMMALUT_MAX(bits)+0.5, 5.922606E-01*GAMMALUT_MAX(bits)+0.5, 5.992181E-01*GAMMALUT_MAX(bits)+0.5, 6.062230E-01*GAMMALUT_MAX(bits)+0.5,  \
	6.132754E-01*GAMMALUT_MAX(bits)+0.5, 6.203755E-01*GAMMALUT_MAX(bits)+0.5, 6.275233E-01*GAMMALUT_MAX(bits)+0.5, 6.347189E-01*GAMMALUT_MAX(bits)+0.5,  \
	6.419625E-01*GAMMALUT_MAX(bits)+0.5, 6.492540E-01*GAMMALUT_MAX(bits)+0.5, 6.565936E-01*GAMMALUT_MAX(bits)+0.5, 6.639813E-01*GAMMALUT_MAX(bits)+0.5,  \
	6.714174E-01*GAMMALUT_MAX(bits)+0.5, 6.789018E-01*GAMMALUT_MAX(bits)+0.5, 6.864346E-01*GAMMALUT_MAX(bits)+0.5, 6.940159E-01*GAMMALUT_MAX(bits)+0.5,  \
	7.016459E-01*GAMMALUT_MAX(bits)+0.5, 7.093246E-01*GAMMALUT_MAX(bits)+0.5, 7.170521E-01*GAMMALUT_MAX(bits)+0.5, 7.248285E-01*GAMMALUT_MAX(bits)+0.5,  \
	7.326538E-01*GAMMALUT_MAX(bits)+0.5, 7.405282E-01*GAMMALUT_MAX(bits)+0.5, 7.484518E-01*GAMMALUT_MAX(bits)+0.5, 7.564246E-01*GAMMALUT_MAX(bits)+0.5,  \
	7.644467E-01*GAMMALUT_MAX(bits)+0.5, 7.725182E-01*GAMMALUT_MAX(bits)+0.5, 7.806392E-01*GAMMALUT_MAX(bits)+0.5, 7.888098E-01*GAMMALUT_MAX(bits)+0.5,  \
	7.970301E-01*GAMMALUT_MAX(bits)+0.5, 8.053001E-01*GAMMALUT_MAX(bits)+0.5, 8.136200E-01*GAMMALUT_MAX(bits)+0.5, 8.219898E-01*GAMMALUT_MAX(bits)+0.5,  \
	8.304096E-01*GAMMALUT_MAX(bits)+0.5, 8.388795E-01*GAMMALUT_MAX(bits)+0.5, 8.473996E-01*GAMMALUT_MAX(bits)+0.5, 8.559700E-01*GAMMALUT_MAX(bits)+0.5,  \
	8.645907E-01*GAMMALUT_MAX(bits)+0.5, 8.732618E-01*GAMMALUT_MAX(bits)+0.5, 8.819835E-01*GAMMALUT_MAX(bits)+0.5, 8.907557E-01*GAMMALUT_MAX(bits)+0.5,  \
	8.995787E-01*GAMMALUT_MAX(bits)+0.5, 9.084524E-01*GAMMALUT_MAX(bits)+0.5, 9.173770E-01*GAMMALUT_MAX(bits)+0.5, 9.263525E-01*GAMMALUT_MAX(bits)+0.5,  \
	9.353790E-01*GAMMALUT_MAX(bits)+0.5, 9.444566E-01*GAMMALUT_MAX(bits)+0.5, 9.535854E-01*GAMMALUT_MAX(bits)+0.5, 9.627655E-01*GAMMALUT_MAX(bits)+0.5,  \
	9.719969E-01*GAMMALUT_MAX(bits)+0.5, 9.812797E-01*GAMMALUT_MAX(bits)+0.5, 9.906141E-01*GAMMALUT_MAX(bits)+0.5, 1.000000E+00*GAMMALUT_MAX(bits)+0.5,  \
}


GAMMALUT_2P4( 8, uint8_t );
GAMMALUT_2P4( 10, uint16_t );
