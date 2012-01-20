/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "declarations.h"
#include <framework/core/inputevent.h>
#include <framework/otml/declarations.h>

class UIManager
{
public:
    void init();
    void terminate();

    void render();
    void resize(const Size& size);
    void inputEvent(const InputEvent& event);

    bool importStyle(const std::string& file);
    void importStyleFromOTML(const OTMLNodePtr& styleNode);
    OTMLNodePtr getStyle(const std::string& styleName);
    std::string getStyleClass(const std::string& styleName);

    UIWidgetPtr loadUI(const std::string& file, const UIWidgetPtr& parent = nullptr);
    UIWidgetPtr loadWidgetFromOTML(const OTMLNodePtr& widgetNode, const UIWidgetPtr& parent);

    void setMouseReceiver(const UIWidgetPtr& widget) { m_mouseReceiver = widget; }
    void setKeyboardReceiver(const UIWidgetPtr& widget) { m_keyboardReceiver = widget; }
    void setDebugBoxesDrawing(bool enabled) { m_drawDebugBoxes = enabled; }
    void setDraggingWidget(const UIWidgetPtr& widget) { m_draggingWidget = widget; }
    void resetMouseReceiver() { m_mouseReceiver = m_rootWidget; }
    void resetKeyboardReceiver() { m_keyboardReceiver = m_rootWidget; }
    UIWidgetPtr getMouseReceiver() { return m_mouseReceiver; }
    UIWidgetPtr getKeyboardReceiver() { return m_keyboardReceiver; }
    UIWidgetPtr getDraggingWidget() { return m_draggingWidget; }
    UIWidgetPtr getRootWidget() { return m_rootWidget; }

    bool isOnInputEvent() { return m_isOnInputEvent; }
    bool isDrawingDebugBoxes() { return m_drawDebugBoxes; }

private:
    UIWidgetPtr m_rootWidget;
    UIWidgetPtr m_mouseReceiver;
    UIWidgetPtr m_keyboardReceiver;
    UIWidgetPtr m_draggingWidget;
    bool m_isOnInputEvent;
    Boolean<false> m_drawDebugBoxes;
    std::map<std::string, OTMLNodePtr> m_styles;
};

extern UIManager g_ui;

#endif
