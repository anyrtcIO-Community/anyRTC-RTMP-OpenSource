package io.anyrtc.live.internal;

import android.annotation.TargetApi;
import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.media.projection.MediaProjectionManager;
import android.os.Bundle;

import io.anyrtc.live.ArScreenService;

public class ArScreenCapture {

    public ArScreenCapture() {
    }

    @TargetApi(21)
    public static class ArScreenCaptureAssistantActivity extends Activity {

        private long nativePusherPtr;
        private MediaProjectionManager mMediaProjectionManager;
        public ArScreenCaptureAssistantActivity() {
        }

        public void onCreate(Bundle savedInstanceState) {
            super.onCreate(savedInstanceState);
            this.requestWindowFeature(1);
            nativePusherPtr = getIntent().getLongExtra("pusherId",0);
            Intent service = new Intent(this, ArScreenService.class);
            startService(service);
            this.mMediaProjectionManager = (MediaProjectionManager)this.getApplicationContext().getSystemService(Context.MEDIA_PROJECTION_SERVICE);
            Intent intent = this.mMediaProjectionManager.createScreenCaptureIntent();
            try {
                this.startActivityForResult(intent, 100);
            } catch (Exception var4) {
                VideoCapturerDevice.mediaProjectionPermissionResultData = null;
                this.finish();
            }

        }

        public void onActivityResult(int requestCode, int resultCode, Intent data) {
            VideoCapturerDevice.mediaProjectionPermissionResultData = data;
            if (data == null){
                stopService(new Intent(this,ArScreenService.class));
            }else {
                ArScreenService screenService = ArScreenService.getSharedInstance();
                screenService.showNotification(nativePusherPtr);
            }
            this.finish();
        }

        protected void onDestroy() {
            super.onDestroy();
        }
    }
}
