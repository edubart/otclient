#ifndef UIELEMENTSKIN_H
#define UIELEMENTSKIN_H

#include <global.h>
#include <graphics/image.h>
#include <ui/uielement.h>
#include <graphics/font.h>
#include <otml/otmlnode.h>

class UIElementSkin
{
public:
    UIElementSkin(const std::string& name, UI::ElementType elementType) :
        m_name(name),
        m_elementType(elementType) { }
    UIElementSkin() : m_elementType(UI::Element) { }
    virtual ~UIElementSkin() { }

    /// Load the skin from a FML node
    virtual void load(OTMLNode* node);
    /// Apply the skin to an element
    virtual void apply(UIElement *element);
    /// Draw the skinned element
    virtual void draw(UIElement *element);

    const std::string& getName() const { return m_name; }
    const Size& getDefaultSize() const { return m_defaultSize; }
    UI::ElementType getElementType() const { return m_elementType; }
    ImagePtr getDefaultImage() const { return m_defaultImage; }
    FontPtr getFont() const { return m_font; }
    Color getFontColor() const { return m_fontColor; }

protected:
    ImagePtr loadImage(OTMLNode* node);
    FontPtr loadFont(OTMLNode* node);

private:
    std::string m_name;
    UI::ElementType m_elementType;
    Size m_defaultSize;
    ImagePtr m_defaultImage;
    FontPtr m_font;
    Color m_fontColor;
};

typedef boost::shared_ptr<UIElementSkin> UIElementSkinPtr;

#endif // UIELEMENTSKIN_H
