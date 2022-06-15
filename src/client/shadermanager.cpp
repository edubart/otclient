/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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
#include <framework/core/resourcemanager.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/ogl/painterogl2_shadersources.h>
#include <framework/graphics/paintershaderprogram.h>

ShaderManager g_shaders;

void ShaderManager::init()
{
    if (!g_graphics.canUseShaders())
        return;

    m_defaultItemShader = createFragmentShaderFromCode("Item", std::string{ glslMainFragmentShader } + glslTextureSrcFragmentShader.data());
    setupItemShader(m_defaultItemShader);

    m_defaultOutfitShader = createFragmentShaderFromCode("Outfit", std::string{ glslMainFragmentShader } + glslTextureSrcFragmentShader.data());
    setupItemShader(m_defaultOutfitShader);

    m_defaultMountShader = createFragmentShaderFromCode("Mount", std::string{ glslMainFragmentShader } + glslTextureSrcFragmentShader.data());
    setupItemShader(m_defaultMountShader);

    m_defaultMapShader = createFragmentShaderFromCode("Map", std::string{ glslMainFragmentShader } + glslTextureSrcFragmentShader.data());

    PainterShaderProgram::release();
}

void ShaderManager::terminate()
{
    m_defaultItemShader = nullptr;
    m_defaultOutfitShader = nullptr;
    m_defaultMountShader = nullptr;
    m_defaultMapShader = nullptr;
    m_shaders.clear();
}

PainterShaderProgramPtr ShaderManager::createShader(const std::string_view name)
{
    if (!g_graphics.canUseShaders()) {
        g_logger.error(stdext::format("unable to create shader '%s', shaders are not supported", name));
        return nullptr;
    }

    PainterShaderProgramPtr shader(new PainterShaderProgram);
    m_shaders[name.data()] = shader;
    return shader;
}

PainterShaderProgramPtr ShaderManager::createFragmentShader(const std::string_view name, const std::string_view file)
{
    PainterShaderProgramPtr shader = createShader(name);
    if (!shader)
        return nullptr;

    const auto& path = g_resources.guessFilePath(file, "frag");

    shader->addShaderFromSourceCode(ShaderType::VERTEX, std::string{ glslMainWithTexCoordsVertexShader } + glslPositionOnlyVertexShader.data());
    if (!shader->addShaderFromSourceFile(ShaderType::FRAGMENT, path)) {
        g_logger.error(stdext::format("unable to load fragment shader '%s' from source file '%s'", name, path));
        return nullptr;
    }

    if (!shader->link()) {
        g_logger.error(stdext::format("unable to link shader '%s' from file '%s'", name, path));
        return nullptr;
    }

    registerShader(name, shader);
    return shader;
}

PainterShaderProgramPtr ShaderManager::createFragmentShaderFromCode(const std::string_view name, const std::string_view code)
{
    PainterShaderProgramPtr shader = createShader(name);
    if (!shader)
        return nullptr;

    shader->addShaderFromSourceCode(ShaderType::VERTEX, std::string{ glslMainWithTexCoordsVertexShader } + glslPositionOnlyVertexShader.data());
    if (!shader->addShaderFromSourceCode(ShaderType::FRAGMENT, code)) {
        g_logger.error(stdext::format("unable to load fragment shader '%s'", name));
        return nullptr;
    }

    if (!shader->link()) {
        g_logger.error(stdext::format("unable to link shader '%s'", name));
        return nullptr;
    }

    m_shaders[name.data()] = shader;
    return shader;
}

PainterShaderProgramPtr ShaderManager::createItemShader(const std::string_view name, const std::string_view file)
{
    PainterShaderProgramPtr shader = createFragmentShader(name, file);
    if (shader)
        setupItemShader(shader);

    return shader;
}

PainterShaderProgramPtr ShaderManager::createOutfitShader(const std::string_view name, const std::string_view file)
{
    PainterShaderProgramPtr shader = createFragmentShader(name, file);
    if (shader)
        setupOutfitShader(shader);

    return shader;
}

PainterShaderProgramPtr ShaderManager::createMountShader(const std::string_view name, const std::string_view file)
{
    PainterShaderProgramPtr shader = createFragmentShader(name, file);
    if (shader)
        setupMountShader(shader);

    return shader;
}

PainterShaderProgramPtr ShaderManager::createMapShader(const std::string_view name, const std::string_view file)
{
    PainterShaderProgramPtr shader = createFragmentShader(name, file);
    if (shader)
        setupMapShader(shader);

    return shader;
}

void ShaderManager::setupItemShader(const PainterShaderProgramPtr& shader)
{
    if (!shader)
        return;

    shader->bindUniformLocation(ITEM_ID_UNIFORM, "u_ItemId");
}

void ShaderManager::setupOutfitShader(const PainterShaderProgramPtr& shader)
{
    if (!shader)
        return;

    shader->bindUniformLocation(OUTFIT_ID_UNIFORM, "u_OutfitId");
}

void ShaderManager::setupMountShader(const PainterShaderProgramPtr& shader)
{
    if (!shader)
        return;

    shader->bindUniformLocation(MOUNT_ID_UNIFORM, "u_MountId");
}

void ShaderManager::setupMapShader(const PainterShaderProgramPtr& shader)
{
    if (!shader)
        return;

    shader->bindUniformLocation(MAP_CENTER_COORD, "u_MapCenterCoord");
    shader->bindUniformLocation(MAP_GLOBAL_COORD, "u_MapGlobalCoord");
    shader->bindUniformLocation(MAP_WALKOFFSET, "u_WalkOffset");
    shader->bindUniformLocation(MAP_ZOOM, "u_MapZoom");
}

void ShaderManager::registerShader(const std::string_view name, const PainterShaderProgramPtr& shader)
{
    m_shaders[name.data()] = shader;
}

PainterShaderProgramPtr ShaderManager::getShader(const std::string_view name)
{
    const auto it = m_shaders.find(name.data());
    if (it != m_shaders.end())
        return it->second;

    return nullptr;
}
