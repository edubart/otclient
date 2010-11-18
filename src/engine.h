#ifndef ENGINE_H
#define ENGINE_H

struct InputEvent;

class Engine
{
public:
    Engine();
    ~Engine();

    void init();
    void terminate();

    void run();
    void stop();

    bool isRunning() const { return m_running; }
    bool isStopping() const { return m_stopping; }
    unsigned long getLastFrameTicks() const { return m_lastFrameTicks; }

    // events fired by platform
    void onClose();
    void onResize(int width, int height);
    void onInputEvent(InputEvent *event);

private:
    void render();
    void update(int elapsedTicks);

    bool m_stopping;
    bool m_running;

    unsigned long m_lastFrameTicks;
};

extern Engine g_engine;

#endif // ENGINE_H

