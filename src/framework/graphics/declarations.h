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
class SpriteSheet;
class Pool;
class FramedPool;
class DrawPool;

using ImagePtr = stdext::shared_object_ptr<Image>;
using TexturePtr = stdext::shared_object_ptr<Texture>;
using AnimatedTexturePtr = stdext::shared_object_ptr<AnimatedTexture>;
using BitmapFontPtr = stdext::shared_object_ptr<BitmapFont>;
using CachedTextPtr = stdext::shared_object_ptr<CachedText>;
using FrameBufferPtr = stdext::shared_object_ptr<FrameBuffer>;
using ShaderPtr = stdext::shared_object_ptr<Shader>;
using ShaderProgramPtr = stdext::shared_object_ptr<ShaderProgram>;
using PainterShaderProgramPtr = stdext::shared_object_ptr<PainterShaderProgram>;
using ParticlePtr = stdext::shared_object_ptr<Particle>;
using ParticleTypePtr = stdext::shared_object_ptr<ParticleType>;
using ParticleEmitterPtr = stdext::shared_object_ptr<ParticleEmitter>;
using ParticleAffectorPtr = stdext::shared_object_ptr<ParticleAffector>;
using ParticleSystemPtr = stdext::shared_object_ptr<ParticleSystem>;
using ParticleEffectPtr = stdext::shared_object_ptr<ParticleEffect>;
using ParticleEffectTypePtr = stdext::shared_object_ptr<ParticleEffectType>;
using SpriteSheetPtr = stdext::shared_object_ptr<SpriteSheet>;
using ShaderList = std::vector<ShaderPtr>;

using PoolPtr = std::shared_ptr<Pool>;
using PoolFramedPtr = std::shared_ptr<FramedPool>;

#endif
