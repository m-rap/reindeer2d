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

struct Vertex2 {
    float x, y, z;
    unsigned char r, g, b, a;
};

struct Drawable {
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

    Drawable();

    void init(Drawable* parent);
    virtual void deinit();
    virtual void draw();

    void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
    void addVtx(float x, float y);
    void addTriangle(int* idx);

    Drawable* addchild();
    Drawable* rectfill(float x1, float y1, float width, float height);
    Drawable* circlefill(float x1, float y1, float r);
    //Drawable* rectstroke(float x1, float y1, float width, float height);
    //Drawable* circlestroke(float x1, float y1, float r);

    void end();
};

struct Container;

struct Canvas : public Drawable {
    Container* container;

    float xscreen, yscreen, width, height;
    float small, big;
    
    Canvas();

    virtual void deinit();
    virtual void draw();

    void init();
    void resize(int xscreen, int yscreen, int w, int h);
};

#endif //COMPASS_CANVAS_H
