#include "tekstur.h"
#include <cstdio>

GLuint loadBMP(const char* fn) {
    FILE* f = fopen(fn, "rb");
    if(!f) { printf("Gagal: %s\n", fn); return 0; }

    unsigned char hdr[54];
    fread(hdr, 1, 54, f);
    if(hdr[0]!='B' || hdr[1]!='M') { fclose(f); return 0; }

    int dp = *(int*)&hdr[0x0A];
    int w  = *(int*)&hdr[0x12];
    int h  = *(int*)&hdr[0x16];
    int sz = *(int*)&hdr[0x22];
    if(!sz) sz = w*h*3;
    if(!dp) dp = 54;

    unsigned char* d = new unsigned char[sz];
    fseek(f, dp, SEEK_SET);
    fread(d, 1, sz, f);
    fclose(f);

    for(int i=0; i<sz; i+=3) {
        unsigned char t = d[i];
        d[i] = d[i+2];
        d[i+2] = t;
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, d);

    delete[] d;
    return id;
}