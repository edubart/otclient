#ifndef ENGINE_H
#define ENGINE_H

#include <global.h>
#include "input.h"

class Engine
{
public:
    Engine() : m_stopping(false),
               m_running(false),
               m_calculateFps(false) { }

    void init();
    void terminate();

    /// Poll events
    void poll();
    /// Main loop
    void run();

    /// Stops main loop
    void stop();

    /// Change current game state
    bool isRunning() const { return m_running; }
    bool isStopping() const { return m_stopping; }

    /// Fired by platform on window close
    void onClose();
    /// Fired by platform on window resize
    void onResize(const Size& size);
    /// Fired by platform on mouse/keyboard input
    void onInputEvent(const InputEvent& event);

    /// Enable FPS counter on screen
    void enableFpsCounter(bool enable = true) { m_calculateFps = enable; };

    /// Return the current ticks on this frame
    int getCurrentFrameTicks() const { return m_lastFrameTicks; }

private:
    bool m_stopping;
    bool m_running;
    bool m_calculateFps;

    int m_lastFrameTicks;
};

extern Engine g_engine;

#endif // ENGINE_H

