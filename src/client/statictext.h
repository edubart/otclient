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

#ifndef STATICTEXT_H
#define STATICTEXT_H

#include "thing.h"
#include <framework/graphics/cachedtext.h>
#include <framework/core/timer.h>

// @bindclass
class StaticText : public Thing
{
public:
    StaticText();

    void drawText(const Point& dest, const Rect& parentRect);

    std::string getName() { return m_name; }
    Otc::MessageMode getMessageMode() { return m_mode; }
    std::string getFirstMessage() { return m_messages[0].first; }

    bool isYell() { return m_mode == Otc::MessageYell || m_mode == Otc::MessageMonsterYell || m_mode == Otc::MessageBarkLoud; }

    void setText(const std::string& text);
    void setFont(const std::string& fontName);
    bool addMessage(const std::string& name, Otc::MessageMode mode, const std::string& text);

    StaticTextPtr asStaticText() { return static_self_cast<StaticText>(); }
    bool isStaticText() { return true; }

    void setColor(const Color& color) { m_color = color; }
    Color getColor() { return m_color; }

private:
    void update();
    void scheduleUpdate();
    void compose();

    stdext::boolean<false> m_yell;
    std::deque<std::pair<std::string, ticks_t>> m_messages;
    std::string m_name;
    Otc::MessageMode m_mode;
    Color m_color;
    CachedText m_cachedText;
    ScheduledEventPtr m_updateEvent;
};

#endif
