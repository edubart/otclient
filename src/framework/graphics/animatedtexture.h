#ifndef ANIMATEDTEXTURE_H
#define ANIMATEDTEXTURE_H

#include "texture.h"

class AnimatedTexture : public Texture
{
public:
    AnimatedTexture(int width, int height, int channels, int numFrames, uchar *framesPixels, int *framesDelay);
    virtual ~AnimatedTexture();

    void enableBilinearFilter();
    void processAnimation();

    AnimatedTexturePtr asAnimatedTexture() { return std::static_pointer_cast<AnimatedTexture>(shared_from_this()); }

private:
    std::vector<uint> m_framesTextureId;
    std::vector<int> m_framesDelay;
    int m_numFrames;
    int m_currentFrame;
    int m_lastAnimCheckTicks;
};

#endif
