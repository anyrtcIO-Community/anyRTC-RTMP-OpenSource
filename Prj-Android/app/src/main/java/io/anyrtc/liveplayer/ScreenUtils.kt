package io.anyrtc.liveplayer

import android.app.Activity
import android.content.res.Resources

object ScreenUtils {

    fun adapterScreen(activity: Activity, targetDP: Int, isVertical: Boolean) {
        val sysDisplayMetrics = Resources.getSystem().displayMetrics
        val activityDisplayMetrics = activity.resources.displayMetrics

        if (isVertical) {
            activityDisplayMetrics.density = activityDisplayMetrics.heightPixels / targetDP.toFloat()
        } else {
            activityDisplayMetrics.density = activityDisplayMetrics.widthPixels / targetDP.toFloat()
        }
        activityDisplayMetrics.scaledDensity = activityDisplayMetrics.density * (sysDisplayMetrics.scaledDensity / sysDisplayMetrics.density)
        activityDisplayMetrics.densityDpi = (160 * activityDisplayMetrics.density).toInt()
    }

    fun resetScreen(activity: Activity) {
        val sysDisplayMetrics = Resources.getSystem().displayMetrics
        val appDisplayMetrics = activity.application.resources.displayMetrics
        val activityDisplayMetrics = activity.resources.displayMetrics

        activityDisplayMetrics.density = sysDisplayMetrics.density
        activityDisplayMetrics.scaledDensity = sysDisplayMetrics.scaledDensity
        activityDisplayMetrics.densityDpi = sysDisplayMetrics.densityDpi

        appDisplayMetrics.density = sysDisplayMetrics.density
        appDisplayMetrics.scaledDensity = sysDisplayMetrics.scaledDensity
        appDisplayMetrics.densityDpi = sysDisplayMetrics.densityDpi
    }
}
