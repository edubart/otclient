package com.otclient.mobile;

import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.BaseInputConnection;

public class InputConnectionForNative extends BaseInputConnection {

    public InputConnectionForNative(View targetView, boolean fullEditor) {
        super(targetView, fullEditor);
    }
    
    @Override // This handles the keycodes from soft keyboard
    public boolean sendKeyEvent(KeyEvent event) {
        int keyCode = event.getKeyCode();
        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            if (event.isPrintingKey()) {
                commitText(String.valueOf((char) event.getUnicodeChar()), 1);
            }
            FakeEditText.onNativeKeyDown(keyCode);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {
            FakeEditText.onNativeKeyUp(keyCode);
            return true;
        }
        return super.sendKeyEvent(event);
    }

    @Override // Typed text
    public boolean commitText(CharSequence text, int newCursorPosition) {
        nativeCommitText(text.toString(), newCursorPosition);

        return super.commitText(text, newCursorPosition);
    }
    
    @Override // Workaround to capture backspace key
    public boolean deleteSurroundingText(int beforeLength, int afterLength) {       
        if (beforeLength == 1 && afterLength == 0) {
            return super.sendKeyEvent(new KeyEvent(KeyEvent.ACTION_DOWN, KeyEvent.KEYCODE_DEL))
                && super.sendKeyEvent(new KeyEvent(KeyEvent.ACTION_UP, KeyEvent.KEYCODE_DEL));
        }

        return super.deleteSurroundingText(beforeLength, afterLength);
    }

    public static native void nativeCommitText(String text, int newCursorPosition);
}
