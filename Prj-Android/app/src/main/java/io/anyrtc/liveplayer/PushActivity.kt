package io.anyrtc.liveplayer

import android.content.ClipData
import android.content.ClipboardManager
import android.content.Context
import android.os.Build.VERSION.SDK_INT
import android.os.Bundle
import android.util.Log
import android.view.KeyEvent
import android.view.View
import android.widget.AdapterView
import android.widget.SeekBar
import android.widget.Toast
import coil.ImageLoader
import coil.decode.GifDecoder
import coil.decode.ImageDecoderDecoder
import coil.load
import io.anyrtc.live.*
import io.anyrtc.live.ArLiveDef.ArLiveVideoResolution
import io.anyrtc.liveplayer.databinding.ActivityPushBinding
import kotlinx.coroutines.MainScope
import kotlinx.coroutines.delay
import kotlinx.coroutines.launch
import java.math.BigDecimal
import java.text.DecimalFormat

class PushActivity : BaseActivity() {
    private val binding by lazy { ActivityPushBinding.inflate(layoutInflater) }
    private val liveEngine by lazy { ArLiveEngine.create(this)}
    private val pusher by lazy { liveEngine.createArLivePusher() }
    private val player by lazy { liveEngine.createArLivePlayer() }
    private var pushUrl = ""

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        immersive(darkMode = false)
        val pushType = intent.getIntExtra("pushType",0);
        val resolution = intent.getIntExtra("resolution",0)
        pushUrl = intent.getStringExtra("url").toString()
        when(resolution){
            0->{
                pusher.setVideoQuality(ArLiveDef.ArLiveVideoEncoderParam(ArLiveVideoResolution.ArLiveVideoResolution1280x720).apply {
                    if (pushType!=0){
                        videoBitrate = 2500
                    }
                })
            }
            1->{
                pusher.setVideoQuality(ArLiveDef.ArLiveVideoEncoderParam(ArLiveVideoResolution.ArLiveVideoResolution960x540).apply {
                    if (pushType!=0){
                        videoBitrate = 2500
                    }
                })
            }
            2->{
                pusher.setVideoQuality(ArLiveDef.ArLiveVideoEncoderParam(ArLiveVideoResolution.ArLiveVideoResolution640x360).apply {
                    if (pushType!=0){
                        videoBitrate = 2500
                    }
                })
            }
        }
        if (pushType == 0){
            pusher.setRenderView(binding.videoView)
            pusher.setRenderMirror(ArLiveDef.ArLiveMirrorType.ArLiveMirrorTypeAuto)
            pusher.startCamera(true)
        }else{
            binding.ivBeauty.visibility = View.GONE
            val imageLoader = ImageLoader.Builder(this)
                .componentRegistry {
                    if (SDK_INT >= 28) {
                        add(ImageDecoderDecoder(this@PushActivity))
                    } else {
                        add(GifDecoder())
                    }
                }
                .build()
            binding.ivGif.load("https://gimg2.baidu.com/image_search/src=http%3A%2F%2Fhbimg.b0.upaiyun.com%2F60ed9921abb8a968651aae697626dc816624cc4770c32-uwUmhP_fw658&refer=http%3A%2F%2Fhbimg.b0.upaiyun.com&app=2002&size=f9999,10000&q=a80&n=0&g=0n&fmt=jpeg?sec=1645166781&t=ff39058ea3e782746361d8b2bea68511",imageLoader)
            pusher.startScreenCapture()
        }
        pusher.startPush(pushUrl)

        initView()

    }

    private fun initView(){
        binding.run {
            tvUrl.text = pushUrl
            ivExit.setOnClickListener {
                pusher.stopPush()
                ArLiveEngine.release()
                finish()
            }
            ivSwitch.setOnClickListener {
                pusher.deviceManager.switchCamera()
            }
            tvUrl.setOnClickListener {
                val clipboard = getSystemService(Context.CLIPBOARD_SERVICE) as ClipboardManager
                val clip: ClipData = ClipData.newPlainText("pushUrl",pushUrl)
                clipboard.setPrimaryClip(clip)
                Toast.makeText(this@PushActivity, "已复制", Toast.LENGTH_SHORT).show()
            }
            ivBeauty.setOnClickListener {
                ivBeauty.isSelected = !ivBeauty.isSelected
                pusher.beautyManager.setBeautyEffect(ivBeauty.isSelected)
            }
            ivAudio.setOnClickListener {
                ivAudio.isSelected = !ivAudio.isSelected
                if (ivAudio.isSelected){
                    pusher.pauseAudio()
                }else{
                    pusher.resumeAudio()
                }
            }
            ivVideo.setOnClickListener {
                ivVideo.isSelected = !ivVideo.isSelected
                if (ivVideo.isSelected){
                    pusher.pauseVideo()
                }else{
                    pusher.resumeVideo()
                }
            }

            ivMirro.setOnClickListener {
                ivMirro.isSelected = !ivMirro.isSelected
                pusher.setEncoderMirror(ivMirro.isSelected)
            }

            pusher.setObserver(object :ArLivePusherObserver(){
                override fun onPushStatusUpdate(
                    status: ArLiveDef.ArLivePushStatus?,
                    msg: String?,
                    extraInfo: Bundle?
                ) {
                    super.onPushStatusUpdate(status, msg, extraInfo)
                    when(status){
                        ArLiveDef.ArLivePushStatus.ArLivePushStatusConnecting->{
                            tvState.setText("连接中....")
                        }
                        ArLiveDef.ArLivePushStatus.ArLivePushStatusConnectSuccess->{
                            tvState.setText("连接成功....")
                        }
                        ArLiveDef.ArLivePushStatus.ArLivePushStatusDisconnected->{
                            tvState.setText("连接断开....")
                        }
                        ArLiveDef.ArLivePushStatus.ArLivePushStatusReconnecting->{
                            tvState.setText("重连中....")
                        }
                    }
                }
            })

        }
    }

    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        if (keyCode == KeyEvent.KEYCODE_BACK){
            pusher.stopPush()
            ArLiveEngine.release()
            finish()
            return true
        }
        return super.onKeyDown(keyCode, event)
    }



}