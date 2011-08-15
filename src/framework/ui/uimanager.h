#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "declarations.h"
#include <framework/platform/platformevent.h>
#include <framework/otml/declarations.h>

class UIManager
{
public:
    void init();
    void terminate();

    void render();
    void resize(const Size& size);
    void inputEvent(const PlatformEvent& event);

    bool importStyles(const std::string& file);
    void importStyleFromOTML(const OTMLNodePtr& styleNode);
    OTMLNodePtr getStyle(const std::string& styleName);

    UIWidgetPtr loadUI(const std::string& file);
    UIWidgetPtr loadWidgetFromOTML(const OTMLNodePtr& widgetNode);

    UIWidgetPtr getRootWidget() { return m_rootWidget; }

private:
    UIWidgetPtr m_rootWidget;
    std::map<std::string, OTMLNodePtr> m_styles;
};

extern UIManager g_ui;

#endif
