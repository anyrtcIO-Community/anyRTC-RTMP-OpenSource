package io.anyrtc.liveplayer

import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.widget.Toast
import androidx.core.app.ActivityCompat
import io.anyrtc.liveplayer.databinding.ActivityMainBinding


class MainActivity : BaseActivity() {

    private val binding by lazy { ActivityMainBinding.inflate(layoutInflater) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        immersiveRes(R.color.white,true)
        binding.run {
            cardPush.setOnClickListener {
                go(InputActivity::class.java, Pair("type",0))
            }
            cardPull.setOnClickListener {
                go(InputActivity::class.java,Pair("type",1))
            }
            cardLive.setOnClickListener {
                toast("敬请期待")
            }
        }

    }


}