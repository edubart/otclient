#include "graphics.h"
#include "logger.h"

#include <GL/gl.h>
#include <GL/glu.h>

Graphics g_graphics;

Graphics::Graphics()
{

}

Graphics::~Graphics()
{

}

void Graphics::init()
{
    // setup opengl
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // black background
    glEnable(GL_ALPHA_TEST); // enable alpha
    glAlphaFunc(GL_GREATER, 0.0f); // default alpha mode
    glDisable(GL_DEPTH_TEST); // we are rendering 2D only, we don't need it

    notice("GPU %s", (const char*)glGetString(GL_RENDERER));
    notice("OpenGL %s", (const char*)glGetString(GL_VERSION));
}

void Graphics::terminate()
{

}

void Graphics::resize(int width, int height)
{
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

}
