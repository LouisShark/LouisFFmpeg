package ilouis.me.louisffmpeg;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.EditText;
import android.widget.Spinner;

import java.io.File;

public class MainActivity extends AppCompatActivity {

    VideoView videoView;
    private Spinner sp_video;
    private boolean isClick = false;
    private EditText rtmpAddr;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        videoView = (VideoView) findViewById(R.id.surface);
        sp_video = (Spinner) findViewById(R.id.sp_video);
        //多种格式的视频列表
        String[] videoArray = getResources().getStringArray(R.array.video_list);
        ArrayAdapter<String> adapter = new ArrayAdapter<String>(this,
                android.R.layout.simple_list_item_1,
                android.R.id.text1, videoArray);
        sp_video.setAdapter(adapter);
        rtmpAddr = (EditText) findViewById(R.id.rtmp_addr);
    }


    public void load(View view) {
        String video = sp_video.getSelectedItem().toString();
        String input = new File(Environment.getExternalStorageDirectory(), video).getAbsolutePath();
        videoView.play(input);
    }

    public void start_music(View view) {
        String input = new File(Environment.getExternalStorageDirectory(), "input.mp3").getAbsolutePath();
        String output = new File(Environment.getExternalStorageDirectory(), "output.pcm").getAbsolutePath();
        videoView.sound(input, output);
    }

    public void opelStart(View view) {
        if (isClick) {
            isClick = !isClick;
            videoView.stop();
        } else {
            isClick = !isClick;
            videoView.sound1();
        }
    }

    @Override
    protected void onPause() {
        super.onPause();
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        videoView.releaseSync();
    }

    public void playNet(View view) {
        File file = new File(Environment.getExternalStorageDirectory(), "Warcraft3_End.avi");
        if (!TextUtils.isEmpty(rtmpAddr.getText().toString())) {
//            videoView.playSyncNet("rtmp://live.hkstv.hk.lxdns.com/live/hks");
            videoView.playSyncNet(rtmpAddr.getText().toString().trim());
        }
//        videoView.playSyncNet(file.getAbsolutePath());
    }

    public void stopNet(View view) {
        videoView.releaseSync();
    }
}
