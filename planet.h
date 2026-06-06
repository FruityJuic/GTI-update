#ifndef PLANET_H
#define PLANET_H

#include <GL/glut.h>

// Variabel sudut orbit
extern float sudutMerkurius, sudutVenus, sudutBumi, sudutMars, sudutBulan;
// Variabel self-rotation
extern float spinBumi, spinMars, spinVenus, spinMerkurius;
// Variabel fade planet
extern float planetAlpha;
// Tekstur planet
extern GLuint texMatahari, texMerkurius, texVenus, texBumi, texMars, texBulan;

void initLighting();
void drawOrbit(float radius, float alpha);
void drawSphereAlpha(float radius, GLuint tex, int sl, int st, float brightness, float alpha);
void drawSunEmissive(float radius, GLuint tex, float alpha);
void drawSpaceBase();
void glShadowProjection(float l[3], float e[3], float n[3]);

#endif