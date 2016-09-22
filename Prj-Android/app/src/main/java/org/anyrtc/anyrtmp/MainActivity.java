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
