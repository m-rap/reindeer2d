//
// Created by Rian Prakoso on 7/28/20.
//

#include "EglContainer.h"
#include <string.h>
#include <dlfcn.h>
#include <math.h>
#include <android_native_app_glue.h>

EglContainer container;
Drawable* compass = NULL;

void constructDraw(Container* container1) {
    Drawable* canvas = (Drawable*)&container1->canvas;
    LOGI("constructDraw, canvas: %x", canvas);
    container1->s1px = 1 / ((Canvas*)canvas->derived)->small;
    float s1px = container1->s1px;
    //float linewidth = 10 * s1px;
    float linewidth = 0.01;
    LOGI("s1px %f linewidth %f", s1px, linewidth);

    //Drawable_setColor(canvas, 255, 0, 0, 255);
    //Drawable_circlefill(canvas, 0, 0, 0.83);

    //Drawable_setColor(canvas, 0, 255, 0, 255);
    //Drawable_rectfill(canvas, 0, 0, 0.5, 0.5);

    Drawable_setColor(canvas, 50, 50, 50, 255);
    Drawable_circlefill(canvas, 0, 0, 0.83);

    compass = Drawable_addchild(canvas);

    Drawable_setColor(canvas, 255, 255, 255, 255);
    Drawable* circ1 = Drawable_circlefill(compass, 0, 0, 0.7);
    
    Drawable_setColor(canvas, 0, 0, 0, 255);
    Drawable* circ2 = Drawable_circlefill(compass, 0, 0, 0.7 - linewidth);
    
    Drawable_setColor(canvas, 255, 255, 255, 255);

    float needleLength = 0.05;
    //float needleRadius = 0.7 - 35 * s1px;
    float needleRadius = 0.7 - needleLength / 2;

    for (int degree = 0; degree < 360; degree += 5) {
        double radians = degree * M_PI / 180;
        float r;
        float nl;
        //if (degree % 90 == 0) {
        if (degree % 45 == 0) {
            nl = needleLength + 0.04;
            r = needleRadius - 0.02;
        } else if (degree % 5 == 0 && degree % 10 != 0) {
            nl = needleLength - 0.02;
            r = needleRadius + 0.01;
        } else {
            nl = needleLength;
            r = needleRadius;
        }

        float x = (float)(cos(radians) * r), y = (float)(sin(radians) * r);
        Drawable* needle = Drawable_rectfill(compass, x, y, nl, linewidth);

        if (degree == 90) {
            Drawable_setColor(canvas, 10, 200, 10, 255);
        }

        float xOuter = (float)(cos(radians) * (r + 0.11)), yOuter = (float)(sin(radians) * (r + 0.11));
        Drawable* needleOuter = Drawable_rectfill(canvas, xOuter, yOuter, nl, linewidth);
        if (degree == 90) {
            Drawable_setColor(canvas, 255, 255, 255, 255);
        }

        needle->rotation = degree;
        needleOuter->rotation = degree;
    }

    Drawable_setColor(canvas, 255, 0, 0, 255);
    Drawable* hand = Drawable_addchild(compass);
    float handHeight = 0.1;
    float handWidth = 0.08;
    float handRadius = 0.7 - 0.25;

    hand->vtxBuffer = (Vertex2*)malloc(sizeof(Vertex2) * 3);
    hand->idxBuffer = (GLushort *)malloc(sizeof(GLushort) * 3);
    Drawable_addVtx(hand, 0, handRadius);
    Drawable_addVtx(hand, handWidth / 2, handRadius - handHeight);
    Drawable_addVtx(hand, -handWidth / 2, handRadius - handHeight);
    
    int tr1[] = {0, 2, 1};
    Drawable_addTriangle(hand, tr1);
    
    hand->mode = GL_TRIANGLES;

    Drawable_end(canvas);
}

void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    LOGI("engine_handle_cmd %d", cmd);
    if (app->userData == NULL) {
        return;
    }
    EglContainer* container1 = (EglContainer*)app->userData;
    Container* container12 = (Container*)container1;
    
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            break;
        case APP_CMD_INIT_WINDOW:
            LOGI("APP_CMD_INIT_WINDOW %x", container1->window);
            if (((struct android_app*)container1->extData)->window != NULL) {
                container1->window = ((struct android_app*)container1->extData)->window;
                Container_init(container12);
                constructDraw(container12);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            Container_deinit(container12);
            break;
        case APP_CMD_GAINED_FOCUS:
            container12->animating = true;
            break;
        case APP_CMD_LOST_FOCUS:
            container12->animating = false;
            break;
        default:
            break;
    }
    LOGI("engine_handle_cmd %d done", cmd);
}

void android_main(struct android_app* state) {
    LOGI("android_main");

    int secDiff = 0, prevSecDiff = 0;
    timeval start;
    gettimeofday(&start, NULL);

    createEglContainer(&container, state);

    state->userData = &container;
    state->onAppCmd = engine_handle_cmd;

    int fps = 0;

    LOGI("to loop");
    Container& container2 = *((Container*)&container);

    while (true) {
        int ident;
        int events;
        struct android_poll_source* source;


        timeval now;
        gettimeofday(&now, NULL);
        secDiff = now.tv_sec - start.tv_sec;

        while ((ident=ALooper_pollAll(0, NULL, &events,
                                      (void**)&source)) >= 0) {

            if (source != NULL) {
                source->process(state, source);
            }

            if (ident == LOOPER_ID_USER) {
            }

            if (secDiff != prevSecDiff) {
            }

            if (state->destroyRequested != 0) {
                LOGI("destroyRequested");
                Container_deinit(&container2);
                break;
            }
        }

        if (state->destroyRequested != 0) {
            break;
        }

        if (secDiff != prevSecDiff) {
            //LOGI("fps %d", fps);
            if (compass != NULL) {
            }
            fps = 0;
        }

        //LOGI("running %d animating %d", container2.running, container2.animating);
        if (container2.running && container2.animating) {
            //compass->rotation += 0.01;
            Container_draw(&container2);
        }

        fps++;

        prevSecDiff = secDiff;
    }


    LOGI("main end");
}
