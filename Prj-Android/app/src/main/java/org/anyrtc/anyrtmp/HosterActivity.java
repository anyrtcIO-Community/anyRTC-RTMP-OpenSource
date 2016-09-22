/**
 *  Copyright (c) 2016 The AnyRTC project authors. All Rights Reserved.
 *
 *  Please visit https://www.anyrtc.io for detail.
 *
 * The GNU General Public License is a free, copyleft license for
 * software and other kinds of works.
 *
 * The licenses for most software and other practical works are designed
 * to take away your freedom to share and change the works.  By contrast,
 * the GNU General Public License is intended to guarantee your freedom to
 * share and change all versions of a program--to make sure it remains free
 * software for all its users.  We, the Free Software Foundation, use the
 * GNU General Public License for most of our software; it applies also to
 * any other work released this way by its authors.  You can apply it to
 * your programs, too.
 * See the GNU LICENSE file for more info.
 */
package org.anyrtc.anyrtmp;

import android.app.Activity;
import android.os.Bundle;
import android.text.SpannableString;
import android.view.View;
import android.widget.TextView;

import org.anyrtc.core.AnyRTMP;
import org.anyrtc.core.RTMPHosterHelper;
import org.anyrtc.core.RTMPHosterKit;
import org.webrtc.SurfaceViewRenderer;
import org.webrtc.VideoRenderer;

/**
 * Created by Eric on 2016/9/16.
 */
public class HosterActivity extends Activity implements RTMPHosterHelper {
    private RTMPHosterKit mHoster = null;

    private TextView mTxtStatus = null;
    private SurfaceViewRenderer mSurfaceView = null;
    private VideoRenderer mRenderer = null;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_hoster);

        {//* Init UI
            mTxtStatus = (TextView) findViewById(R.id.txt_rtmp_status);
            mSurfaceView = (SurfaceViewRenderer) findViewById(R.id.suface_view);
            mSurfaceView.init(AnyRTMP.Inst().Egl().getEglBaseContext(), null);
            mRenderer = new VideoRenderer(mSurfaceView);
        }

        {
            String rtmpUrl = getIntent().getExtras().getString("rtmp_url");
            mHoster = new RTMPHosterKit(this, this);
            mHoster.SetVideoCapturer(mRenderer.GetRenderPointer(), true);
            mHoster.StartRtmpStream(rtmpUrl);
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        if (mHoster != null) {
            mHoster.StopRtmpStream();
            mHoster.Clear();
            mHoster = null;
        }
    }

    /**
     * the button click event listener
     *
     * @param btn
     */
    public void OnBtnClicked(View btn) {
        if (btn.getId() == R.id.btn_close) {
            if (mHoster != null) {
                mHoster.StopRtmpStream();
                mHoster.Clear();
                mHoster = null;
            }
            finish();
        } else if (btn.getId() == R.id.btn_switch_camera) {
            if (null != mHoster) {
                mHoster.SwitchCamera();
            }
        }
    }


    /**
     * Implements for RTMPHosterHelper
     */
    @Override
    public void OnRtmpStreamOK() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mTxtStatus.setText(R.string.str_rtmp_success);
            }
        });
    }

    @Override
    public void OnRtmpStreamReconnecting(final int times) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mTxtStatus.setText(String.format(getString(R.string.str_rtmp_reconnecting), times));
            }
        });
    }

    @Override
    public void OnRtmpStreamStatus(final int delayMs, final int netBand) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mTxtStatus.setText(String.format(getString(R.string.str_rtmp_status), delayMs, netBand));
            }
        });
    }

    @Override
    public void OnRtmpStreamFailed(final int code) {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mTxtStatus.setText(R.string.str_rtmp_failed);
            }
        });
    }

    @Override
    public void OnRtmpStreamClosed() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                mTxtStatus.setText(R.string.str_rtmp);
            }
        });
    }
}
