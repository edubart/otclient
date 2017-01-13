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


#include "graphicalapplication.h"
#include <framework/core/clock.h>
#include <framework/core/eventdispatcher.h>
#include <framework/platform/platformwindow.h>
#include <framework/ui/uimanager.h>
#include <framework/graphics/graphics.h>
#include <framework/graphics/particlemanager.h>
#include <framework/graphics/texturemanager.h>
#include <framework/graphics/painter.h>
#include <framework/input/mouse.h>

#ifdef FW_SOUND
#include <framework/sound/soundmanager.h>
#endif

GraphicalApplication g_app;

void GraphicalApplication::init(std::vector<std::string>& args)
{
    Application::init(args);

    // setup platform window
    g_window.init();
    g_window.hide();
    g_window.setOnResize(std::bind(&GraphicalApplication::resize, this, std::placeholders::_1));
    g_window.setOnInputEvent(std::bind(&GraphicalApplication::inputEvent, this, std::placeholders::_1));
    g_window.setOnClose(std::bind(&GraphicalApplication::close, this));

    g_mouse.init();

    // initialize ui
    g_ui.init();

    // initialize graphics
    g_graphics.init();

    // fire first resize event
    resize(g_window.getSize());

#ifdef FW_SOUND
    // initialize sound
    g_sounds.init();
#endif
}

void GraphicalApplication::deinit()
{
    // hide the window because there is no render anymore
    g_window.hide();

    Application::deinit();
}

void GraphicalApplication::terminate()
{
    // destroy particles
    g_particles.terminate();

    // destroy any remaining widget
    g_ui.terminate();

    Application::terminate();
    m_terminated = false;

#ifdef FW_SOUND
    // terminate sound
    g_sounds.terminate();
#endif

    g_mouse.terminate();

    // terminate graphics
    m_foreground = nullptr;
    g_graphics.terminate();
    g_window.terminate();


    m_terminated = true;
}

void GraphicalApplication::run()
{
    m_running = true;

    // first clock update
    g_clock.update();

    // run the first poll
    poll();
    g_clock.update();

    // show window
    g_window.show();

    // run the second poll
    poll();
    g_clock.update();

    g_lua.callGlobalField("g_app", "onRun");

    while(!m_stopping) {
        // poll all events before rendering
        poll();

        if(g_window.isVisible()) {
            // the screen consists of two panes
            // background pane - high updated and animated pane (where the game are stuff happens)
            // foreground pane - steady pane with few animated stuff (UI)
            bool redraw = false;
            bool updateForeground = false;

            bool cacheForeground = g_graphics.canCacheBackbuffer() && m_foregroundFrameCounter.getMaxFps() != 0;

            if(m_backgroundFrameCounter.shouldProcessNextFrame()) {
                redraw = true;

                if(m_mustRepaint || m_foregroundFrameCounter.shouldProcessNextFrame()) {
                    m_mustRepaint = false;
                    updateForeground = true;
                }
            }

            if(redraw) {
                if(cacheForeground) {
                    Rect viewportRect(0, 0, g_painter->getResolution());

                    // draw the foreground into a texture
                    if(updateForeground) {
                        m_foregroundFrameCounter.processNextFrame();

                        // draw foreground
                        g_painter->setAlphaWriting(true);
                        g_painter->clear(Color::alpha);
                        g_ui.render(Fw::ForegroundPane);

                        // copy the foreground to a texture
                        m_foreground->copyFromScreen(viewportRect);

                        g_painter->clear(Color::black);
                        g_painter->setAlphaWriting(false);
                    }

                    // draw background (animated stuff)
                    m_backgroundFrameCounter.processNextFrame();
                    g_ui.render(Fw::BackgroundPane);

                    // draw the foreground (steady stuff)
                    g_painter->setColor(Color::white);
                    g_painter->setOpacity(1.0);
                    g_painter->drawTexturedRect(viewportRect, m_foreground, viewportRect);
                } else {
                    m_foregroundFrameCounter.processNextFrame();
                    m_backgroundFrameCounter.processNextFrame();
                    g_ui.render(Fw::BothPanes);
                }

                // update screen pixels
                g_window.swapBuffers();
            }

            // only update the current time once per frame to gain performance
            g_clock.update();

            if(m_backgroundFrameCounter.update())
                g_lua.callGlobalField("g_app", "onFps", m_backgroundFrameCounter.getLastFps());
            m_foregroundFrameCounter.update();

            int sleepMicros = m_backgroundFrameCounter.getMaximumSleepMicros();
            if(sleepMicros >= AdaptativeFrameCounter::MINIMUM_MICROS_SLEEP)
                stdext::microsleep(sleepMicros);

        } else {
            // sleeps until next poll to avoid massive cpu usage
            stdext::millisleep(POLL_CYCLE_DELAY+1);
            g_clock.update();
        }
    }

    m_stopping = false;
    m_running = false;
}

void GraphicalApplication::poll()
{
#ifdef FW_SOUND
    g_sounds.poll();
#endif

    // poll window input events
    g_window.poll();
    g_particles.poll();
    g_textures.poll();

    Application::poll();
}

void GraphicalApplication::close()
{
    m_onInputEvent = true;
    Application::close();
    m_onInputEvent = false;
}

void GraphicalApplication::resize(const Size& size)
{
    m_onInputEvent = true;
    g_graphics.resize(size);
    g_ui.resize(size);
    m_onInputEvent = false;

    if(g_graphics.canCacheBackbuffer()) {
        m_foreground = TexturePtr(new Texture(size));
        m_foreground->setUpsideDown(true);
    }
    m_mustRepaint = true;
}

void GraphicalApplication::inputEvent(const InputEvent& event)
{
    m_onInputEvent = true;
    g_ui.inputEvent(event);
    m_onInputEvent = false;
}
