#ifndef UIVERTICALLAYOUT_H
#define UIVERTICALLAYOUT_H

#include "uilayout.h"

class UIVerticalLayout : public UILayout
{
public:
    UIVerticalLayout(UIWidgetPtr parentWidget);

    virtual void update();
    virtual void addWidget(const UIWidgetPtr& widget);
    virtual void removeWidget(const UIWidgetPtr& widget);
};

#endif
