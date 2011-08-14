#ifndef GRAPHICSDECLARATIONS_H
#define GRAPHICSDECLARATIONS_H

#include <global.h>

class Texture;
class Font;
class Image;
class BorderImage;
class TextArea;
class FrameBuffer;

typedef std::weak_ptr<Texture> TextureWeakPtr;

typedef std::shared_ptr<Texture> TexturePtr;
typedef std::shared_ptr<Font> FontPtr;
typedef std::shared_ptr<Image> ImagePtr;
typedef std::shared_ptr<BorderImage> BorderImagePtr;
typedef std::shared_ptr<TextArea> TextAreaPtr;
typedef std::shared_ptr<FrameBuffer> FrameBufferPtr;

#endif
