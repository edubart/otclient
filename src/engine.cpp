#include "engine.h"
#include "platform.h"
#include "graphics.h"
#include "const.h"
#include "input.h"

Engine g_engine;

Engine::Engine() :
    m_stopping(false),
    m_running(false),
    m_lastFrameTicks(0)
{
}

Engine::~Engine()
{
}

void Engine::init()
{
    Platform::init();

    int width = 640;
    int height = 480;

    // create the window
    Platform::createWindow(width, height, 550, 450);
    Platform::setWindowTitle(APP_NAME);
    Platform::setVsync();

    // initialize graphics stuff
    g_graphics.init();

    // finally show the window
    onResize(width, height);
    Platform::showWindow();
    Platform::hideMouseCursor();
}

void Engine::terminate()
{
    Platform::showMouseCursor();
    Platform::terminate();
    g_graphics.terminate();
}

void Engine::run()
{
    unsigned long ticks;

    m_running = true;
    m_lastFrameTicks = Platform::getTicks();

    while(!m_stopping) {
        // fire platform events
        Platform::poll();

        // update
        ticks = Platform::getTicks();
        update(ticks - m_lastFrameTicks);
        m_lastFrameTicks = ticks;

        // render
        render();

        // swap buffers
        Platform::swapBuffers();
    }

    m_lastFrameTicks = 0;
    m_stopping = false;
    m_running = false;
}

void Engine::stop()
{
    m_stopping = true;
}

void Engine::render()
{
    g_graphics.beginRender();
    g_graphics.endRender();
}

void Engine::update(int elapsedTicks)
{

}

void Engine::onClose()
{
    stop();
}

void Engine::onResize(int width, int height)
{

}

void Engine::onInputEvent(InputEvent *event)
{

}
