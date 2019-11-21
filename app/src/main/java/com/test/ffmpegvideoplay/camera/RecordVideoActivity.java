package com.test.ffmpegvideoplay.camera;

import android.app.Activity;
import android.graphics.ImageFormat;
import android.graphics.Point;
import android.hardware.Camera;
import android.media.MediaRecorder;
import android.os.Bundle;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

import com.test.ffmpegvideoplay.Play;
import com.test.ffmpegvideoplay.R;

import java.io.File;
import java.io.IOException;

public class RecordVideoActivity extends Activity {

    private TextView mUpCancelTip;
    private TextView mCancelTip;
    private SurfaceView mPreview;
    private Camera mCamera;
    private MediaRecorder mRecorder;
    private String mPath;
    private int mCameraPosition = 1;
    private Play play;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_camera);

        mPreview = (SurfaceView) findViewById(R.id.record_preview);

        File file = new File("/sdcard/Download/wu/testDI.h264");
        file.delete();
        try {
            file.createNewFile();
        } catch (IOException e) {
            e.printStackTrace();
        }


        play = new Play();

        play.initConvertImage();

        setListener();
    }

    private void createCamera() {
        mCamera = Camera.open(mCameraPosition);
        Camera.Parameters parameters = mCamera.getParameters();
        Point bestCameraSize = getBestCameraSize(parameters.getPreviewSize().width, parameters.getPreviewSize().height);
        parameters.setPreviewSize(bestCameraSize.x, bestCameraSize.y);
        mCamera.setParameters(parameters);
        mCamera.setDisplayOrientation(90);

        int buffSize = parameters.getPreviewSize().width * parameters.getPreviewSize().height * ImageFormat.getBitsPerPixel(ImageFormat.NV21) / 8;
        mCamera.addCallbackBuffer(new byte[buffSize]);

        mCamera.setPreviewCallbackWithBuffer(new Camera.PreviewCallback() {
            @Override
            public void onPreviewFrame(byte[] data, Camera camera) {
//                Log.e("test", data.length + "  previewcallback");
                mCamera.addCallbackBuffer(data);


                play.convertImage(data);



            }
        });

    }

    private void setListener() {


        mPreview.getHolder().addCallback(new SurfaceHolder.Callback() {
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

    @Override
    protected void onPause() {
        super.onPause();
        releaseResource();
        finish();
    }

    private void releaseResource() {
        try {
            if (mRecorder != null) {
                try {
                    mRecorder.setOnErrorListener(null);
                    mRecorder.setOnInfoListener(null);
                    mRecorder.setPreviewDisplay(null);
                    mRecorder.stop();
                } catch (Exception e) {
                }

                mRecorder.release();
                mRecorder = null;
            }

            if (mCamera != null) {
                mCamera.lock();
                mCamera.stopPreview();
                mCamera.release();
                mCamera = null;
            }
        } catch (Exception e) {
        }
    }

    private void releaseRecordecourse() {
        if (mRecorder != null) {
            try {
                mRecorder.setOnErrorListener(null);
                mRecorder.setOnInfoListener(null);
                mRecorder.setPreviewDisplay(null);
                mRecorder.stop();
            } catch (Exception e) {

            }

            mRecorder.release();
            mRecorder = null;
        }
    }

    @Override
    public void finish() {
        releaseResource();
        super.finish();
    }

    private Point getBestCameraSize(int orgWidth, int orgHeight) {
        Point point = new Point();
        int screenW = 1080;
        int screenH = 1920;
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
}
