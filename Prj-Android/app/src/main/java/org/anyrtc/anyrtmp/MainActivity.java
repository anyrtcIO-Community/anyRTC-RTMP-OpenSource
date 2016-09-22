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

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.EditText;

import org.anyrtc.core.AnyRTMP;

public class MainActivity extends AppCompatActivity {

    private EditText mEditRtmpUrl;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        {//* Init UI
            mEditRtmpUrl = (EditText) findViewById(R.id.edit_rtmp_url);
        }
        AnyRTMP.Inst();
    }

    public void OnBtnClicked(View view) {
        String rtmpUrl = mEditRtmpUrl.getEditableText().toString();
        if (rtmpUrl.length() == 0) {
            return;
        }
        if (view.getId() == R.id.btn_start_live) {
            Intent it = new Intent(this, HosterActivity.class);
            Bundle bd = new Bundle();
            bd.putString("rtmp_url", rtmpUrl);
            it.putExtras(bd);
            startActivity(it);
        } else {
            Intent it = new Intent(this, GuestActivity.class);
            Bundle bd = new Bundle();
            bd.putString("rtmp_url", rtmpUrl);
            it.putExtras(bd);
            startActivity(it);
        }
    }
}
