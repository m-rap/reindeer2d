//
// Created by Rian Prakoso on 7/30/20.
//

#include "EglContainer.h"


int EglContainer_init(Container* container) {
	((EglContainer*)container->derivedObj)->init();
}

int EglContainer_deinit(Container* container) {
	((EglContainer*)container->derivedObj)->deinit();
}

void EglContainer_swapBuffers(Container* container) {
	((EglContainer*)container->derivedObj)->swapBuffers();
}


void EglContainer_construct(EglContainer& eglContainer) {
	Container_construct(eglContainer.parent);
	eglContainer.parent.derivedObj = &eglContainer;
	eglContainer.parent.init = EglContainer_init;
	eglContainer.parent.deinit = EglContainer_deinit;
	eglContainer.parent.swapBuffers = EglContainer_swapBuffers;
}



int EglContainer::initEgl() {
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
    surface = eglCreateWindowSurface(display, config, window, NULL);
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

    width = w;
    height = h;
    this->display = display;
    this->surface = surface;
    this->context = context;

    LOGI("w %d h %d", width, height);

    canvas.container = this;
    canvas.init();
    canvas.resize(0, 0, w, h);

    running = true;

    LOGI("initEgl done");

    return 0;
}

int EglContainer::deinitEgl() {
    LOGI("deinitEgl %d", running);
    if (!running) {
        return 0;
    }
    running = false;
    animating = false;

    canvas.deinit();

    eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    if (context != EGL_NO_CONTEXT) {
        eglDestroyContext(display, context);
    }
    if (surface != EGL_NO_SURFACE) {
        eglDestroySurface(display, surface);
    }
    eglTerminate(display);

    display = EGL_NO_DISPLAY;
    context = EGL_NO_CONTEXT;
    surface = EGL_NO_SURFACE;

    return 0;
}

void EglContainer::swapBuffers() {
    eglSwapBuffers(display, surface);
}
