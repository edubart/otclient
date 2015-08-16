package com.otclient.mobile;

import android.os.Handler;
import android.os.Looper;
import android.view.Surface;

public class NativeFacadeCalls {
    private static NativeSurfaceView nativeSurfaceView;
    private static KeyboardSoftHandler keyboardSoftHandler;
    private static Handler handler;
           
    public static void initialize() {
        handler = new Handler(Looper.getMainLooper());
        keyboardSoftHandler = new KeyboardSoftHandler();
        nativeSurfaceView = new NativeSurfaceView();
        MainActivity.getInstance().addViewToLayout(nativeSurfaceView);
    }
    
    public static void destroy() {
        handler = null;
        nativeSurfaceView = null;
        keyboardSoftHandler = null;
    }
    
    public static boolean isSurfaceReady() {
        return nativeSurfaceView.isSurfaceReady();
    }
    
    /*
     * Static methods called from JNI
     */
    
    public static Surface getNativeSurface() {
        return nativeSurfaceView.getSurface();
    }
    
    public static void showKeyboardSoft() {
        handler.post(new Runnable() {
            @Override
            public void run() {
                keyboardSoftHandler.showKeyboardSoft();
            }
        });
    }
}
