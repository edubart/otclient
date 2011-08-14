#ifndef UILINEEDIT_H
#define UILINEEDIT_H

#include "uiwidget.h"
#include <graphics/textarea.h>

class UILineEdit : public UIWidget
{
public:
    UILineEdit();

    static UILineEditPtr create();

    virtual void loadStyleFromOTML(const OTMLNodePtr& styleNode);
    virtual void render();

    void setText(const std::string& text) { m_textArea.setText(text); }
    std::string getText() const { return m_textArea.getText(); }

protected:
    virtual void onGeometryUpdate();

private:
    //TODO: move textarea to here
    TextArea m_textArea;
};

#endif
