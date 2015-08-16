package com.otclient.mobile;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.KeyEvent;
import android.view.View;
import android.widget.RelativeLayout;
import android.widget.RelativeLayout.LayoutParams;

public class MainActivity extends Activity {
    public static final String APP_TAG = "OTClientMob";
    
    private static MainActivity instance;    
    private RelativeLayout layout;
    
    private static boolean started;
        
    static {
        started = false;
        //android.os.Debug.waitForDebugger();
        System.loadLibrary("otclient");
    }
    
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.v(APP_TAG, "onCreate()");
        super.onCreate(savedInstanceState);

        initialize();
    }
    
    private void initialize() {
        instance = this;
        
        layout = new RelativeLayout(this);
        RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(
                LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT);
        layout.setLayoutParams(params);
        
        setContentView(layout);
        
        if( !started ) {
            nativeInit();
            started = true;
        }
        
        NativeFacadeCalls.initialize();
    }
        
    @Override
    protected void onPause() {
        Log.v(APP_TAG, "onPause()");
        super.onPause();
        nativePause();
    }
    
    @Override
    protected void onResume() {
        Log.v(APP_TAG, "onResume()");
        super.onResume();
        
        if( NativeFacadeCalls.isSurfaceReady() )
            nativeResume();
    }
    
    @Override
    protected void onDestroy() {
        Log.v(APP_TAG, "onDestroy()");
        super.onDestroy();
        
        if(isFinishing()) {
            NativeFacadeCalls.destroy();
            destroy();
            nativeDestroy();
        }
    }
    
    private void destroy() {
        instance = null;
        layout = null;
    }
    
    @Override // Ignore certain special keys so they're handled by Android
    public boolean dispatchKeyEvent(KeyEvent event) {
        int keyCode = event.getKeyCode();
        if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
            keyCode == KeyEvent.KEYCODE_VOLUME_UP ||
            keyCode == KeyEvent.KEYCODE_CAMERA ||
            keyCode == 168 || /* API 11: KeyEvent.KEYCODE_ZOOM_IN */
            keyCode == 169 /* API 11: KeyEvent.KEYCODE_ZOOM_OUT */
            ) {
            return false;
        }
        return super.dispatchKeyEvent(event);
    }
    
    public static MainActivity getInstance() {
        return instance;
    }
    
    public void addViewToLayout(View view) {
        layout.addView(view);
    }
    
    public native void nativeInit();
    public native void nativePause();
    public native void nativeResume();
    public native void nativeDestroy();
}
