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

#include <framework/graphics/paintershaderprogram.h>
#include "declarations.h"

 //@bindsingleton g_shaders
class ShaderManager
{
public:
    enum
    {
        ITEM_ID_UNIFORM = 10,
        OUTFIT_ID_UNIFORM = 11,
        MOUNT_ID_UNIFORM = 12,
        SHADER_ID_UNIFORM = 13,
        MAP_ZOOM = 14,
        MAP_WALKOFFSET = 15,
        MAP_CENTER_COORD = 16,
        MAP_GLOBAL_COORD = 17,
    };

    void init();
    void terminate();

    void registerShader(const std::string& name, const PainterShaderProgramPtr& shader);
    void setupMapShader(const PainterShaderProgramPtr& shader);
    void setupItemShader(const PainterShaderProgramPtr& shader);
    void setupOutfitShader(const PainterShaderProgramPtr& shader);
    void setupMountShader(const PainterShaderProgramPtr& shader);

    PainterShaderProgramPtr createShader(const std::string& name);
    PainterShaderProgramPtr createFragmentShader(const std::string& name, std::string file);
    PainterShaderProgramPtr createFragmentShaderFromCode(const std::string& name, const std::string& code);

    PainterShaderProgramPtr createItemShader(const std::string& name, const std::string& file);
    PainterShaderProgramPtr createOutfitShader(const std::string& name, const std::string& file);
    PainterShaderProgramPtr createMountShader(const std::string& name, const std::string& file);
    PainterShaderProgramPtr createMapShader(const std::string& name, const std::string& file);

    const PainterShaderProgramPtr& getDefaultItemShader() { return m_defaultItemShader; }
    const PainterShaderProgramPtr& getDefaultOutfitShader() { return m_defaultOutfitShader; }
    const PainterShaderProgramPtr& getDefaultMountShader() { return m_defaultMountShader; }
    const PainterShaderProgramPtr& getDefaultMapShader() { return m_defaultMapShader; }

    PainterShaderProgramPtr getShader(const std::string& name);

private:

    PainterShaderProgramPtr m_defaultItemShader, m_defaultOutfitShader, m_defaultMountShader, m_defaultMapShader;
    std::unordered_map<std::string, PainterShaderProgramPtr> m_shaders;
};

extern ShaderManager g_shaders;

#endif
