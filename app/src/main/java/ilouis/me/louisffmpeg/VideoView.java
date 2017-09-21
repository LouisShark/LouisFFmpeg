package ilouis.me.louisffmpeg;

import android.content.Context;
import android.graphics.PixelFormat;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

/**
 * Created by LouisShark on 2017/9/21.
 * this is on ilouis.me.louisffmpeg.
 */

public class VideoView extends SurfaceView {
    // Used to load the 'native-lib' library on application startup.
    static {
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

    private SurfaceHolder mSurfaceHolder;

    public VideoView(Context context) {
        this(context, null);
    }

    public VideoView(Context context, AttributeSet attrs) {
        super(context, attrs);
        init();
    }

    public VideoView(Context context, AttributeSet attrs, int defStyle) {
        super(context, attrs, defStyle);
    }

    private void init() {
        mSurfaceHolder = getHolder();
        mSurfaceHolder.setFormat(PixelFormat.RGBA_8888);
        //注册回调方法
//        mSurfaceHolder.addCallback(this);
        setFocusable(true);
        setKeepScreenOn(true);
        setFocusableInTouchMode(true);
    }

    public void play(final String inputStr) {
        new Thread(new Runnable() {
            @Override
            public void run() {
                render(inputStr, mSurfaceHolder.getSurface());
            }
        }).start();
    }

    public native void render(String inputStr, Surface surface);

}
