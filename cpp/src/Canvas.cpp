//
// Created by m-rap on 02/08/20.
//

#include "Canvas.h"
#include "Container.h"
#include <math.h>
#include <stdlib.h>


Drawable* drawablePool;
int takenDrawableCount = 0;
GLushort nSegments = 200;


//void _rectvtx(Drawable* this, float x, float y, float width, float height);
//void _circlevtx(Drawable* this, float x, float y, float r);

//void _rectfill(Drawable* this, float x, float y, float width, float height);
////void _rectstroke(Drawable* this, float x1, float y1, float width, float height);
//void _circlefill(Drawable* this, float x1, float y1, float r);
////void _circlestroke(Drawable* this, float x, float y, float r);


Drawable::Drawable() {
}

void Drawable::deinit() {
    //LOGI("drawable deinit %08x %d %d", this, initialized, childrenCount);
    if (this->initialized) {
        this->initialized = 0;

        if (this->vtxBuffer != NULL) {
            free(this->vtxBuffer);
        }
        if (this->idxBuffer != NULL) {
            free(this->idxBuffer);
        }
        glDeleteBuffers(1, &this->vbo);
        glDeleteBuffers(1, &this->ibo);

        for (int i = 0; i < this->childrenCount; i++) {
            this->children[i]->deinit();
        }
        this->childrenCount = 0;
    }
}

void Drawable::init(Drawable* parent1) {
    //LOGI("drawable init");
    this->initialized = 1;

    this->childrenCount = 0;

    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ibo);

    this->vtxBuffSize = 0;
    this->idxBuffSize = 0;
    this->x = 0;
    this->y = 0;
    this->rotation = 0;
    this->scale = 1;
    //this->lineWidth = 1;
    this->parent = parent1;

    Drawable* tmp = this;
    while (tmp != NULL) {
        if (tmp->parent == NULL) {
            this->canvas = tmp;
            break;
        }
        tmp = tmp->parent;
    }
    this->setColor(this->canvas->r, this->canvas->g, this->canvas->b, this->canvas->a);
    //this->lineWidth = this->canvas->lineWidth;
}

void Drawable::draw() {
    //LOGI("inside Drawable::draw %x %d", this, this->childrenCount);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(this->x, this->y, 0);
    glRotatef(this->rotation, 0, 0, 1);
    glScalef(this->scale, this->scale, this->scale);

    if (this->idxBuffSize > 0) {
        //LOGI("drawing buffer %d", this->vbo);

        //glLineWidth(lineWidth);
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(Vertex2), (void*)offsetof(Vertex2, x));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex2), (void*)offsetof(Vertex2, r));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
        glDrawElements(this->mode, this->idxBuffSize, GL_UNSIGNED_SHORT, 0);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    for (int i = 0; i < this->childrenCount; i++) {
        //LOGI("children %x draw", this->children[i]);
        this->children[i]->draw();
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void Drawable::setColor(unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1) {
    //LOGI("Drawable %x color: %d,%d,%d,%d", this, r1, g1, b1, a1);
    this->r = r1;
    this->g = g1;
    this->b = b1;
    this->a = a1;
}

void Drawable::addVtx(float x, float y) {
    Vertex2 tmp = {
        x,
        y,
        0.0f,
        this->r,
        this->g,
        this->b,
        this->a
    };
    this->vtxBuffer[this->vtxBuffSize++] = tmp;

    //int n = this->vtxBuffSize - 1;
    //Vertex2* v = &this->vtxBuffer[n];
    //LOGI("%f %f %f %d %d %d %d", v->x, v->y, v->z, v->r, v->g, v->b, v->a);
}

void Drawable::addTriangle(int* idx) {
    if (this->idxBuffSize > 1024 - 3) {
        return;
    }
    this->idxBuffer[this->idxBuffSize++] = idx[0];
    this->idxBuffer[this->idxBuffSize++] = idx[1];
    this->idxBuffer[this->idxBuffSize++] = idx[2];
}

void _rectvtx(Drawable* that, float x1, float y1, float width, float height) {
    //LOGI("_rectvtx");
    that->x = x1;
    that->y = y1;

    that->vtxBuffer = (Vertex2*)malloc(sizeof(Vertex2) * 4);

    that->addVtx(-width / 2, -height / 2);
    that->addVtx(width / 2, -height / 2);
    that->addVtx(width / 2, height / 2);
    that->addVtx(-width / 2, height / 2);
}

void _circlevtx(Drawable* that, float x1, float y1, float radius) {
    //LOGI("_circlevtx");
    that->x = x1;
    that->y = y1;

    that->vtxBuffer = (Vertex2*)malloc(sizeof(Vertex2) * (nSegments + 1));

    that->addVtx(0, 0);
    for (int i = 0; i < nSegments; i++) {
        double degree = (double)(i * 360) / nSegments;
        double radians = degree * M_PI / 180;
        that->addVtx((float)(cos(radians) * radius), (float)(sin(radians) * radius));
    }
}

void _rectfill(Drawable* that, float x1, float y1, float width, float height) {
    _rectvtx(that, x1, y1, width, height);

    that->mode = GL_TRIANGLES;

    that->idxBuffer = (GLushort*)malloc(sizeof(GLushort) * 6);

    int idx1[] = {that->vtxBuffSize - (GLushort)4, that->vtxBuffSize - (GLushort)3, that->vtxBuffSize - (GLushort)2};
    that->addTriangle(idx1);

    int idx2[] = {that->vtxBuffSize - (GLushort)4, that->vtxBuffSize - (GLushort)2, that->vtxBuffSize - (GLushort)1};
    that->addTriangle(idx2);
    //LOGI("_rectfill %d", that->idxBuffSize);
}

void _circlefill(Drawable* that, float x1, float y1, float radius) {
    _circlevtx(that, x1, y1, radius);

    that->mode = GL_TRIANGLE_FAN;

    that->idxBuffer = (GLushort*)malloc(sizeof(GLushort) * (nSegments + 1));

    GLushort idxStart = that->vtxBuffSize - nSegments - 1;
    //char idxStr[2000] = "";
    for (GLushort i = 0; i < nSegments; i++) {
        that->idxBuffer[that->idxBuffSize++] = idxStart + i;
        //sprintf(idxStr, "%s %d", idxStr, that->idxBuffer[that->idxBuffSize - 1]);
    }
    that->idxBuffer[that->idxBuffSize++] = idxStart + 1;
    //sprintf(idxStr, "%s %d", idxStr, that->idxBuffer[that->idxBuffSize - 1]);
    //LOGI("%s", idxStr);
    //LOGI("_circlefill %d", that->idxBuffSize);
}

//void Drawable::_rectstroke(float x1, float y1, float width, float height) {
//    _rectvtx(x1, y1, width, height);
//
//    mode = GL_LINE_LOOP;
//
//    idxBuffer = (GLushort*)malloc(sizeof(GLushort) * 4);
//
//    idxBuffer[idxBuffSize++] = vtxBuffSize - 4;
//    idxBuffer[idxBuffSize++] = vtxBuffSize - 3;
//    idxBuffer[idxBuffSize++] = vtxBuffSize - 2;
//    idxBuffer[idxBuffSize++] = vtxBuffSize - 1;
//}
//
//void Drawable::_circlestroke(float x, float y, float radius) {
//    //LOGI("_circlestroke");
//    _circlevtx(x, y, radius);
//
//    mode = GL_LINE_LOOP;
//
//    idxBuffer = (GLushort*)malloc(sizeof(GLushort) * nSegments);
//
//    int idxStart = vtxBuffSize - nSegments - 1;
//    for (GLushort i = 0; i < nSegments - 1; i++) {
//        idxBuffer[idxBuffSize++] = idxStart + i;
//    }
//}

Drawable* Drawable::addchild() {
    if (takenDrawableCount >= 1024) {
        return NULL;
    }
    Drawable* d = this->children[this->childrenCount++] = &drawablePool[takenDrawableCount++];
    d->init(this);
    return d;
}

Drawable* Drawable::rectfill(float x1, float y1, float width, float height) {
    Drawable* d = addchild();
    _rectfill(d, x1, y1, width, height);
    return d;
}

Drawable* Drawable::circlefill(float x1, float y1, float radius) {
    Drawable* d = addchild();
    _circlefill(d, x1, y1, radius);
    return d;
}

//Drawable* Drawable::rectstroke(float x1, float y1, float width, float height) {
//    Drawable* d = fds->addchild();
//    d->_rectstroke(x1, y1, width, height);
//    return d;
//}
//
//Drawable* Drawable::circlestroke(float x1, float y1, float radius) {
//    Drawable* d = fds->addchild();
//    d->_circlestroke(x1, y1, radius);
//    return d;
//}

void Drawable::end() {
    //LOGI("taken %d", takenDrawableCount);
    LOGI("bufferdata %d %d %d %d", this->vbo, this->ibo, this->vtxBuffSize, this->idxBuffSize);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, this->vtxBuffSize * sizeof(Vertex2), this->vtxBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->idxBuffSize * sizeof(GLushort), this->idxBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    for (int i = 0; i < this->childrenCount; i++) {
        this->children[i]->end();
    }
}

void Canvas::init() {
    //drawablePool = (Drawable*)malloc(sizeof(Drawable) * 1024);
    drawablePool = new Drawable[1024];

    LOGI("canvas init");
    Drawable::init(NULL);
    takenDrawableCount = 0;

    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    //glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
}

void Canvas::deinit() {
    Drawable::deinit();
    takenDrawableCount = 0;

    //free(drawablePool);
    delete[] drawablePool;
}

void Canvas::draw() {
    //LOGI("inside _Canvas_draw %x", this);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);

    Drawable::draw();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

Canvas::Canvas() : Drawable() {
}

void Canvas::resize(int xscreen1, int yscreen1, int w, int h) {
    //LOGI("layout %f %f %f %f", xscreen, yscreen, width, height);
    //LOGI("viewport %f %f %f %f", xscreen, yscreen + big / 2 - small / 2, small, small);
    if (this->xscreen == (float)xscreen1 &&
        this->yscreen == (float)yscreen1 &&
        this->width   == (float)w &&
        this->height  == (float)h) {
        //LOGI("layout same");
        return;
    }

    this->xscreen = xscreen1;
    this->yscreen = yscreen1;
    this->width = w;
    this->height = h;
    this->small = this->width;
    this->big = this->height;
    if (this->height < this->width) {
        this->small = this->height;
        this->big = this->width;
    }
    //glViewport(0, big/4 - (container->height - big) - (0.005 * small), small, small);
    //glViewport(0, big/4 - (container->height - big), small, small);
    //glViewport(0, big/4 - (container->height - big) - (0.023 * small), small, small);
    //glViewport(0, big/4, small, small);
    //glViewport(0, alalay, small, small);
    //glViewport(0, big - small, small, small);
    //glViewport(0, container->height / 4 + container->s1px * (container->height - big), small, small);
    //glViewport(0, container->height / 4, small, small);
    //glViewport(xscreen, yscreen + big / 2 - small / 2, small, small);
    glViewport(this->xscreen, this->big / 2 - this->small / 2 + (this->container->height - this->big - this->yscreen), this->small, this->small);
    //alalay += 0.1;

    LOGI("layout changed %f %f %f %f", this->xscreen, this->big / 2 - this->small / 2 + (this->container->height - this->big - this->yscreen), this->small, this->small);
}