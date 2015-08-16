package com.otclient.mobile;

import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;

public class FakeEditText extends View implements View.OnKeyListener {
    InputConnection ic;

    public FakeEditText() {
        super(MainActivity.getInstance());
        setFocusableInTouchMode(true);
        setFocusable(true);
        setOnKeyListener(this);
    }

    @Override
    public boolean onCheckIsTextEditor() {
        return true;
    }

    @Override // This handles the hardware keyboard input
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        if (event.isPrintingKey()) {
            if (event.getAction() == KeyEvent.ACTION_DOWN) {
                ic.commitText(String.valueOf((char) event.getUnicodeChar()), 1);
            }
            return true;
        }

        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            onNativeKeyDown(keyCode);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {
            onNativeKeyUp(keyCode);
            return true;
        }

        return false;
    }   
    
    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        ic = new InputConnectionForNative(this, true);
        
        return ic;
    }

    public static native void onNativeKeyDown(int keyCode);
    public static native void onNativeKeyUp(int keyCode);
}