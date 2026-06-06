#include "roket.h"
#include "kamera.h"
#include "efek.h"
#include "planet.h"
#include <cmath>
#include <cstdlib>

const float PI_R = 3.14159265f;

float roketX=0, roketY=0, roketZ=0;
float roketDX=0, roketDY=0, roketDZ=0;
bool  roketAktif = false;
float jarakDariPusat = 0.0f;
float konstantaGravitasiMatahari = 40.0f;
bool  efekJarakAktif = false;

const float PLANET_FADE_START = 35.0f, PLANET_FADE_END = 80.0f;
const float FOG_START = 40.0f, FOG_FULL = 90.0f;

// ---- PARTIKEL ----
const int MAX_PARTIKEL = 400;
struct Partikel { float x,y,z,vx,vy,vz,life,maxLife; bool aktif; };
Partikel partikel[MAX_PARTIKEL];

void resetPartikel() {
    for(int i=0; i<MAX_PARTIKEL; i++) partikel[i].aktif = false;
}

void spawnPartikel(float rx, float ry, float rz,
                   float rdx, float rdy, float rdz) {
    int sp = 0;
    for(int i=0; i<MAX_PARTIKEL && sp<5; i++) {
        if(partikel[i].aktif) continue;
        Partikel &p = partikel[i];
        float s = 0.25f;
        p.x = rx - rdx + ((float)rand()/RAND_MAX-0.5f)*s;
        p.y = ry - rdy + ((float)rand()/RAND_MAX-0.5f)*s;
        p.z = rz - rdz + ((float)rand()/RAND_MAX-0.5f)*s;
        float sp2 = 0.06f + (float)rand()/RAND_MAX*0.10f;
        p.vx = -rdx*sp2 + ((float)rand()/RAND_MAX-0.5f)*0.04f;
        p.vy = -rdy*sp2 + ((float)rand()/RAND_MAX-0.5f)*0.04f;
        p.vz = -rdz*sp2 + ((float)rand()/RAND_MAX-0.5f)*0.04f;
        p.maxLife = 0.5f + (float)rand()/RAND_MAX*0.5f;
        p.life    = p.maxLife;
        p.aktif   = true;
        sp++;
    }
}

// ---- TRAIL ----
const int MAX_TRAIL = 100;
float trailX[MAX_TRAIL], trailY[MAX_TRAIL], trailZ[MAX_TRAIL];
int trailLen = 0;

void resetTrail() { trailLen = 0; }

void pushTrail(float x, float y, float z) {
    if(trailLen < MAX_TRAIL) {
        trailX[trailLen] = x;
        trailY[trailLen] = y;
        trailZ[trailLen] = z;
        trailLen++;
    } else {
        for(int i=0; i<MAX_TRAIL-1; i++) {
            trailX[i] = trailX[i+1];
            trailY[i] = trailY[i+1];
            trailZ[i] = trailZ[i+1];
        }
        trailX[MAX_TRAIL-1] = x;
        trailY[MAX_TRAIL-1] = y;
        trailZ[MAX_TRAIL-1] = z;
    }
}

// ---- EFEK JARAK ----
void updateEfekJarak() {
    if(!efekJarakAktif) { fogDensity=0; planetAlpha=1; return; }
    float d = jarakDariPusat;
    fogDensity  = d<FOG_START ? 0 : (d>FOG_FULL ? 1 :
                  (d-FOG_START)/(FOG_FULL-FOG_START));
    planetAlpha = d<PLANET_FADE_START ? 1 : (d>PLANET_FADE_END ? 0 :
                  1-(d-PLANET_FADE_START)/(PLANET_FADE_END-PLANET_FADE_START));
}

// ---- LAUNCH ----
void launchRoket() {
    roketAktif       = true;
    efekJarakAktif   = true;
    modKamera        = 1;
    kameraIkutRoket  = true;

    roketX = -20.0f; roketY = -20.0f; roketZ = -20.0f;
    roketDX = 1.0f;  roketDY = 0.0f;  roketDZ = 0.0f;

    targetX = roketX; targetY = roketY; targetZ = roketZ;

    resetTrail();
    resetPartikel();
}

// ---- UPDATE ----
void updateRoket() {
    if(!roketAktif) return;

    float dirX = -roketX, dirY = -roketY, dirZ = -roketZ;
    float distSq = dirX*dirX + dirY*dirY + dirZ*dirZ;
    jarakDariPusat = sqrt(distSq);

    if(jarakDariPusat < 9.5f) {
        roketAktif=false; kameraIkutRoket=false;
        modKamera=0; efekJarakAktif=false;
        fogDensity=0; planetAlpha=1;
        targetX=0; targetY=0; targetZ=0;
        resetTrail(); resetPartikel();
        return;
    }

    float percepatan = konstantaGravitasiMatahari / distSq;
    if(jarakDariPusat > 0.0001f) {
        dirX /= jarakDariPusat;
        dirY /= jarakDariPusat;
        dirZ /= jarakDariPusat;
    }

    roketDX += dirX * percepatan;
    roketDY += dirY * percepatan;
    roketDZ += dirZ * percepatan;
    roketX  += roketDX;
    roketY  += roketDY;
    roketZ  += roketDZ;

    if(jarakDariPusat > 200.0f) {
        roketAktif=false; kameraIkutRoket=false;
        modKamera=0; efekJarakAktif=false;
        fogDensity=0; planetAlpha=1;
        targetX=0; targetY=0; targetZ=0;
        resetTrail(); resetPartikel();
        return;
    }

    updateEfekJarak();
    pushTrail(roketX, roketY, roketZ);

    float speed = sqrt(roketDX*roketDX + roketDY*roketDY + roketDZ*roketDZ);
    if(speed > 0.0001f)
        spawnPartikel(roketX, roketY, roketZ,
                      roketDX/speed, roketDY/speed, roketDZ/speed);

    targetX += (roketX - targetX) * 0.06f;
    targetY += (roketY - targetY) * 0.06f;
    targetZ += (roketZ - targetZ) * 0.06f;

    for(int i=0; i<MAX_PARTIKEL; i++) {
        if(!partikel[i].aktif) continue;
        partikel[i].x    += partikel[i].vx;
        partikel[i].y    += partikel[i].vy;
        partikel[i].z    += partikel[i].vz;
        partikel[i].life -= 0.016f;
        if(partikel[i].life <= 0) partikel[i].aktif = false;
    }
}

// ---- DRAW ROKET ----
void drawRoket(bool isShadow) {
    if(!roketAktif) return;

    if(!isShadow) {
        // Trail
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glLineWidth(2.0f);
        glBegin(GL_LINE_STRIP);
        for(int i=0; i<trailLen; i++) {
            float t = (float)i / MAX_TRAIL;
            glColor4f(0.5f, 0.8f, 1.0f, t*0.6f);
            glVertex3f(trailX[i], trailY[i], trailZ[i]);
        }
        glEnd();
        glLineWidth(1.0f);

        // Partikel
        glPointSize(3.0f);
        glBegin(GL_POINTS);
        for(int i=0; i<MAX_PARTIKEL; i++) {
            if(!partikel[i].aktif) continue;
            float t = partikel[i].life / partikel[i].maxLife;
            glColor4f(1.0f, 0.5f+t*0.5f, t*0.3f, t*0.8f);
            glVertex3f(partikel[i].x, partikel[i].y, partikel[i].z);
        }
        glEnd();
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }

    glPushMatrix();
        glTranslatef(roketX, roketY, roketZ);

        float ux=0, uy=1, uz=0;
        float rx2 = uy*roketDZ - uz*roketDY;
        float ry2 = uz*roketDX - ux*roketDZ;
        float rz2 = ux*roketDY - uy*roketDX;
        float rl  = sqrt(rx2*rx2+ry2*ry2+rz2*rz2);
        if(rl < 0.001f) { rx2=1; ry2=0; rz2=0; rl=1; }
        rx2/=rl; ry2/=rl; rz2/=rl;

        float ux2 = roketDY*rz2 - roketDZ*ry2;
        float uy2 = roketDZ*rx2 - roketDX*rz2;
        float uz2 = roketDX*ry2 - roketDY*rx2;

        float mat[16] = {
            rx2,ry2,rz2,0,
            ux2,uy2,uz2,0,
            roketDX,roketDY,roketDZ,0,
            0,0,0,1
        };
        glMultMatrixf(mat);

        GLUquadric* q = gluNewQuadric();
        gluQuadricNormals(q, GLU_SMOOTH);

        if(isShadow) glColor4f(0.1f, 0.1f, 0.15f, 0.7f);
        else         glColor3f(0.88f, 0.88f, 0.92f);
        glPushMatrix(); glTranslatef(0,0,-1);
            gluCylinder(q, 0.3f, 0.3f, 2, 12, 3);
        glPopMatrix();

        if(!isShadow) glColor3f(0.9f, 0.15f, 0.1f);
        glPushMatrix(); glTranslatef(0,0,1);
            gluCylinder(q, 0.3f, 0, 1, 12, 3);
        glPopMatrix();

        if(!isShadow) glColor3f(0.55f, 0.55f, 0.65f);
        glPushMatrix(); glTranslatef(0,0,-1);
            gluDisk(q, 0, 0.3f, 12, 2);
        glPopMatrix();

        if(!isShadow) glColor3f(0.25f, 0.4f, 0.8f);
        for(int f=0; f<4; f++) {
            glPushMatrix(); glRotatef(f*90.0f, 0, 0, 1);
            glBegin(GL_TRIANGLES);
                glNormal3f(0,1,0);
                glVertex3f(0.3f,0,-1);
                glVertex3f(0.9f,0,-1.6f);
                glVertex3f(0.3f,0,-0.1f);
            glEnd();
            glPopMatrix();
        }

        if(!isShadow) glColor3f(0.1f, 0.35f, 0.75f);
        glPushMatrix();
            glTranslatef(0, 0.31f, 0.3f);
            glRotatef(90, 1, 0, 0);
            gluDisk(q, 0, 0.15f, 10, 2);
        glPopMatrix();

        if(!isShadow) glColor3f(0.7f, 0.35f, 0.1f);
        glPushMatrix(); glTranslatef(0,0,-1.05f);
            gluCylinder(q, 0.32f, 0.28f, 0.2f, 12, 2);
        glPopMatrix();

        gluDeleteQuadric(q);
    glPopMatrix();
}