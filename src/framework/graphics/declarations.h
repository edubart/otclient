/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#ifndef FRAMEWORK_GRAPHICS_DECLARATIONS_H
#define FRAMEWORK_GRAPHICS_DECLARATIONS_H

#include <framework/global.h>
#include "glutil.h"

class Texture;
class TextureManager;
class Image;
class AnimatedTexture;
class BitmapFont;
class CachedText;
class FrameBuffer;
class FrameBufferManager;
class Shader;
class ShaderProgram;
class PainterShaderProgram;
class Particle;
class ParticleEmitter;
class ParticleAffector;
class ParticleSystem;
class ParticleEffect;
class ParticleEffectType;

typedef boost::intrusive_ptr<Image> ImagePtr;
typedef boost::intrusive_ptr<Texture> TexturePtr;
typedef boost::intrusive_ptr<AnimatedTexture> AnimatedTexturePtr;
typedef boost::intrusive_ptr<BitmapFont> BitmapFontPtr;
typedef boost::intrusive_ptr<CachedText> CachedTextPtr;
typedef boost::intrusive_ptr<FrameBuffer> FrameBufferPtr;
typedef boost::intrusive_ptr<Shader> ShaderPtr;
typedef boost::intrusive_ptr<ShaderProgram> ShaderProgramPtr;
typedef boost::intrusive_ptr<PainterShaderProgram> PainterShaderProgramPtr;
typedef boost::intrusive_ptr<Particle> ParticlePtr;
typedef boost::intrusive_ptr<ParticleEmitter> ParticleEmitterPtr;
typedef boost::intrusive_ptr<ParticleAffector> ParticleAffectorPtr;
typedef boost::intrusive_ptr<ParticleSystem> ParticleSystemPtr;
typedef boost::intrusive_ptr<ParticleEffect> ParticleEffectPtr;
typedef boost::intrusive_ptr<ParticleEffectType> ParticleEffectTypePtr;
typedef std::vector<ShaderPtr> ShaderList;

#endif
