#ifndef FRAMEWORK_GRAPHICS_DECLARATIONS_H
#define FRAMEWORK_GRAPHICS_DECLARATIONS_H

#include <framework/global.h>

class Texture;
class Font;
class Image;
class BorderImage;
class FrameBuffer;

typedef std::weak_ptr<Texture> TextureWeakPtr;

typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<Font> FontPtr;
typedef std::shared_ptr<Image> ImagePtr;
typedef std::shared_ptr<BorderImage> BorderImagePtr;
typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;

#endif
