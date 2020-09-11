//
// Created by m-rap on 02/08/20.
//

#ifndef COMPASS_CANVAS_H
#define COMPASS_CANVAS_H

#ifdef USE_GLES
    #include <GLES/gl.h>
    #include <GLES2/gl2.h>
    #include <GLES2/gl2ext.h>
#elif defined __APPLE__
    #include <OpenGL/gl.h>
#else
    #include <GL/glew.h>
    #ifdef _MSC_VER
        #include <GL/wglew.h>
    #endif
    #include <GL/gl.h>
#endif

#ifdef __ANDROID_API__


#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#else

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define LOGI(...) (printf(__VA_ARGS__))

#endif

typedef struct SVertex2 {
    float x, y, z;
    unsigned char r, g, b, a;
} Vertex2;

typedef struct SDrawable Drawable;

struct SDrawable {
    void* derived;

    GLenum mode;
    Vertex2* vtxBuffer;
    GLushort* idxBuffer;
    GLushort vtxBuffSize;
    GLushort idxBuffSize;
    unsigned char r, g, b, a;
    float x, y;
    float rotation;
    float scale;
    GLuint vbo;
    GLuint ibo;
    uint8_t initialized;
    //float lineWidth;

    Drawable* canvas;
    Drawable* parent;
    Drawable* children[1024];
    int childrenCount;

    
    void (*deinit)(Drawable* obj);
    void (*draw)(Drawable* obj);
};

void createDrawable(Drawable* obj);

void Drawable_init(Drawable* obj, Drawable* parent);
void Drawable_deinit(Drawable* obj);
void Drawable_draw(Drawable* obj);

void Drawable_setColor(Drawable* obj, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void Drawable_addVtx(Drawable* obj, float x, float y);
void Drawable_addTriangle(Drawable* obj, int* idx);

Drawable* Drawable_addchild(Drawable* obj);
Drawable* Drawable_rectfill(Drawable* obj, float x1, float y1, float width, float height);
Drawable* Drawable_circlefill(Drawable* obj, float x1, float y1, float r);
//Drawable* Drawable_rectstroke(Drawable* obj, float x1, float y1, float width, float height);
//Drawable* Drawable_circlestroke(Drawable* obj, float x1, float y1, float r);

void Drawable_end(Drawable* obj);


typedef struct SContainer Container;

typedef struct SCanvas Canvas;

struct SCanvas {
    Drawable base;

    Container* container;

    float xscreen, yscreen, width, height;
    float small, big;
    //float alalay;
};

void createCanvas(Canvas* obj);

void Canvas_init(Canvas* obj);
void Canvas_resize(Canvas* obj, int xscreen, int yscreen, int w, int h);

#endif //COMPASS_CANVAS_H
