package com.example.v_mail;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import androidx.appcompat.app.AppCompatActivity;
import androidx.work.PeriodicWorkRequest;
import androidx.work.PeriodicWorkRequest.Builder;
import androidx.work.WorkInfo;
import androidx.work.WorkManager;

import com.example.v_mail.background.MailUpdateWorker;
import com.example.v_mail.ui.LoginActivity;
import com.example.v_mail.ui.SignupActivity;

import com.example.v_mail.api.ApiClient;

import java.util.concurrent.TimeUnit;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ApiClient.init(this);

        Button loginButton = findViewById(R.id.loginButton);
        Button signupButton = findViewById(R.id.signupButton);

        loginButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this,
                        LoginActivity.class);
                startActivity(intent);
            }
        });

        signupButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this,
                        SignupActivity.class);
                startActivity(intent);
            }
        });


        PeriodicWorkRequest workRequest = new
                PeriodicWorkRequest.Builder(MailUpdateWorker.class,
                15, TimeUnit.MINUTES)
                .build();

        WorkManager.getInstance(getApplicationContext())
                .getWorkInfosForUniqueWorkLiveData("MailUpdate")
                .observe(this, workInfos -> {
                    for (WorkInfo workInfo : workInfos) {
                        Log.d("WorkManager", "State: " + workInfo.getState());
                    }
                });


        WorkManager.getInstance(getApplicationContext()).enqueue(workRequest);

    }
}