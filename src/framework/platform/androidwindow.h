/*
 * Copyright (c) 2010-2014 OTClient <https://github.com/edubart/otclient>
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

#ifndef ANDROIDWINDOW_H
#define ANDROIDWINDOW_H

#include "platformwindow.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <jni.h>
#include <EGL/egl.h>
#include <queue>

class AndroidWindow : public PlatformWindow
{
    enum KeyCode {
        KEY_UNDEFINED,
        KEY_BACKSPACE = 66,
        KEY_ENTER = 67
    };

    enum EventType {
        TOUCH_DOWN,
        TOUCH_UP,
        TOUCH_MOTION,
        TOUCH_LONGPRESS,
        KEY_DOWN,
        KEY_UP,
        TEXTINPUT,
        EVENT_UNDEFINED
    };

    enum NativeMessage {
        RECREATE_CONTEXT,
        APP_TERMINATE
    };

    struct NativeEvent {
        NativeEvent() {}

        NativeEvent(EventType type, float x, float y) :
            type(type), text(""), keyCode(KEY_UNDEFINED), x(x), y(y) {}

        NativeEvent(EventType type, std::string text) :
            type(type), text(text), keyCode(KEY_UNDEFINED), x(0), y(0) {}

        NativeEvent(EventType type, KeyCode keyCode) :
            type(type), text(""), keyCode(keyCode), x(0), y(0) {}

        static KeyCode getKeyCodeFromInt(int);
        static EventType getEventTypeFromInt(int);

        EventType type;
        std::string text;
        KeyCode keyCode;
        float x;
        float y;
    };

    void internalInitGL();
    void internalCheckGL();
    void internalChooseGL();
    void internalCreateGLContext();
    void internalDestroyGLContext();
    void internalConnectGLContext();

    void processKeyDownOrKeyUp();
    void processTextInput();
    void processFingerdownAndFingerup();
    void processFingermotion();

    void handleNativeEvents();

    JNIEnv* getJNIEnv();
    std::string getStringFromJString( jstring );

    // JNI Java calls
    void getJavaSurfaceAndSet();
public:
    AndroidWindow();
    ~AndroidWindow();

    void init();
    void terminate();

    void move(const Point& pos);
    void resize(const Size& size);
    void show();
    void hide();
    void maximize();
    void poll();
    void swapBuffers();
    void showMouse();
    void hideMouse();

    void setMouseCursor(int cursorId);
    void restoreMouseCursor();

    void setTitle(const std::string& title);
    void setMinimumSize(const Size& minimumSize);
    void setFullscreen(bool fullscreen);
    void setVerticalSync(bool enable);
    void setIcon(const std::string& iconFile);
    void setClipboardText(const std::string& text);

    Size getDisplaySize();
    std::string getClipboardText();
    std::string getPlatformType();

    void initializeJNI(JNIEnv*);
    void nativePause();
    void nativeResume();
    void nativeDestroy();
    void onNativeResize(int, int);
    void onNativeSurfaceChanged();
    void onNativeSurfaceDestroyed();
    void onNativeTouch(int actionType, float x, float y);
    void nativeCommitText(jstring);
    void onNativeKeyDown(int);
    void onNativeKeyUp(int);
    void showKeyboardSoft();
protected:
    int internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot);
private:
    JavaVM* m_javaVM;
    ANativeWindow* m_window;
    jclass m_nativeFacadeCalls;
    jmethodID m_midGetNativeSurface;
    jmethodID m_midShowKeyboardSoft;

    EGLConfig m_eglConfig;
    EGLContext m_eglContext;
    EGLDisplay m_eglDisplay;
    EGLSurface m_eglSurface;

    std::queue<NativeEvent> m_events;
    std::queue<NativeMessage> m_messages;
    NativeEvent m_currentEvent;
};

extern "C" {

void Java_com_otclient_mobile_NativeMainThread_nativeStartApp(
    JNIEnv*, jobject);

void Java_com_otclient_mobile_MainActivity_nativeInit(
    JNIEnv*, jobject);

void Java_com_otclient_mobile_MainActivity_nativeResume(
    JNIEnv*, jobject);

void Java_com_otclient_mobile_MainActivity_nativePause(
    JNIEnv*, jobject);

void Java_com_otclient_mobile_MainActivity_nativeDestroy(
    JNIEnv*, jobject);

void Java_com_otclient_mobile_NativeSurfaceView_onNativeResize(
    JNIEnv*, jobject, jint, jint);

void Java_com_otclient_mobile_NativeSurfaceView_onNativeSurfaceChanged(
    JNIEnv*, jobject);

void Java_com_otclient_mobile_NativeSurfaceView_onNativeSurfaceDestroyed(
    JNIEnv*, jobject);

void Java_com_otclient_mobile_NativeSurfaceView_onNativeTouch(
    JNIEnv* env, jobject obj, jint actionType, jfloat x, jfloat y);

void Java_com_otclient_mobile_InputConnectionForNative_nativeCommitText(
    JNIEnv*, jobject, jstring );

void Java_com_otclient_mobile_FakeEditText_onNativeKeyDown(
    JNIEnv*, jobject, jint );

void Java_com_otclient_mobile_FakeEditText_onNativeKeyUp(
    JNIEnv*, jobject, jint );

int main(int, char* argv[]);
}

#endif
