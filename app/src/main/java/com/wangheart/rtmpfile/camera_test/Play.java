package com.wangheart.rtmpfile.camera_test;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by LC on 2017/11/20.
 */

public class Play  implements SurfaceHolder.Callback {
    static{

        System.loadLibrary("recordSDK");
    }

    private SurfaceView surfaceView;



    @Override
    public void surfaceCreated(SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(SurfaceHolder surfaceHolder, int i, int i1, int i2) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder surfaceHolder) {

    }

    public native void play();
}
