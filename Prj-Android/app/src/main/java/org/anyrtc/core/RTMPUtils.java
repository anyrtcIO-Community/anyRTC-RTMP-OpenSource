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
package org.anyrtc.core;

import android.os.Build;
import android.util.Log;

/**
 * RTMPUtils provides helper functions for managing thread safety.
 */
public final class RTMPUtils {

  private RTMPUtils() {
  }

  /**
   * NonThreadSafe is a helper class used to help verify that methods of a
   * class are called from the same thread.
   */
  public static class NonThreadSafe {
    private final Long threadId;

    public NonThreadSafe() {
      // Store thread ID of the creating thread.
      threadId = Thread.currentThread().getId();
    }

   /** Checks if the method is called on the valid/creating thread. */
    public boolean calledOnValidThread() {
       return threadId.equals(Thread.currentThread().getId());
    }
  }

  /** Helper method which throws an exception  when an assertion has failed. */
  public static void assertIsTrue(boolean condition) {
    if (!condition) {
      throw new AssertionError("Expected condition to be true");
    }
  }

  /** Helper method for building a string of thread information.*/
  public static String getThreadInfo() {
    return "@[name=" + Thread.currentThread().getName()
        + ", id=" + Thread.currentThread().getId() + "]";
  }

  /** Information about the current build, taken from system properties. */
  public static void logDeviceInfo(String tag) {
    Log.d(tag, "Android SDK: " + Build.VERSION.SDK_INT + ", "
        + "Release: " + Build.VERSION.RELEASE + ", "
        + "Brand: " + Build.BRAND + ", "
        + "Device: " + Build.DEVICE + ", "
        + "Id: " + Build.ID + ", "
        + "Hardware: " + Build.HARDWARE + ", "
        + "Manufacturer: " + Build.MANUFACTURER + ", "
        + "Model: " + Build.MODEL + ", "
        + "Product: " + Build.PRODUCT);
  }
}
