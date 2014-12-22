#include "sdlwindow.h"

void SDLWindow::init() {
}

void SDLWindow::terminate() {
}

void SDLWindow::move(const Point& pos) {
    // android doesn't has window
}

void SDLWindow::resize(const Size& size) {
    // android doesn't resize window
}

void SDLWindow::show() {
    // android doesn't need to show activity, it's open automacally
}

void SDLWindow::hide() {

}

void SDLWindow::maximize() {
    // android doesn't has window
}

void SDLWindow::poll() {

}

void SDLWindow::swapBuffers() {
}

void SDLWindow::showMouse() {
    // android doesn't has mouse
}

void SDLWindow::hideMouse() {
    // android doesn't has mouse
}

int SDLWindow::internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot) {
    // android doesn't has mouse
    return 0;
}

void SDLWindow::setMouseCursor(int cursorId) {
    // android doesn't has mouse
}

void SDLWindow::restoreMouseCursor() {
    // android doesn't has mouse
}

void SDLWindow::setTitle(const std::string& title) {
    // android doesn't need to set title, the app make it
}

void SDLWindow::setMinimumSize(const Size& minimumSize) {
    // android doesn't has window
}

void SDLWindow::setFullscreen(bool fullscreen) {
    // android doesn't has window
}

void SDLWindow::setVerticalSync(bool enable) {
    // TODO
}

void SDLWindow::setIcon(const std::string& iconFile) {
    // android doesn't has window
}

void SDLWindow::setClipboardText(const std::string& text) {
}

Size SDLWindow::getDisplaySize() {
    Size TODO;
    return TODO;
}

std::string SDLWindow::getClipboardText() {
    return nullptr;
}

std::string SDLWindow::getPlatformType() {
    return "Android";
}
