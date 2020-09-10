//
// Created by Rian Prakoso on 7/30/20.
//

#include "Container.h"
#include <string.h>

void createContainer(Container* container) {
    if (container == NULL) {
        return;
    }

    memset(container, 0, sizeof(Container));

    createCanvas(&container->canvas);
}

void Container_draw(Container* obj) {
    Drawable_draw((Drawable*)&obj->canvas);

    obj->swapBuffers(obj);
}

void Container_init(Container* obj) {
    obj->init(obj);
}

void Container_deinit(Container* obj) {
    obj->deinit(obj);
}

void Container_swapBuffers(Container* obj) {
    obj->swapBuffers(obj);
}