package io.anyrtc.liveplayer;

import android.Manifest;
import android.content.pm.PackageManager;
import android.media.AudioFormat;
import android.media.AudioRecord;
import android.media.MediaRecorder;
import android.util.Log;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;


public class RecordThread extends Thread {

    private RecordDataCallback callback;


    public interface RecordDataCallback{
       void onFrame(byte[] audioFrame);
    }


    private volatile boolean stopped;
    private RecordThread thread;
    private AudioRecord audioRecord;
    public static final int DEFAULT_SAMPLE_RATE = 48000;
    /**
     * 1 corresponds to AudioFormat.CHANNEL_IN_MONO;
     * 2 corresponds to AudioFormat.CHANNEL_IN_STEREO
     */
    public static final int DEFAULT_CHANNEL_COUNT = 1, DEFAULT_CHANNEL_CONFIG = AudioFormat.CHANNEL_IN_MONO;
    private byte[] buffer;

    RecordThread(String name, RecordDataCallback callback) {
        this.callback = callback;
        int bufferSize = AudioRecord.getMinBufferSize(DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_CONFIG,
                AudioFormat.ENCODING_PCM_16BIT);
        audioRecord = new AudioRecord(MediaRecorder.AudioSource.MIC, DEFAULT_SAMPLE_RATE, DEFAULT_CHANNEL_COUNT,
                AudioFormat.ENCODING_PCM_16BIT, bufferSize);
        buffer = new byte[bufferSize];
    }

    @Override
    public void run() {
        try {
            audioRecord.startRecording();
            while (!stopped) {
                int result = audioRecord.read(buffer, 0, buffer.length);
                if (result >= 0) {
                    callback.onFrame(buffer);
                } else {
                    logRecordError(result);
                }
                Log.d("MainActivity", "byte size is :" + result);
            }
            release();
        } catch (Exception e) {
            e.printStackTrace();
        }
    }
    private void release()
    {
        if (audioRecord != null)
        {
            audioRecord.stop();
            buffer = null;
        }
    }

    private void logRecordError(int error)
    {
        String message = "";
        switch (error)
        {
            case AudioRecord.ERROR:
                message = "generic operation failure";
                break;
            case AudioRecord.ERROR_BAD_VALUE:
                message = "failure due to the use of an invalid value";
                break;
            case AudioRecord.ERROR_DEAD_OBJECT:
                message = "object is no longer valid and needs to be recreated";
                break;
            case AudioRecord.ERROR_INVALID_OPERATION:
                message = "failure due to the improper use of method";
                break;
        }
        Log.e("MainActivity", message);
    }

}