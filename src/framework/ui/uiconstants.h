#ifndef UICONSTANTS_H
#define UICONSTANTS_H

namespace UI {
    enum ElementType {
        Element = 0,
        Container,
        Panel,
        Window,
        Label,
        TextEdit,
        Button,
        CheckBox,
        LineDecoration
    };

    enum AnchorPoint {
        AnchorNone = 0,
        AnchorTop,
        AnchorBottom,
        AnchorLeft,
        AnchorRight,
        AnchorVerticalCenter,
        AnchorHorizontalCenter,
    };
}

#endif  // UICONSTANTS_H
