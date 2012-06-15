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

#ifndef STATICTEXT_H
#define STATICTEXT_H

#include "thing.h"
#include <framework/graphics/cachedtext.h>
#include <framework/graphics/fontmanager.h>
#include <framework/core/timer.h>

class StaticText : public Thing
{
public:
    StaticText();

    void draw(const Point& dest, const Rect& parentRect);

    std::string getName() { return m_name; }
    Otc::SpeakType getMessageType() { return m_messageType; }
    std::string getFirstMessage() { return m_messages[0]; }

    bool isYell() { return m_messageType == Otc::SpeakYell || m_messageType == Otc::SpeakMonsterYell; }

    bool addMessage(const std::string& name, Otc::SpeakType type, const std::string& message);

    StaticTextPtr asStaticText() { return std::static_pointer_cast<StaticText>(shared_from_this()); }
    bool isStaticText() { return true; }

private:
    void update();
    void scheduleUpdate();
    void compose();

    Boolean<false> m_yell;
    std::deque<std::string> m_messages;
    std::string m_name;
    Otc::SpeakType m_messageType;
    Color m_color;
    CachedText m_cachedText;
    ScheduledEventPtr m_updateEvent;
};

#endif
