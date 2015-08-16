package com.otclient.mobile;

public class NativeMainThread {
    private static final NativeMainThread instance;
    private Thread nativeThread;
    
    static {
        instance = new NativeMainThread();
    }
    
    private NativeMainThread() {}
    
    public void start() {
        if( nativeThread == null ) {
            nativeThread = new Thread(
                    new NativeThread(), "NativeThread" );
            nativeThread.start();
        }
    }
    
    public static NativeMainThread getInstance() {
        return instance;
    }
    
    /*
     * Native methods implemented on C++
     */
    
    public native void nativeStartApp();
    
    private class NativeThread implements Runnable {

        @Override
        public void run() {
            nativeStartApp();
        }
    }
}
