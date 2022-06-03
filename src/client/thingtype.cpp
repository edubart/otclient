/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#include "thingtype.h"
#include "game.h"
#include "lightview.h"
#include "map.h"
#include "spriteappearances.h"
#include "spritemanager.h"

#include <framework/core/eventdispatcher.h>
#include <framework/core/filestream.h>
#include <framework/graphics/drawpool.h>
#include <framework/graphics/image.h>
#include <framework/graphics/texture.h>
#include <framework/otml/otml.h>

void ThingType::serialize(const FileStreamPtr& fin)
{
    for (int i = 0; i < ThingLastAttr; ++i) {
        if (!hasAttr(static_cast<ThingAttr>(i)))
            continue;

        int attr = i;
        if (g_game.getClientVersion() >= 780) {
            if (attr == ThingAttrChargeable)
                attr = ThingAttrWritable;
            else if (attr >= ThingAttrWritable)
                attr += 1;
        } else if (g_game.getClientVersion() >= 1000) {
            if (attr == ThingAttrNoMoveAnimation)
                attr = 16;
            else if (attr >= ThingAttrPickupable)
                attr += 1;
        }

        fin->addU8(attr);
        switch (attr) {
            case ThingAttrDisplacement:
            {
                fin->addU16(m_displacement.x);
                fin->addU16(m_displacement.y);
                break;
            }
            case ThingAttrLight:
            {
                const auto light = m_attribs.get<Light>(attr);
                fin->addU16(light.intensity);
                fin->addU16(light.color);
                break;
            }
            case ThingAttrMarket:
            {
                auto market = m_attribs.get<MarketData>(attr);
                fin->addU16(market.category);
                fin->addU16(market.tradeAs);
                fin->addU16(market.showAs);
                fin->addString(market.name);
                fin->addU16(market.restrictVocation);
                fin->addU16(market.requiredLevel);
                break;
            }
            case ThingAttrUsable:
            case ThingAttrElevation:
            case ThingAttrGround:
            case ThingAttrWritable:
            case ThingAttrWritableOnce:
            case ThingAttrMinimapColor:
            case ThingAttrCloth:
            case ThingAttrLensHelp:
                fin->addU16(m_attribs.get<uint16>(attr));
                break;
            default:
                break;
        }
    }
    fin->addU8(ThingLastAttr);

    fin->addU8(m_size.width());
    fin->addU8(m_size.height());

    if (m_size.width() > 1 || m_size.height() > 1)
        fin->addU8(m_realSize);

    fin->addU8(m_layers);
    fin->addU8(m_numPatternX);
    fin->addU8(m_numPatternY);
    fin->addU8(m_numPatternZ);
    fin->addU8(m_animationPhases);

    if (g_game.getFeature(Otc::GameEnhancedAnimations)) {
        if (m_animationPhases > 1 && m_animator != nullptr) {
            m_animator->serialize(fin);
        }
    }

    for (const int i : m_spritesIndex) {
        if (g_game.getFeature(Otc::GameSpritesU32))
            fin->addU32(i);
        else
            fin->addU16(i);
    }
}

void ThingType::unserializeAppearance(uint16 clientId, ThingCategory category, const appearances::Appearance& appearance)
{
    m_null = false;
    m_id = clientId;
    m_category = category;

    const appearances::AppearanceFlags& flags = appearance.flags();

    if (flags.has_bank()) {
        m_attribs.set<uint16_t>(ThingAttrGround, flags.bank().waypoints());
    }

    if (flags.has_clip()) {
        m_attribs.set(ThingAttrGroundBorder, flags.clip());
    }

    if (flags.has_bottom()) {
        m_attribs.set(ThingAttrOnBottom, flags.bottom());
    }

    if (flags.has_top()) {
        m_attribs.set(ThingAttrOnTop, flags.top());
    }

    if (flags.has_container()) {
        m_attribs.set(ThingAttrContainer, flags.container());
    }

    if (flags.has_cumulative()) {
        m_attribs.set(ThingAttrStackable, flags.cumulative());
    }

    if (flags.has_multiuse()) {
        m_attribs.set(ThingAttrMultiUse, flags.multiuse());
    }

    if (flags.has_forceuse()) {
        m_attribs.set(ThingAttrForceUse, flags.forceuse());
    }

    if (flags.has_write()) {
        m_attribs.set(ThingAttrWritable, flags.write().max_text_length());
    }

    if (flags.has_write_once()) {
        m_attribs.set(ThingAttrWritableOnce, flags.write_once().max_text_length_once());
    }

    if (flags.has_liquidpool()) {
        m_attribs.set(ThingAttrSplash, flags.liquidpool());
    }

    if (flags.has_unpass()) {
        m_attribs.set(ThingAttrNotWalkable, flags.unpass());
    }

    if (flags.has_unmove()) {
        m_attribs.set(ThingAttrNotMoveable, flags.unmove());
    }

    if (flags.has_unsight()) {
        m_attribs.set(ThingAttrBlockProjectile, flags.unsight());
    }

    if (flags.has_avoid()) {
        m_attribs.set(ThingAttrNotPathable, flags.avoid());
    }

    // no_movement_animation (?)

    if (flags.has_take()) {
        m_attribs.set(ThingAttrPickupable, flags.take());
    }

    if (flags.has_liquidcontainer()) {
        m_attribs.set(ThingAttrFluidContainer, flags.liquidcontainer());
    }

    if (flags.has_hang()) {
        m_attribs.set(ThingAttrHangable, flags.hang());
    }

    if (flags.has_hook()) {
        const auto hookDirection = flags.hook().direction();
        if (hookDirection == HOOK_TYPE_EAST) {
            m_attribs.set(ThingAttrHookEast, true);
        } else if (hookDirection == HOOK_TYPE_SOUTH) {
            m_attribs.set(ThingAttrHookSouth, true);
        }
    }

    if (flags.has_light()) {
        m_attribs.set(ThingAttrLight, Light(flags.light().brightness(), flags.light().color()));
    }

    if (flags.has_rotate()) {
        m_attribs.set(ThingAttrRotateable, flags.rotate());
    }

    if (flags.has_dont_hide()) {
        m_attribs.set(ThingAttrDontHide, flags.dont_hide());
    }

    if (flags.has_translucent()) {
        m_attribs.set(ThingAttrTranslucent, flags.translucent());
    }

    if (flags.has_shift()) {
        m_displacement = Point(flags.shift().x(), flags.shift().y());
        m_attribs.set(ThingAttrDisplacement, true);
    }

    if (flags.has_height()) {
        m_attribs.set<uint16_t>(ThingAttrElevation, flags.height().elevation());
    }

    if (flags.has_lying_object()) {
        m_attribs.set(ThingAttrLyingCorpse, flags.lying_object());
    }

    if (flags.has_animate_always()) {
        m_attribs.set(ThingAttrAnimateAlways, flags.animate_always());
    }

    if (flags.has_automap()) {
        m_attribs.set(ThingAttrMinimapColor, static_cast<uint16_t>(flags.automap().color()));
    }

    if (flags.has_lenshelp()) {
        m_attribs.set(ThingAttrLensHelp, flags.lenshelp().id());
    }

    if (flags.has_fullbank()) {
        m_attribs.set(ThingAttrFullGround, flags.fullbank());
    }

    if (flags.has_ignore_look()) {
        m_attribs.set(ThingAttrLook, flags.ignore_look());
    }

    if (flags.has_clothes()) {
        m_attribs.set(ThingAttrCloth, flags.clothes().slot());
    }

    // default action

    if (flags.has_market()) {
        MarketData market;
        market.category = flags.market().category();
        market.tradeAs = flags.market().trade_as_object_id();
        market.showAs = flags.market().show_as_object_id();
        market.name = flags.market().name();

        for (const int32_t voc : flags.market().restrict_to_profession()) {
            market.restrictVocation |= voc;
        }

        market.requiredLevel = flags.market().minimum_level();
        m_attribs.set(ThingAttrMarket, market);
    }

    if (flags.has_wrap()) {
        m_attribs.set(ThingAttrWrapable, flags.wrap());
    }

    if (flags.has_unwrap()) {
        m_attribs.set(ThingAttrUnwrapable, flags.unwrap());
    }

    if (flags.has_topeffect()) {
        m_attribs.set(ThingAttrTopEffect, flags.topeffect());
    }

    // npcsaledata
    // charged to expire
    // corpse
    // player_corpse
    // cyclopediaitem
    // ammo
    // show_off_socket
    // reportable

    if (flags.has_upgradeclassification()) {
        m_attribs.set<uint16_t>(ThingAttrUpgradeClassification, flags.upgradeclassification().upgrade_classification());
    }

    // now lets parse sprite data
    m_animationPhases = 0;
    int totalSpritesCount = 0;

    std::vector<Size> sizes;
    std::vector<int> total_sprites;

    for (const auto& framegroup : appearance.frame_group()) {
        const int frameGroupType = framegroup.fixed_frame_group();
        const auto& spriteInfo = framegroup.sprite_info();
        const auto& animation = spriteInfo.animation();
        const auto& sprites = spriteInfo.sprite_id();
        const auto& spritesPhases = animation.sprite_phase();

        m_numPatternX = spriteInfo.pattern_width();
        m_numPatternY = spriteInfo.pattern_height();
        m_numPatternZ = spriteInfo.pattern_depth();
        m_layers = spriteInfo.layers();

        m_animationPhases += std::max<int>(1, spritesPhases.size());

        SpriteSheetPtr sheet = g_spriteAppearances.getSheetBySpriteId(spriteInfo.sprite_id(0), false);
        if (sheet) {
            m_size = sheet->getSpriteSize() / SPRITE_SIZE;
            sizes.push_back(m_size);
        }

        // animations
        if (spritesPhases.size() > 1) {
            const auto animator = AnimatorPtr(new Animator);
            animator->unserializeAppearance(animation);

            if (frameGroupType == FrameGroupMoving)
                m_animator = animator;
            else if (frameGroupType == FrameGroupIdle || frameGroupType == FrameGroupInitial)
                m_idleAnimator = animator;
        }

        const int totalSprites = m_layers * m_numPatternX * m_numPatternY * m_numPatternZ * std::max<int>(1, spritesPhases.size());
        total_sprites.push_back(totalSprites);

        if (totalSpritesCount + totalSprites > 4096)
            stdext::throw_exception("a thing type has more than 4096 sprites");

        m_spritesIndex.resize(totalSpritesCount + totalSprites);
        for (int j = totalSpritesCount, spriteId = 0; j < (totalSpritesCount + totalSprites); ++j, ++spriteId) {
            m_spritesIndex[j] = spriteInfo.sprite_id(spriteId);
        }

        totalSpritesCount += totalSprites;
    }

    if (sizes.size() > 1) {
        // correction for some sprites
        for (auto& s : sizes) {
            m_size.setWidth(std::max<int>(m_size.width(), s.width()));
            m_size.setHeight(std::max<int>(m_size.height(), s.height()));
        }
        const size_t expectedSize = m_size.area() * m_layers * m_numPatternX * m_numPatternY * m_numPatternZ * m_animationPhases;
        if (expectedSize != m_spritesIndex.size()) {
            const std::vector sprites(std::move(m_spritesIndex));
            m_spritesIndex.clear();
            m_spritesIndex.reserve(expectedSize);
            for (size_t i = 0, idx = 0; i < sizes.size(); ++i) {
                const int totalSprites = total_sprites[i];
                if (m_size == sizes[i]) {
                    for (int j = 0; j < totalSprites; ++j) {
                        m_spritesIndex.push_back(sprites[idx++]);
                    }
                    continue;
                }
                const size_t patterns = (totalSprites / sizes[i].area());
                for (size_t p = 0; p < patterns; ++p) {
                    for (int x = 0; x < m_size.width(); ++x) {
                        for (int y = 0; y < m_size.height(); ++y) {
                            if (x < sizes[i].width() && y < sizes[i].height()) {
                                m_spritesIndex.push_back(sprites[idx++]);
                                continue;
                            }
                            m_spritesIndex.push_back(0);
                        }
                    }
                }
            }
        }
    }

    m_textures.resize(m_animationPhases);
    m_blankTextures.resize(m_animationPhases);
    m_smoothTextures.resize(m_animationPhases);
    m_texturesFramesRects.resize(m_animationPhases);
    m_texturesFramesOriginRects.resize(m_animationPhases);
    m_texturesFramesOffsets.resize(m_animationPhases);
}

void ThingType::unserialize(uint16 clientId, ThingCategory category, const FileStreamPtr& fin)
{
    m_null = false;
    m_id = clientId;
    m_category = category;

    int count = 0, attr = -1;
    bool done = false;
    for (int i = 0; i < ThingLastAttr; ++i) {
        ++count;
        attr = fin->getU8();
        if (attr == ThingLastAttr) {
            done = true;
            break;
        }

        if (g_game.getClientVersion() >= 1000) {
            /* In 10.10+ all attributes from 16 and up were
             * incremented by 1 to make space for 16 as
             * "No Movement Animation" flag.
             */
            if (attr == 16)
                attr = ThingAttrNoMoveAnimation;
            else if (attr == 254) { // Usable
                m_attribs.set(ThingAttrUsable, true);
                continue;
            } else if (attr == 35) { // Default Action
                m_attribs.set(ThingAttrDefaultAction, fin->getU16());
                continue;
            } else if (attr > 16)
                attr -= 1;
        } else if (g_game.getClientVersion() >= 860) {
            /* Default attribute values follow
             * the format of 8.6-9.86.
             * Therefore no changes here.
             */
        } else if (g_game.getClientVersion() >= 780) {
            /* In 7.80-8.54 all attributes from 8 and higher were
             * incremented by 1 to make space for 8 as
             * "Item Charges" flag.
             */
            if (attr == 8) {
                m_attribs.set(ThingAttrChargeable, true);
                continue;
            }
            if (attr > 8)
                attr -= 1;
        } else if (g_game.getClientVersion() >= 755) {
            /* In 7.55-7.72 attributes 23 is "Floor Change". */
            if (attr == 23)
                attr = ThingAttrFloorChange;
        } else if (g_game.getClientVersion() >= 740) {
            /* In 7.4-7.5 attribute "Ground Border" did not exist
             * attributes 1-15 have to be adjusted.
             * Several other changes in the format.
             */
            if (attr > 0 && attr <= 15)
                attr += 1;
            else if (attr == 16)
                attr = ThingAttrLight;
            else if (attr == 17)
                attr = ThingAttrFloorChange;
            else if (attr == 18)
                attr = ThingAttrFullGround;
            else if (attr == 19)
                attr = ThingAttrElevation;
            else if (attr == 20)
                attr = ThingAttrDisplacement;
            else if (attr == 22)
                attr = ThingAttrMinimapColor;
            else if (attr == 23)
                attr = ThingAttrRotateable;
            else if (attr == 24)
                attr = ThingAttrLyingCorpse;
            else if (attr == 25)
                attr = ThingAttrHangable;
            else if (attr == 26)
                attr = ThingAttrHookSouth;
            else if (attr == 27)
                attr = ThingAttrHookEast;
            else if (attr == 28)
                attr = ThingAttrAnimateAlways;

            /* "Multi Use" and "Force Use" are swapped */
            if (attr == ThingAttrMultiUse)
                attr = ThingAttrForceUse;
            else if (attr == ThingAttrForceUse)
                attr = ThingAttrMultiUse;
        }

        switch (attr) {
            case ThingAttrDisplacement:
            {
                if (g_game.getClientVersion() >= 755) {
                    m_displacement.x = fin->getU16();
                    m_displacement.y = fin->getU16();
                } else {
                    m_displacement.x = 8;
                    m_displacement.y = 8;
                }
                m_attribs.set(attr, true);
                break;
            }
            case ThingAttrLight:
            {
                Light light;
                light.intensity = fin->getU16();
                light.color = fin->getU16();
                m_attribs.set(attr, light);
                break;
            }
            case ThingAttrMarket:
            {
                MarketData market;
                market.category = fin->getU16();
                market.tradeAs = fin->getU16();
                market.showAs = fin->getU16();
                market.name = fin->getString();
                market.restrictVocation = fin->getU16();
                market.requiredLevel = fin->getU16();
                m_attribs.set(attr, market);
                break;
            }
            case ThingAttrElevation:
            {
                m_attribs.set<uint16_t>(attr, fin->getU16());
                break;
            }
            case ThingAttrUsable:
            case ThingAttrGround:
            case ThingAttrWritable:
            case ThingAttrWritableOnce:
            case ThingAttrMinimapColor:
            case ThingAttrCloth:
            case ThingAttrLensHelp:
                m_attribs.set(attr, fin->getU16());
                break;
            default:
                m_attribs.set(attr, true);
                break;
        }
    }

    if (!done)
        stdext::throw_exception(stdext::format("corrupt data (id: %d, category: %d, count: %d, lastAttr: %d)",
                                               m_id, m_category, count, attr));

    const bool hasFrameGroups = category == ThingCategoryCreature && g_game.getFeature(Otc::GameIdleAnimations);
    const uint8 groupCount = hasFrameGroups ? fin->getU8() : 1;

    m_animationPhases = 0;
    int totalSpritesCount = 0;

    std::vector<Size> sizes;
    std::vector<int> total_sprites;

    for (int i = 0; i < groupCount; ++i) {
        uint8 frameGroupType = FrameGroupDefault;
        if (hasFrameGroups)
            frameGroupType = fin->getU8();

        const uint8 width = fin->getU8();
        const uint8 height = fin->getU8();
        m_size = { width, height };
        sizes.push_back(m_size);
        if (width > 1 || height > 1) {
            m_realSize = fin->getU8();
            m_exactSize = std::min<int>(m_realSize, std::max<int>(width * SPRITE_SIZE, height * SPRITE_SIZE));
        } else
            m_exactSize = SPRITE_SIZE;

        m_layers = fin->getU8();
        m_numPatternX = fin->getU8();
        m_numPatternY = fin->getU8();
        if (g_game.getClientVersion() >= 755)
            m_numPatternZ = fin->getU8();
        else
            m_numPatternZ = 1;

        const int groupAnimationsPhases = fin->getU8();
        m_animationPhases += groupAnimationsPhases;

        if (groupAnimationsPhases > 1 && g_game.getFeature(Otc::GameEnhancedAnimations)) {
            const auto animator = AnimatorPtr(new Animator);
            animator->unserialize(groupAnimationsPhases, fin);

            if (frameGroupType == FrameGroupMoving)
                m_animator = animator;
            else if (frameGroupType == FrameGroupIdle)
                m_idleAnimator = animator;
        }

        const int totalSprites = m_size.area() * m_layers * m_numPatternX * m_numPatternY * m_numPatternZ * groupAnimationsPhases;
        total_sprites.push_back(totalSprites);

        if (totalSpritesCount + totalSprites > 4096)
            stdext::throw_exception("a thing type has more than 4096 sprites");

        m_spritesIndex.resize(totalSpritesCount + totalSprites);
        for (int j = totalSpritesCount; j < (totalSpritesCount + totalSprites); ++j)
            m_spritesIndex[j] = g_game.getFeature(Otc::GameSpritesU32) ? fin->getU32() : fin->getU16();

        totalSpritesCount += totalSprites;
    }

    if (sizes.size() > 1) {
        // correction for some sprites
        for (auto& s : sizes) {
            m_size.setWidth(std::max<int>(m_size.width(), s.width()));
            m_size.setHeight(std::max<int>(m_size.height(), s.height()));
        }
        const size_t expectedSize = m_size.area() * m_layers * m_numPatternX * m_numPatternY * m_numPatternZ * m_animationPhases;
        if (expectedSize != m_spritesIndex.size()) {
            const std::vector sprites(std::move(m_spritesIndex));
            m_spritesIndex.clear();
            m_spritesIndex.reserve(expectedSize);
            for (size_t i = 0, idx = 0; i < sizes.size(); ++i) {
                const int totalSprites = total_sprites[i];
                if (m_size == sizes[i]) {
                    for (int j = 0; j < totalSprites; ++j) {
                        m_spritesIndex.push_back(sprites[idx++]);
                    }
                    continue;
                }
                const size_t patterns = (totalSprites / sizes[i].area());
                for (size_t p = 0; p < patterns; ++p) {
                    for (int x = 0; x < m_size.width(); ++x) {
                        for (int y = 0; y < m_size.height(); ++y) {
                            if (x < sizes[i].width() && y < sizes[i].height()) {
                                m_spritesIndex.push_back(sprites[idx++]);
                                continue;
                            }
                            m_spritesIndex.push_back(0);
                        }
                    }
                }
            }
        }
    }

    m_textures.resize(m_animationPhases);
    m_blankTextures.resize(m_animationPhases);
    m_smoothTextures.resize(m_animationPhases);
    m_texturesFramesRects.resize(m_animationPhases);
    m_texturesFramesOriginRects.resize(m_animationPhases);
    m_texturesFramesOffsets.resize(m_animationPhases);
}

void ThingType::exportImage(const std::string_view fileName)
{
    if (m_null)
        stdext::throw_exception("cannot export null thingtype");

    if (m_spritesIndex.empty())
        stdext::throw_exception("cannot export thingtype without sprites");

    const ImagePtr image(new Image(Size(SPRITE_SIZE * m_size.width() * m_layers * m_numPatternX, SPRITE_SIZE * m_size.height() * m_animationPhases * m_numPatternY * m_numPatternZ)));
    for (int z = 0; z < m_numPatternZ; ++z) {
        for (int y = 0; y < m_numPatternY; ++y) {
            for (int x = 0; x < m_numPatternX; ++x) {
                for (int l = 0; l < m_layers; ++l) {
                    for (int a = 0; a < m_animationPhases; ++a) {
                        for (int w = 0; w < m_size.width(); ++w) {
                            for (int h = 0; h < m_size.height(); ++h) {
                                image->blit(Point(SPRITE_SIZE * (m_size.width() - w - 1 + m_size.width() * x + m_size.width() * m_numPatternX * l),
                                                  SPRITE_SIZE * (m_size.height() - h - 1 + m_size.height() * y + m_size.height() * m_numPatternY * a + m_size.height() * m_numPatternY * m_animationPhases * z)),
                                            g_sprites.getSpriteImage(m_spritesIndex[getSpriteIndex(w, h, l, x, y, z, a)]));
                            }
                        }
                    }
                }
            }
        }
    }

    image->savePNG(fileName);
}

void ThingType::unserializeOtml(const OTMLNodePtr& node)
{
    for (const OTMLNodePtr& node2 : node->children()) {
        if (node2->tag() == "opacity")
            m_opacity = node2->value<float>();
        else if (node2->tag() == "notprewalkable")
            m_attribs.set(ThingAttrNotPreWalkable, node2->value<bool>());
        else if (node2->tag() == "image")
            m_customImage = node2->value();
        else if (node2->tag() == "full-ground") {
            if (node2->value<bool>())
                m_attribs.set(ThingAttrFullGround, true);
            else
                m_attribs.remove(ThingAttrFullGround);
        }
    }
}

void ThingType::draw(const Point& dest, float scaleFactor, int layer, int xPattern, int yPattern, int zPattern, int animationPhase, TextureType textureType, Color color, LightView* lightView)
{
    if (m_null)
        return;

    if (animationPhase >= m_animationPhases)
        return;

    const TexturePtr& texture = getTexture(animationPhase, textureType); // texture might not exists, neither its rects.
    if (!texture)
        return;

    const uint frameIndex = getTextureIndex(layer, xPattern, yPattern, zPattern);
    if (frameIndex >= m_texturesFramesRects[animationPhase].size())
        return;

    const Point textureOffset = m_texturesFramesOffsets[animationPhase][frameIndex];
    const Rect textureRect = m_texturesFramesRects[animationPhase][frameIndex];

    const Rect screenRect(dest + (textureOffset - m_displacement - (m_size.toPoint() - Point(1)) * SPRITE_SIZE) * scaleFactor, textureRect.size() * scaleFactor);

    if (m_opacity < 1.0f)
        color = Color(1.0f, 1.0f, 1.0f, m_opacity);

    if (getCategory() == ThingCategoryMissile || isSingleGround()) {
        g_drawPool.forceGrouping(true);
    }

    g_drawPool.addTexturedRect(screenRect, texture, textureRect, color, dest);
    g_drawPool.forceGrouping(false);

    if (lightView && hasLight()) {
        const Light light = getLight();
        if (light.intensity > 0) {
            lightView->addLightSource(screenRect.center(), light);
        }
    }
}

TexturePtr ThingType::getTexture(int animationPhase, const TextureType txtType)
{
    if (m_null) {
        return nullptr;
    }

    const bool allBlank = txtType == TextureType::ALL_BLANK,
        smooth = txtType == TextureType::SMOOTH;

    TexturePtr& animationPhaseTexture = (
        allBlank ? m_blankTextures :
        smooth ? m_smoothTextures : m_textures)[animationPhase];

    if (animationPhaseTexture) return animationPhaseTexture;

    // we don't need layers in common items, they will be pre-drawn
    int textureLayers = 1;
    int numLayers = m_layers;
    if (m_category == ThingCategoryCreature && numLayers >= 2) {
        // 5 layers: outfit base, red mask, green mask, blue mask, yellow mask
        textureLayers = 5;
        numLayers = 5;
    }

    const bool useCustomImage = animationPhase == 0 && !m_customImage.empty();
    const int indexSize = textureLayers * m_numPatternX * m_numPatternY * m_numPatternZ;
    const Size textureSize = getBestTextureDimension(m_size.width(), m_size.height(), indexSize);
    const ImagePtr fullImage = useCustomImage ? Image::load(m_customImage) : ImagePtr(new Image(textureSize * SPRITE_SIZE));

    m_texturesFramesRects[animationPhase].resize(indexSize);
    m_texturesFramesOriginRects[animationPhase].resize(indexSize);
    m_texturesFramesOffsets[animationPhase].resize(indexSize);

    const bool protobufSupported = g_game.getProtocolVersion() >= 1281;

    static Color maskColors[] = { Color::red, Color::green, Color::blue, Color::yellow };

    for (int z = 0; z < m_numPatternZ; ++z) {
        for (int y = 0; y < m_numPatternY; ++y) {
            for (int x = 0; x < m_numPatternX; ++x) {
                for (int l = 0; l < numLayers; ++l) {
                    const bool spriteMask = m_category == ThingCategoryCreature && l > 0;
                    const int frameIndex = getTextureIndex(l % textureLayers, x, y, z);

                    Point framePos = Point(frameIndex % (textureSize.width() / m_size.width()) * m_size.width(),
                                           frameIndex / (textureSize.width() / m_size.width()) * m_size.height()) * SPRITE_SIZE;

                    if (!useCustomImage) {
                        if (protobufSupported) {
                            const uint spriteIndex = getSpriteIndex(-1, -1, spriteMask ? 1 : l, x, y, z, animationPhase);
                            ImagePtr spriteImage = g_sprites.getSpriteImage(m_spritesIndex[spriteIndex]);
                            if (!spriteImage) {
                                return nullptr;
                            }

                            // verifies that the first block in the lower right corner is transparent.
                            if (spriteImage->hasTransparentPixel()) {
                                fullImage->setTransparentPixel(true);
                            }

                            if (allBlank) {
                                spriteImage->overwrite(Color::white);
                            } else if (spriteMask) {
                                spriteImage->overwriteMask(maskColors[l - 1]);
                            }

                            fullImage->blit(framePos, spriteImage);
                        } else {
                            for (int h = 0; h < m_size.height(); ++h) {
                                for (int w = 0; w < m_size.width(); ++w) {
                                    const uint spriteIndex = getSpriteIndex(w, h, spriteMask ? 1 : l, x, y, z, animationPhase);
                                    ImagePtr spriteImage = g_sprites.getSpriteImage(m_spritesIndex[spriteIndex]);

                                    // verifies that the first block in the lower right corner is transparent.
                                    if (h == 0 && w == 0 && (!spriteImage || spriteImage->hasTransparentPixel())) {
                                        fullImage->setTransparentPixel(true);
                                    }

                                    if (spriteImage) {
                                        if (allBlank) {
                                            spriteImage->overwrite(Color::white);
                                        } else if (spriteMask) {
                                            spriteImage->overwriteMask(maskColors[l - 1]);
                                        }

                                        Point spritePos = Point(m_size.width() - w - 1,
                                                                m_size.height() - h - 1) * SPRITE_SIZE;

                                        fullImage->blit(framePos + spritePos, spriteImage);
                                    }
                                }
                            }
                        }
                    }

                    Rect drawRect(framePos + Point(m_size.width(), m_size.height()) * SPRITE_SIZE - Point(1), framePos);
                    for (int fx = framePos.x; fx < framePos.x + m_size.width() * SPRITE_SIZE; ++fx) {
                        for (int fy = framePos.y; fy < framePos.y + m_size.height() * SPRITE_SIZE; ++fy) {
                            const uint8* p = fullImage->getPixel(fx, fy);
                            if (p[3] != 0x00) {
                                drawRect.setTop(std::min<int>(fy, drawRect.top()));
                                drawRect.setLeft(std::min<int>(fx, drawRect.left()));
                                drawRect.setBottom(std::max<int>(fy, drawRect.bottom()));
                                drawRect.setRight(std::max<int>(fx, drawRect.right()));
                            }
                        }
                    }

                    m_texturesFramesRects[animationPhase][frameIndex] = drawRect;
                    m_texturesFramesOriginRects[animationPhase][frameIndex] = Rect(framePos, Size(m_size.width(), m_size.height()) * SPRITE_SIZE);
                    m_texturesFramesOffsets[animationPhase][frameIndex] = drawRect.topLeft() - framePos;
                }
            }
        }
    }

    if (m_opacity < 1.0f)
        fullImage->setTransparentPixel(true);

    m_opaque = !fullImage->hasTransparentPixel();

    animationPhaseTexture = TexturePtr(new Texture(fullImage, true, false, m_size.area() == 1 && !hasElevation(), false));
    if (smooth)
        animationPhaseTexture->setSmooth(true);

    return animationPhaseTexture;
}

Size ThingType::getBestTextureDimension(int w, int h, int count)
{
    int k = 1;
    while (k < w)
        k <<= 1;
    w = k;

    k = 1;
    while (k < h)
        k <<= 1;
    h = k;

    const int numSprites = w * h * count;
    assert(numSprites <= SPRITE_SIZE * SPRITE_SIZE);
    assert(w <= SPRITE_SIZE);
    assert(h <= SPRITE_SIZE);

    Size bestDimension = { SPRITE_SIZE };
    for (int i = w; i <= SPRITE_SIZE; i <<= 1) {
        for (int j = h; j <= SPRITE_SIZE; j <<= 1) {
            Size candidateDimension = { i, j };
            if (candidateDimension.area() < numSprites)
                continue;
            if ((candidateDimension.area() < bestDimension.area()) ||
               (candidateDimension.area() == bestDimension.area() && candidateDimension.width() + candidateDimension.height() < bestDimension.width() + bestDimension.height()))
                bestDimension = candidateDimension;
        }
    }

    return bestDimension;
}

uint ThingType::getSpriteIndex(int w, int h, int l, int x, int y, int z, int a)
{
    uint index = ((((((a % m_animationPhases)
                      * m_numPatternZ + z)
                     * m_numPatternY + y)
                    * m_numPatternX + x)
                   * m_layers + l)
                  * m_size.height() + h)
        * m_size.width() + w;

    if (w == -1 && h == -1) { // protobuf does not use width and height, because sprite image is the exact sprite size, not split by 32x32, so -1 is passed instead
        index = ((((a % m_animationPhases)
                   * m_numPatternZ + z)
                  * m_numPatternY + y)
                 * m_numPatternX + x)
            * m_layers + l;
    }

    assert(index < m_spritesIndex.size());
    return index;
}

uint ThingType::getTextureIndex(int l, int x, int y, int z)
{
    return ((l * m_numPatternZ + z)
            * m_numPatternY + y)
        * m_numPatternX + x;
}

int ThingType::getExactSize(int layer, int xPattern, int yPattern, int zPattern, int animationPhase)
{
    if (m_null)
        return 0;

    getTexture(animationPhase); // we must calculate it anyway.
    const int frameIndex = getTextureIndex(layer, xPattern, yPattern, zPattern);
    const Size size = m_texturesFramesOriginRects[animationPhase][frameIndex].size() - m_texturesFramesOffsets[animationPhase][frameIndex].toSize();
    return std::max<int>(size.width(), size.height());
}

void ThingType::setPathable(bool var)
{
    if (var == true)
        m_attribs.remove(ThingAttrNotPathable);
    else
        m_attribs.set(ThingAttrNotPathable, true);
}

int ThingType::getAnimationPhases()
{
    if (m_animator) return m_animator->getAnimationPhases();

    if (m_category == ThingCategoryCreature) return m_animationPhases - 1;

    return m_animationPhases;
}

int ThingType::getExactHeight()
{
    if (m_null)
        return 0;

    if (m_exactHeight != -1)
        return m_exactHeight;

    getTexture(0);
    const int frameIndex = getTextureIndex(0, 0, 0, 0);
    const Size size = m_texturesFramesOriginRects[0][frameIndex].size() - m_texturesFramesOffsets[0][frameIndex].toSize();

    return m_exactHeight = size.height();
}
