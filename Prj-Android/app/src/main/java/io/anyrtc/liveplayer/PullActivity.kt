package io.anyrtc.liveplayer

import android.animation.Animator
import android.animation.AnimatorSet
import android.animation.ObjectAnimator
import android.graphics.Bitmap
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.SystemClock
import android.view.animation.AnimationSet
import androidx.databinding.ObservableField

import io.anyrtc.live.*
import io.anyrtc.live.internal.VideoCapturerDevice
import io.anyrtc.live.util.ArJavaI420Buffer
import io.anyrtc.liveplayer.databinding.ActivityPullBinding
import org.webrtc.*
import java.nio.ByteBuffer
import java.util.concurrent.TimeUnit
import android.view.animation.AccelerateDecelerateInterpolator




class PullActivity : AppCompatActivity() {

    private val binding by lazy { ActivityPullBinding.inflate(layoutInflater) }
    private val liveEngine by lazy { ArLiveEngine.create(this) }
    private val player by lazy { liveEngine.createArLivePlayer() }
    private val playStatus = PlayStatus()
    private var displayMode = ArLiveDef.ArLiveFillMode.ArLiveFillModeFit
    private val setAnimator = AnimatorSet()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        binding.playStatus = playStatus
        binding.root.statusPadding()
        immersive()
        player.setRenderView(binding.playView)
        player.setRenderFillMode(displayMode)
        val url = intent.getStringExtra("url")
        player.setPlayMode(ArLiveDef.ArLivePlayMode.ArLivePlayModeLive)
        player.setObserver(object :ArLivePlayerObserver(){
            override fun onAudioPlayStatusUpdate(
                player: ArLivePlayer?,
                status: ArLiveDef.ArLivePlayStatus?,
                reason: ArLiveDef.ArLiveStatusChangeReason?,
                extraInfo: Bundle?
            ) {
                super.onAudioPlayStatusUpdate(player, status, reason, extraInfo)
                when(status){
                    ArLiveDef.ArLivePlayStatus.ArLivePlayStatusLoading->{
                        playStatus.status.set("缓冲中....")
                    }
                    ArLiveDef.ArLivePlayStatus.ArLivePlayStatusPlaying->{
                        playStatus.status.set("播放中....")
                    }
                    ArLiveDef.ArLivePlayStatus.ArLivePlayStatusStopped->{
                        playStatus.status.set("已停止....")
                    }
                }

            }

            override fun onError(
                player: ArLivePlayer?,
                code: Int,
                msg: String?,
                extraInfo: Bundle?
            ) {
                super.onError(player, code, msg, extraInfo)
            }

            override fun onPlayoutVolumeUpdate(player: ArLivePlayer?, volume: Int) {
                super.onPlayoutVolumeUpdate(player, volume)
            }

            override fun onReceiveSeiMessage(
                player: ArLivePlayer?,
                payloadType: Int,
                data: ByteArray?
            ) {
                super.onReceiveSeiMessage(player, payloadType, data)

            }

            override fun onRenderVideoFrame(
                player: ArLivePlayer?,
                videoFrame: ArLiveDef.ArLiveVideoFrame?
            ) {
                super.onRenderVideoFrame(player, videoFrame)
            }

            override fun onSnapshotComplete(player: ArLivePlayer?, image: Bitmap?) {
                super.onSnapshotComplete(player, image)
                setAnimator.cancel()
                binding.ivSnapResult.setImageBitmap(image)
                val animation1 = ObjectAnimator.ofFloat(binding.ivSnapResult,"alpha",0f,1f)
                animation1.setDuration(300)
                val animation2 = ObjectAnimator.ofFloat(binding.ivSnapResult,"translationX",0f,binding.ivSnapResult.width.toFloat())
                setAnimator.play(animation2).after(animation1).after(1600)
                setAnimator.setInterpolator(AccelerateDecelerateInterpolator())
                setAnimator.addListener(object :Animator.AnimatorListener{
                    override fun onAnimationStart(animation: Animator?) {
                    }

                    override fun onAnimationEnd(animation: Animator?) {
                        binding.ivSnapResult.alpha = 0f
                        binding.ivSnapResult.translationX= 0f
                    }

                    override fun onAnimationCancel(animation: Animator?) {
                    }

                    override fun onAnimationRepeat(animation: Animator?) {
                    }

                })
                setAnimator.start()
            }

            override fun onStatisticsUpdate(
                player: ArLivePlayer?,
                statistics: ArLiveDef.ArLivePlayerStatistics?
            ) {
                super.onStatisticsUpdate(player, statistics)
            }

            override fun onVideoPlayStatusUpdate(
                player: ArLivePlayer?,
                status: ArLiveDef.ArLivePlayStatus?,
                reason: ArLiveDef.ArLiveStatusChangeReason?,
                extraInfo: Bundle?
            ) {
                super.onVideoPlayStatusUpdate(player, status, reason, extraInfo)
            }

            override fun onWarning(
                player: ArLivePlayer?,
                code: Int,
                msg: String?,
                extraInfo: Bundle?
            ) {
                super.onWarning(player, code, msg, extraInfo)
            }
        })
        player.setCacheParams(1f,2f)
        player.startPlay(url)
        binding.run {
            binding.btnExit.setOnClickListener {
                ArLiveEngine.release()
                finish()
            }

            ivSnap.setOnClickListener {
                player.snapshot()
            }

            tvMode.setOnClickListener {
                if (displayMode == ArLiveDef.ArLiveFillMode.ArLiveFillModeFit){
                    displayMode = ArLiveDef.ArLiveFillMode.ArLiveFillModeFill
                }else{
                    displayMode = ArLiveDef.ArLiveFillMode.ArLiveFillModeFit
                }
                player.setRenderFillMode(displayMode)
                playStatus?.mode?.set(if (displayMode == ArLiveDef.ArLiveFillMode.ArLiveFillModeFit) "Fit 比例显示" else "Fill 填充")
            }
        }

    }

    override fun onBackPressed() {
        ArLiveEngine.release()
        finish()
    }

    inner class PlayStatus{
        var status = ObservableField("")
        var mode = ObservableField("Fit 比例显示")
    }

}