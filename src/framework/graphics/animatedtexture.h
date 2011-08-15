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

private:
    std::vector<uint> m_framesTextureId;
    std::vector<int> m_framesDelay;
    int m_numFrames;
    int m_currentFrame;
    int m_lastAnimCheckTicks;
};

typedef std::shared_ptr<AnimatedTexture> AnimatedTexturePtr;
typedef std::weak_ptr<AnimatedTexture> AnimatedTextureWeakPtr;

#endif
