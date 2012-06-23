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

#ifndef THINGTYPEMANAGER_H
#define THINGTYPEMANAGER_H

#include <framework/global.h>
#include <framework/core/declarations.h>

#include "thingtypedat.h"
#include "thingtypeotb.h"

class ThingTypeManager
{
public:
    void init();
    void terminate();

    bool loadDat(const std::string& file);
    bool loadOtb(const std::string& file);
    bool loadXml(const std::string& file);

    void addOtbType(const ThingTypeOtbPtr& otbType);
    const ThingTypeOtbPtr& findOtbForClientId(uint16 id);

    const ThingTypeDatPtr& getNullDatType() { return m_nullDatType; }
    const ThingTypeOtbPtr& getNullOtbType() { return m_nullOtbType; }

    const ThingTypeDatPtr& getDatType(uint16 id, DatCategory category);
    const ThingTypeOtbPtr& getOtbType(uint16 id);

    uint32 getDatSignature() { return m_datSignature; }
    uint32 getOtbVersion() { return m_otbVersion; }
    uint32 getOtbMajorVersion() { return m_otbMajorVersion; }
    uint32 getOtbMinorVersion() { return m_otbMinorVersion; }

    bool isDatLoaded() { return m_datLoaded; }
    bool isXmlLoaded() { return m_xmlLoaded; }
    bool isOtbLoaded() { return m_otbLoaded; }

    bool isValidDatId(uint16 id, DatCategory category) { return id >= 1 && id < m_datTypes[category].size(); }
    bool isValidOtbId(uint16 id) { return id >= 1 && id < m_otbTypes.size(); }

private:
    ThingTypeDatList m_datTypes[DatLastCategory];
    ThingTypeOtbList m_otbTypes;

    ThingTypeDatPtr m_nullDatType;
    ThingTypeOtbPtr m_nullOtbType;

    bool m_datLoaded;
    bool m_xmlLoaded;
    bool m_otbLoaded;

    uint32 m_otbVersion;
    uint32 m_otbMinorVersion;
    uint32 m_otbMajorVersion;
    uint32 m_datSignature;
};

extern ThingTypeManager g_things;

#endif
