package com.example.v_mail.background;

import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.content.Context;
import android.content.SharedPreferences;
import android.os.Build;

import androidx.annotation.NonNull;
import androidx.core.app.NotificationCompat;
import androidx.work.Worker;
import androidx.work.WorkerParameters;

import com.example.v_mail.mail.MailService;
import com.example.v_mail.mail.Email;

import java.util.List;

public class MailUpdateWorker extends Worker {

    private static final String CHANNEL_ID = "mail_updates";

    public MailUpdateWorker(@NonNull Context context, @NonNull WorkerParameters workerParams) {
        super(context, workerParams);
    }

    @NonNull
    @Override
    public Result doWork() {
        SharedPreferences sharedPreferences = getApplicationContext()
            .getSharedPreferences("vmail_prefs", Context.MODE_PRIVATE);
        String username = sharedPreferences.getString("username", "");
        String password = sharedPreferences.getString("password", "");

        if (username == null || username.isEmpty() || password == null || password.isEmpty()) {
            return Result.failure();
        }

        MailService mailService = new MailService(
            "82.179.140.18",
            25,
            "82.179.140.18",
            110
        );

        try {
            List<Email> emails = mailService.getInbox(username, password);

            if (!emails.isEmpty()) {
                showNotification("New Emails", "You have " + emails.size() + " new emails");
            }

            return Result.success();
        } catch (Exception e) {
            e.printStackTrace();
            return Result.retry();
        }
    }

    private void showNotification(String title, String message) {
        Context context = getApplicationContext();
        NotificationManager notificationManager =
        (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationChannel channel = new NotificationChannel(
                CHANNEL_ID,
                "Mail Updates",
                NotificationManager.IMPORTANCE_DEFAULT
            );
            notificationManager.createNotificationChannel(channel);
        }

        NotificationCompat.Builder notificationBuilder = new NotificationCompat.Builder(context, CHANNEL_ID)
            .setSmallIcon(android.R.drawable.ic_dialog_email)
            .setContentTitle(title)
            .setContentText(message)
            .setPriority(NotificationCompat.PRIORITY_DEFAULT);

        notificationManager.notify(1, notificationBuilder.build());
    }
}
