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
    m_serverId = 0;
    m_clientId = 0;
}

void ThingTypeOtb::unserialize(const FileStreamPtr& fin)
{
    m_null = false;

    uint8 zero = fin->getU8();
    assert(zero == 0);
    m_category = (OtbCategory)fin->getU8();

    fin->getU32(); // skip flags

    bool done = false;
    for(int i=0;i<OtbLastAttrib; ++i) {
        int attr = fin->getU8();
        if(attr == 0) {
            done = true;
            break;
        } else if(attr == 0xFF) {
            fin->seek(fin->tell() - 1);
            done = true;
            break;
        }

        int len = fin->getU16();

        switch(attr) {
            case OtbAttribServerId:
                m_serverId = fin->getU16();
                assert(len == 2);
                break;
            case OtbAttribClientId:
                m_clientId = fin->getU16();
                assert(len == 2);
                break;
            default:
                fin->skip(len); // skip attribute
                break;
        }
    }

    if(!done)
        stdext::throw_exception("failed to unserialize otb type, corrupt otb?");
}
