/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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

#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include "declarations.h"
#include <framework/graphics/paintershaderprogram.h>

//@bindsingleton g_shaders
class ShaderManager
{
public:
    enum {
        ITEM_ID_UNIFORM = 10,
        MAP_CENTER_COORD = 10,
        MAP_GLOBAL_COORD = 11,
        MAP_ZOOM = 12
    };

    void init();
    void terminate();

    PainterShaderProgramPtr createShader(const std::string& name);
    PainterShaderProgramPtr createFragmentShader(const std::string& name, std::string file);
    PainterShaderProgramPtr createFragmentShaderFromCode(const std::string& name, const std::string& code);

    PainterShaderProgramPtr createItemShader(const std::string& name, const std::string& file);
    PainterShaderProgramPtr createMapShader(const std::string& name, const std::string& file);

    const PainterShaderProgramPtr& getDefaultItemShader() { return m_defaultItemShader; }
    const PainterShaderProgramPtr& getDefaultMapShader() { return m_defaultMapShader; }

    PainterShaderProgramPtr getShader(const std::string& name);

private:
    void setupItemShader(const PainterShaderProgramPtr& shader);
    void setupMapShader(const PainterShaderProgramPtr& shader);

    PainterShaderProgramPtr m_defaultItemShader;
    PainterShaderProgramPtr m_defaultMapShader;
    std::unordered_map<std::string, PainterShaderProgramPtr> m_shaders;
};


extern ShaderManager g_shaders;

#endif

