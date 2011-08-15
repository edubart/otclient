#ifndef UILAYOUT_H
#define UILAYOUT_H

#include "declarations.h"

class UILayout
{
public:
    virtual void updateWidget(const UIWidgetPtr& widget) = 0;
    virtual void updateWidgetChildren(const UIWidgetPtr& widget) = 0;
};

#endif
