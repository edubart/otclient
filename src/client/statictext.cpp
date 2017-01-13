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

#include "statictext.h"
#include "map.h"
#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/fontmanager.h>

StaticText::StaticText()
{
    m_mode = Otc::MessageNone;
    m_color = Color::white;
    m_cachedText.setFont(g_fonts.getFont("verdana-11px-rounded"));
    m_cachedText.setAlign(Fw::AlignCenter);
}

void StaticText::drawText(const Point& dest, const Rect& parentRect)
{
    Size textSize = m_cachedText.getTextSize();
    Rect rect = Rect(dest - Point(textSize.width() / 2, textSize.height()) + Point(20, 5), textSize);
    Rect boundRect = rect;
    boundRect.bind(parentRect);

    // draw only if the real center is not too far from the parent center, or its a yell
    //if(g_map.isAwareOfPosition(m_position) || isYell()) {
        g_painter->setColor(m_color);
        m_cachedText.draw(boundRect);
    //}
}

void StaticText::setFont(const std::string& fontName)
{
    m_cachedText.setFont(g_fonts.getFont(fontName));
}

void StaticText::setText(const std::string& text)
{
    m_cachedText.setText(text);
}

bool StaticText::addMessage(const std::string& name, Otc::MessageMode mode, const std::string& text)
{
    //TODO: this could be moved to lua
    // first message
    if(m_messages.size() == 0) {
        m_name = name;
        m_mode = mode;
    }
    // check if we can really own the message
    else if(m_name != name || m_mode != mode) {
        return false;
    }
    // too many messages
    else if(m_messages.size() > 10) {
        m_messages.pop_front();
        m_updateEvent->cancel();
        m_updateEvent = nullptr;
    }

    int delay = std::max<int>(Otc::STATIC_DURATION_PER_CHARACTER * text.length(), Otc::MIN_STATIC_TEXT_DURATION);
    if(isYell())
        delay *= 2;

    m_messages.push_back(std::make_pair(text, g_clock.millis() + delay));
    compose();

    if(!m_updateEvent)
        scheduleUpdate();

    return true;
}

void StaticText::update()
{
    m_messages.pop_front();
    if(m_messages.empty()) {
        // schedule removal
        auto self = asStaticText();
        g_dispatcher.addEvent([self]() { g_map.removeThing(self); });
    } else {
        compose();
        scheduleUpdate();
    }
}

void StaticText::scheduleUpdate()
{
    int delay = std::max<int>(m_messages.front().second - g_clock.millis(), 0);

    auto self = asStaticText();
    m_updateEvent = g_dispatcher.scheduleEvent([self]() {
        self->m_updateEvent = nullptr;
        self->update();
    }, delay);
}

void StaticText::compose()
{
    //TODO: this could be moved to lua
    std::string text;

    if(m_mode == Otc::MessageSay) {
        text += m_name;
        text += " says:\n";
        m_color = Color(239, 239, 0);
    } else if(m_mode == Otc::MessageWhisper) {
        text += m_name;
        text += " whispers:\n";
        m_color = Color(239, 239, 0);
    } else if(m_mode == Otc::MessageYell) {
        text += m_name;
        text += " yells:\n";
        m_color = Color(239, 239, 0);
    } else if(m_mode == Otc::MessageMonsterSay || m_mode == Otc::MessageMonsterYell || m_mode == Otc::MessageSpell
              || m_mode == Otc::MessageBarkLow || m_mode == Otc::MessageBarkLoud) {
        m_color = Color(254, 101, 0);
    } else if(m_mode == Otc::MessageNpcFrom || m_mode == Otc::MessageNpcFromStartBlock) {
        text += m_name;
        text += " says:\n";
        m_color = Color(95, 247, 247);
    } else {
        g_logger.warning(stdext::format("Unknown speak type: %d", m_mode));
    }

    for(uint i = 0; i < m_messages.size(); ++i) {
        text += m_messages[i].first;

        if(i < m_messages.size() - 1)
            text += "\n";
    }

    m_cachedText.setText(text);
    m_cachedText.wrapText(275);
}
