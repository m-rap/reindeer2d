//
// Created by Rian Prakoso on 7/30/20.
//

#ifndef EGLCONTAINER_H
#define EGLCONTAINER_H

#ifdef USE_EGL

#include <EGL/egl.h>

#ifdef __ANDROID_API__

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "native-activity", __VA_ARGS__))

#else

#define LOGI(...) (printf(__VA_ARGS__))

#endif

#include "Container.h"

struct EglContainer {
    Container base;

    EGLSurface surface;
    EGLContext context;
    EGLDisplay display;
    EGLNativeWindowType window;
    void* extData;
    //android_app* app;

    EglContainer(void* extData);
};

#endif

#endif //EGLCONTAINER_H
