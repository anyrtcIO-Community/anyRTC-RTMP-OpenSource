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

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import java.util.concurrent.Exchanger;
import java.util.concurrent.Executor;

/**
 * Looper based executor class.
 */
public class LooperExecutor extends Thread implements Executor {
	private static final String TAG = "LooperExecutor";
	// Object used to signal that looper thread has started and Handler instance
	// associated with looper thread has been allocated.
	private final Object looperStartedEvent = new Object();
	private Handler handler = null;
	private boolean running = false;
	private long threadId;

	@Override
	public void run() {
		Looper.prepare();
		synchronized (looperStartedEvent) {
			Log.d(TAG, "Looper thread started.");
			handler = new Handler();
			threadId = Thread.currentThread().getId();
			looperStartedEvent.notify();
		}
		Looper.loop();
	}

	/**
	 * 启动线程池中线程
	 */
	public synchronized void requestStart() {
		if (running) {
			return;
		}
		running = true;
		handler = null;
		start();
		// Wait for Hander allocation.
		synchronized (looperStartedEvent) {
			while (handler == null) {
				try {
					looperStartedEvent.wait();
				} catch (InterruptedException e) {
					Log.e(TAG, "Can not start looper thread");
					running = false;
				}
			}
		}
	}

	/**
	 * 停止线程池中线程
	 */
	public synchronized void requestStop() {
		if (!running) {
			return;
		}
		running = false;
		handler.post(new Runnable() {
			@Override
			public void run() {
				Looper.myLooper().quit();
				Log.d(TAG, "Looper thread finished.");
			}
		});
	}

	/**
	 * Checks if current thread is a looper thread.
	 * 
	 * @return true/false
	 */
	public boolean checkOnLooperThread() {
		return (Thread.currentThread().getId() == threadId);
	}

	@Override
	public synchronized void execute(final Runnable runnable) {
		if (!running) {
			Log.w(TAG, "Running looper executor without calling requestStart()");
			return;
		}
		if (Thread.currentThread().getId() == threadId) {
			runnable.run();
		} else {
			handler.post(runnable);
		}
	}

	/**
	 * Exchanges |value| with |exchanger|, converting InterruptedExceptions to
	 * RuntimeExceptions (since we expect never to see these).
	 * 
	 * @param exchanger
	 * @param value
	 * @return
	 */
	public static <T> T exchange(Exchanger<T> exchanger, T value) {
		try {
			return exchanger.exchange(value);
		} catch (InterruptedException e) {
			throw new RuntimeException(e);
		}
	}

}
