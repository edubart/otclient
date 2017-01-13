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

#include "shadermanager.h"
#include <framework/graphics/paintershaderprogram.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/ogl/painterogl2_shadersources.h>
#include <framework/core/resourcemanager.h>

ShaderManager g_shaders;

void ShaderManager::init()
{
    if(!g_graphics.canUseShaders())
        return;

    m_defaultItemShader = createFragmentShaderFromCode("Item", glslMainFragmentShader + glslTextureSrcFragmentShader);
    setupItemShader(m_defaultItemShader);

    m_defaultMapShader = createFragmentShaderFromCode("Map", glslMainFragmentShader + glslTextureSrcFragmentShader);

    PainterShaderProgram::release();
}

void ShaderManager::terminate()
{
    m_defaultItemShader = nullptr;
    m_defaultMapShader = nullptr;
    m_shaders.clear();
}

PainterShaderProgramPtr ShaderManager::createShader(const std::string& name)
{
    if(!g_graphics.canUseShaders()) {
        g_logger.error(stdext::format("unable to create shader '%s', shaders are not supported", name));
        return nullptr;
    }

    PainterShaderProgramPtr shader(new PainterShaderProgram);
    m_shaders[name] = shader;
    return shader;
}

PainterShaderProgramPtr ShaderManager::createFragmentShader(const std::string& name, std::string file)
{
    PainterShaderProgramPtr shader = createShader(name);
    if(!shader)
        return nullptr;

    file = g_resources.guessFilePath(file, "frag");

    shader->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
    if(!shader->addShaderFromSourceFile(Shader::Fragment, file)) {
        g_logger.error(stdext::format("unable to load fragment shader '%s' from source file '%s'", name, file));
        return nullptr;
    }

    if(!shader->link()) {
        g_logger.error(stdext::format("unable to link shader '%s' from file '%s'", name, file));
        return nullptr;
    }

    m_shaders[name] = shader;
    return shader;
}

PainterShaderProgramPtr ShaderManager::createFragmentShaderFromCode(const std::string& name, const std::string& code)
{
    PainterShaderProgramPtr shader = createShader(name);
    if(!shader)
        return nullptr;

    shader->addShaderFromSourceCode(Shader::Vertex, glslMainWithTexCoordsVertexShader + glslPositionOnlyVertexShader);
    if(!shader->addShaderFromSourceCode(Shader::Fragment, code)) {
        g_logger.error(stdext::format("unable to load fragment shader '%s'", name));
        return nullptr;
    }

    if(!shader->link()) {
        g_logger.error(stdext::format("unable to link shader '%s'", name));
        return nullptr;
    }

    m_shaders[name] = shader;
    return shader;
}

PainterShaderProgramPtr ShaderManager::createItemShader(const std::string& name, const std::string& file)
{
    PainterShaderProgramPtr shader = createFragmentShader(name, file);
    if(shader)
        setupItemShader(shader);
    return shader;
}

PainterShaderProgramPtr ShaderManager::createMapShader(const std::string& name, const std::string& file)
{
    PainterShaderProgramPtr shader = createFragmentShader(name, file);
    if(shader)
        setupMapShader(shader);
    return shader;
}

void ShaderManager::setupItemShader(const PainterShaderProgramPtr& shader)
{
    if(!shader)
        return;
    shader->bindUniformLocation(ITEM_ID_UNIFORM, "u_ItemId");
}

void ShaderManager::setupMapShader(const PainterShaderProgramPtr& shader)
{
    if(!shader)
        return;
    shader->bindUniformLocation(MAP_CENTER_COORD, "u_MapCenterCoord");
    shader->bindUniformLocation(MAP_GLOBAL_COORD, "u_MapGlobalCoord");
    shader->bindUniformLocation(MAP_ZOOM, "u_MapZoom");
}

PainterShaderProgramPtr ShaderManager::getShader(const std::string& name)
{
    auto it = m_shaders.find(name);
    if(it != m_shaders.end())
        return it->second;
    return nullptr;
}

