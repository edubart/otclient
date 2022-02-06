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

 // GENERAL
#define SPRITE_SIZE 32

// APPEARANCES
#define BYTES_IN_SPRITE_SHEET 384 * 384 * 4
#define LZMA_UNCOMPRESSED_SIZE BYTES_IN_SPRITE_SHEET + 122
#define LZMA_HEADER_SIZE LZMA_PROPS_SIZE + 8
#define SPRITE_SHEET_WIDTH_BYTES 384 * 4

// TILE
#define MAX_ELEVATION 24
#define MAX_THINGS 10
#define RENDER_WALKING_CREATURES_BEHIND 0 // 1 to enable
#define TRANSPARENT_FLOOR_VIEW_RANGE 2

// CREATURE
#define SHIELD_BLINK_TICKS 500
#define VOLATILE_SQUARE_DURATION 1000
#define ADJUST_CREATURE_INFORMATION_BASED_ON_CROP_SIZE 0 // 1 to enable

// HIGHLIGHT
#define HIGHTLIGHT_FADE_START 60
#define HIGHTLIGHT_FADE_END 120

// WALK SYSTEM
// 1 - Nostalrius 7.2
// 0 - Nekiro TFS1.4-Downgrades and 7.72-downgrade, YurOTS 7.76, OTServ 0.6.4 (8.6), Evolutions (7.6), OTHire 7.2
#define FORCE_NEW_WALKING_FORMULA 0

// FLOOR
#define SEA_FLOOR 7
#define MAX_Z 15
#define UNDERGROUND_FLOOR SEA_FLOOR + 1
#define AWARE_UNDEGROUND_FLOOR_RANGE 2

// TICKS
#define INVISIBLE_TICKS_PER_FRAME 500
#define ITEM_TICKS_PER_FRAME 500
#define EFFECT_TICKS_PER_FRAME 75
#define MISSILE_TICKS_PER_FRAME 75

// TEXT
#define ANIMATED_TEXT_DURATION 1000
#define STATIC_DURATION_PER_CHARACTER 60
#define MIN_STATIC_TEXT_DURATION 3000

// ENCRYPTION SYSTEM
#define ENABLE_ENCRYPTION 0
#define ENCRYPTION_PASSWORD "SET_YOUR_PASSWORD_HERE"
