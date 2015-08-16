package com.otclient.mobile;

import android.content.Context;
import android.view.View;
import android.view.inputmethod.InputMethodManager;

public class KeyboardSoftHandler{
    private View editText;

    public KeyboardSoftHandler() {
        editText = new FakeEditText();
        MainActivity.getInstance()
            .addViewToLayout(editText);
    }
    
    public void showKeyboardSoft() {
        editText.setVisibility(View.VISIBLE);
        editText.requestFocus();

        InputMethodManager imm = (InputMethodManager) MainActivity
                .getInstance().getSystemService(Context.INPUT_METHOD_SERVICE);
        imm.showSoftInput(editText, 0);
    }

    public void hideKeyboardSoft() {
        editText.setVisibility(View.GONE);

        InputMethodManager imm = (InputMethodManager) MainActivity
                .getInstance().getSystemService(
                        Context.INPUT_METHOD_SERVICE);
        imm.hideSoftInputFromWindow(editText.getWindowToken(), 0);
    }
}
