#include <global.h>
#include <core/resources.h>
#include <ui/ui.h>
#include <ui/uiloader.h>
#include <script/luainterface.h>
#include <otml/otml.h>
#include <ui/uianchorlayout.h>
#include <util/translator.h>

#include <boost/algorithm/string.hpp>

UILoader g_uiLoader;

UIElementPtr UILoader::createElementFromId(const std::string& id)
{
    UIElementPtr element;

    std::vector<std::string> split;
    boost::split(split, id, boost::is_any_of(std::string("#")));
    if(split.size() != 2) {
        return element;
    }

    std::string elementType = split[0].substr(1);
    std::string elementId = split[1];

    if(elementType == "panel")
        element = UIElementPtr(new UIContainer(UI::Panel));
    else if(elementType == "button")
        element = UIElementPtr(new UIButton);
    else if(elementType == "label")
        element = UIElementPtr(new UILabel);
    else if(elementType == "window")
        element = UIElementPtr(new UIWindow);
    else if(elementType == "textEdit")
        element = UIElementPtr(new UITextEdit);
    else if(elementType == "lineDecoration")
        element = UIElementPtr(new UIElement(UI::LineDecoration));
    else if(elementType == "checkBox")
        element = UIElementPtr(new UICheckBox);

    if(element)
        element->setId(elementId);

    return element;
}

UIElementPtr UILoader::loadFromFile(std::string fileName, const UIContainerPtr& parent)
{
    UIElementPtr element;

    if(!boost::ends_with(".otml", fileName))
        fileName += ".otml";

    std::stringstream fin;
    if(!g_resources.loadFile(fileName, fin))
        return element;

    try {
        OTMLParser parser(fin, fileName);
        OTMLNode* doc = parser.getDocument();

        // get element id
        std::string elementId = doc->front()->tag();

        // first we should populate all elements
        // only after that we can load anchors

        // create element interpreting it's id
        element = createElementFromId(elementId);
        if(!element) {
            logError(doc->front()->generateErrorMessage("invalid root element type"));
            return element;
        }
        parent->addChild(element);

        // populete it
        if(element->asUIContainer())
            populateContainer(element->asUIContainer(), doc->front());

        // now do the real load
        loadElements(element, doc->front());

        // report onLoad events
        element->onLoad();
    } catch(OTMLException e) {
        logError("ERROR: Failed to load ui ", g_resources.resolvePath(fileName) ,": ", e.what());
    }

    return element;
}

void UILoader::populateContainer(const UIContainerPtr& parent, OTMLNode* node)
{
    // populate ordered elements
    foreach(OTMLNode* cnode, *node) {
        std::string id = cnode->tag();
        if(id[0] == '%') {
            UIElementPtr element = createElementFromId(id);
            if(!element) {
                logError(cnode->generateErrorMessage("invalid element type"));
                continue;
            }
            parent->addChild(element);

            // also populate this element if it's a parent
            if(element->asUIContainer())
                populateContainer(element->asUIContainer(), cnode);
        }
    }
}

void UILoader::loadElements(const UIElementPtr& parent, OTMLNode* node)
{
    loadElement(parent, node);

    if(UIContainerPtr container = parent->asUIContainer()) {
        foreach(const UIElementPtr& element, container->getChildren()) {
            foreach(OTMLNode* cnode, *node) {
                // node found, load it
                if(boost::ends_with(cnode->tag(), "#" + element->getId())) {
                    loadElements(element, cnode);
                    break;
                }
            }
        }
    }
}

void UILoader::loadElement(const UIElementPtr& element, OTMLNode* node)
{
    // set element skin
    if(OTMLNode* cnode = node->at("skin")) {
        if(cnode->hasValue()) {
            element->setSkin(g_uiSkins.getElementSkin(element->getElementType(), cnode->value()));
        } else {
            UIElementSkinPtr skin = UIElementSkinPtr(new UIElementSkin());
            skin->load(cnode);
            element->setSkin(skin);
        }
    } else // apply default skin
        element->applyDefaultSkin();

    // load size
    Size size = element->getSize();
    size = node->readAt("size", size);
    size.setWidth(node->readAt("width", size.width()));
    size.setHeight(node->readAt("height", size.height()));
    if(size.isValid())
        element->setSize(size);

    // load margins
    element->setMarginLeft(node->readAtPath("margin/left", 0));
    element->setMarginRight(node->readAtPath("margin/right", 0));
    element->setMarginTop(node->readAtPath("margin/top", 0));
    element->setMarginBottom(node->readAtPath("margin/bottom", 0));

    // load anchors
    loadElementAnchor(element, AnchorLeft, node->atPath("anchors/left"));
    loadElementAnchor(element, AnchorRight, node->atPath("anchors/right"));
    loadElementAnchor(element, AnchorTop, node->atPath("anchors/top"));
    loadElementAnchor(element, AnchorBottom, node->atPath("anchors/bottom"));
    loadElementAnchor(element, AnchorHorizontalCenter, node->atPath("anchors/horizontalCenter"));
    loadElementAnchor(element, AnchorVerticalCenter, node->atPath("anchors/verticalCenter"));

    // load basic element events
    loadElementScriptFunction(element, node->at("onLoad"));

    // load specific element type
    switch(element->getElementType()) {
        case UI::Button:
            loadButton(std::static_pointer_cast<UIButton>(element), node);
            break;
        case UI::Window:
            loadWindow(std::static_pointer_cast<UIWindow>(element), node);
            break;
        case UI::Label:
            loadLabel(std::static_pointer_cast<UILabel>(element), node);
            break;
        case UI::TextEdit:
            loadTextEdit(std::static_pointer_cast<UITextEdit>(element), node);
            break;
        default:
            break;
     }
}

void UILoader::loadElementAnchor(const UIElementPtr& anchoredElement, AnchorPoint anchoredEdge, OTMLNode* node)
{
    if(!node)
        return;

    std::string anchorDescription = node->value();
    if(anchorDescription.empty()) {
        logError(node->generateErrorMessage("anchor is empty, did you forget to fill it?"));
        return;
    }

    UIAnchorLayoutPtr layout = std::dynamic_pointer_cast<UIAnchorLayout>(anchoredElement->getLayout());
    if(!layout) {
        logError(node->generateErrorMessage("could not add anchor, because this element does not participate of an anchor layout"));
        return;
    }

    std::vector<std::string> split;
    boost::split(split, anchorDescription, boost::is_any_of(std::string(".")));
    if(split.size() != 2) {
        logError(node->generateErrorMessage("invalid anchor"));
        return;
    }

    std::string anchorLineElementId = split[0];
    AnchorPoint anchorLineEdge = UIAnchorLayout::parseAnchorPoint(split[1]);

    if(anchorLineEdge == AnchorNone) {
        logError(node->generateErrorMessage("invalid anchor type"));
        return;
    }

    if(!layout->addAnchor(anchoredElement, anchoredEdge, AnchorLine(anchorLineElementId, anchorLineEdge)))
        logError(node->generateErrorMessage("anchoring failed"));
}

void UILoader::loadElementScriptFunction(const UIElementPtr& element, OTMLNode* node)
{
    if(!node)
        return;

    std::string functionDesc;
    functionDesc += g_resources.resolvePath(node->what()) + ":" + element->getId();
    functionDesc += "[" + node->tag() + "]";

    LuaValuePtr function = g_lua.loadFunction(node->value(), functionDesc);
    if(function->isFunction())
        element->setField(node->tag(), function);
    else
        logError(node->generateErrorMessage("failed to parse inline lua script"));
}

void UILoader::loadButton(const UIButtonPtr& button, OTMLNode* node)
{
    button->setText(node->valueAt("text"));
    loadElementScriptFunction(button, node->at("onClick"));
}

void UILoader::loadWindow(const UIWindowPtr& window, OTMLNode* node)
{
    window->setTitle(node->readAt("title", std::string()));
}

void UILoader::loadLabel(const UILabelPtr& label, OTMLNode* node)
{
    label->setText(node->readAt("text", std::string()));
    label->setAlign(parseAlignment(node->readAt("align", std::string("left"))));
}

void UILoader::loadTextEdit(const UITextEditPtr& textEdit, OTMLNode* node)
{
    textEdit->setText(node->readAt("text", std::string()));
}
