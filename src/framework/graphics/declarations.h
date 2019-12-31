/*
 * Copyright (c) 2010-2020 OTClient <https://github.com/edubart/otclient>
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
class ParticleType;
class ParticleEmitter;
class ParticleAffector;
class ParticleSystem;
class ParticleEffect;
class ParticleEffectType;

typedef stdext::shared_object_ptr<Image> ImagePtr;
typedef stdext::shared_object_ptr<Texture> TexturePtr;
typedef stdext::shared_object_ptr<AnimatedTexture> AnimatedTexturePtr;
typedef stdext::shared_object_ptr<BitmapFont> BitmapFontPtr;
typedef stdext::shared_object_ptr<CachedText> CachedTextPtr;
typedef stdext::shared_object_ptr<FrameBuffer> FrameBufferPtr;
typedef stdext::shared_object_ptr<Shader> ShaderPtr;
typedef stdext::shared_object_ptr<ShaderProgram> ShaderProgramPtr;
typedef stdext::shared_object_ptr<PainterShaderProgram> PainterShaderProgramPtr;
typedef stdext::shared_object_ptr<Particle> ParticlePtr;
typedef stdext::shared_object_ptr<ParticleType> ParticleTypePtr;
typedef stdext::shared_object_ptr<ParticleEmitter> ParticleEmitterPtr;
typedef stdext::shared_object_ptr<ParticleAffector> ParticleAffectorPtr;
typedef stdext::shared_object_ptr<ParticleSystem> ParticleSystemPtr;
typedef stdext::shared_object_ptr<ParticleEffect> ParticleEffectPtr;
typedef stdext::shared_object_ptr<ParticleEffectType> ParticleEffectTypePtr;
typedef std::vector<ShaderPtr> ShaderList;

#endif
