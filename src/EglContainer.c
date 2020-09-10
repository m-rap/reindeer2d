//
// Created by Rian Prakoso on 7/30/20.
//

#include "EglContainer.h"
#include <string.h>

int _EglContainer_init(Container* obj);

int _EglContainer_deinit(Container* obj);

void _EglContainer_swapBuffers(Container* obj);

void createEglContainer(EglContainer* container, void* extData) {
    if (container == NULL) {
        return;
    }

    memset(container, 0, sizeof(EglContainer));

    createContainer(&container->base);

    container->base.derived = container;
    container->base.init = _EglContainer_init;
    container->base.deinit = _EglContainer_deinit;
    container->base.swapBuffers = _EglContainer_swapBuffers;

    container->extData = extData;
    container->display = EGL_NO_DISPLAY;
    container->context = EGL_NO_CONTEXT;
    container->surface = EGL_NO_SURFACE;
}

int _EglContainer_init(Container* obj) {
    LOGI("initEgl");
    EglContainer* obj2 = (EglContainer*)obj->derived;

    const EGLint attribs[] = {
            EGL_SURFACE_TYPE,    EGL_WINDOW_BIT,
            EGL_RED_SIZE,        8,
            EGL_GREEN_SIZE,      8,
            EGL_BLUE_SIZE,       8,

            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            //EGL_ALPHA_SIZE,   EGL_DONT_CARE,
            //EGL_DEPTH_SIZE,   16,
            //EGL_SAMPLES,      4,

            EGL_NONE
    };
    //EGLint contextAttribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE};

    EGLint w, h, format;
    EGLint numConfigs = 128;
    EGLConfig config = NULL;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, NULL, NULL);

    eglChooseConfig(display, attribs, NULL, 0, &numConfigs);

    LOGI("numConfigs: %d", numConfigs);

    EGLConfig supportedConfigs[128];
    eglChooseConfig(display, attribs, supportedConfigs, 128, &numConfigs);

    LOGI("numConfigs: %d", numConfigs);

    int i = 0;
    for (; i < numConfigs; i++) {
        EGLConfig cfg = supportedConfigs[i];
        EGLint r, g, b, d;
        if (eglGetConfigAttrib(display, cfg, EGL_RED_SIZE, &r)   &&
            eglGetConfigAttrib(display, cfg, EGL_GREEN_SIZE, &g) &&
            eglGetConfigAttrib(display, cfg, EGL_BLUE_SIZE, &b)  &&
            eglGetConfigAttrib(display, cfg, EGL_DEPTH_SIZE, &d) &&
            r == 8 && g == 8 && b == 8 && d == 0 ) {

            config = supportedConfigs[i];
            break;
        }
    }
    if (i == numConfigs) {
        config = supportedConfigs[0];
    }

    if (config == NULL) {
        LOGI("Unable to initialize EGLConfig");
        return -1;
    }

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
    surface = eglCreateWindowSurface(display, config, obj2->window, NULL);
    context = eglCreateContext(display, config, NULL, NULL);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGI("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    LOGI("OpenGL Info: %s", glGetString(GL_VENDOR));
    LOGI("OpenGL Info: %s", glGetString(GL_RENDERER));
    LOGI("OpenGL Info: %s", glGetString(GL_VERSION));
    LOGI("OpenGL Info: %s", glGetString(GL_EXTENSIONS));

    obj->width = w;
    obj->height = h;
    obj2->display = display;
    obj2->surface = surface;
    obj2->context = context;

    LOGI("w %d h %d", obj->width, obj->height);

    obj->canvas.container = obj;
    Canvas_init(&obj->canvas);
    Canvas_resize(&obj->canvas, 0, 0, w, h);

    obj->running = 1;

    LOGI("initEgl done");

    return 0;
}

int _EglContainer_deinit(Container* obj) {
    LOGI("deinitEgl %d", obj->running);

    EglContainer* obj2 = (EglContainer*)obj->derived;

    if (!obj->running) {
        return 0;
    }
    obj->running = 0;
    obj->animating = 0;

    Drawable_deinit((Drawable*)&obj->canvas);

    eglMakeCurrent(obj2->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (obj2->context != EGL_NO_CONTEXT) {
        eglDestroyContext(obj2->display, obj2->context);
    }
    if (obj2->surface != EGL_NO_SURFACE) {
        eglDestroySurface(obj2->display, obj2->surface);
    }
    eglTerminate(obj2->display);

    obj2->display = EGL_NO_DISPLAY;
    obj2->context = EGL_NO_CONTEXT;
    obj2->surface = EGL_NO_SURFACE;

    return 0;
}

void _EglContainer_swapBuffers(Container* obj) {
    EglContainer* obj2 = (EglContainer*)obj->derived;
    eglSwapBuffers(obj2->display, obj2->surface);
}
