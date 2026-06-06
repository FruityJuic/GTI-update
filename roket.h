#ifndef ROKET_H
#define ROKET_H

#include <GL/glut.h>

extern float roketX, roketY, roketZ;
extern float roketDX, roketDY, roketDZ;
extern bool  roketAktif;
extern float jarakDariPusat;
extern float konstantaGravitasiMatahari;
extern bool  efekJarakAktif;

void resetPartikel();
void resetTrail();
void launchRoket();
void updateRoket();
void drawRoket(bool isShadow);

#endif