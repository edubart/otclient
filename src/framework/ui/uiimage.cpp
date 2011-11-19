#include "uiimage.h"
#include <framework/otml/otmlnode.h>

void UIImage::draw(const Rect& screenCoords)
{

}

void UIImage::applyStyle(const OTMLNodePtr& styleNode)
{
    /*
    for(const OTMLNodePtr& node : styleNode->children()) {
        if(node->tag() == "image-source")
            setImageSource(node->value());
        else if(node->tag() == "image-clip")
            setImageClip(node->value<Rect>());
        else if(node->tag() == "image-rect")
            setImageRect(node->value<Rect>());
        else if(node->tag() == "image-fixed-ratio")
            setImageFixedRatio(node->value<bool>());
        else if(node->tag() == "image-repeated")
            setImageRepeated(node->value<bool>());
        else if(node->tag() == "image-smooth")
            setImageSmooth(node->value<bool>());
        else if(node->tag() == "image-color")
            setImageColor(node->value<Color>());
        else if(node->tag() == "image-border-top")
            setImageBorderTop(node->value<int>());
        else if(node->tag() == "image-border-right")
            setImageBorderRight(node->value<int>());
        else if(node->tag() == "image-border-bottom")
            setImageBorderBottom(node->value<int>());
        else if(node->tag() == "image-border-left")
            setImageBorderLeft(node->value<int>());
        else if(node->tag() == "image-border") {

        }
    }
    */
}

