package ilouis.me.louisffmpeg;

import android.os.Bundle;
import android.os.Environment;
import android.support.v7.app.AppCompatActivity;
import android.view.View;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    // Used to load the 'native-lib' library on application startup.
    static{
        System.loadLibrary("avcodec-56");
        System.loadLibrary("avdevice-56");
        System.loadLibrary("avfilter-5");
        System.loadLibrary("avformat-56");
        System.loadLibrary("avutil-54");
        System.loadLibrary("postproc-53");
        System.loadLibrary("swresample-1");
        System.loadLibrary("swscale-3");
        System.loadLibrary("ffmpegL");
    }
    private InputStream inputStream = null;
    private FileOutputStream outputStream = null;
    private File file;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);


        file = new File(Environment.getExternalStorageDirectory(), "input.mp4");
        if (!file.exists()) {
            new Thread(new Runnable() {
                @Override
                public void run() {
                    try {
                        int bytesum = 0;
                        int byteread;
                        inputStream = getResources().getAssets().open("input.mp4");
                        outputStream = new FileOutputStream(file);
                        byte[] buffer = new byte[1024];
                        while ((byteread = inputStream.read(buffer)) != -1) {
                            bytesum += byteread;
                            System.out.println(bytesum);
                            outputStream.write(buffer, 0, byteread);
                        }
                        inputStream.close();
                    } catch (Exception e) {
                        System.out.println("复制单个文件操作出错");
                        e.printStackTrace();
                    }
                }
            }).start();
        }

    }

    /**
     * A native method that is implemented by the 'native-lib' native library,
     * which is packaged with this application.
     */
    public native void open(String inputPath, String outPath);

    public void load(View view) {
        String inputPath = file.getAbsolutePath();
        String outputPath = new File(Environment.getExternalStorageDirectory(), "output.yuv").getAbsolutePath();
        open(inputPath, outputPath);
    }
}
