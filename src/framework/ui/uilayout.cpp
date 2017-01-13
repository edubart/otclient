/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
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

#include "uilayout.h"
#include "uiwidget.h"

#include <framework/core/eventdispatcher.h>

void UILayout::update()
{
    //logTraceCounter();
    if(!m_parentWidget)
        return;

    /*
    UIWidgetPtr parent = parentWidget;
    do {
        UILayoutPtr ownerLayout = parent->getLayout();
        if(ownerLayout && ownerLayout->isUpdateDisabled())
            return;
        parent = parent->getParent();
    } while(parent);
    */

    if(m_updateDisabled)
        return;

    if(m_updating) {
        updateLater();
        return;
    }

    m_updating = true;
    internalUpdate();
    m_parentWidget->onLayoutUpdate();
    m_updating = false;
}

void UILayout::updateLater()
{
    if(m_updateDisabled || m_updateScheduled)
        return;

    if(!getParentWidget())
        return;

    auto self = static_self_cast<UILayout>();
    g_dispatcher.addEvent([self] {
        self->m_updateScheduled = false;
        self->update();
    });
    m_updateScheduled = true;
}
