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


#include "thingtypeotb.h"

#include <framework/core/filestream.h>

ThingTypeOtb::ThingTypeOtb()
{
    m_category = OtbInvalidCateogry;
}

void ThingTypeOtb::unserialize(OtbCategory category, const FileStreamPtr& fin)
{
    m_null = false;
    m_category = category;
    fin->getU32(); // skip flags

    bool done = false;
    for(int i=0;i<OtbLastAttrib; ++i) {
        int attr = fin->getU8();

        if(attr == 0) {
            done = true;
            break;
        }

        uint16 len = fin->getU16();

        switch(attr) {
            case OtbAttribServerId:
                m_serverId = fin->getU16();
                break;
            case OtbAttribClientId:
                m_clientId = fin->getU16();
                break;
            case OtbAttribSpeed:
                fin->getU16(); // skip speed
                break;
            default:
                fin->seek(len); // skip attribute
                break;
        }
    }

    if(!done)
        stdext::throw_exception("failed to unserialize otb type, corrupt otb?");
}
