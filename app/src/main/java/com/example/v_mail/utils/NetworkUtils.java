package com.example.v_mail.utils;

import android.content.Context;
import android.net.wifi.WifiManager;
import android.text.format.Formatter;

import okhttp3.Call;
import okhttp3.Callback;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.Response;

import java.io.IOException;
import java.util.concurrent.CompletableFuture;

public class NetworkUtils {

    public static String getLocalIpAddress(Context context) {
        WifiManager wifiManager = (WifiManager) context.getApplicationContext().getSystemService(Context.WIFI_SERVICE);
        int ipAddress = wifiManager.getConnectionInfo().getIpAddress();
        return Formatter.formatIpAddress(ipAddress);
    }

    public static CompletableFuture<String> getExternalIpAddress() {
        CompletableFuture<String> futureIp = new CompletableFuture<>();
        OkHttpClient client = new OkHttpClient();

        Request request = new Request.Builder()
                .url("https://api.ipify.org?format=json")
                .build();

        client.newCall(request).enqueue(new Callback() {
            @Override
            public void onFailure(Call call, IOException e) {
                futureIp.completeExceptionally(e);
            }

            @Override
            public void onResponse(Call call, Response response) throws IOException {
                if (response.isSuccessful()) {
                    String responseBody = response.body().string();
                    String ip = responseBody.replaceAll(".*\"ip\":\"(.*?)\".*", "$1");
                    futureIp.complete(ip);
                } else {
                    futureIp.completeExceptionally(new IOException("Unexpected code " + response));
                }
            }
        });

        return futureIp;
    }
}
