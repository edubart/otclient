package com.otclient.mobile;

import android.util.Log;
import android.view.GestureDetector;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.View;

public class NativeSurfaceView extends SurfaceView implements
    SurfaceHolder.Callback, View.OnTouchListener {
    
    private Surface surface;
    private GestureDetector gestureDetector;
    private int currentWidth;
    private int currentHeight;
    private boolean surfaceReady;
    
    private final int LONGPRESS_EVENT = 3;
    
    public NativeSurfaceView() {
        super(MainActivity.getInstance());
        getHolder().addCallback(this);
        setFocusable(true);
        setFocusableInTouchMode(true);
        requestFocus();
        setOnTouchListener(this);
                
        currentWidth = 0;
        currentHeight = 0;
        surfaceReady = false;
        
        gestureDetector = new GestureDetector(
                new GestureDetector.SimpleOnGestureListener() {
            public void onLongPress(MotionEvent event) {
                onNativeTouch(LONGPRESS_EVENT, event.getX(), event.getY());
            }
        });
    }

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        Log.d(MainActivity.APP_TAG, "surfaceCreated");
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width,
            int height) {
        Log.d(MainActivity.APP_TAG, "surfaceChanged");
        
        surface = holder.getSurface();
        
        currentWidth = width;
        currentHeight = height;
        surfaceReady = true;
        
        onNativeResize(width, height);
        onNativeSurfaceChanged();
        
        NativeMainThread.getInstance().start();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        Log.d(MainActivity.APP_TAG, "surfaceDestroyed");
        surface = null;
        surfaceReady = false;
        
        onNativeSurfaceDestroyed();
    }
    
    @Override
    public boolean onTouch(View view, MotionEvent event) {
        gestureDetector.onTouchEvent(event);
        /* Ref: http://developer.android.com/training/gestures/multi.html */
        final int pointerCount = event.getPointerCount();
        int action = event.getActionMasked();
        int i = -1;
        float x,y;
        
        switch(action) {
            case MotionEvent.ACTION_MOVE:
                for (i = 0; i < pointerCount; i++) {
                    x = event.getX(i);
                    y = event.getY(i);
                    onNativeTouch(action, x, y);
                }
                break;
            
            case MotionEvent.ACTION_UP:
            case MotionEvent.ACTION_DOWN:
                // Primary pointer up/down, the index is always zero
                i = 0;
            case MotionEvent.ACTION_POINTER_UP:
            case MotionEvent.ACTION_POINTER_DOWN:
                // Non primary pointer up/down
                if (i == -1) {
                    i = event.getActionIndex();
                }
                
                x = event.getX(i);
                y = event.getY(i);
                onNativeTouch(action, x, y);
                break;
            
            default:
                break;
        }

        return true;
    }
    
    public Surface getSurface() {
        return surface;
    }
    
    public boolean isSurfaceReady() {
        return surfaceReady;
    }
    
    /*
     * Native methods implemented on C++
     */
    public native void onNativeSurfaceChanged();
    public native void onNativeSurfaceDestroyed();
    public native void onNativeResize(int width, int height);
    public native void onNativeTouch(int actionType, float x, float y);
}
