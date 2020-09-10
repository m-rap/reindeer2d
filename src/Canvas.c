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


//void _rectvtx(Drawable* obj, float x, float y, float width, float height);
//void _circlevtx(Drawable* obj, float x, float y, float r);

//void _rectfill(Drawable* obj, float x, float y, float width, float height);
////void _rectstroke(Drawable* obj, float x1, float y1, float width, float height);
//void _circlefill(Drawable* obj, float x1, float y1, float r);
////void _circlestroke(Drawable* obj, float x, float y, float r);


void _Drawable_deinit(Drawable* obj) {
    //LOGI("drawable deinit %08x %d %d", obj, initialized, childrenCount);
    if (obj->initialized) {
        obj->initialized = 0;

        if (obj->vtxBuffer != NULL) {
            free(obj->vtxBuffer);
        }
        if (obj->idxBuffer != NULL) {
            free(obj->idxBuffer);
        }
        glDeleteBuffers(1, &obj->vbo);
        glDeleteBuffers(1, &obj->ibo);

        for (int i = 0; i < obj->childrenCount; i++) {
            obj->children[i]->deinit(obj->children[i]);
        }
        obj->childrenCount = 0;
    }
}

void Drawable_init(Drawable* obj, Drawable* parent1) {
    //LOGI("drawable init");
    obj->initialized = 1;

    obj->childrenCount = 0;

    glGenBuffers(1, &obj->vbo);
    glGenBuffers(1, &obj->ibo);

    obj->vtxBuffSize = 0;
    obj->idxBuffSize = 0;
    obj->x = 0;
    obj->y = 0;
    obj->rotation = 0;
    obj->scale = 1;
    //obj->lineWidth = 1;
    obj->parent = parent1;

    Drawable* tmp = obj;
    while (tmp != NULL) {
        if (tmp->parent == NULL) {
            obj->canvas = tmp;
            break;
        }
        tmp = tmp->parent;
    }
    Drawable_setColor(obj, obj->canvas->r, obj->canvas->g, obj->canvas->b, obj->canvas->a);
    //obj->lineWidth = obj->canvas->lineWidth;
}

void _Drawable_draw(Drawable* obj) {
    //LOGI("inside _Drawable_draw %x %d", obj, obj->childrenCount);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    glTranslatef(obj->x, obj->y, 0);
    glRotatef(obj->rotation, 0, 0, 1);
    glScalef(obj->scale, obj->scale, obj->scale);

    if (obj->idxBuffSize > 0) {
        //LOGI("drawing buffer %d", obj->vbo);

        //glLineWidth(lineWidth);
        glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);

        glVertexPointer(3, GL_FLOAT, sizeof(Vertex2), (void*)offsetof(Vertex2, x));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex2), (void*)offsetof(Vertex2, r));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ibo);
        glDrawElements(obj->mode, obj->idxBuffSize, GL_UNSIGNED_SHORT, 0);

        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_COLOR_ARRAY);
    }

    for (int i = 0; i < obj->childrenCount; i++) {
        obj->children[i]->draw(obj->children[i]);
    }

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void createDrawable(Drawable* obj) {
    if (obj == NULL)
        return;
    
    memset(obj, 0, sizeof(Drawable));
    obj->deinit = _Drawable_deinit;
    obj->draw = _Drawable_draw;
}

void Drawable_deinit(Drawable* obj) {
    obj->deinit(obj);
}

void Drawable_draw(Drawable* obj) {
    obj->draw(obj);
}

void Drawable_setColor(Drawable* obj, unsigned char r1, unsigned char g1, unsigned char b1, unsigned char a1) {
    //LOGI("Drawable %x color: %d,%d,%d,%d", obj, r1, g1, b1, a1);
    obj->r = r1;
    obj->g = g1;
    obj->b = b1;
    obj->a = a1;
}

void Drawable_addVtx(Drawable* obj, float x, float y) {
    Vertex2 tmp = {
        .x = x,
        .y = y,
        .r = obj->r,
        .g = obj->g,
        .b = obj->b,
        .a = obj->a
    };
    obj->vtxBuffer[obj->vtxBuffSize++] = tmp;

    //int n = obj->vtxBuffSize - 1;
    //Vertex2* v = &obj->vtxBuffer[n];
    //LOGI("%f %f %f %d %d %d %d", v->x, v->y, v->z, v->r, v->g, v->b, v->a);
}

void Drawable_addTriangle(Drawable* obj, int* idx) {
    if (obj->idxBuffSize > 1024 - 3) {
        return;
    }
    obj->idxBuffer[obj->idxBuffSize++] = idx[0];
    obj->idxBuffer[obj->idxBuffSize++] = idx[1];
    obj->idxBuffer[obj->idxBuffSize++] = idx[2];
}

void _rectvtx(Drawable* obj, float x1, float y1, float width, float height) {
    //LOGI("_rectvtx");
    obj->x = x1;
    obj->y = y1;

    obj->vtxBuffer = (Vertex2*)malloc(sizeof(Vertex2) * 4);

    Drawable_addVtx(obj, -width / 2, -height / 2);
    Drawable_addVtx(obj, width / 2, -height / 2);
    Drawable_addVtx(obj, width / 2, height / 2);
    Drawable_addVtx(obj, -width / 2, height / 2);
}

void _circlevtx(Drawable* obj, float x1, float y1, float radius) {
    //LOGI("_circlevtx");
    obj->x = x1;
    obj->y = y1;

    obj->vtxBuffer = (Vertex2*)malloc(sizeof(Vertex2) * (nSegments + 1));

    Drawable_addVtx(obj, 0, 0);
    for (int i = 0; i < nSegments; i++) {
        double degree = (double)(i * 360) / nSegments;
        double radians = degree * M_PI / 180;
        Drawable_addVtx(obj, (float)(cos(radians) * radius), (float)(sin(radians) * radius));
    }
}

void _rectfill(Drawable* obj, float x1, float y1, float width, float height) {
    _rectvtx(obj, x1, y1, width, height);

    obj->mode = GL_TRIANGLES;

    obj->idxBuffer = (GLushort*)malloc(sizeof(GLushort) * 6);

    int idx1[] = {obj->vtxBuffSize - (GLushort)4, obj->vtxBuffSize - (GLushort)3, obj->vtxBuffSize - (GLushort)2};
    Drawable_addTriangle(obj, idx1);

    int idx2[] = {obj->vtxBuffSize - (GLushort)4, obj->vtxBuffSize - (GLushort)2, obj->vtxBuffSize - (GLushort)1};
    Drawable_addTriangle(obj, idx2);
    //LOGI("_rectfill %d", obj->idxBuffSize);
}

void _circlefill(Drawable* obj, float x1, float y1, float radius) {
    _circlevtx(obj, x1, y1, radius);

    obj->mode = GL_TRIANGLE_FAN;

    obj->idxBuffer = (GLushort*)malloc(sizeof(GLushort) * (nSegments + 1));

    GLushort idxStart = obj->vtxBuffSize - nSegments - 1;
    //char idxStr[2000] = "";
    for (GLushort i = 0; i < nSegments; i++) {
        obj->idxBuffer[obj->idxBuffSize++] = idxStart + i;
        //sprintf(idxStr, "%s %d", idxStr, obj->idxBuffer[obj->idxBuffSize - 1]);
    }
    obj->idxBuffer[obj->idxBuffSize++] = idxStart + 1;
    //sprintf(idxStr, "%s %d", idxStr, obj->idxBuffer[obj->idxBuffSize - 1]);
    //LOGI("%s", idxStr);
    //LOGI("_circlefill %d", obj->idxBuffSize);
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

Drawable* Drawable_addchild(Drawable* obj) {
    if (takenDrawableCount >= 1024) {
        return NULL;
    }
    Drawable* d = obj->children[obj->childrenCount++] = &drawablePool[takenDrawableCount++];
    createDrawable(d);
    Drawable_init(d, obj);
    return d;
}

Drawable* Drawable_rectfill(Drawable* obj, float x1, float y1, float width, float height) {
    Drawable* d = Drawable_addchild(obj);
    _rectfill(d, x1, y1, width, height);
    return d;
}

Drawable* Drawable_circlefill(Drawable* obj, float x1, float y1, float radius) {
    Drawable* d = Drawable_addchild(obj);
    _circlefill(d, x1, y1, radius);
    return d;
}

//Drawable* Drawable::rectstroke(float x1, float y1, float width, float height) {
//    Drawable* d = Drawable_addchild();
//    d->_rectstroke(x1, y1, width, height);
//    return d;
//}
//
//Drawable* Drawable::circlestroke(float x1, float y1, float radius) {
//    Drawable* d = Drawable_addchild();
//    d->_circlestroke(x1, y1, radius);
//    return d;
//}

void Drawable_end(Drawable* obj) {
    //LOGI("taken %d", takenDrawableCount);
    LOGI("bufferdata %d %d", obj->vtxBuffSize, obj->idxBuffSize);
    glBindBuffer(GL_ARRAY_BUFFER, obj->vbo);
    glBufferData(GL_ARRAY_BUFFER, obj->vtxBuffSize * sizeof(Vertex2), obj->vtxBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj->ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj->idxBuffSize * sizeof(GLushort), obj->idxBuffer, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    for (int i = 0; i < obj->childrenCount; i++) {
        Drawable_end(obj->children[i]);
    }
}

void Canvas_init(Canvas* obj) {
    drawablePool = (Drawable*)malloc(sizeof(Drawable) * 1024);

    LOGI("canvas init");
    Drawable_init(&obj->base, NULL);
    takenDrawableCount = 0;

    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    //glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);
}

void _Canvas_deinit(Drawable* obj) {
    _Drawable_deinit(obj);
    takenDrawableCount = 0;

    free(drawablePool);
}

void _Canvas_draw(Drawable* obj) {
    //LOGI("inside _Canvas_draw %x", obj);

    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0, 0, 0, 0);

    _Drawable_draw(obj);

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
    canvas->base.deinit = _Canvas_deinit;
    canvas->base.draw = _Canvas_draw;
}

void Canvas_resize(Canvas* obj, int xscreen1, int yscreen1, int w, int h) {
    //LOGI("layout %f %f %f %f", xscreen, yscreen, width, height);
    //LOGI("viewport %f %f %f %f", xscreen, yscreen + big / 2 - small / 2, small, small);
    if (obj->xscreen == (float)xscreen1 &&
        obj->yscreen == (float)yscreen1 &&
        obj->width   == (float)w &&
        obj->height  == (float)h) {
        //LOGI("layout same");
        return;
    }

    obj->xscreen = xscreen1;
    obj->yscreen = yscreen1;
    obj->width = w;
    obj->height = h;
    obj->small = obj->width;
    obj->big = obj->height;
    if (obj->height < obj->width) {
        obj->small = obj->height;
        obj->big = obj->width;
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
    glViewport(obj->xscreen, obj->big / 2 - obj->small / 2 + (obj->container->height - obj->big - obj->yscreen), obj->small, obj->small);
    //alalay += 0.1;

    LOGI("layout changed %f %f %f %f", obj->xscreen, obj->big / 2 - obj->small / 2 + (obj->container->height - obj->big - obj->yscreen), obj->small, obj->small);
}