package com.example.v_mail.api;

import android.content.Context;
import android.content.SharedPreferences;

import com.example.v_mail.dto.request.TokenRequest;
import com.example.v_mail.dto.response.AuthenticationResponse;

import okhttp3.OkHttpClient;
import retrofit2.Retrofit;
import retrofit2.converter.gson.GsonConverterFactory;
import retrofit2.Call;
import retrofit2.Callback;
import retrofit2.Response;

public class ApiClient {
    private static final String BASE_URL = "http://localhost:8081/api/auth/";
    private static Retrofit retrofit;
    private static SharedPreferences sharedPreferences;
    private static final String PREFS_NAME = "vMailPrefs";
    private static final String ACCESS_TOKEN_KEY = "accessToken";
    private static final String REFRESH_TOKEN_KEY = "refreshToken";

    public static void init(Context context) {
        sharedPreferences = context.getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE);

        OkHttpClient client = new OkHttpClient.Builder()
                .addInterceptor(new AuthInterceptor(context))
                .build();

        if (retrofit == null) {
            retrofit = new Retrofit.Builder()
                    .baseUrl(BASE_URL)
                    .client(client)
                    .addConverterFactory(GsonConverterFactory.create())
                    .build();
        }
    }

    public static AuthApi getAuthApi() {
        return retrofit.create(AuthApi.class);
    }

    public static void saveTokens(String accessToken, String refreshToken) {
        SharedPreferences.Editor editor = sharedPreferences.edit();
        editor.putString(ACCESS_TOKEN_KEY, accessToken);
        editor.putString(REFRESH_TOKEN_KEY, refreshToken);
        editor.apply();
    }

    public static String getAccessToken() {
        return sharedPreferences.getString(ACCESS_TOKEN_KEY, null);
    }

    public static String getRefreshToken() {
        return sharedPreferences.getString(REFRESH_TOKEN_KEY, null);
    }

    public static void refreshAccessToken(final TokenRequest tokenRequest, final Callback<AuthenticationResponse> callback) {
        AuthApi authApi = getAuthApi();
        authApi.refreshToken(tokenRequest).enqueue(new Callback<AuthenticationResponse>() {
            @Override
            public void onResponse(Call<AuthenticationResponse> call, Response<AuthenticationResponse> response) {
                if (response.isSuccessful()) {
                    AuthenticationResponse newAuthResponse = response.body();
                    saveTokens(newAuthResponse.getToken(), newAuthResponse.getRefreshToken());
                    callback.onResponse(call, response);
                } else {
                    callback.onFailure(call, new Throwable("Failed to refresh token"));
                }
            }

            @Override
            public void onFailure(Call<AuthenticationResponse> call, Throwable t) {
                callback.onFailure(call, t);
            }
        });
    }
}
