#include "sdlplatform.h"

void SDLPlatform::init() {
}

void SDLPlatform::terminate() {
}

void SDLPlatform::move(const Point& pos) {
}

void SDLPlatform::resize(const Size& size) {
}

void SDLPlatform::show() {
}

void SDLPlatform::hide() {
}

void SDLPlatform::maximize() {
}

void SDLPlatform::poll() {
}

void SDLPlatform::swapBuffers() {
}

void SDLPlatform::showMouse() {
}

void SDLPlatform::hideMouse() {
}

int SDLPlatform::internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot) {
    return 0;
}

void SDLPlatform::setMouseCursor(int cursorId) {
}

void SDLPlatform::restoreMouseCursor() {
}

void SDLPlatform::setTitle(const std::string& title) {
}

void SDLPlatform::setMinimumSize(const Size& minimumSize) {
}

void SDLPlatform::setFullscreen(bool fullscreen) {
}

void SDLPlatform::setVerticalSync(bool enable) {
}

void SDLPlatform::setIcon(const std::string& iconFile) {
}

void SDLPlatform::setClipboardText(const std::string& text) {
}

Size SDLPlatform::getDisplaySize() {
    Size TODO;
    return TODO;
}

std::string SDLPlatform::getClipboardText() {
    return nullptr;
}

std::string SDLPlatform::getPlatformType() {
    return nullptr;
}
