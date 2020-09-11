//
// Created by Rian Prakoso on 7/30/20.
//

#include "EglContainer.h"
#include <string.h>

void EglContainer::EglContainer(void* extData) : Container() {
    container->extData = extData;
    container->display = EGL_NO_DISPLAY;
    container->context = EGL_NO_CONTEXT;
    container->surface = EGL_NO_SURFACE;
}

int EglContainer::init() {
    LOGI("initEgl");

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
    surface = eglCreateWindowSurface(display, config, this->window, NULL);
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

    this->width = w;
    this->height = h;
    this->display = display;
    this->surface = surface;
    this->context = context;

    LOGI("w %d h %d", this->width, this->height);

    this->canvas.container = this;
    this->canvas->init();
    this->canvas->resize(0, 0, w, h);

    this->running = 1;

    LOGI("initEgl done");

    return 0;
}

int EglContainer::deinit() {
    LOGI("deinitEgl %d", this->running);

    if (!this->running) {
        return 0;
    }
    this->running = 0;
    this->animating = 0;

    this->canvas->deinit();

    eglMakeCurrent(this->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (this->context != EGL_NO_CONTEXT) {
        eglDestroyContext(this->display, this->context);
    }
    if (this->surface != EGL_NO_SURFACE) {
        eglDestroySurface(this->display, this->surface);
    }
    eglTerminate(this->display);

    this->display = EGL_NO_DISPLAY;
    this->context = EGL_NO_CONTEXT;
    this->surface = EGL_NO_SURFACE;

    return 0;
}

void EglContainer::swapBuffers() {
    eglSwapBuffers(this->display, this->surface);
}
