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

    
    void (*deinit)(Drawable* this);
    void (*draw)(Drawable* this);
};

void createDrawable(Drawable* this);

void drawableInit(Drawable* this, Drawable* parent);

void setColor(Drawable* this, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
void addVtx(Drawable* this, float x, float y);
void addTriangle(Drawable* this, int* idx);

void _rectvtx(Drawable* this, float x, float y, float width, float height);
void _circlevtx(Drawable* this, float x, float y, float r);

void _rectfill(Drawable* this, float x, float y, float width, float height);
//void _rectstroke(Drawable* this, float x1, float y1, float width, float height);
void _circlefill(Drawable* this, float x1, float y1, float r);
//void _circlestroke(Drawable* this, float x, float y, float r);

Drawable* addchild(Drawable* this);
Drawable* rectfill(Drawable* this, float x1, float y1, float width, float height);
Drawable* circlefill(Drawable* this, float x1, float y1, float r);
//Drawable* rectstroke(Drawable* this, float x1, float y1, float width, float height);
//Drawable* circlestroke(Drawable* this, float x1, float y1, float r);

void end(Drawable* this);

typedef struct SContainer Container;

typedef struct SCanvas Canvas;

struct SCanvas {
    Drawable base;

    Container* container;

    float xscreen, yscreen, width, height;
    float small, big;
    //float alalay;
};

void createCanvas(Canvas* this);

void canvasInit(Canvas* canvas);
void resize(Canvas* this, int xscreen, int yscreen, int w, int h);

#endif //COMPASS_CANVAS_H
