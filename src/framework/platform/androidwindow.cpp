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

#ifdef ANDROID

#include "androidwindow.h"

AndroidWindow::AndroidWindow() {
    m_minimumSize = Size(600, 480);
    m_size = Size(600, 480);
    m_window = 0;

    m_keyMap[AndroidWindow::KEY_ENTER] = Fw::KeyEnter;
    m_keyMap[AndroidWindow::KEY_BACKSPACE] = Fw::KeyBackspace;
}

AndroidWindow::~AndroidWindow() {
    JNIEnv* env = getJNIEnv();
    env->DeleteGlobalRef(m_nativeFacadeCalls);

    internalDestroyGLContext();
}

AndroidWindow::KeyCode AndroidWindow::NativeEvent::getKeyCodeFromInt(int keyCode) {
    switch (keyCode) {
        case 66:
            return KEY_ENTER;
        case 67:
            return KEY_BACKSPACE;
        default:
            return KEY_UNDEFINED;
    }
}

AndroidWindow::EventType AndroidWindow::NativeEvent::getEventTypeFromInt(int actionType) {
    switch (actionType) {
        case 0:
            return TOUCH_DOWN;
        case 1:
            return TOUCH_UP;
        case 2:
            return TOUCH_MOTION;
        case 3:
            return TOUCH_LONGPRESS;
        default:
            return EVENT_UNDEFINED;
    }
}

JNIEnv* AndroidWindow::getJNIEnv() {
    JNIEnv *env;

    if (m_javaVM->AttachCurrentThread(&env, NULL) < 0) {
        g_logger.fatal("failed to attach current thread");
        return 0;
    }

    return env;
}

std::string AndroidWindow::getStringFromJString( jstring text ) {
    JNIEnv* env = getJNIEnv();

    const char* newChar = env->GetStringUTFChars(text,NULL);
    std::string newText = newChar;
    env->ReleaseStringUTFChars(text, newChar);

    return newText;
}

void AndroidWindow::getJavaSurfaceAndSet() {
    jobject nativeWindowObject;
    JNIEnv* env = getJNIEnv();

    nativeWindowObject = env->CallStaticObjectMethod(m_nativeFacadeCalls, m_midGetNativeSurface);
    m_window = ANativeWindow_fromSurface(env, nativeWindowObject);
    env->DeleteLocalRef(nativeWindowObject);
}

void AndroidWindow::internalInitGL() {
    internalCheckGL();
    internalChooseGL();
    internalConnectGLContext();
    internalCreateGLContext();
}

void AndroidWindow::internalCheckGL() {
    m_eglDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(m_eglDisplay == EGL_NO_DISPLAY)
        g_logger.fatal("EGL not supported");

    if(!eglInitialize(m_eglDisplay, NULL, NULL))
        g_logger.fatal("Unable to initialize EGL");
}

void AndroidWindow::internalChooseGL() {
    static int attrList[] = {
#if OPENGL_ES==2
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
#else
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES_BIT,
#endif
        EGL_RED_SIZE, 4,
        EGL_GREEN_SIZE, 4,
        EGL_BLUE_SIZE, 4,
        EGL_ALPHA_SIZE, 4,
        EGL_NONE
    };

    EGLint numConfig;

    if(!eglChooseConfig(m_eglDisplay, attrList, &m_eglConfig, 1, &numConfig))
        g_logger.fatal("Failed to choose EGL config");

    if(numConfig != 1)
        g_logger.warning("Didn't got the exact EGL config");

    EGLint vid;
    if(!eglGetConfigAttrib(m_eglDisplay, m_eglConfig, EGL_NATIVE_VISUAL_ID, &vid))
        g_logger.fatal("Unable to get visual EGL visual id");

    ANativeWindow_setBuffersGeometry(m_window, 0, 0, vid);
}

void AndroidWindow::internalCreateGLContext() {
    EGLint attrList[] = {
#if OPENGL_ES==2
        EGL_CONTEXT_CLIENT_VERSION, 2,
#else
        EGL_CONTEXT_CLIENT_VERSION, 1,
#endif
        EGL_NONE
    };

    m_eglContext = eglCreateContext(m_eglDisplay, m_eglConfig, EGL_NO_CONTEXT, attrList);
    if(m_eglContext == EGL_NO_CONTEXT )
        g_logger.fatal(stdext::format("Unable to create EGL context: %s", eglGetError()));

    if (!eglMakeCurrent(m_eglDisplay, m_eglSurface, m_eglSurface, m_eglContext))
        g_logger.fatal("Unable to connect EGL context into Android native window");
}

void AndroidWindow::internalDestroyGLContext() {
    if (m_window == NULL)
        return;

    if(m_eglDisplay) {
        eglMakeCurrent(m_eglDisplay, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

        if (m_eglSurface) {
            eglDestroySurface(m_eglDisplay, m_eglSurface);
            m_eglSurface = EGL_NO_SURFACE;
        }

        if(m_eglContext) {
            eglDestroyContext(m_eglDisplay, m_eglContext);
            m_eglContext = EGL_NO_CONTEXT;
        }

        eglTerminate(m_eglDisplay);
        m_eglDisplay = EGL_NO_DISPLAY;
    }
}

void AndroidWindow::internalConnectGLContext() {
    m_eglSurface = eglCreateWindowSurface(m_eglDisplay, m_eglConfig, m_window, NULL);
    if(m_eglSurface == EGL_NO_SURFACE)
        g_logger.fatal(stdext::format("Unable to create EGL surface: %s", eglGetError()));
}

void AndroidWindow::terminate() {
    nativePause();
}

void AndroidWindow::poll() {
    handleNativeEvents();

    while( !m_events.empty() ) {
        m_currentEvent = m_events.front();

        processKeyDownOrKeyUp();

        switch( m_currentEvent.type ) {
            case TEXTINPUT:
                processTextInput();
                break;
            case TOUCH_DOWN:
            case TOUCH_LONGPRESS:
            case TOUCH_UP:
                processFingerdownAndFingerup();
                break;
            case TOUCH_MOTION:
                processFingermotion();
        }

        m_events.pop();
    }

    fireKeysPress();
}

void AndroidWindow::processKeyDownOrKeyUp() {
    if(m_currentEvent.type == KEY_DOWN || m_currentEvent.type == KEY_UP) {
        Fw::Key keyCode = Fw::KeyUnknown;
        KeyCode key = m_currentEvent.keyCode;

        if(m_keyMap.find(key) != m_keyMap.end())
            keyCode = m_keyMap[key];

        if(m_currentEvent.type == KEY_DOWN)
            processKeyDown(keyCode);
        else if(m_currentEvent.type == KEY_UP)
            processKeyUp(keyCode);
    }
}

void AndroidWindow::processTextInput() {
    std::string text = m_currentEvent.text;
    KeyCode keyCode = m_currentEvent.keyCode;

    if(text.length() == 0 || keyCode == KEY_ENTER || keyCode == KEY_BACKSPACE)
        return;

    if(m_onInputEvent) {
        m_inputEvent.reset(Fw::KeyTextInputEvent);
        m_inputEvent.keyText = text;
        m_onInputEvent(m_inputEvent);
    }
}

void AndroidWindow::processFingerdownAndFingerup() {
    bool isTouchdown = m_currentEvent.type == TOUCH_DOWN;
    Fw::MouseButton mouseButton = (m_currentEvent.type == TOUCH_LONGPRESS) ?
        Fw::MouseRightButton : Fw::MouseLeftButton;

    m_inputEvent.reset();
    m_inputEvent.type = (isTouchdown) ? Fw::MousePressInputEvent : Fw::MouseReleaseInputEvent;
    m_inputEvent.mouseButton = Fw::MouseLeftButton;
    m_mouseButtonStates[mouseButton] = isTouchdown;

    Point newMousePos(m_currentEvent.x, m_currentEvent.y);
    m_inputEvent.mouseMoved = newMousePos - m_inputEvent.mousePos;
    m_inputEvent.mousePos = newMousePos;

    if(m_onInputEvent)
        m_onInputEvent(m_inputEvent);
}

void AndroidWindow::processFingermotion() {
    m_inputEvent.reset();
    m_inputEvent.type = Fw::MouseMoveInputEvent;

    Point newMousePos(m_currentEvent.x, m_currentEvent.y);
    m_inputEvent.mouseMoved = newMousePos - m_inputEvent.mousePos;
    m_inputEvent.mousePos = newMousePos;

    if (m_onInputEvent)
        m_onInputEvent(m_inputEvent);
}

void AndroidWindow::swapBuffers() {
    eglSwapBuffers(m_eglDisplay, m_eglSurface);
}

void AndroidWindow::setVerticalSync(bool enable) {
    eglSwapInterval(m_eglDisplay, enable ? 1 : 0);
}

std::string AndroidWindow::getClipboardText() {
    // TODO
    return "";
}

void AndroidWindow::setClipboardText(const std::string& text) {
    // TODO
}

Size AndroidWindow::getDisplaySize() {
    return m_size;
}

std::string AndroidWindow::getPlatformType() {
    return "ANDROID-EGL";
}

void AndroidWindow::init() {
    internalInitGL();
    nativeResume();
}

void AndroidWindow::show() {}

void AndroidWindow::hide() {}

void AndroidWindow::maximize() {}

void AndroidWindow::move(const Point& pos) {}

void AndroidWindow::resize(const Size& size) {}

void AndroidWindow::showMouse() {}

void AndroidWindow::hideMouse() {}

int AndroidWindow::internalLoadMouseCursor(const ImagePtr& image, const Point& hotSpot) {
    return 0;
}

void AndroidWindow::setMouseCursor(int cursorId) {}

void AndroidWindow::restoreMouseCursor() {}

void AndroidWindow::setTitle(const std::string& title) {}

void AndroidWindow::setMinimumSize(const Size& minimumSize) {}

void AndroidWindow::setFullscreen(bool fullscreen) {}

void AndroidWindow::setIcon(const std::string& iconFile) {}

void AndroidWindow::initializeJNI(JNIEnv* env) {
    env->GetJavaVM(&m_javaVM);
    env = getJNIEnv();

    jclass temp = m_nativeFacadeCalls = env->FindClass(
        "com/otclient/mobile/NativeFacadeCalls");
    m_nativeFacadeCalls = (jclass)env->NewGlobalRef(temp);

    m_midGetNativeSurface = env->GetStaticMethodID(m_nativeFacadeCalls,
        "getNativeSurface", "()Landroid/view/Surface;");

    m_midShowKeyboardSoft = env->GetStaticMethodID(m_nativeFacadeCalls,
        "showKeyboardSoft", "()V");
}

void AndroidWindow::nativePause() {
    m_visible = false;
}

void AndroidWindow::nativeResume() {
    m_visible = true;
}

void AndroidWindow::nativeDestroy() {
    m_messages.push(APP_TERMINATE);
}

void AndroidWindow::onNativeResize(int width, int height) {
    m_size = Size(width, height);
}

void AndroidWindow::nativeCommitText(jstring jString) {
    std::string text = getStringFromJString(jString);

    m_events.push(NativeEvent(TEXTINPUT, text));
}

void AndroidWindow::onNativeSurfaceChanged() {
    if( m_eglSurface == EGL_NO_SURFACE ) {
        if( m_window )
            ANativeWindow_release(m_window);

        getJavaSurfaceAndSet();
                
        m_messages.push(RECREATE_CONTEXT);

        nativeResume();
    }
}

void AndroidWindow::onNativeSurfaceDestroyed() {
    internalDestroyGLContext();
}

void AndroidWindow::onNativeTouch(int actionType,
                            float x, float y) {
    EventType type = NativeEvent::getEventTypeFromInt(actionType);
    
    m_events.push(NativeEvent(type, x, y));
}

void AndroidWindow::onNativeKeyDown( int keyCode ) {
    KeyCode key = NativeEvent::getKeyCodeFromInt(keyCode);
    
    m_events.push(NativeEvent(KEY_DOWN, key));
}

void AndroidWindow::onNativeKeyUp( int keyCode ) {
    KeyCode key = NativeEvent::getKeyCodeFromInt(keyCode);

    m_events.push(NativeEvent(KEY_UP, key));
}

// these events need to be processed on native thread.
void AndroidWindow::handleNativeEvents() {
    NativeMessage nativeMessage;

    while (!m_messages.empty()) {
        nativeMessage = m_messages.front();

        switch (nativeMessage) {
            case RECREATE_CONTEXT:
                if(!m_eglContext)
                    internalInitGL();
                break;
            case APP_TERMINATE:
                raise(SIGTERM);
                break;
        }

        m_messages.pop();
    }
}

void AndroidWindow::showKeyboardSoft() {
    JNIEnv* env = getJNIEnv();
    env->CallStaticVoidMethod(m_nativeFacadeCalls, m_midShowKeyboardSoft);
}

/*
 * Java JNI functions
*/

void Java_com_otclient_mobile_MainActivity_nativeInit(
    JNIEnv* env, jobject obj) {
    ((AndroidWindow&)g_window).initializeJNI(env);
}

void Java_com_otclient_mobile_MainActivity_nativePause(
    JNIEnv* env, jobject obj) {
    ((AndroidWindow&)g_window).nativePause();
}

void Java_com_otclient_mobile_MainActivity_nativeResume(
    JNIEnv* env, jobject obj) {
    ((AndroidWindow&)g_window).nativeResume();
}

void Java_com_otclient_mobile_MainActivity_nativeDestroy(
    JNIEnv* env, jobject obj) {
    ((AndroidWindow&)g_window).nativeDestroy();
}

void Java_com_otclient_mobile_NativeSurfaceView_onNativeResize(
    JNIEnv*, jobject, jint width, jint height) {
    ((AndroidWindow&)g_window).onNativeResize(width, height);
}

void Java_com_otclient_mobile_NativeSurfaceView_onNativeSurfaceChanged(
        JNIEnv* env, jobject obj) {
    ((AndroidWindow&) g_window).onNativeSurfaceChanged();
}

void Java_com_otclient_mobile_NativeSurfaceView_onNativeSurfaceDestroyed(
    JNIEnv* env, jobject obj) {
    ((AndroidWindow&)g_window).onNativeSurfaceDestroyed();
}

void Java_com_otclient_mobile_NativeSurfaceView_onNativeTouch(
        JNIEnv* env, jobject obj, jint actionType, jfloat x, jfloat y) {
    ((AndroidWindow&) g_window).onNativeTouch(actionType, x, y);
}

void Java_com_otclient_mobile_InputConnectionForNative_nativeCommitText(
        JNIEnv* env, jobject obj, jstring text ) {
    ((AndroidWindow&) g_window).nativeCommitText(text);
}

void Java_com_otclient_mobile_FakeEditText_onNativeKeyDown(
        JNIEnv* env, jobject obj, jint keyCode ) {
    ((AndroidWindow&) g_window).onNativeKeyDown(keyCode);
}

void Java_com_otclient_mobile_FakeEditText_onNativeKeyUp(
        JNIEnv* env, jobject obj, jint keyCode ) {
    ((AndroidWindow&) g_window).onNativeKeyUp(keyCode);
}

void Java_com_otclient_mobile_NativeMainThread_nativeStartApp(
        JNIEnv*, jobject ) {
    char *argv[2];
    argv[0] = strdup("OTClient");
    argv[1] = NULL;
    main(1, argv);
}

#endif // ANDROID
