package io.anyrtc.liveplayer

import android.content.Intent
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.text.TextUtils
import android.view.View
import android.widget.RadioGroup
import android.widget.Toast
import androidx.activity.result.ActivityResultLauncher
import androidx.activity.result.ActivityResultRegistry
import androidx.activity.result.contract.ActivityResultContracts
import androidx.databinding.BaseObservable
import androidx.databinding.ObservableInt
import io.anyrtc.liveplayer.databinding.ActivityInputBinding

class InputActivity : BaseActivity(),RadioGroup.OnCheckedChangeListener,View.OnClickListener{

    private val binding by lazy { ActivityInputBinding.inflate(layoutInflater) }

    private var pushType = 0 //0 camera 1 screen
    private var resolution = 0 //0-720 1-540 2-360
    private val config = Config()
    private var requestPermission: ActivityResultLauncher<Array<String>>?=null
    val VIDEO_1 = "https://www.apple.com/105/media/us/iphone-x/2017/01df5b43-28e4-4848-bf20-490c34a926a7/films/feature/iphone-x-feature-tpl-cc-us-20170912_1920x1080h.mp4"

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        binding.config = config
        immersiveRes(R.color.white,true)

        val type = intent.getIntExtra("type",0)
        config.type.set(type)

         requestPermission = registerForActivityResult(ActivityResultContracts.RequestMultiplePermissions()){
            if (it.values.isAllGranted()){
                go(PushActivity::class.java, Pair("url",binding.etUrl.text.toString()),Pair("pushType",pushType),Pair("resolution",resolution))
            }else{
                toast("请打开相关权限")
            }
        }
        binding.run {
            rgType.check(R.id.rb_camera)
            rgResolution.check(R.id.rb_a)
            btnStart.setOnClickListener(this@InputActivity)
            rgType.setOnCheckedChangeListener(this@InputActivity)
            rgResolution.setOnCheckedChangeListener(this@InputActivity)
            imgBack.setOnClickListener { finish() }

        }

    }

    inner class Config  {
        var type = ObservableInt(0)
    }

    override fun onCheckedChanged(group: RadioGroup?, checkedId: Int) {
        when(checkedId){
            R.id.rb_a->{
                this@InputActivity.resolution=0
            }
            R.id.rb_b->{
                this@InputActivity.resolution=1
            }
            R.id.rb_c->{
                this@InputActivity.resolution=2
            }
            R.id.rb_aa->{
                this@InputActivity.resolution=3
            }
            R.id.rb_camera-> {
                this@InputActivity.pushType = 0
            }
            R.id.rb_screen->{
                this@InputActivity.pushType = 1
            }
            else->{
                this@InputActivity.pushType = 2
            }
        }
    }

    override fun onClick(v: View?) {
        when(v?.id){
            R.id.btn_start->{
                if (TextUtils.isEmpty(binding.etUrl.text.toString())){
                    toast("请输入推/拉流地址")
                    return
                }
                when(config.type.get()){
                    0->{
                        requestPermission?.launch(arrayOf(android.Manifest.permission.CAMERA,android.Manifest.permission.RECORD_AUDIO))
                    }
                    1->{
                        go(PullActivity::class.java, Pair("url",VIDEO_1))
                    }
                }
            }
        }
    }


}