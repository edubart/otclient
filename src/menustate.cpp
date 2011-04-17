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


#include <prerequisites.h>
#include <core/engine.h>
#include <core/dispatcher.h>
#include <graphics/framebuffer.h>
#include <graphics/graphics.h>
#include <graphics/textures.h>
#include <graphics/borderedimage.h>
#include <graphics/fonts.h>
#include <ui/ui.h>
#include "menustate.h"

void MenuState::onEnter()
{
    m_background = g_textures.get("background.png");
    m_background->enableBilinearFilter();

    UIContainerPtr mainMenuPanel = UILoader::loadFile("ui/mainMenu.yml")->asUIContainer();

    UIButtonPtr button = boost::static_pointer_cast<UIButton>(mainMenuPanel->getChildById("exitGameButton"));
    button->setOnClick(boost::bind(&MenuState::onClose, this));

    button = boost::static_pointer_cast<UIButton>(mainMenuPanel->getChildById("enterGameButton"));
    button->setOnClick(boost::bind(&MenuState::enterGameButton_clicked, this));

    button = boost::static_pointer_cast<UIButton>(mainMenuPanel->getChildById("infoButton"));
    button->setOnClick(boost::bind(&MenuState::infoButton_clicked, this));

    button = boost::static_pointer_cast<UIButton>(mainMenuPanel->getChildById("optionsButton"));
    button->setOnClick(boost::bind(&MenuState::optionsButton_clicked, this));
}

void MenuState::onLeave()
{

}

void MenuState::onClose()
{
    g_engine.stop();
}

bool MenuState::onInputEvent(const InputEvent& event)
{
    return false;
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

void MenuState::enterGameButton_clicked()
{
    UIWindowPtr window = boost::static_pointer_cast<UIWindow>(UILoader::loadFile("ui/enterGameWindow.yml"));
    window->getParent()->lockElement(window);

    UIButtonPtr button = boost::static_pointer_cast<UIButton>(window->getChildById("cancelButton"));
    button->setOnClick([] {
        UIWindowPtr window = boost::static_pointer_cast<UIWindow>(UIContainer::getRootContainer()->getChildById("enterGameWindow"));
        window->getParent()->unlockElement();
        window->destroy();
    });

    button = boost::static_pointer_cast<UIButton>(window->getChildById("okButton"));
    button->setOnClick(boost::bind(&MenuState::enterGameWindowOkButton_clicked, this));
}

void MenuState::infoButton_clicked()
{
    UIWindowPtr window = boost::static_pointer_cast<UIWindow>(UILoader::loadFile("ui/infoWindow.yml"));
    window->getParent()->lockElement(window);
    UIButtonPtr button = boost::static_pointer_cast<UIButton>(window->getChildById("okButton"));
    button->setOnClick([] {
        UIWindowPtr window = boost::static_pointer_cast<UIWindow>(UIContainer::getRootContainer()->getChildById("infoWindow"));
        window->getParent()->unlockElement();
        window->destroy();
    });
}

void MenuState::enterGameWindowOkButton_clicked()
{
    UIContainerPtr enterGameWindow = boost::static_pointer_cast<UIContainer>(UIContainer::getRootContainer()->getChildById("enterGameWindow"));

    std::string accountName = boost::static_pointer_cast<UITextEdit>(enterGameWindow->getChildById("accountNameTextEdit"))->getText();
    std::string password = boost::static_pointer_cast<UITextEdit>(enterGameWindow->getChildById("passwordTextEdit"))->getText();

    //m_protocolLogin = ProtocolLoginPtr(new ProtocolLogin);
    //m_protocolLogin->login(accountName, password);
}

void MenuState::optionsButton_clicked()
{
    UIWindowPtr window = boost::static_pointer_cast<UIWindow>(UILoader::loadFile("ui/optionsWindow.yml"));
    window->getParent()->lockElement(window);
    UIButtonPtr button = boost::static_pointer_cast<UIButton>(window->getChildById("okButton"));
    button->setOnClick([] {
        UIWindowPtr window = boost::static_pointer_cast<UIWindow>(UIContainer::getRootContainer()->getChildById("optionsWindow"));
        window->getParent()->unlockElement();
        window->destroy();
    });
}
