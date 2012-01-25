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

#include "statictext.h"
#include "map.h"
#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/graphics.h>

StaticText::StaticText()
{
    m_font = g_fonts.getFont("verdana-11px-rounded");
}

void StaticText::draw(const Point& p, const Rect& visibleRect)
{
    Rect rect = Rect(p - Point(m_textSize.width() / 2, m_textSize.height()) + Point(20, 5), m_textSize);
    Rect boundRect = rect;
    boundRect.bind(visibleRect);
    if((boundRect.center() - rect.center()).length() < visibleRect.width() / 15)
        m_font->renderText(m_text, boundRect, Fw::AlignCenter, m_color);
}

bool StaticText::addMessage(const std::string& name, const std::string& type, const std::string& message)
{
    // First message
    if(m_messages.size() == 0) {
        m_name = name;
        m_messageType = type;
    }
    else {
        // we can only add another message if it follows these conditions
        if(m_name != name || m_messageType != type)
            return false;
    }

    m_messages.push_back(message);
    compose();

    auto self = asStaticText();
    g_dispatcher.scheduleEvent([self]() {
        self->removeMessage();
    }, std::max<int>(DURATION_PER_CHARACTER * message.length(), MIN_DURATION));

    return true;
}

void StaticText::removeMessage()
{
    m_messages.erase(m_messages.begin());

    if(m_messages.empty()) {
        // schedule removal
        auto self = asStaticText();
        g_dispatcher.addEvent([self]() { g_map.removeThing(self); });
    }
    else
        compose();
}

void StaticText::compose()
{
    std::string text;
    text.clear();

    if(m_messageType == "say") {
        text += m_name;
        text += " says:\n";
        m_color = Color(239, 239, 0);
    }
    else if(m_messageType == "whisper") {
        text += m_name;
        text += " whispers:\n";
        m_color = Color(239, 239, 0);
    }
    else if(m_messageType == "yell") {
        text += m_name;
        text += " yells:\n";
        m_color = Color(239, 239, 0);
    }
    else if(m_messageType == "monsterSay" || m_messageType == "monsterYell") {
        m_color = Color(254, 101, 0);
    }
    else if(m_messageType == "npcToPlayer") {
        text += m_name;
        text += " says:\n";
        m_color = Color(95, 247, 247);
    }
    else {
        logWarning("unknown speak type: ", m_messageType);
    }

    for(uint i = 0; i < m_messages.size(); ++i) {
        text += m_messages[i];

        if(i < m_messages.size() - 1)
            text += "\n";
    }

    m_text = m_font->wrapText(text, 200);
    m_textSize = m_font->calculateTextRectSize(m_text);
}
