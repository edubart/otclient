#include "fontmanager.h"

#include <framework/graphics/graphics.h>
#include <framework/graphics/texture.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

Graphics g_graphics;

void Graphics::init()
{
    // setup opengl
    glEnable(GL_ALPHA_TEST); // enable alpha by default
    glAlphaFunc(GL_GREATER, 0.0f); // default alpha func
    glDisable(GL_DEPTH_TEST); // we are rendering 2D only, we don't need depth buffer
    glEnable(GL_TEXTURE_2D); // enable textures by default
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    logInfo("GPU ", glGetString(GL_RENDERER));
    logInfo("OpenGL ", glGetString(GL_VERSION));

    m_drawing = false;
    m_opacity = 255;
    m_emptyTexture = TexturePtr(new Texture);

    bindColor(Color::white);
}

void Graphics::terminate()
{
    g_fonts.releaseFonts();
    m_emptyTexture.reset();
}

bool Graphics::isExtensionSupported(const char *extension)
{
    const GLubyte *extensions = NULL;
    const GLubyte *start;
    GLubyte *where, *terminator;
    where = (GLubyte *)strchr(extension, ' ');

    if(where || *extension == '\0')
        return 0;

    extensions = glGetString(GL_EXTENSIONS);

    start = extensions;
    while(true) {
        where = (GLubyte *) strstr((const char *)start, extension);
        if(!where)
            break;

        terminator = where + strlen(extension);

        if(where == start || *(where - 1) == ' ')
            if(*terminator == ' ' || *terminator == '\0')
                return 1;

        start = terminator;
    }
    return 0;
}

void Graphics::resize(const Size& size)
{
    m_screenSize = size;
    restoreViewport();
}

void Graphics::restoreViewport()
{
    const int& width = m_screenSize.width();
    const int& height = m_screenSize.height();

    // resize gl viewport
    glViewport(0, 0, width, height);

    /*
     0,0---------0,w
     |           |
     |           |
     |           |
     h,0---------h,w
     */
    // setup view region like above
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0f, width, height, 0.0f);

    // back to model view
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Graphics::beginRender()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
}

void Graphics::endRender()
{
    assert(!m_drawing);
}

void Graphics::drawTexturedRect(const Rect& screenCoords,
                                const TexturePtr& texture,
                                const Rect& textureCoords)
{
    if(screenCoords.isEmpty() || texture->getId() == 0)
        return;

    // rect correction for opengl
    int right = screenCoords.right() + 1;
    int bottom = screenCoords.bottom() + 1;
    int top = screenCoords.top();
    int left = screenCoords.left();
    const Size& textureSize = texture->getGlSize();

    float textureRight;
    float textureBottom;
    float textureTop;
    float textureLeft;

    if(textureCoords.isEmpty()) {
        textureRight = 1.0f;
        textureBottom = 1.0f;
        textureTop = 0.0f;
        textureLeft = 0.0f;
    } else {
        textureRight = (float)(textureCoords.right() + 1) / textureSize.width();
        textureBottom = (float)(textureCoords.bottom() + 1) / textureSize.height();
        textureTop = (float)textureCoords.top() / textureSize.height();
        textureLeft = (float)textureCoords.left() / textureSize.width();
    }

    if(!m_drawing) {
        bindTexture(texture);
        glBegin(GL_QUADS);
    }

    glTexCoord2f(textureLeft,  textureTop);    glVertex2i(left,  top);
    glTexCoord2f(textureLeft,  textureBottom); glVertex2i(left,  bottom);
    glTexCoord2f(textureRight, textureBottom); glVertex2i(right, bottom);
    glTexCoord2f(textureRight, textureTop);    glVertex2i(right, top);

    if(!m_drawing)
        glEnd();

}

void Graphics::drawRepeatedTexturedRect(const Rect& screenCoords,
                                        const TexturePtr& texture,
                                        const Rect& textureCoords)
{
    if(screenCoords.isEmpty() || texture->getId() == 0 || textureCoords.isEmpty())
        return;

    if(!m_drawing) {
        bindTexture(texture);
        startDrawing();
    }

    // render many repeated texture rects
    Rect virtualScreenCoords(0,0,screenCoords.size());
    for(int y = 0; y <= virtualScreenCoords.height(); y += textureCoords.height()) {
        for(int x = 0; x <= virtualScreenCoords.width(); x += textureCoords.width()) {
            Rect partialCoords(x, y, textureCoords.size());
            Rect partialTextureCoords = textureCoords;

            // partialCoords to screenCoords bottomRight
            if(partialCoords.bottom() > virtualScreenCoords.bottom()) {
                partialTextureCoords.setBottom(partialTextureCoords.bottom() +
                                               (virtualScreenCoords.bottom() - partialCoords.bottom()));
                partialCoords.setBottom(virtualScreenCoords.bottom());
            }
            if(partialCoords.right() > virtualScreenCoords.right()) {
                partialTextureCoords.setRight(partialTextureCoords.right() +
                                              (virtualScreenCoords.right() - partialCoords.right()));
                partialCoords.setRight(virtualScreenCoords.right());
            }

            partialCoords.translate(screenCoords.topLeft());
            drawTexturedRect(partialCoords, texture, partialTextureCoords);
        }
    }

    if(!m_drawing)
        stopDrawing();
}

void Graphics::drawFilledRect(const Rect& screenCoords,
                              const Color& color)
{
    assert(!m_drawing);

    if(screenCoords.isEmpty())
        return;

    // rect correction for opengl
    int right = screenCoords.right() + 1;
    int bottom = screenCoords.bottom() + 1;
    int top = screenCoords.top();
    int left = screenCoords.left();

    glPushAttrib(GL_CURRENT_BIT);
    glColor4ubv(color.rgbaPtr());
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    glVertex2i(left,  top);
    glVertex2i(left,  bottom);
    glVertex2i(right, bottom);
    glVertex2i(right, top);

    glEnd();
    glEnable(GL_TEXTURE_2D);
    glPopAttrib();
}


void Graphics::drawBoundingRect(const Rect& screenCoords,
                                const Color& color,
                                int innerLineWidth)
{
    assert(!m_drawing);

    if(screenCoords.isEmpty() || 2 * innerLineWidth > screenCoords.height())
        return;

    // rect correction for opengl
    int right = screenCoords.right()+1;
    int bottom = screenCoords.bottom()+1;
    int top = screenCoords.top();
    int left = screenCoords.left();

    glPushAttrib(GL_CURRENT_BIT);
    glColor4ubv(color.rgbaPtr());
    glDisable(GL_TEXTURE_2D);
    glBegin(GL_QUADS);

    // top line
    glVertex2i(left,  top);
    glVertex2i(left,  top + innerLineWidth);
    glVertex2i(right, top + innerLineWidth);
    glVertex2i(right, top);

    // left
    glVertex2i(left, screenCoords.top() + innerLineWidth);
    glVertex2i(left, bottom - innerLineWidth);
    glVertex2i(left + innerLineWidth, bottom - innerLineWidth);
    glVertex2i(left + innerLineWidth, screenCoords.top() + innerLineWidth);

    // bottom line
    glVertex2i(left,  bottom);
    glVertex2i(left,  bottom - innerLineWidth);
    glVertex2i(right, bottom - innerLineWidth);
    glVertex2i(right, bottom);

    // right line
    glVertex2i(right                 , top + innerLineWidth);
    glVertex2i(right                 , bottom - innerLineWidth);
    glVertex2i(right - innerLineWidth, bottom - innerLineWidth);
    glVertex2i(right - innerLineWidth, top + innerLineWidth);

    glEnd();
    glEnable(GL_TEXTURE_2D);
    glPopAttrib();
}

void Graphics::bindColor(const Color& color)
{
    Color tmp = color;
    tmp.setAlpha(std::min((uint8)m_opacity, color.a()));
    glColor4ubv(tmp.rgbaPtr());
}

void Graphics::bindTexture(const TexturePtr& texture)
{
    glBindTexture(GL_TEXTURE_2D, texture->getId());
}

void Graphics::startDrawing()
{
    assert(!m_drawing);
    glBegin(GL_QUADS);
    m_drawing = true;
}

void Graphics::stopDrawing()
{
    assert(m_drawing);
    glEnd();
    m_drawing = false;
}

