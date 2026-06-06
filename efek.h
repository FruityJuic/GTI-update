#ifndef EFEK_H
#define EFEK_H

#include <GL/glut.h>

extern float globalTime;
extern float fogDensity;
extern int   winWidth, winHeight;

void generateBintang();
void drawBintang();
void generateNebula();
void drawNebula();
void drawSunGlow();
void drawPlanetAtmosphere(float radius, float r, float g, float b, float alpha);
void generateAsteroid();
void updateAsteroid();
void drawAsteroidBelt(float alpha);
void drawLensFlare();
void drawVignetteOverlay();
bool project3D(float wx, float wy, float wz, float &sx, float &sy);

#endif