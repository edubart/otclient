#ifndef UIWINDOW_H
#define UIWINDOW_H

#include <global.h>
#include <ui/uicontainer.h>

class UIWindow;
typedef std::shared_ptr<UIWindow> UIWindowPtr;

class UIWindow : public UIContainer
{
public:
    UIWindow() :
        UIContainer(UI::Window),
        m_moving(false) { }

    static UIWindowPtr create() { return UIWindowPtr(new UIWindow); }

    void onInputEvent(const InputEvent& event);

    void setTitle(const std::string& title) { m_title = title; }
    std::string getTitle() const { return m_title; }

    virtual const char *getLuaTypeName() const { return "UIWindow"; }

    virtual bool isFocusable() const { return true; }

private:
    std::string m_title;
    bool m_moving;
    Point m_movingReference;
};

#endif // UIWINDOW_H
