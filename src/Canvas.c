//
// Created by m-rap on 02/08/20.
//

#include "Canvas.h"
#include <math.h>
#include "Container.h"


Drawable* drawablePool;
int takenDrawableCount = 0;
GLushort nSegments = 200;


void Drawable_deinit(Drawable* this) {
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
            this->children[i]->deinit(this->children[i]);
        }
        this->childrenCount = 0;
    }
}

void drawableInit(Drawable* this, Drawable* parent1) {
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
    setColor(this, this->canvas->r, this->canvas->g, this->canvas->b, this->canvas->a);
    //this->lineWidth = this->canvas->lineWidth;
}

void Drawable_draw(Drawable* this) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(this->x, this->y, 0);
    glRotatef(this->rotation, 0, 0, 1);
    glScalef(this->scale, this->scale, this->scale);

    if (this->idxBuffSize > 0) {
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
        this->draw(this->children[i]);
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void createDrawable(Drawable* this) {
    if (this == NULL)
        return;
    
    memset(this, 0, sizeof(Drawable));
    //this->init = Drawable_init;
    this->deinit = Drawable_deinit;
    this->draw = Drawable_draw;
}

void setColor(Drawable* this, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1) {
    this->r = r1;
    this->g = g1;
    this->b = b1;
    this->a = a1;
}

void addVtx(Drawable* this, float x, float y) {
    Vertex2 tmp = {
        .x = x,
        .y = y,
        .r = this->r,
        .g = this->g,
        .b = this->b,
        .a = this->a
    };
    this->vtxBuffer[this->vtxBuffSize++] = tmp;
}

void addTriangle(Drawable* this, int* idx) {
    if (this->idxBuffSize > 1024 - 3) {
        return;
    }
    this->idxBuffer[this->idxBuffSize++] = idx[0];
    this->idxBuffer[this->idxBuffSize++] = idx[1];
    this->idxBuffer[this->idxBuffSize++] = idx[2];
}

void _rectvtx(Drawable* this, float x1, float y1, float width, float height) {
    //LOGI("_rectvtx");
    this->x = x1;
    this->y = y1;

    this->vtxBuffer = (Vertex2*)malloc(sizeof(Vertex2) * 4);

    addVtx(this, -width / 2, -height / 2);
    addVtx(this, width / 2, -height / 2);
    addVtx(this, width / 2, height / 2);
    addVtx(this, -width / 2, height / 2);
}

void _circlevtx(Drawable* this, float x1, float y1, float radius) {
    //LOGI("_circlevtx");
    this->x = x1;
    this->y = y1;

    this->vtxBuffer = (Vertex2*)malloc(sizeof(Vertex2) * (nSegments + 1));

    addVtx(this, 0, 0);
    for (int i = 0; i < nSegments; i++) {
        double degree = (double)(i * 360) / nSegments;
        double radians = degree * M_PI / 180;
        addVtx(this, (float)(cos(radians) * radius), (float)(sin(radians) * radius));
    }
}

void _rectfill(Drawable* this, float x1, float y1, float width, float height) {
    _rectvtx(this, x1, y1, width, height);

    this->mode = GL_TRIANGLES;

    this->idxBuffer = (GLushort*)malloc(sizeof(GLushort) * 6);

    int idx1[] = {this->vtxBuffSize - (GLushort)4, this->vtxBuffSize - (GLushort)3, this->vtxBuffSize - (GLushort)2};
    addTriangle(this, idx1);

    int idx2[] = {this->vtxBuffSize - (GLushort)4, this->vtxBuffSize - (GLushort)2, this->vtxBuffSize - (GLushort)1};
    addTriangle(this, idx2);
    //LOGI("_rectfill %d", this->idxBuffSize);
}

void _circlefill(Drawable* this, float x1, float y1, float radius) {
    _circlevtx(this, x1, y1, radius);

    this->mode = GL_TRIANGLE_FAN;

    this->idxBuffer = (GLushort*)malloc(sizeof(GLushort) * (nSegments + 1));

    GLushort idxStart = this->vtxBuffSize - nSegments - 1;
    for (GLushort i = 0; i < nSegments; i++) {
        this->idxBuffer[this->idxBuffSize++] = idxStart + i;
    }
    this->idxBuffer[this->idxBuffSize++] = idxStart + 1;
    //LOGI("_circlefill %d", this->idxBuffSize);
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

Drawable* addchild(Drawable* this) {
    if (takenDrawableCount >= 1024) {
        return NULL;
    }
    Drawable* d = this->children[this->childrenCount++] = &drawablePool[takenDrawableCount++];
    drawableInit(d, this);
    return d;
}

Drawable* rectfill(Drawable* this, float x1, float y1, float width, float height) {
    Drawable* d = addchild(this);
    _rectfill(d, x1, y1, width, height);
    return d;
}

Drawable* circlefill(Drawable* this, float x1, float y1, float radius) {
    Drawable* d = addchild(this);
    _circlefill(d, x1, y1, radius);
    return d;
}

//Drawable* Drawable::rectstroke(float x1, float y1, float width, float height) {
//    Drawable* d = addchild();
//    d->_rectstroke(x1, y1, width, height);
//    return d;
//}
//
//Drawable* Drawable::circlestroke(float x1, float y1, float radius) {
//    Drawable* d = addchild();
//    d->_circlestroke(x1, y1, radius);
//    return d;
//}

void end(Drawable* this) {
    //LOGI("taken %d", takenDrawableCount);
    //LOGI("bufferdata %d %d", vtxBuffSize, idxBuffSize);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, this->vtxBuffSize * sizeof(Vertex2), this->vtxBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->idxBuffSize * sizeof(GLushort), this->idxBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    for (int i = 0; i < this->childrenCount; i++) {
        end(this->children[i]);
    }
}

void canvasInit(Canvas* this) {
    drawablePool = (Drawable*)malloc(sizeof(Drawable) * 1024);

    LOGI("canvas init");
    drawableInit(&this->base, NULL);
    takenDrawableCount = 0;

    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    //glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
}

void Canvas_deinit(Drawable* this) {
    Drawable_deinit(this);
    takenDrawableCount = 0;

    free(drawablePool);
}

void Canvas_draw(Drawable* this) {
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);

    Drawable_draw(this);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void createCanvas(Canvas* canvas) {
    if (canvas == NULL) {
        return;
    }
    memset(canvas, 0, sizeof(Canvas));
    createDrawable(&canvas->base);
    canvas->base.derived = canvas;
    //canvas->base.init = Canvas_init;
    canvas->base.deinit = Canvas_deinit;
    canvas->base.draw = Canvas_draw;
}

void resize(Canvas* this, int xscreen1, int yscreen1, int w, int h) {
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
}