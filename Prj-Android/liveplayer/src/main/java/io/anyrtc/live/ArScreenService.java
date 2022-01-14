package io.anyrtc.live;

import android.annotation.TargetApi;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.os.Build;
import android.os.IBinder;

import androidx.annotation.Nullable;
import androidx.core.app.NotificationCompat;
import io.anyrtc.live.internal.NativeInstance;

public class ArScreenService extends Service {

    private static ArScreenService sharedInstance;
    private int NOTIFICATION_ID = 1888;

    @Nullable
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
    public static ArScreenService getSharedInstance() {
        return sharedInstance;
    }
    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        if (sharedInstance != null){
            return START_NOT_STICKY;
        }
         sharedInstance = this;
        return START_NOT_STICKY;
    }

    public void showNotification(long pusherNativePtr){
        Notification notification = getNotification(this);
        startForeground(NOTIFICATION_ID, notification);
        NativeInstance.getSharedInstance().startScreenCapture(pusherNativePtr);
    }

    public void stopScreen(){
        stopSelf();
        sharedInstance = null;
    }

    @Override
    public void onCreate() {
        super.onCreate();

    }


    private String NOTIFICATION_CHANNEL_ID = "io.anyrtc.live";
    private String NOTIFICATION_CHANNEL_NAME = "io.anyrtc.live";

    public Notification getNotification(Context context){
        createNotificationChannel(context);
        Notification notification = createNotification(context);
        NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(NOTIFICATION_ID, notification);
        return notification;
    }

    @TargetApi(Build.VERSION_CODES.O)
    private void createNotificationChannel( Context context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                    NOTIFICATION_CHANNEL_ID,
                    NOTIFICATION_CHANNEL_NAME,
                    NotificationManager.IMPORTANCE_LOW
            );
            channel.setLockscreenVisibility(Notification.VISIBILITY_PRIVATE);
            NotificationManager manager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
            manager.createNotificationChannel(channel);
        }
    }

    private Notification createNotification(Context context){
        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, NOTIFICATION_CHANNEL_ID);
        builder.setContentTitle("屏幕录制");
        builder.setContentText("屏幕录制中");
        builder.setSmallIcon(R.drawable.screen_notification_icon);
        builder.setOngoing(true);
        builder.setCategory(Notification.CATEGORY_SERVICE);
        builder.setPriority(Notification.PRIORITY_LOW);
        builder.setShowWhen(true);
        return builder.build();
    }


}
