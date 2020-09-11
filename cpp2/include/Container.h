//
// Created by Rian Prakoso on 7/30/20.
//

#ifndef COMPASS_CONTAINER_H
#define COMPASS_CONTAINER_H

#ifdef __ANDROID_API__

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#else

#define LOGI(...) (printf(__VA_ARGS__))

#endif

#include "Canvas.h"

struct SContainer {
    void* derived;

    uint8_t running;
    uint8_t animating;
    int width, height;

    Canvas canvas;
    float s1px;

    int (*init)(Container* obj);
    int (*deinit)(Container* obj);
    void (*swapBuffers)(Container* obj);
};

void createContainer(Container* container);

void Container_draw(Container* obj);
void Container_init(Container* obj);
void Container_deinit(Container* obj);
void Container_swapBuffers(Container* obj);

#endif //COMPASS_CONTAINER_H
