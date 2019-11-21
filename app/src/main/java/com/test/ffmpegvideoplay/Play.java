package com.test.ffmpegvideoplay;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by LC on 2017/11/20.
 */

public class Play  implements SurfaceHolder.Callback {
    static{

        System.loadLibrary("native-lib");
    }

    private SurfaceView surfaceView;
    public   void playJava(final String path) {
        if (surfaceView == null) {
            return;
        }

        new Thread(new Runnable() {
            @Override
            public void run() {
                play(path);
            }
        }).start();
    }

    public void setSurfaceView(SurfaceView surfaceView) {
        this.surfaceView = surfaceView;
        display(surfaceView.getHolder().getSurface());
        surfaceView.getHolder().addCallback(this);

    }

    public native void initConvertImage();

    public native void convertImage(byte[] data);

    public native int play(String path);

    public native void display(Surface surface);


    public native void  release();

    public native void stop();

   public native int getTotalTime();

   public native double getCurrentPosition();

   public native void seekTo(int msec);

   public native void stepBack();//快退

    public native void stepUp();//快进


    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        display(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }
}
