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
    uint8_t running;
    uint8_t animating;
    int width, height;

    Canvas canvas;
    float s1px;

    int (*init)(Container* container);
    int (*deinit)(Container* container);
    void (*swapBuffers)(Container* container);
};

void createContainer(Container* container);

void draw(Container* obj);

#endif //COMPASS_CONTAINER_H
