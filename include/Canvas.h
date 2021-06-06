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
typedef struct SNode Node;

struct SDrawable {
    GLenum mode;
    Vertex2* vtxBuffer;
    GLushort* idxBuffer;
    GLushort vtxBuffSize;
    GLushort idxBuffSize;
    unsigned char r, g, b, a;
    GLuint vbo;
    GLuint ibo;
    uint8_t initialized;
    //float lineWidth;
    
    Node* owner;
 
    void (*deinit)(Drawable* obj);
    void (*draw)(Drawable* obj);
};

struct SNode {
    void* derived;
    Drawable* drawables[1024];
    Node* parent;
    Node* children[1024];
    uint8_t initialized;
    int drawablesCount;
    int childrenCount;
    unsigned char r, g, b, a;
    float x, y;
    float rotation;
    float scale;

    Node* canvas;

    void (*deinit)(Node* obj);
    void (*draw)(Node* obj);
};

#ifdef __cplusplus
extern "C" {
#endif

void createDrawable(Drawable* obj);

void Drawable_init(Drawable* obj, Node* owner);
void Drawable_deinit(Drawable* obj);
void Drawable_draw(Drawable* obj);

void Drawable_addVtx(Drawable* obj, float x, float y);
void Drawable_addTriangle(Drawable* obj, int* idx);

void createNode(Node* obj);

void Node_init(Node* obj, Node* parent);
void Node_deinit(Node* obj);
void Node_draw(Node* obj);

void Node_setColor(Node* obj, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
Node* Node_addChild(Node* obj);
Drawable* Node_addDrawable(Node* obj);
void Node_rectfill(Node* obj, float x1, float y1, float width, float height);
void Node_circlefill(Node* obj, float x1, float y1, float r);
//Drawable* Drawable_rectstroke(Drawable* obj, float x1, float y1, float width, float height);
//Drawable* Drawable_circlestroke(Drawable* obj, float x1, float y1, float r);
void Node_putText(Node* obj, float x, float y, int alignment);
void Node_end(Node* obj);

void Drawable_end(Drawable* obj);

#ifdef __cplusplus
}
#endif

typedef struct SContainer Container;

typedef struct SCanvas Canvas;

struct SCanvas {
    Node base;

    Container* container;

    float xscreen, yscreen, width, height;
    float small, big;
    //float alalay;
};

#ifdef __cplusplus
extern "C" {
#endif

void createCanvas(Canvas* obj);

void Canvas_init(Canvas* obj);
void Canvas_resize(Canvas* obj, int xscreen, int yscreen, int w, int h);

#ifdef __cplusplus
}
#endif

#endif //COMPASS_CANVAS_H
