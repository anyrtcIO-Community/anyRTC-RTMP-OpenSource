package io.anyrtc.liveplayer

import android.app.Activity
import android.content.Context
import android.content.Intent
import android.content.res.Resources
import android.os.Build
import android.util.TypedValue
import android.view.View
import android.view.ViewGroup
import android.view.WindowManager
import android.widget.RelativeLayout
import android.widget.Toast
import androidx.annotation.ColorInt
import androidx.annotation.ColorRes

private const val COLOR_TRANSPARENT = 0

public fun View.Gone() {
    this.visibility = View.GONE
}

public fun View.Show() {
    this.visibility = View.VISIBLE
}

public fun Activity.toast(msg: String) {
    Toast.makeText(this, msg, Toast.LENGTH_SHORT).show()
}

public fun <T> Activity.go(clazz: Class<T>) {
    startActivity(Intent(this, clazz))
}

public fun <T> Activity.go(clazz: Class<T>,vararg params:Pair<String,Any>) {
    startActivity(Intent(this, clazz).apply {
        params.forEach {
            if (it.second is Int) {
                this.putExtra(it.first, it.second as Int)
            }else {
                this.putExtra(it.first, it.second as String)
            }
        }
    })
}


fun Activity.immersiveRes(@ColorRes color: Int, darkMode: Boolean? = null) =
    immersive(resources.getColor(color), darkMode)

fun Activity.immersive(@ColorInt color: Int = COLOR_TRANSPARENT, darkMode: Boolean? = null) {
    when {
        Build.VERSION.SDK_INT >= 21 -> {
            when (color) {
                COLOR_TRANSPARENT -> {
                    window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
                    var systemUiVisibility = window.decorView.systemUiVisibility
                    systemUiVisibility = systemUiVisibility or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    systemUiVisibility = systemUiVisibility or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    window.decorView.systemUiVisibility = systemUiVisibility
                    window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
                    window.statusBarColor = color
                }
                else -> {
                    window.clearFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
                    var systemUiVisibility = window.decorView.systemUiVisibility
                    systemUiVisibility =
                        systemUiVisibility and View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                    systemUiVisibility = systemUiVisibility and View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    window.decorView.systemUiVisibility = systemUiVisibility
                    window.addFlags(WindowManager.LayoutParams.FLAG_DRAWS_SYSTEM_BAR_BACKGROUNDS)
                    window.statusBarColor = color
                }
            }
        }
        Build.VERSION.SDK_INT >= 19 -> {
            window.addFlags(WindowManager.LayoutParams.FLAG_TRANSLUCENT_STATUS)
            if (color != COLOR_TRANSPARENT) {
                setTranslucentView(window.decorView as ViewGroup, color)
            }
        }
    }
    if (darkMode != null) {
        darkMode(darkMode)
    }
}

private fun Context.setTranslucentView(container: ViewGroup, color: Int) {
    if (Build.VERSION.SDK_INT >= 19) {
        var simulateStatusBar: View? = container.findViewById(android.R.id.custom)
        if (simulateStatusBar == null && color != 0) {
            simulateStatusBar = View(container.context)
            simulateStatusBar.id = android.R.id.custom
            val lp = ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, statusBarHeight)
            container.addView(simulateStatusBar, lp)
        }
        simulateStatusBar?.setBackgroundColor(color)
    }
}

fun Activity.darkMode(darkMode: Boolean = true) {
    if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
        var systemUiVisibility = window.decorView.systemUiVisibility
        systemUiVisibility = if (darkMode) {
            systemUiVisibility or View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR
        } else {
            systemUiVisibility and View.SYSTEM_UI_FLAG_LIGHT_STATUS_BAR.inv()
        }
        window.decorView.systemUiVisibility = systemUiVisibility
    }
}

val Context?.statusBarHeight: Int
    get() {
        this ?: return 0
        var result = 24
        val resId = resources.getIdentifier("status_bar_height", "dimen", "android")
        result = if (resId > 0) {
            resources.getDimensionPixelSize(resId)
        } else {
            TypedValue.applyDimension(
                TypedValue.COMPLEX_UNIT_DIP,
                result.toFloat(), Resources.getSystem().displayMetrics
            ).toInt()
        }
        return result
    }

fun View.statusPadding(remove: Boolean = false) {
    if (this is RelativeLayout) {
        throw UnsupportedOperationException("Unsupported set statusPadding for RelativeLayout")
    }
    if (Build.VERSION.SDK_INT >= 19) {
        val statusBarHeight = context.statusBarHeight
        val lp = layoutParams
        if (lp != null && lp.height > 0) {
            lp.height += statusBarHeight //增高
        }
        if (remove) {
            if (paddingTop < statusBarHeight) return
            setPadding(
                paddingLeft, paddingTop - statusBarHeight,
                paddingRight, paddingBottom
            )
        } else {
            if (paddingTop >= statusBarHeight) return
            setPadding(
                paddingLeft, paddingTop + statusBarHeight,
                paddingRight, paddingBottom
            )
        }
    }
}

fun Collection<Boolean>.isAllGranted():Boolean{
    var count = 0
    this.forEach {
        if (it) count++
    }
    return count==this.size
}