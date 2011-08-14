#ifndef UIEVENT_H
#define UIEVENT_H

#include <global.h>

struct UIMouseEvent {
    Point mousePos;
    Point mouseMoved;
    MouseButton button;
    MouseWheelDirection wheelDirection;
};

struct UIKeyEvent {
    uchar keycode;
    int keyboardModifiers;
};

#endif
