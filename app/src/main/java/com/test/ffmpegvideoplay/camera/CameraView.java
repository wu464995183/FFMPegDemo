package com.test.ffmpegvideoplay.camera;

import android.app.Activity;
import android.content.Context;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Point;
import android.hardware.Camera;
import android.os.Build;
import android.util.AttributeSet;
import android.view.Display;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.view.TextureView;
import android.view.ViewConfiguration;
import android.view.WindowManager;

import java.io.IOException;
import java.lang.reflect.Method;

public class CameraView extends SurfaceView {

    private int mWhichCamera;//0 后置 1 前置
    private Camera mCamera;
    private int mCameraId;
    private static final int MAX_PREVIEW_WIDTH = 1920;//Camera2 API 保证的最大预览宽高
    private static final int MAX_PREVIEW_HEIGHT = 1080;

    public CameraView(Context context) {
        super(context);
    }

    public CameraView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public CameraView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
    }


    private void createCamera() {
        mCamera = Camera.open(0);
        Camera.Parameters parameters = mCamera.getParameters();
        Point bestCameraSize = getBestCameraSize(parameters.getPreviewSize().width, parameters.getPreviewSize().height);
        parameters.setPreviewSize(bestCameraSize.x, bestCameraSize.y);
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90);
    }

    public static int getDisplayWidth(Activity context) {
        WindowManager windowManager = context.getWindowManager();
        Display display = windowManager.getDefaultDisplay();
        return display.getWidth();
    }

    public static int getDisplayHeight(Activity context) {
        WindowManager windowManager = context.getWindowManager();
        Display display = windowManager.getDefaultDisplay();
        if (hasNavigationBar(context)) {
            return display.getHeight() + getNavigationBarSize(context);
        }
        return display.getHeight();
    }

    public static boolean isTablet(final Context context) {
        return (context.getResources().getConfiguration().screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK) >= Configuration.SCREENLAYOUT_SIZE_LARGE;
    }

    public static int getNavigationBarSize(final Context context) {
        int size = 0;
        Resources resources = context.getResources();

        boolean isPortrait = true;
        int resourceId;
        if (isTablet(context)) {
            resourceId = resources.getIdentifier(isPortrait ? "navigation_bar_height" : "navigation_bar_height_landscape", "dimen", "android");
        } else {
            resourceId = resources.getIdentifier(isPortrait ? "navigation_bar_height" : "navigation_bar_width", "dimen", "android");
        }

        if (resourceId > 0) {
            size = resources.getDimensionPixelSize(resourceId);
        }

        if (size <= 0) {
            size = 100;
        }

        return size;
    }


    public static boolean hasNavigationBar(Context context) {
        Resources res = context.getResources();
        int resourceId = res.getIdentifier("config_showNavigationBar", "bool", "android");

        if (resourceId != 0) {
            String sNavBarOverride = "";
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
                try {
                    Class c = Class.forName("android.os.SystemProperties");
                    //noinspection unchecked
                    Method m = c.getDeclaredMethod("get", String.class);
                    m.setAccessible(true);
                    sNavBarOverride = (String) m.invoke(null, "qemu.hw.mainkeys");
                } catch (Throwable e) {
                    sNavBarOverride = "";
                }
            }

            boolean hasNav = res.getBoolean(resourceId);

            // Check override flag (see static block)
            if ("1".equals(sNavBarOverride)) {
                hasNav = false;
            } else if ("0".equals(sNavBarOverride)) {
                hasNav = true;
            }

            return hasNav;
        } else {
            return !ViewConfiguration.get(context).hasPermanentMenuKey();
        }
    }

    private Point getBestCameraSize(int orgWidth, int orgHeight) {
        Point point = new Point();
        int screenW = 1080;
        int screenH = 1900;
        double orgScale = (double) orgWidth / orgHeight;
        double screenScale = (double) screenW / screenH;
        double screenScale1 = (double) screenH / screenW;
        if (orgScale != screenScale && orgScale != screenScale1) {
            if (orgHeight > orgWidth) {
                point.y = orgHeight;
                point.x = (int) (orgHeight * screenScale);
            } else {
                if (orgWidth * screenScale > orgHeight) {
                    point.x = (int) (orgHeight / screenScale);
                    point.y = orgHeight;
                } else {
                    point.x = orgWidth;
                    point.y = (int) (orgWidth * screenScale);

                }
            }
        } else {
            point.x = orgWidth;
            point.y = orgHeight;
        }

        return point;
    }

    private void init() {

        getHolder().addCallback(new SurfaceHolder.Callback() {
            @Override
            public void surfaceCreated(SurfaceHolder holder) {
                try {
                    createCamera();
                    mCamera.setPreviewDisplay(holder);
                    mCamera.startPreview();
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            @Override
            public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {

            }

            @Override
            public void surfaceDestroyed(SurfaceHolder holder) {

            }
        });

    }

}
