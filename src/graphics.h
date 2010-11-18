#ifndef GRAPHICS_H
#define GRAPHICS_H

class Graphics
{
public:
    Graphics();
    ~Graphics();

    void init();
    void terminate();

    void resize(int width, int height);

    void beginRender();
    void endRender();
};

extern Graphics g_graphics;

#endif // GRAPHICS_H
