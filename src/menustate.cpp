/* The MIT License
 *
 * Copyright (c) 2010 OTClient, https://github.com/edubart/otclient
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


#include "menustate.h"
#include "graphics/framebuffer.h"
#include "graphics/graphics.h"
#include "graphics/textures.h"
#include "core/engine.h"
#include "graphics/fonts.h"
#include "core/dispatcher.h"
#include "ui/ui.h"
#include "net/connections.h"
#include "graphics/borderedimage.h"


void MenuState::onEnter()
{
    m_background = g_textures.get("background.png");
    m_background->enableBilinearFilter();

    UIElementPtr mainMenuPanel = UILoader::loadFile("ui/mainMenu.yml", UIContainer::getRootContainer());
/*
    UIButtonPtr button = std::static_pointer_cast<UIButton>(mainMenuPanel->getChildById("exitGame-button"));
    button->onClick([]{ g_engine.stop(); });
    
    button = std::static_pointer_cast<UIButton>(mainMenuPanel->getChildById("enterGame-button"));
    button->onClick([]{
        UIContainer::load("ui/enterGame-window.yml");
    });*/
}

void MenuState::onLeave()
{

}

void MenuState::onClose()
{
    g_engine.stop();
}

void MenuState::onInputEvent(const InputEvent& event)
{

}

void MenuState::onResize(const Size& size)
{

}

void MenuState::render()
{
    // render background
    static Size minTexCoordsSize(1240, 880);
    const Size& screenSize = g_graphics.getScreenSize();
    const Size& texSize = m_background->getSize();
    Size texCoordsSize = screenSize;
    if(texCoordsSize < minTexCoordsSize)
        texCoordsSize.scale(minTexCoordsSize, KEEP_ASPECT_RATIO_BY_EXPANDING);
    texCoordsSize = texCoordsSize.boundedTo(texSize);
    Rect texCoords(0, 0, texCoordsSize);
    texCoords.moveBottomRight(texSize.toPoint());
    g_graphics.drawTexturedRect(Rect(0, 0, screenSize), m_background, texCoords);
}

void MenuState::createMainMenu()
{
/*
    int y = 0;

    m_menuPanel = UIPanelPtr(new UIPanel);
    m_menuPanel->setSkin("roundedGridPanel");
    m_menuPanel->anchorLeft(g_ui->left());
    m_menuPanel->anchorBottom(g_ui->bottom());
    m_menuPanel->setSize(Size(118, 172));
    m_menuPanel->setMargin(0, 60, 70, 0);
    g_ui->addChild(m_menuPanel);

    // main menu
    UIButtonPtr enterGameButton = UIButtonPtr(new UIButton("Enter Game"));
    enterGameButton->anchorTop(m_menuPanel->top());
    enterGameButton->anchorHorizontalCenter(m_menuPanel->horizontalCenter());
    enterGameButton->setMargin(y += 16);
    m_menuPanel->addChild(enterGameButton);

    UIButtonPtr button = UIButtonPtr(new UIButton("Access Account"));
    button->anchorTop(m_menuPanel->top());
    button->anchorHorizontalCenter(m_menuPanel->horizontalCenter());
    button->setMargin(y += 30);
    m_menuPanel->addChild(button);

    button = UIButtonPtr(new UIButton("Options"));
    button->anchorTop(m_menuPanel->top());
    button->anchorHorizontalCenter(m_menuPanel->horizontalCenter());
    button->setMargin(y += 30);
    m_menuPanel->addChild(button);

    button = UIButtonPtr(new UIButton("Info"));
    button->anchorTop(m_menuPanel->top());
    button->anchorHorizontalCenter(m_menuPanel->horizontalCenter());
    button->setMargin(y += 30);
    m_menuPanel->addChild(button);

    button = UIButtonPtr(new UIButton("Exit Game"));
    button->anchorLeft(m_menuPanel->left());
    button->anchorTop(m_menuPanel->top());
    button->anchorHorizontalCenter(m_menuPanel->horizontalCenter());
    button->setMargin(y += 30);
    button->onClick([]{ g_engine.stop(); });
    m_menuPanel->addChild(button);

    // login window
    UIWindowPtr window(new UIWindow("Enter Game"));
    UIElementWeakPtr weakWindow(window);
    window->setSize(Size(236, 178));
    window->anchorHorizontalCenter(g_ui->horizontalCenter());
    window->anchorVerticalCenter(g_ui->verticalCenter());
    window->setVisible(false);
    g_ui->addChild(window);

    UILabelPtr label(new UILabel("Account name:"));
    label->anchorLeft(window->left());
    label->anchorTop(window->top());
    label->setMargin(18, 33);
    window->addChild(label);

    label = UILabelPtr(new UILabel("Password:"));
    label->anchorLeft(window->left());
    label->anchorTop(window->top());
    label->setMargin(18, 62);
    window->addChild(label);

    label = UILabelPtr(new UILabel("If you don't have\nan account yet:"));
    label->anchorLeft(window->left());
    label->anchorTop(window->top());
    label->setMargin(18, 87);
    window->addChild(label);

    button = UIButtonPtr(new UIButton("Create Account"));
    button->anchorLeft(window->left());
    button->anchorTop(window->top());
    button->setMargin(132, 94);
    window->addChild(button);

    button = UIButtonPtr(new UIButton("Ok"));
    button->setSize(Size(43, 20));
    button->anchorRight(window->right());
    button->anchorBottom(window->bottom());
    button->setMargin(0, 0, 10, 66);
    button->onClick([weakWindow]{
        UIElementPtr window = weakWindow.lock();
        if(window)
            window->setVisible(false);
    });
    window->addChild(button);

    button = UIButtonPtr(new UIButton("Cancel"));
    button->setSize(Size(43, 20));
    button->anchorRight(window->right());
    button->anchorBottom(window->bottom());
    button->setMargin(0, 0, 10, 13);
    button->onClick([weakWindow]{
        UIElementPtr window = weakWindow.lock();
        if(window)
            window->setVisible(false);
    });
    window->addChild(button);

    UITextEditPtr textEdit(new UITextEdit);
    textEdit->anchorRight(window->right());
    textEdit->anchorTop(window->top());
    textEdit->setMargin(32, 0, 0, 18);
    window->addChild(textEdit);

    textEdit = UITextEditPtr(new UITextEdit);
    textEdit->anchorRight(window->right());
    textEdit->anchorTop(window->top());
    textEdit->setMargin(61, 0, 0, 18);
    window->addChild(textEdit);

    enterGameButton->onClick([weakWindow]{
        UIElementPtr window = weakWindow.lock();
        if(window)
            window->setVisible(true);
    });
    */
}
