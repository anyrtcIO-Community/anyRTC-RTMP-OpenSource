package io.anyrtc.liveplayer

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import java.math.BigDecimal
import java.text.DecimalFormat

open class BaseActivity : AppCompatActivity() {


    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        ScreenUtils.adapterScreen(this, 375, false)
    }

    override fun onDestroy() {
        super.onDestroy()
      ScreenUtils.resetScreen(this)
    }
}

fun Float.format():Float{
    val value = BigDecimal(this.toString()).setScale(2, BigDecimal.ROUND_HALF_UP).toFloat()
    val decimalFormat = DecimalFormat("0.00#")
    val result = decimalFormat.format(value)
    return result.toFloat()
}