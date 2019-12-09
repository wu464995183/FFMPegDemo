package com.wangheart.rtmpfile;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.hardware.Camera;
import android.os.Bundle;
import android.view.SurfaceHolder;
import android.view.View;
import android.widget.FrameLayout;

import com.wangheart.rtmpfile.camera.CameraInterface;
import com.wangheart.rtmpfile.ffmpeg.FFmpegHandle;
import com.wangheart.rtmpfile.utils.FileUtil;
import com.wangheart.rtmpfile.utils.LogUtils;
import com.wangheart.rtmpfile.utils.PhoneUtils;
import com.wangheart.rtmpfile.view.MySurfaceView;

import org.jetbrains.annotations.Nullable;

import java.util.List;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

/**
 * Author : eric
 * CreateDate : 2017/11/6  10:57
 * Email : ericli_wang@163.com
 * Version : 2.0
 * Desc :
 * Modified :
 */

public class CameraFFmpegPushRtmpActivity extends Activity implements SurfaceHolder.Callback {
    private MySurfaceView sv;
    private final int WIDTH = 640;
    private int HEIGHT = 480;
    private SurfaceHolder mHolder;
    //    private String url = "rtmp://192.168.31.127/live/test";
    private String url = FileUtil.getMainDir() + "/ffmpeg.flv";
    //采集到每帧数据时间
    long previewTime = 0;
    //每帧开始编码时间
    long encodeTime = 0;
    //采集数量
    int count = 0;
    //编码数量
    int encodeCount = 0;
    //采集数据回调
    private StreamIt mStreamIt;


    @Override
    protected void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);
        init();
    }

    private void init() {
        sv = findViewById(R.id.sv);
        mStreamIt = new StreamIt();
        CameraInterface.getInstance().openCamera(1);
        Camera.Parameters params = CameraInterface.getInstance().getParams();
        params.setPictureFormat(ImageFormat.NV21);
        List<Camera.Size> list = params.getSupportedPictureSizes();
        for (Camera.Size size : list) {
            LogUtils.d(size.width + " " + size.height);
            if (size.width == WIDTH) {
                HEIGHT = size.height;
                break;
            }
        }
        params.setPictureSize(WIDTH, HEIGHT);
        params.setPreviewSize(WIDTH, HEIGHT);
        params.setPreviewFpsRange(30000, 30000);
        List<String> focusModes = params.getSupportedFocusModes();
        if (focusModes.contains("continuous-video")) {
            params.setFocusMode(Camera.Parameters.FOCUS_MODE_CONTINUOUS_VIDEO);
        }
        CameraInterface.getInstance().adjustOrientation(this, new CameraInterface.OnOrientationChangeListener() {
            @Override
            public void onChange(int degree) {
                FrameLayout.LayoutParams lp =
                        (FrameLayout.LayoutParams) sv.getLayoutParams();
                LogUtils.d(PhoneUtils.getWidth() + " " + PhoneUtils.getHeight());
                if (degree == 90) {
                    lp.height = PhoneUtils.getWidth() * WIDTH / HEIGHT;
                } else {
                    lp.height = PhoneUtils.getWidth() * HEIGHT / WIDTH;
                }
                sv.setLayoutParams(lp);
            }
        });

        CameraInterface.getInstance().resetParams(params);
        mHolder = sv.getHolder();
        mHolder.addCallback(this);
        FFmpegHandle.getInstance().initVideo(url, WIDTH, HEIGHT);
    }
    @Override
    protected void onDestroy() {
        super.onDestroy();
        FFmpegHandle.getInstance().close();
        CameraInterface.getInstance().releaseCamera();
    }

    @Override
    protected void onResume() {
        super.onResume();
        if (mHolder != null) {
            CameraInterface.getInstance().startPreview(mHolder, mStreamIt);
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
        CameraInterface.getInstance().stopPreview();
    }


    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        CameraInterface.getInstance().startPreview(mHolder, mStreamIt);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        CameraInterface.getInstance().stopPreview();
        CameraInterface.getInstance().releaseCamera();
    }

    ExecutorService executor = Executors.newSingleThreadExecutor();

    public void btnStart(View view) {
    }

    private boolean isEncode = true;

    public class StreamIt implements Camera.PreviewCallback {
        @Override
        public void onPreviewFrame(final byte[] data, Camera camera) {
            long endTime = System.currentTimeMillis();

            if (isEncode) {
                executor.execute(new Runnable() {
                    @Override
                    public void run() {
                        isEncode = false;
                        encodeTime = System.currentTimeMillis();
                        FFmpegHandle.getInstance().onFrameCallback(data);
                        LogUtils.w("编码第:" + (encodeCount++) + "帧，耗时:" + (System.currentTimeMillis() - encodeTime));
                        isEncode = true;
                    }
                });
            }


            LogUtils.d("采集第:" + (++count) + "帧，距上一帧间隔时间:"
                    + (endTime - previewTime) + "  " + Thread.currentThread().getName());
            previewTime = endTime;
        }
    }
}
