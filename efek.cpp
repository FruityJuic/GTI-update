#include "efek.h"
#include "kamera.h"
#include <cmath>
#include <cstdlib>

const float PI_E = 3.14159265f;

float globalTime = 0.0f;
float fogDensity = 0.0f;
int   winWidth   = 1200;
int   winHeight  = 800;

// ---- BINTANG ----
const int JUMLAH_BINTANG = 800;
struct Bintang { float x,y,z,b,twinklePhase,twinkleSpeed; };
Bintang bintang[JUMLAH_BINTANG];

void generateBintang() {
    srand(42);
    for(int i=0; i<JUMLAH_BINTANG; i++) {
        float theta = ((float)rand()/RAND_MAX)*2*PI_E;
        float phi   = ((float)rand()/RAND_MAX)*PI_E;
        float r     = 220 + (float)rand()/RAND_MAX*30;
        bintang[i].x = r*sin(phi)*cos(theta);
        bintang[i].y = r*sin(phi)*sin(theta);
        bintang[i].z = r*cos(phi);
        bintang[i].b = 0.45f + (float)rand()/RAND_MAX*0.55f;
        bintang[i].twinklePhase = (float)rand()/RAND_MAX*2*PI_E;
        bintang[i].twinkleSpeed = 1.5f + (float)rand()/RAND_MAX*3.0f;
    }
}

void drawBintang() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glPointSize(1.8f);
    glBegin(GL_POINTS);
    for(int i=0; i<JUMLAH_BINTANG-60; i++) {
        float twinkle = 0.75f + 0.25f*sin(globalTime*bintang[i].twinkleSpeed + bintang[i].twinklePhase);
        float b = bintang[i].b * twinkle * (1.0f + fogDensity*0.5f);
        if(b>1) b=1;
        float r=b, g=b, bl=b;
        if(i%5==0)      { r=b*0.8f; bl=b; }
        else if(i%7==0) { r=b; g=b*0.9f; bl=b*0.7f; }
        glColor4f(r,g,bl,b);
        glVertex3f(bintang[i].x, bintang[i].y, bintang[i].z);
    }
    glEnd();

    for(int i=JUMLAH_BINTANG-60; i<JUMLAH_BINTANG; i++) {
        float twinkle = 0.7f + 0.3f*sin(globalTime*bintang[i].twinkleSpeed + bintang[i].twinklePhase);
        float b = bintang[i].b * twinkle;
        if(b>1) b=1;

        glPointSize(3.5f);
        glBegin(GL_POINTS);
            glColor4f(b, b*0.95f, b, b);
            glVertex3f(bintang[i].x, bintang[i].y, bintang[i].z);
        glEnd();

        float len = 2.5f;
        float sx=bintang[i].x, sy=bintang[i].y, sz=bintang[i].z;
        float nx=sx/230.0f, ny=sy/230.0f, nz=sz/230.0f;
        float px=-ny-nz, py=nx-nz, pz=nx+ny;
        float qx=ny*nz-nz*ny, qy=nz*nx-nx*nz, qz=nx*ny-ny*nx;
        float pl=sqrt(px*px+py*py+pz*pz); if(pl>0){px/=pl;py/=pl;pz/=pl;}
        float ql=sqrt(qx*qx+qy*qy+qz*qz); if(ql>0){qx/=ql;qy/=ql;qz/=ql;}

        glLineWidth(0.8f);
        glBegin(GL_LINES);
            glColor4f(b,b,b,b*0.6f); glVertex3f(sx+px*len, sy+py*len, sz+pz*len);
            glColor4f(b,b,b,0);      glVertex3f(sx-px*len*0.3f, sy-py*len*0.3f, sz-pz*len*0.3f);
            glColor4f(b,b,b,b*0.6f); glVertex3f(sx-px*len, sy-py*len, sz-pz*len);
            glColor4f(b,b,b,0);      glVertex3f(sx+px*len*0.3f, sy+py*len*0.3f, sz+pz*len*0.3f);
            glColor4f(b,b,b,b*0.6f); glVertex3f(sx+qx*len, sy+qy*len, sz+qz*len);
            glColor4f(b,b,b,0);      glVertex3f(sx-qx*len*0.3f, sy-qy*len*0.3f, sz-qz*len*0.3f);
            glColor4f(b,b,b,b*0.6f); glVertex3f(sx-qx*len, sy-qy*len, sz-qz*len);
            glColor4f(b,b,b,0);      glVertex3f(sx+qx*len*0.3f, sy+qy*len*0.3f, sz+qz*len*0.3f);
        glEnd();
        glLineWidth(1.0f);
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ---- NEBULA ----
const int NEBULA_POIN = 1500;
struct NebulaPoint { float x,y,z,r,g,b,a; };
NebulaPoint nebulaCloud[NEBULA_POIN];

void generateNebula() {
    srand(99);
    for(int i=0; i<500; i++) {
        float t=(float)rand()/RAND_MAX, u=(float)rand()/RAND_MAX, v=(float)rand()/RAND_MAX;
        float r=190+(float)rand()/RAND_MAX*30;
        float theta=-0.5f+t*1.0f, phi=0.3f+u*1.0f;
        nebulaCloud[i].x=r*sin(phi)*cos(theta);
        nebulaCloud[i].y=r*sin(phi)*sin(theta)*0.5f;
        nebulaCloud[i].z=r*cos(phi);
        nebulaCloud[i].r=0.5f+v*0.5f;
        nebulaCloud[i].g=0.05f+(float)rand()/RAND_MAX*0.2f;
        nebulaCloud[i].b=0.6f+(float)rand()/RAND_MAX*0.4f;
        nebulaCloud[i].a=0.04f+(float)rand()/RAND_MAX*0.08f;
    }
    for(int i=500; i<1000; i++) {
        float t=(float)rand()/RAND_MAX, u=(float)rand()/RAND_MAX;
        float r=195+(float)rand()/RAND_MAX*25;
        float theta=2.5f+t*1.5f, phi=1.5f+u*1.2f;
        nebulaCloud[i].x=r*sin(phi)*cos(theta);
        nebulaCloud[i].y=r*sin(phi)*sin(theta)*0.4f;
        nebulaCloud[i].z=r*cos(phi);
        nebulaCloud[i].r=0.05f+(float)rand()/RAND_MAX*0.15f;
        nebulaCloud[i].g=0.4f+(float)rand()/RAND_MAX*0.4f;
        nebulaCloud[i].b=0.8f+(float)rand()/RAND_MAX*0.2f;
        nebulaCloud[i].a=0.04f+(float)rand()/RAND_MAX*0.07f;
    }
    for(int i=1000; i<NEBULA_POIN; i++) {
        float t=(float)rand()/RAND_MAX, u=(float)rand()/RAND_MAX;
        float r=185+(float)rand()/RAND_MAX*35;
        float theta=PI_E+t*0.8f, phi=0.8f+u*0.6f;
        nebulaCloud[i].x=r*sin(phi)*cos(theta);
        nebulaCloud[i].y=(r*sin(phi)*sin(theta))*0.3f;
        nebulaCloud[i].z=r*cos(phi);
        nebulaCloud[i].r=0.6f+(float)rand()/RAND_MAX*0.4f;
        nebulaCloud[i].g=0.7f+(float)rand()/RAND_MAX*0.3f;
        nebulaCloud[i].b=0.1f+(float)rand()/RAND_MAX*0.2f;
        nebulaCloud[i].a=0.03f+(float)rand()/RAND_MAX*0.06f;
    }
}

void drawNebula() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glPointSize(4.5f);
    glBegin(GL_POINTS);
    for(int i=0; i<NEBULA_POIN; i++) {
        glColor4f(nebulaCloud[i].r, nebulaCloud[i].g,
                  nebulaCloud[i].b, nebulaCloud[i].a);
        glVertex3f(nebulaCloud[i].x, nebulaCloud[i].y, nebulaCloud[i].z);
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ---- SUN GLOW ----
void drawSunGlow() {
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    float pulse  = 0.88f + 0.12f*sin(globalTime*1.1f);
    float pulse2 = 0.80f + 0.20f*sin(globalTime*0.7f + 1.0f);

    struct CoronaLayer { float r, aCenter, cr, cg, cb; };
    CoronaLayer layers[] = {
        {  9.8f, 0.90f, 1.00f, 1.00f, 0.95f },
        { 12.5f, 0.60f, 1.00f, 0.95f, 0.70f },
        { 16.0f, 0.35f, 1.00f, 0.80f, 0.30f },
        { 21.0f, 0.18f, 1.00f, 0.55f, 0.10f },
        { 28.0f, 0.08f, 0.90f, 0.30f, 0.05f },
        { 38.0f, 0.03f, 0.70f, 0.15f, 0.02f },
    };
    int steps = 80;
    for(int li=0; li<6; li++) {
        float R   = layers[li].r * pulse;
        float aIn = layers[li].aCenter * pulse;
        glBegin(GL_TRIANGLE_FAN);
            glColor4f(layers[li].cr, layers[li].cg, layers[li].cb, aIn);
            glVertex3f(0,0,0);
            for(int i=0; i<=steps; i++) {
                float a = i*2*PI_E/steps;
                glColor4f(layers[li].cr, layers[li].cg, layers[li].cb, 0);
                glVertex3f(cos(a)*R, sin(a)*R, 0);
            }
        glEnd();
    }

    for(int ri=0; ri<12; ri++) {
        float baseAngle = ri*2*PI_E/12 + globalTime*0.05f;
        float rayLen    = 35.0f + 8.0f*sin(globalTime*0.6f + ri*0.8f);
        float rayW      = 0.012f*PI_E;
        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.85f, 0.4f, 0.22f*pulse);
            glVertex3f(cos(baseAngle)*9.5f, sin(baseAngle)*9.5f, 0);
            glColor4f(1.0f, 0.7f, 0.2f, 0.0f);
            glVertex3f(cos(baseAngle+rayW)*rayLen, sin(baseAngle+rayW)*rayLen, 0);
            glVertex3f(cos(baseAngle-rayW)*rayLen, sin(baseAngle-rayW)*rayLen, 0);
        glEnd();
    }

    for(int ri=0; ri<20; ri++) {
        float baseAngle = ri*2*PI_E/20 + globalTime*0.13f + PI_E/20;
        float rayLen    = 18.0f + 4.0f*sin(globalTime*1.1f + ri);
        float rayW      = 0.008f*PI_E;
        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.9f, 0.6f, 0.15f*pulse2);
            glVertex3f(cos(baseAngle)*9.0f, sin(baseAngle)*9.0f, 0);
            glColor4f(1.0f, 0.8f, 0.3f, 0.0f);
            glVertex3f(cos(baseAngle+rayW)*rayLen, sin(baseAngle+rayW)*rayLen, 0);
            glVertex3f(cos(baseAngle-rayW)*rayLen, sin(baseAngle-rayW)*rayLen, 0);
        glEnd();
    }

    for(int fi=0; fi<3; fi++) {
        float fAngle = fi*2*PI_E/3 + globalTime*0.2f;
        float fPulse = 0.5f + 0.5f*sin(globalTime*2.5f + fi*1.3f);
        float fLen   = 14.0f + 5.0f*fPulse;
        float fW     = 0.02f*PI_E;
        glBegin(GL_TRIANGLES);
            glColor4f(1.0f, 0.95f, 0.7f, 0.35f*fPulse);
            glVertex3f(cos(fAngle)*9.2f, sin(fAngle)*9.2f, 0);
            glColor4f(1.0f, 0.6f, 0.1f, 0.0f);
            glVertex3f(cos(fAngle+fW)*fLen, sin(fAngle+fW)*fLen, 0);
            glVertex3f(cos(fAngle-fW)*fLen, sin(fAngle-fW)*fLen, 0);
        glEnd();
    }

    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ---- ATMOSPHERE ----
void drawPlanetAtmosphere(float radius, float r, float g, float b, float alpha) {
    if(alpha <= 0.01f) return;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);

    float pulse = 0.92f + 0.08f*sin(globalTime*0.5f);

    GLUquadric* q1 = gluNewQuadric();
    gluQuadricNormals(q1, GLU_SMOOTH);
    glColor4f(r, g, b, 0.10f * alpha * pulse);
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        gluSphere(q1, radius * 1.08f, 32, 32);
    glPopMatrix();
    gluDeleteQuadric(q1);

    GLUquadric* q2 = gluNewQuadric();
    gluQuadricNormals(q2, GLU_SMOOTH);
    glColor4f(r, g, b, 0.055f * alpha * pulse);
    glPushMatrix();
        glRotatef(-90, 1, 0, 0);
        gluSphere(q2, radius * 1.22f, 24, 24);
    glPopMatrix();
    gluDeleteQuadric(q2);

    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ---- ASTEROID ----
const int ASTEROID_COUNT = 1200;
struct Asteroid { float ang, r, y, size, speed; };
Asteroid asteroids[ASTEROID_COUNT];

void generateAsteroid() {
    srand(77);
    for(int i=0; i<ASTEROID_COUNT; i++) {
        asteroids[i].ang   = ((float)rand()/RAND_MAX)*2*PI_E;
        asteroids[i].r     = 52.0f + ((float)rand()/RAND_MAX)*9.0f;
        asteroids[i].y     = ((float)rand()/RAND_MAX-0.5f)*2.5f;
        asteroids[i].size  = 0.05f + (float)rand()/RAND_MAX*0.25f;
        asteroids[i].speed = 0.00015f + (float)rand()/RAND_MAX*0.0002f;
    }
}

void updateAsteroid() {
    for(int i=0; i<ASTEROID_COUNT; i++) {
        asteroids[i].ang += asteroids[i].speed;
        if(asteroids[i].ang > 2*PI_E) asteroids[i].ang -= 2*PI_E;
    }
}

void drawAsteroidBelt(float alpha) {
    if(alpha <= 0) return;
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glPointSize(1.8f);
    glBegin(GL_POINTS);
    for(int i=0; i<ASTEROID_COUNT; i++) {
        float x = cos(asteroids[i].ang)*asteroids[i].r;
        float z = sin(asteroids[i].ang)*asteroids[i].r;
        float shade = 0.5f + asteroids[i].size*1.5f;
        if(shade > 0.9f) shade = 0.9f;
        glColor4f(shade*0.8f, shade*0.75f, shade*0.65f, alpha*0.7f);
        glVertex3f(x, asteroids[i].y, z);
    }
    glEnd();
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

// ---- LENS FLARE ----
bool project3D(float wx, float wy, float wz, float &sx, float &sy) {
    GLdouble mv[16], proj[16];
    GLint vp[4];
    glGetDoublev(GL_MODELVIEW_MATRIX, mv);
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    glGetIntegerv(GL_VIEWPORT, vp);
    GLdouble ox, oy, oz;
    if(!gluProject(wx, wy, wz, mv, proj, vp, &ox, &oy, &oz)) return false;
    if(oz<0 || oz>1) return false;
    sx = (float)ox;
    sy = (float)oy;
    return true;
}

void drawLensFlare() {
    float sx, sy;
    if(!project3D(0,0,0,sx,sy)) return;
    if(sx<-50 || sx>winWidth+50 || sy<-50 || sy>winHeight+50) return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, winHeight);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    float cx = winWidth*0.5f, cy = winHeight*0.5f;
    float distFromCenter = sqrt((sx-cx)*(sx-cx)+(sy-cy)*(sy-cy));
    float maxDist = sqrt(cx*cx+cy*cy);
    float intensity = 1.0f - distFromCenter/maxDist;
    if(intensity < 0) intensity = 0;

    float vx = cx-sx, vy = cy-sy;
    float pulse = 0.8f + 0.2f*sin(globalTime*1.5f);

    struct FlareElem { float t, r, ri, a; int type; float cr,cg,cb; };
    FlareElem elems[] = {
        {0.0f,  65, 0,  0.35f*pulse, 0, 1.0f,0.97f,0.7f},
        {0.0f,  30, 0,  0.50f*pulse, 0, 1.0f,0.95f,0.6f},
        {0.0f,  12, 0,  0.70f*pulse, 0, 1.0f,1.0f, 0.9f},
        {0.25f, 20, 15, 0.25f,       2, 0.5f,0.7f, 1.0f},
        {0.40f, 14, 0,  0.22f,       1, 0.8f,0.5f, 1.0f},
        {0.55f,  8, 0,  0.30f,       0, 0.6f,0.8f, 1.0f},
        {0.65f, 18, 0,  0.15f,       0, 1.0f,0.6f, 0.3f},
        {0.75f,  6, 0,  0.35f,       0, 0.9f,0.9f, 1.0f},
        {0.88f, 22, 0,  0.12f,       1, 0.4f,0.6f, 1.0f},
        {1.05f, 10, 0,  0.25f,       0, 0.7f,0.5f, 1.0f},
        {1.20f,  5, 0,  0.40f,       0, 1.0f,1.0f, 1.0f},
    };

    for(int i=0; i<11; i++) {
        float px = sx + vx*elems[i].t;
        float py = sy + vy*elems[i].t;
        float a  = elems[i].a * intensity;
        float R  = elems[i].r;
        float cr=elems[i].cr, cg=elems[i].cg, cb=elems[i].cb;

        if(elems[i].type == 0) {
            glBegin(GL_TRIANGLE_FAN);
                glColor4f(cr,cg,cb,a);
                glVertex2f(px,py);
                for(int j=0; j<=48; j++) {
                    float ang = j*2*PI_E/48;
                    glColor4f(cr,cg,cb,0);
                    glVertex2f(px+cos(ang)*R, py+sin(ang)*R);
                }
            glEnd();
        } else if(elems[i].type == 1) {
            glBegin(GL_TRIANGLE_FAN);
                glColor4f(cr,cg,cb,a);
                glVertex2f(px,py);
                for(int j=0; j<=6; j++) {
                    float ang = j*2*PI_E/6 + globalTime*0.3f;
                    glColor4f(cr,cg,cb,0);
                    glVertex2f(px+cos(ang)*R, py+sin(ang)*R);
                }
            glEnd();
        } else if(elems[i].type == 2) {
            float Ri = elems[i].ri;
            glBegin(GL_QUAD_STRIP);
            for(int j=0; j<=48; j++) {
                float ang = j*2*PI_E/48;
                glColor4f(cr,cg,cb,a); glVertex2f(px+cos(ang)*Ri, py+sin(ang)*Ri);
                glColor4f(cr,cg,cb,0); glVertex2f(px+cos(ang)*R,  py+sin(ang)*R);
            }
            glEnd();
        }
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

// ---- VIGNETTE ----
void drawVignetteOverlay() {
    if(fogDensity <= 0) return;
    glMatrixMode(GL_PROJECTION); glPushMatrix(); glLoadIdentity();
    gluOrtho2D(0, winWidth, 0, winHeight);
    glMatrixMode(GL_MODELVIEW); glPushMatrix(); glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    float a  = fogDensity*0.55f;
    float va = fogDensity*0.75f;
    float cx = winWidth*0.5f, cy = winHeight*0.5f;

    glColor4f(0,0,0.05f,a);
    glBegin(GL_QUADS);
        glVertex2f(0,0); glVertex2f(winWidth,0);
        glVertex2f(winWidth,winHeight); glVertex2f(0,winHeight);
    glEnd();

    glBegin(GL_QUADS);
        glColor4f(0,0,0,va); glVertex2f(0,0); glVertex2f(winWidth,0);
        glColor4f(0,0,0,0);  glVertex2f(winWidth,cy); glVertex2f(0,cy);
    glEnd();
    glBegin(GL_QUADS);
        glColor4f(0,0,0,0);  glVertex2f(0,cy); glVertex2f(winWidth,cy);
        glColor4f(0,0,0,va); glVertex2f(winWidth,winHeight); glVertex2f(0,winHeight);
    glEnd();
    glBegin(GL_QUADS);
        glColor4f(0,0,0,va); glVertex2f(0,0);
        glColor4f(0,0,0,0);  glVertex2f(cx,0);
        glColor4f(0,0,0,0);  glVertex2f(cx,winHeight);
        glColor4f(0,0,0,va); glVertex2f(0,winHeight);
    glEnd();
    glBegin(GL_QUADS);
        glColor4f(0,0,0,0);  glVertex2f(cx,0);
        glColor4f(0,0,0,va); glVertex2f(winWidth,0);
        glColor4f(0,0,0,va); glVertex2f(winWidth,winHeight);
        glColor4f(0,0,0,0);  glVertex2f(cx,winHeight);
    glEnd();

    if(fogDensity > 0.25f) {
        float ta = (fogDensity-0.25f)/0.75f;
        glColor4f(0.4f,0.75f,1.0f,ta*0.9f);
        glRasterPos2f(winWidth/2.0f-130, winHeight-55);
        const char* w = ">> MENINGGALKAN TATA SURYA <<";
        for(const char* c=w; *c; c++) glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *c);

        float bw = 220.0f*fogDensity;
        glColor4f(0.15f,0.5f,1.0f,ta*0.65f);
        glBegin(GL_QUADS);
            glVertex2f(winWidth/2.0f-110, winHeight-75);
            glVertex2f(winWidth/2.0f-110+bw, winHeight-75);
            glVertex2f(winWidth/2.0f-110+bw, winHeight-65);
            glVertex2f(winWidth/2.0f-110, winHeight-65);
        glEnd();
        glColor4f(0.4f,0.75f,1.0f,ta*0.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(winWidth/2.0f-110,winHeight-75);
            glVertex2f(winWidth/2.0f+110,winHeight-75);
            glVertex2f(winWidth/2.0f+110,winHeight-65);
            glVertex2f(winWidth/2.0f-110,winHeight-65);
        glEnd();
    }

    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}