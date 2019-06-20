package com.lamer.ffmpeg;

import android.Manifest;
import android.app.Activity;
import android.os.Build;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.TextView;

public class MainActivity extends AppCompatActivity {

    private TextView mTextView;
    private FFVideoView mVideoView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        mTextView = findViewById(R.id.sample_text);
        mVideoView = findViewById(R.id.videoView);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            requestPermissions(new String[]{Manifest.permission.READ_EXTERNAL_STORAGE}, 0);
        }
    }

    public void onButtonClick(View view) {
        int id = view.getId();

        switch (id) {
            case R.id.button_protocol:
                setInfoText(Utils.urlProtocolInfo());
                break;
            case R.id.button_codec:
                setInfoText(Utils.avCodecInfo());
                break;
            case R.id.button_filter:
                setInfoText(Utils.avFilterInfo());
                break;
            case R.id.button_format:
                setInfoText(Utils.avFormatInfo());
                break;
            case R.id.button_init:
                Utils.init();
                break;
            case R.id.button_play:
                String videoPath = "/sdcard/Download/test.mp4";
                mVideoView.playVideo(videoPath);
                break;
        }
    }

    private void setInfoText(String content) {
        if (mTextView != null) {
            mTextView.setText(content);
        }
    }

}
