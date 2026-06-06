#include "kamera.h"
#include <cmath>

const float PI_K = 3.14159265f;

float camYaw   = 45.0f;
float camPitch = 30.0f;
float camDist  = 100.0f;
float targetX  = 0, targetY = 0, targetZ = 0;
float eyeX     = 0, eyeY    = 0, eyeZ    = 0;
bool  mouseDrag    = false;
int   mouseLastX   = 0, mouseLastY = 0;
int   modKamera    = 0;
bool  kameraIkutRoket = false;

void hitungEye() {
    float radH = camYaw   * PI_K / 180.0f;
    float radV = camPitch * PI_K / 180.0f;
    eyeX = targetX + camDist * cos(radV) * cos(radH);
    eyeY = targetY + camDist * sin(radV);
    eyeZ = targetZ + camDist * cos(radV) * sin(radH);
}

void mouseButton(int btn, int state, int x, int y) {
    if(btn == GLUT_LEFT_BUTTON) {
        mouseDrag  = (state == GLUT_DOWN);
        mouseLastX = x;
        mouseLastY = y;
    }
    if(btn == 3) { camDist -= 3.0f; if(camDist<5) camDist=5; glutPostRedisplay(); }
    if(btn == 4) { camDist += 3.0f; glutPostRedisplay(); }
}

void mouseMotion(int x, int y) {
    if(!mouseDrag) return;
    int dx = x - mouseLastX;
    int dy = y - mouseLastY;
    mouseLastX = x;
    mouseLastY = y;
    camYaw   += dx * 0.4f;
    camPitch -= dy * 0.4f;
    if(camPitch >  89) camPitch =  89;
    if(camPitch < -89) camPitch = -89;
    glutPostRedisplay();
}