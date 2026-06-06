#include "planet.h"
#include "efek.h"
#include <cmath>

const float PI_P = 3.14159265f;

// Definisi variabel sudut orbit
float sudutMerkurius = 0.3f, sudutVenus = 0.2f;
float sudutBumi      = 0.1f, sudutMars  = 0.08f;
float sudutBulan     = 0.0f;

// Definisi variabel self-rotation
float spinBumi      = 0.0f, spinMars  = 0.0f;
float spinVenus     = 0.0f, spinMerkurius = 0.0f;

// Variabel fade
float planetAlpha = 1.0f;

// Tekstur
GLuint texMatahari, texMerkurius, texVenus, texBumi, texMars, texBulan;

// ---- SHADOW PROJECTION ----
void glShadowProjection(float l[3], float e[3], float n[3]) {
    float d, c;
    float mat[16];

    d = n[0]*l[0] + n[1]*l[1] + n[2]*l[2];
    c = e[0]*n[0] + e[1]*n[1] + e[2]*n[2] - d;

    mat[0]  = l[0]*n[0]+c; mat[4]  = n[1]*l[0];   mat[8]  = n[2]*l[0];   mat[12] = -l[0]*c-l[0]*d;
    mat[1]  = n[0]*l[1];   mat[5]  = l[1]*n[1]+c; mat[9]  = n[2]*l[1];   mat[13] = -l[1]*c-l[1]*d;
    mat[2]  = n[0]*l[2];   mat[6]  = n[1]*l[2];   mat[10] = l[2]*n[2]+c; mat[14] = -l[2]*c-l[2]*d;
    mat[3]  = n[0];        mat[7]  = n[1];         mat[11] = n[2];        mat[15] = -d;

    glMultMatrixf(mat);
}

// ---- LIGHTING ----
void initLighting() {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    GLfloat diff[]  = {1.0f, 0.96f, 0.85f, 1.0f};
    GLfloat amb[]   = {0.02f, 0.02f, 0.02f, 1.0f};
    GLfloat spec[]  = {1.0f, 1.0f, 0.95f, 1.0f};

    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diff);
    glLightfv(GL_LIGHT0, GL_AMBIENT,  amb);
    glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION,  1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION,    0.005f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0001f);

    GLfloat mspec[]     = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat globalAmb[] = {0.12f, 0.12f, 0.15f, 1.0f};

    glMaterialfv(GL_FRONT, GL_SPECULAR,  mspec);
    glMaterialf (GL_FRONT, GL_SHININESS, 32.0f);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmb);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,     GL_FALSE);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,  GL_TRUE);
}

// ---- ORBIT LINE ----
void drawOrbit(float radius, float alpha) {
    if(alpha <= 0) return;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.5f, 0.55f, 0.7f, alpha*0.45f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
    for(int i=0; i<360; i++) {
        float a = i*2*PI_P/360.0f;
        glVertex3f(cos(a)*radius, 0, sin(a)*radius);
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ---- DRAW SPHERE ALPHA ----
void drawSphereAlpha(float radius, GLuint tex, int sl, int st,
                     float brightness, float alpha) {
    if(alpha <= 0) return;
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    bool blend = (alpha < 1.0f);
    if(blend) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glColor4f(brightness, brightness, brightness, alpha);
    } else {
        glColor3f(brightness, brightness, brightness);
    }

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    gluQuadricNormals(q, GLU_SMOOTH);
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        gluSphere(q, radius, sl, st);
    glPopMatrix();
    gluDeleteQuadric(q);

    if(blend) { glDepthMask(GL_TRUE); glDisable(GL_BLEND); }
    glDisable(GL_TEXTURE_2D);
}

// ---- DRAW SUN EMISSIVE ----
void drawSunEmissive(float radius, GLuint tex, float alpha) {
    if(alpha <= 0) return;
    glDisable(GL_LIGHTING);

    GLfloat emis[] = {1.0f, 0.97f, 0.80f, alpha};
    glMaterialfv(GL_FRONT, GL_EMISSION, emis);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex);

    if(alpha < 1.0f) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glColor4f(1.0f, 0.97f, 0.85f, alpha);
    } else {
        glColor3f(1.0f, 0.97f, 0.85f);
    }

    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    gluQuadricNormals(q, GLU_SMOOTH);
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        gluSphere(q, radius, 64, 64);
    glPopMatrix();
    gluDeleteQuadric(q);

    if(alpha < 1.0f) { glDepthMask(GL_TRUE); glDisable(GL_BLEND); }
    glDisable(GL_TEXTURE_2D);

    GLfloat noEmis[] = {0,0,0,1};
    glMaterialfv(GL_FRONT, GL_EMISSION, noEmis);
}

// ---- SPACE BASE ----
void drawSpaceBase() {
    glEnable(GL_LIGHTING);
    glPushMatrix();
        float radius = 5.0f;
        float tebal  = 2.0f;

        glTranslatef(-20.0f, -23.0f - tebal, -20.0f);
        glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

        GLfloat baseEmis[] = {0.8f, 0.8f, 0.8f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, baseEmis);
        glColor3f(1.0f, 1.0f, 1.0f);

        GLUquadric* q = gluNewQuadric();
        gluQuadricNormals(q, GLU_SMOOTH);

        glPushMatrix();
            glTranslatef(0.0f, 0.0f, tebal);
            gluDisk(q, 0.0f, radius, 32, 1);
        glPopMatrix();

        GLfloat sideEmis[] = {0.2f, 0.2f, 0.2f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, sideEmis);
        glColor3f(0.7f, 0.7f, 0.7f);
        gluCylinder(q, radius, radius, tebal, 32, 1);

        glPushMatrix();
            glRotatef(180.0f, 1.0f, 0.0f, 0.0f);
            gluDisk(q, 0.0f, radius, 32, 1);
        glPopMatrix();

        gluDeleteQuadric(q);

        GLfloat noEmis[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, noEmis);

        glDisable(GL_LIGHTING);
        glPushMatrix();
            glTranslatef(0.0f, 0.0f, tebal + 0.1f);
            glColor3f(0.4f, 0.8f, 1.0f);

            glLineWidth(3.0f);
            glBegin(GL_LINE_LOOP);
            for(int i=0; i<32; i++) {
                float angle = i * 2.0f * PI_P / 32.0f;
                glVertex3f(cos(angle)*radius, sin(angle)*radius, 0.0f);
            }
            glEnd();

            glLineWidth(2.0f);
            glBegin(GL_LINES);
                glVertex3f(-radius, 0.0f, 0.0f); glVertex3f(radius, 0.0f, 0.0f);
                glVertex3f(0.0f, -radius, 0.0f); glVertex3f(0.0f, radius, 0.0f);
            glEnd();
            glLineWidth(1.0f);
        glPopMatrix();
        glEnable(GL_LIGHTING);

    glPopMatrix();
}