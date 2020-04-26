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

#include "uiwidget.h"
#include "uihorizontallayout.h"
#include "uiverticallayout.h"
#include "uigridlayout.h"
#include "uianchorlayout.h"
#include "uitranslator.h"

#include <framework/graphics/painter.h>
#include <framework/graphics/texture.h>
#include <framework/graphics/texturemanager.h>

void UIWidget::initBaseStyle()
{
    m_backgroundColor = Color::alpha;
    m_borderColor.set(Color::black);
    m_iconColor = Color::white;
    m_color = Color::white;
    m_opacity = 1.0f;
    m_rotation = 0.0f;
    m_iconAlign = Fw::AlignNone;

    // generate an unique id, this is need because anchored layouts find widgets by id
    static unsigned long id = 1;
    m_id = stdext::format("widget%d", id++);
}

void UIWidget::parseBaseStyle(const OTMLNodePtr& styleNode)
{
    // parse lua variables and callbacks first
    for(const OTMLNodePtr& node : styleNode->children()) {
        // lua functions
        if(stdext::starts_with(node->tag(), "@")) {
            // load once
            if(m_firstOnStyle) {
                std::string funcName = node->tag().substr(1);
                std::string funcOrigin = "@" + node->source() + ": [" + node->tag() + "]";
                g_lua.loadFunction(node->value(), funcOrigin);
                luaSetField(funcName);
            }
        // lua fields value
        } else if(stdext::starts_with(node->tag(), "&")) {
            std::string fieldName = node->tag().substr(1);
            std::string fieldOrigin = "@" + node->source() + ": [" + node->tag() + "]";

            g_lua.evaluateExpression(node->value(), fieldOrigin);
            luaSetField(fieldName);
        }
    }
    // load styles used by all widgets
    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "color")
            setColor(node->value<Color>());
        else if(node->tag() == "x")
            setX(node->value<int>());
        else if(node->tag() == "y")
            setY(node->value<int>());
        else if(node->tag() == "pos")
            setPosition(node->value<Point>());
        else if(node->tag() == "width")
            setWidth(node->value<int>());
        else if(node->tag() == "height")
            setHeight(node->value<int>());
        else if(node->tag() == "rect")
            setRect(node->value<Rect>());
        else if(node->tag() == "background")
            setBackgroundColor(node->value<Color>());
        else if(node->tag() == "background-color")
            setBackgroundColor(node->value<Color>());
        else if(node->tag() == "background-offset-x")
            setBackgroundOffsetX(node->value<int>());
        else if(node->tag() == "background-offset-y")
            setBackgroundOffsetY(node->value<int>());
        else if(node->tag() == "background-offset")
            setBackgroundOffset(node->value<Point>());
        else if(node->tag() == "background-width")
            setBackgroundWidth(node->value<int>());
        else if(node->tag() == "background-height")
            setBackgroundHeight(node->value<int>());
        else if(node->tag() == "background-size")
            setBackgroundSize(node->value<Size>());
        else if(node->tag() == "background-rect")
            setBackgroundRect(node->value<Rect>());
        else if(node->tag() == "icon")
            setIcon(stdext::resolve_path(node->value(), node->source()));
        else if(node->tag() == "icon-source")
            setIcon(stdext::resolve_path(node->value(), node->source()));
        else if(node->tag() == "icon-color")
            setIconColor(node->value<Color>());
        else if(node->tag() == "icon-offset-x")
            setIconOffsetX(node->value<int>());
        else if(node->tag() == "icon-offset-y")
            setIconOffsetY(node->value<int>());
        else if(node->tag() == "icon-offset")
            setIconOffset(node->value<Point>());
        else if(node->tag() == "icon-width")
            setIconWidth(node->value<int>());
        else if(node->tag() == "icon-height")
            setIconHeight(node->value<int>());
        else if(node->tag() == "icon-size")
            setIconSize(node->value<Size>());
        else if(node->tag() == "icon-rect")
            setIconRect(node->value<Rect>());
        else if(node->tag() == "icon-clip")
            setIconClip(node->value<Rect>());
        else if(node->tag() == "icon-align")
            setIconAlign(Fw::translateAlignment(node->value()));
        else if(node->tag() == "opacity")
            setOpacity(node->value<float>());
        else if(node->tag() == "rotation")
            setRotation(node->value<float>());
        else if(node->tag() == "enabled")
            setEnabled(node->value<bool>());
        else if(node->tag() == "visible")
            setVisible(node->value<bool>());
        else if(node->tag() == "checked")
            setChecked(node->value<bool>());
        else if(node->tag() == "draggable")
            setDraggable(node->value<bool>());
        else if(node->tag() == "on")
            setOn(node->value<bool>());
        else if(node->tag() == "focusable")
            setFocusable(node->value<bool>());
        else if(node->tag() == "auto-focus")
            setAutoFocusPolicy(Fw::translateAutoFocusPolicy(node->value()));
        else if(node->tag() == "phantom")
            setPhantom(node->value<bool>());
        else if(node->tag() == "size")
            setSize(node->value<Size>());
        else if(node->tag() == "fixed-size")
            setFixedSize(node->value<bool>());
        else if(node->tag() == "clipping")
            setClipping(node->value<bool>());
        else if(node->tag() == "border") {
            auto split = stdext::split(node->value(), " ");
            if(split.size() == 2) {
                setBorderWidth(stdext::safe_cast<int>(split[0]));
                setBorderColor(stdext::safe_cast<Color>(split[1]));
            } else
                throw OTMLException(node, "border param must have its width followed by its color");
        }
        else if(node->tag() == "border-width")
            setBorderWidth(node->value<int>());
        else if(node->tag() == "border-width-top")
            setBorderWidthTop(node->value<int>());
        else if(node->tag() == "border-width-right")
            setBorderWidthRight(node->value<int>());
        else if(node->tag() == "border-width-bottom")
            setBorderWidthBottom(node->value<int>());
        else if(node->tag() == "border-width-left")
            setBorderWidthLeft(node->value<int>());
        else if(node->tag() == "border-color")
            setBorderColor(node->value<Color>());
        else if(node->tag() == "border-color-top")
            setBorderColorTop(node->value<Color>());
        else if(node->tag() == "border-color-right")
            setBorderColorRight(node->value<Color>());
        else if(node->tag() == "border-color-bottom")
            setBorderColorBottom(node->value<Color>());
        else if(node->tag() == "border-color-left")
            setBorderColorLeft(node->value<Color>());
        else if(node->tag() == "margin-top")
            setMarginTop(node->value<int>());
        else if(node->tag() == "margin-right")
            setMarginRight(node->value<int>());
        else if(node->tag() == "margin-bottom")
            setMarginBottom(node->value<int>());
        else if(node->tag() == "margin-left")
            setMarginLeft(node->value<int>());
        else if(node->tag() == "margin") {
            std::string marginDesc = node->value();
            std::vector<std::string> split = stdext::split(marginDesc, " ");
            if(split.size() == 4) {
                setMarginTop(stdext::safe_cast<int>(split[0]));
                setMarginRight(stdext::safe_cast<int>(split[1]));
                setMarginBottom(stdext::safe_cast<int>(split[2]));
                setMarginLeft(stdext::safe_cast<int>(split[3]));
            } else if(split.size() == 3) {
                int marginTop = stdext::safe_cast<int>(split[0]);
                int marginHorizontal = stdext::safe_cast<int>(split[1]);
                int marginBottom = stdext::safe_cast<int>(split[2]);
                setMarginTop(marginTop);
                setMarginRight(marginHorizontal);
                setMarginBottom(marginBottom);
                setMarginLeft(marginHorizontal);
            } else if(split.size() == 2) {
                int marginVertical = stdext::safe_cast<int>(split[0]);
                int marginHorizontal = stdext::safe_cast<int>(split[1]);
                setMarginTop(marginVertical);
                setMarginRight(marginHorizontal);
                setMarginBottom(marginVertical);
                setMarginLeft(marginHorizontal);
            } else if(split.size() == 1) {
                int margin = stdext::safe_cast<int>(split[0]);
                setMarginTop(margin);
                setMarginRight(margin);
                setMarginBottom(margin);
                setMarginLeft(margin);
            }
        }
        else if(node->tag() == "padding-top")
            setPaddingTop(node->value<int>());
        else if(node->tag() == "padding-right")
            setPaddingRight(node->value<int>());
        else if(node->tag() == "padding-bottom")
            setPaddingBottom(node->value<int>());
        else if(node->tag() == "padding-left")
            setPaddingLeft(node->value<int>());
        else if(node->tag() == "padding") {
            std::string paddingDesc = node->value();
            std::vector<std::string> split = stdext::split(paddingDesc, " ");
            if(split.size() == 4) {
                setPaddingTop(stdext::safe_cast<int>(split[0]));
                setPaddingRight(stdext::safe_cast<int>(split[1]));
                setPaddingBottom(stdext::safe_cast<int>(split[2]));
                setPaddingLeft(stdext::safe_cast<int>(split[3]));
            } else if(split.size() == 3) {
                int paddingTop = stdext::safe_cast<int>(split[0]);
                int paddingHorizontal = stdext::safe_cast<int>(split[1]);
                int paddingBottom = stdext::safe_cast<int>(split[2]);
                setPaddingTop(paddingTop);
                setPaddingRight(paddingHorizontal);
                setPaddingBottom(paddingBottom);
                setPaddingLeft(paddingHorizontal);
            } else if(split.size() == 2) {
                int paddingVertical = stdext::safe_cast<int>(split[0]);
                int paddingHorizontal = stdext::safe_cast<int>(split[1]);
                setPaddingTop(paddingVertical);
                setPaddingRight(paddingHorizontal);
                setPaddingBottom(paddingVertical);
                setPaddingLeft(paddingHorizontal);
            } else if(split.size() == 1) {
                int padding = stdext::safe_cast<int>(split[0]);
                setPaddingTop(padding);
                setPaddingRight(padding);
                setPaddingBottom(padding);
                setPaddingLeft(padding);
            }
        }
        // layouts
        else if(node->tag() == "layout") {
            std::string layoutType;
            if(node->hasValue())
                layoutType = node->value();
            else
                layoutType = node->valueAt<std::string>("type", "");

            if(!layoutType.empty()) {
                UILayoutPtr layout;
                if(layoutType == "horizontalBox")
                    layout = UIHorizontalLayoutPtr(new UIHorizontalLayout(static_self_cast<UIWidget>()));
                else if(layoutType == "verticalBox")
                    layout = UIVerticalLayoutPtr(new UIVerticalLayout(static_self_cast<UIWidget>()));
                else if(layoutType == "grid")
                    layout = UIGridLayoutPtr(new UIGridLayout(static_self_cast<UIWidget>()));
                else if(layoutType == "anchor")
                    layout = UIAnchorLayoutPtr(new UIAnchorLayout(static_self_cast<UIWidget>()));
                else
                    throw OTMLException(node, "cannot determine layout type");
                setLayout(layout);
            }

            if(node->hasChildren())
                m_layout->applyStyle(node);
        }
        // anchors
        else if(stdext::starts_with(node->tag(), "anchors.")) {
            UIWidgetPtr parent = getParent();
            if(!parent) {
                if(m_firstOnStyle)
                    throw OTMLException(node, "cannot create anchor, there is no parent widget!");
                else
                    continue;
            }

            UILayoutPtr layout = parent->getLayout();
            UIAnchorLayoutPtr anchorLayout;
            if(layout->isUIAnchorLayout())
                anchorLayout = layout->static_self_cast<UIAnchorLayout>();

            if(!anchorLayout)
                throw OTMLException(node, "cannot create anchor, the parent widget doesn't use anchor layout!");

            std::string what = node->tag().substr(8);
            if(what == "fill") {
                fill(node->value());
            } else if(what == "centerIn") {
                centerIn(node->value());
            } else {
                Fw::AnchorEdge anchoredEdge = Fw::translateAnchorEdge(what);

                if(node->value() == "none") {
                    removeAnchor(anchoredEdge);
                } else {
                    std::vector<std::string> split = stdext::split(node->value(), ".");
                    if(split.size() != 2)
                        throw OTMLException(node, "invalid anchor description");

                    std::string hookedWidgetId = split[0];
                    Fw::AnchorEdge hookedEdge = Fw::translateAnchorEdge(split[1]);

                    if(anchoredEdge == Fw::AnchorNone)
                        throw OTMLException(node, "invalid anchor edge");

                    if(hookedEdge == Fw::AnchorNone)
                        throw OTMLException(node, "invalid anchor target edge");

                    addAnchor(anchoredEdge, hookedWidgetId, hookedEdge);
                }
            }
        }
    }
}

void UIWidget::drawBackground(const Rect& screenCoords)
{
    if(m_backgroundColor.aF() > 0.0f) {
        Rect drawRect = screenCoords;
        drawRect.translate(m_backgroundRect.topLeft());
        if(m_backgroundRect.isValid())
            drawRect.resize(m_backgroundRect.size());
        g_painter->setColor(m_backgroundColor);
        g_painter->drawFilledRect(drawRect);
    }
}

void UIWidget::drawBorder(const Rect& screenCoords)
{
    // top
    if(m_borderWidth.top > 0) {
        g_painter->setColor(m_borderColor.top);

        Rect borderRect(screenCoords.topLeft(), screenCoords.width(), m_borderWidth.top);
        g_painter->drawFilledRect(borderRect);
    }
    // right
    if(m_borderWidth.right > 0) {
        g_painter->setColor(m_borderColor.right);

        Rect borderRect(screenCoords.topRight() - Point(m_borderWidth.right - 1, 0), m_borderWidth.right, screenCoords.height());
        g_painter->drawFilledRect(borderRect);
    }
    // bottom
    if(m_borderWidth.bottom > 0) {
        g_painter->setColor(m_borderColor.bottom);

        Rect borderRect(screenCoords.bottomLeft() - Point(0, m_borderWidth.bottom - 1), screenCoords.width(), m_borderWidth.bottom);
        g_painter->drawFilledRect(borderRect);
    }
    // left
    if(m_borderWidth.left > 0) {
        g_painter->setColor(m_borderColor.left);

        Rect borderRect(screenCoords.topLeft(), m_borderWidth.left, screenCoords.height());
        g_painter->drawFilledRect(borderRect);
    }
}

void UIWidget::drawIcon(const Rect& screenCoords)
{
    if(m_icon) {
        Rect drawRect;
        if(m_iconRect.isValid()) {
            drawRect = screenCoords;
            drawRect.translate(m_iconRect.topLeft());
            drawRect.resize(m_iconRect.size());
        } else {
            drawRect.resize(m_iconClipRect.size());

            if(m_iconAlign == Fw::AlignNone)
                drawRect.moveCenter(screenCoords.center());
            else
                drawRect.alignIn(screenCoords, m_iconAlign);
        }
        drawRect.translate(m_iconOffset);
        g_painter->setColor(m_iconColor);
        g_painter->drawTexturedRect(drawRect, m_icon, m_iconClipRect);
    }
}

void UIWidget::setIcon(const std::string& iconFile)
{
    if(iconFile.empty())
        m_icon = nullptr;
    else
        m_icon = g_textures.getTexture(iconFile);
    if(m_icon && !m_iconClipRect.isValid())
        m_iconClipRect = Rect(0, 0, m_icon->getSize());
}
