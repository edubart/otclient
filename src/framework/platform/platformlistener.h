#ifndef PLATFORMLISTENER_H
#define PLATFORMLISTENER_H

#include "platformevent.h"

class PlatformListener
{
public:
    /// Fired when user tries to close the window
    virtual void onClose() = 0;
    /// Fired when user resize the window
    virtual void onResize(const Size& size) = 0;
    /// Fired when user press a key or move the mouse
    virtual void onPlatformEvent(const PlatformEvent& event) = 0;
};

#endif
