#ifndef KAMERA_H
#define KAMERA_H

#include <GL/glut.h>

// Variabel kamera (extern = dideklarasi di sini, didefinisi di kamera.cpp)
extern float camYaw, camPitch, camDist;
extern float targetX, targetY, targetZ;
extern float eyeX, eyeY, eyeZ;
extern bool  mouseDrag;
extern int   mouseLastX, mouseLastY;
extern int   modKamera;
extern bool  kameraIkutRoket;

void hitungEye();
void mouseButton(int btn, int state, int x, int y);
void mouseMotion(int x, int y);

#endif