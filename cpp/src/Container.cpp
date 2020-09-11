//
// Created by Rian Prakoso on 7/30/20.
//

#include "Container.h"
#include <string.h>

Container::Container() {
}

void Container::draw() {
    //LOGI("inside Container_draw, container: %x canvas: %x", this, &this->canvas);
    this->canvas.draw();

    this->swapBuffers();
}