//
// Created by Rian Prakoso on 7/30/20.
//

#ifndef COMPASS_CONTAINER_H
#define COMPASS_CONTAINER_H

#include <EGL/egl.h>

#ifdef __ANDROID_API__

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#else

#define LOGI(...) (printf(__VA_ARGS__))

#endif

#include "Canvas.h"

struct Container {
    EGLint width, height;
    EGLSurface surface;
    EGLContext context;
    EGLDisplay display;
    EGLNativeWindowType window;
    //android_app* app;
    bool running;
    bool animating;

    Canvas canvas;
    float s1px;

    int initEgl();
    int deinitEgl();

    void draw();
};


#endif //COMPASS_CONTAINER_H
