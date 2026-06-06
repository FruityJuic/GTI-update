#include <GL/glut.h>
#include <cmath>
#include <cstdio>
#include "tekstur.h"
#include "kamera.h"
#include "efek.h"
#include "planet.h"
#include "roket.h"

void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
    glLoadIdentity();

    hitungEye();
    gluLookAt(eyeX,eyeY,eyeZ, targetX,targetY,targetZ, 0,1,0);

    GLfloat lpos[] = {0.0f, 0.0f, 0.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lpos);

    drawNebula();
    drawBintang();

    // Matahari
    float sunA = planetAlpha < 0.3f ? 0.3f : planetAlpha;
    drawSunEmissive(9.0f, texMatahari, sunA);

    // Billboard glow
    glPushMatrix();
        float dx=eyeX, dy=eyeY, dz=eyeZ;
        float dl=sqrt(dx*dx+dy*dy+dz*dz);
        if(dl>0){ dx/=dl; dy/=dl; dz/=dl; }
        float wx=0,wy=1,wz=0;
        if(fabs(dy)>0.98f){ wx=1;wy=0;wz=0; }
        float rx2=wy*dz-wz*dy, ry2=wz*dx-wx*dz, rz2=wx*dy-wy*dx;
        float rl=sqrt(rx2*rx2+ry2*ry2+rz2*rz2);
        if(rl>0){ rx2/=rl; ry2/=rl; rz2/=rl; }
        float ux2=ry2*dz-rz2*dy, uy2=rz2*dx-rx2*dz, uz2=rx2*dy-ry2*dx;
        float bmat[16]={rx2,ry2,rz2,0, ux2,uy2,uz2,0, dx,dy,dz,0, 0,0,0,1};
        glMultMatrixf(bmat);
        drawSunGlow();
    glPopMatrix();

    // Orbit
    drawOrbit(14, planetAlpha);
    drawOrbit(22, planetAlpha);
    drawOrbit(32, planetAlpha);
    drawOrbit(43, planetAlpha);
    drawOrbit(56.5f, planetAlpha*0.7f);

    // Merkurius
    glPushMatrix();
        glRotatef(sudutMerkurius,0,1,0);
        glTranslatef(14,0,0);
        glRotatef(spinMerkurius,0,1,0);
        drawSphereAlpha(1.4f, texMerkurius, 30, 30, 1.0f, planetAlpha);
        drawPlanetAtmosphere(1.4f, 0.9f,0.7f,0.5f, planetAlpha*0.5f);
    glPopMatrix();

    // Venus
    glPushMatrix();
        glRotatef(sudutVenus,0,1,0);
        glTranslatef(22,0,0);
        glRotatef(spinVenus,0,1,0);
        drawSphereAlpha(2.0f, texVenus, 30, 30, 1.0f, planetAlpha);
        drawPlanetAtmosphere(2.0f, 1.0f,0.85f,0.4f, planetAlpha*0.9f);
    glPopMatrix();

    // Bumi + Bulan
    glPushMatrix();
        glRotatef(sudutBumi,0,1,0);
        glTranslatef(32,0,0);
        glRotatef(spinBumi,0,1,0);
        drawSphereAlpha(2.4f, texBumi, 40, 40, 1.0f, planetAlpha);
        drawPlanetAtmosphere(2.4f, 0.3f,0.6f,1.0f, planetAlpha*1.0f);
        drawOrbit(5.5f, planetAlpha);
        glPushMatrix();
            glTranslatef(5.5f*cos(sudutBulan), 0, 5.5f*sin(sudutBulan));
            drawSphereAlpha(0.9f, texBulan, 20, 20, 1.0f, planetAlpha);
            drawPlanetAtmosphere(0.9f, 0.7f,0.7f,0.7f, planetAlpha*0.3f);
        glPopMatrix();
    glPopMatrix();

    // Mars
    glPushMatrix();
        glRotatef(sudutMars,0,1,0);
        glTranslatef(43,0,0);
        glRotatef(spinMars,0,1,0);
        drawSphereAlpha(1.8f, texMars, 30, 30, 1.0f, planetAlpha);
        drawPlanetAtmosphere(1.8f, 1.0f,0.35f,0.1f, planetAlpha*0.6f);
    glPopMatrix();

    // Asteroid belt
    drawAsteroidBelt(planetAlpha);

    // Space Base + Stencil
    glEnable(GL_STENCIL_TEST);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilMask(0xFF);
    drawSpaceBase();
    glDisable(GL_STENCIL_TEST);

    // Roket asli
    drawRoket(false);

    // Bayangan roket
    if(roketAktif && jarakDariPusat < 60.0f) {
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_EQUAL, 1, 0xFF);
        glStencilMask(0x00);

        glPushMatrix();
            float lPos[]    = {0.0f, 0.0f, 0.0f};
            float baseElev[]= {0.0f, -22.95f, 0.0f};
            float baseNorm[]= {0.0f, -1.0f, 0.0f};
            glShadowProjection(lPos, baseElev, baseNorm);
            glDisable(GL_LIGHTING);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            glDisable(GL_CULL_FACE);
            drawRoket(true);
            glEnable(GL_CULL_FACE);
            glDisable(GL_BLEND);
            glEnable(GL_LIGHTING);
        glPopMatrix();

        glDisable(GL_STENCIL_TEST);
    }

    // Efek 2D
    drawLensFlare();
    drawVignetteOverlay();

    // HUD
    glDisable(GL_LIGHTING); glDisable(GL_TEXTURE_2D);
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, winHeight);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();

    glColor3f(0.75f, 0.85f, 1.0f);
    glRasterPos2f(20, 20);
    const char* msg;
    if(!roketAktif)
        msg = "[L]=Launch  [Mouse Drag]=Orbit Kamera  [Scroll/W/S]=Zoom  [R]=Reset";
    else
        msg = "[Mouse Drag]=Look Around  [Scroll/W/S]=Zoom  [C]=Lepas Ikut Roket  [L]=Launch Baru";
    for(const char* c=msg; *c; c++) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);

    if(roketAktif) {
        char buf[80];
        sprintf(buf, "Jarak: %.1f  |  Planet Vis: %.0f%%  |  Gravitasi: %.1f  |  Mode: %s",
                jarakDariPusat, planetAlpha*100.0f, konstantaGravitasiMatahari,
                modKamera==1 ? "Follow Roket" : "Orbit Bebas");
        glColor3f(0.5f, 0.9f, 1.0f);
        glRasterPos2f(20, 38);
        for(const char* c=buf; *c; c++) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);

        const char* bufK = "[1]=Gravitasi Lemah  [2]=Normal  [3]=Kuat";
        glColor3f(0.8f, 0.8f, 0.5f);
        glRasterPos2f(20, 56);
        for(const char* c=bufK; *c; c++) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
    }

    glPopMatrix(); glMatrixMode(GL_PROJECTION); glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
    glutSwapBuffers();
}

void update(int val) {
    globalTime += 0.016f;

    sudutMerkurius += 2.0f;  if(sudutMerkurius>360) sudutMerkurius-=360;
    sudutVenus     += 1.5f;  if(sudutVenus>360)     sudutVenus-=360;
    sudutBumi      += 1.0f;  if(sudutBumi>360)       sudutBumi-=360;
    sudutMars      += 0.7f;  if(sudutMars>360)       sudutMars-=360;
    sudutBulan     += 0.05f;

    spinBumi      += 0.4f;  if(spinBumi>360)      spinBumi-=360;
    spinMars      += 0.35f; if(spinMars>360)      spinMars-=360;
    spinVenus     += 0.1f;  if(spinVenus>360)     spinVenus-=360;
    spinMerkurius += 0.15f; if(spinMerkurius>360) spinMerkurius-=360;

    updateAsteroid();
    updateRoket();
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'w': case 'W': camDist -= 5.0f; break;
        case 's': case 'S': camDist += 5.0f; break;
        case 'r': case 'R':
            camYaw=45; camPitch=30; camDist=100;
            if(!roketAktif){ targetX=0; targetY=0; targetZ=0; }
            break;
        case 'l': case 'L': launchRoket(); break;
        case 'c': case 'C':
            if(roketAktif) {
                if(modKamera==1){ modKamera=0; kameraIkutRoket=false; }
                else            { modKamera=1; kameraIkutRoket=true;  }
            }
            break;
        case '1': konstantaGravitasiMatahari = 20.0f; break;
        case '2': konstantaGravitasiMatahari = 40.0f; break;
        case '3': konstantaGravitasiMatahari = 60.0f; break;
        case '+': konstantaGravitasiMatahari += 1.0f; break;
        case '-': konstantaGravitasiMatahari -= 1.0f; break;
    }
    if(camDist  <   5) camDist  =   5;
    if(camPitch >  89) camPitch =  89;
    if(camPitch < -89) camPitch = -89;
    glutPostRedisplay();
}

void reshape(int w, int h) {
    winWidth = w; winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    gluPerspective(70.0, (GLfloat)w/(GLfloat)h, 0.3, 700.0);
    glMatrixMode(GL_MODELVIEW);
}

void init() {
    glClearColor(0, 0, 0.03f, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glShadeModel(GL_SMOOTH);

    generateBintang();
    generateNebula();
    generateAsteroid();
    initLighting();
    resetPartikel();

    texMatahari  = loadBMP("sun.bmp");
    texMerkurius = loadBMP("mercury.bmp");
    texVenus     = loadBMP("venus.bmp");
    texBumi      = loadBMP("earth.bmp");
    texMars      = loadBMP("mars.bmp");
    texBulan     = loadBMP("moon.bmp");
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH|GLUT_STENCIL);
    glutInitWindowSize(1200, 800);
    glutCreateWindow("Tata Surya");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutTimerFunc(16, update, 0);
    glutMainLoop();
    return 0;
}