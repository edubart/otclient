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
#include <framework/graphics/fontmanager.h>

class StaticText : public Thing
{
public:
    enum {
        DURATION = 3000
    };

    StaticText();

    void draw(const Point& p);

    std::string getName() { return m_name; }
    int getMessageType() { return m_type; }
    std::string getFirstMessage() { return m_messages[0]; }

    bool addMessage(const std::string& name, int type, const std::string& message);
    void removeMessage();

    StaticTextPtr asStaticText() { return std::static_pointer_cast<StaticText>(shared_from_this()); }

private:
    void compose();

    FontPtr m_font;
    Size m_textSize;
    std::vector<std::string> m_messages;
    std::string m_name, m_text;
    int m_type;
    Color m_color;
};

#endif
