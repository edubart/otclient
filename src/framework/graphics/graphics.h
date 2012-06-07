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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "declarations.h"
#include "painter.h"

class Graphics
{
public:
    Graphics();

    enum PainterEngine {
        Painter_Any = 0,
        Painter_OpenGL1,
        Painter_OpenGL2
    };

    void init();
    void terminate();
    bool parseOption(const std::string& option);

    bool isPainterEngineAvailable(PainterEngine painterEngine);
    bool selectPainterEngine(PainterEngine painterEngine);
    PainterEngine getPainterEngine() { return m_selectedPainterEngine; }

    void resize(const Size& size);
    void beginRender();
    void endRender();

    void setViewportSize(const Size& size);

    int getMaxTextureSize() { return m_maxTextureSize; }
    const Size& getViewportSize() { return m_viewportSize; }
    TexturePtr& getEmptyTexture() { return m_emptyTexture; }

    std::string getVendor() { return (const char*)glGetString(GL_VENDOR); }
    std::string getRenderer() { return (const char*)glGetString(GL_RENDERER); }
    std::string getVersion() { return (const char*)glGetString(GL_VERSION); }
    std::string getExtensions() { return (const char*)glGetString(GL_EXTENSIONS); }

    bool canUseDrawArrays();
    bool canUseShaders();
    bool canUseFBO();
    bool canUseBilinearFiltering();
    bool canUseHardwareBuffers();
    bool canUseNonPowerOfTwoTextures();
    bool canUseMipmaps();
    bool canUseHardwareMipmaps();
    bool canUseClampToEdge();
    bool canUseBlendFuncSeparate();

private:
    Size m_viewportSize;
    TexturePtr m_emptyTexture;

    int m_maxTextureSize;
    Boolean<true> m_useDrawArrays;
    Boolean<true> m_useFBO;
    Boolean<false> m_useHardwareBuffers;
    Boolean<true> m_useBilinearFiltering;
    Boolean<true> m_useNonPowerOfTwoTextures;
    Boolean<true> m_useMipmaps;
    Boolean<true> m_useHardwareMipmaps;
    Boolean<true> m_useClampToEdge;
    PainterEngine m_prefferedPainterEngine;
    PainterEngine m_selectedPainterEngine;
};

extern Graphics g_graphics;

#endif
