package com.mon1tor.clockconfigurator;

import android.content.Intent;
import android.nfc.cardemulation.HostApduService;
import android.os.Bundle;
import android.util.Log;

import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import java.util.Calendar;

public class MyHostApduService extends HostApduService {

    private SettingsData settingsData;

    @Override
    public byte[] processCommandApdu(byte[] commandApdu, Bundle extras) {
        if(settingsData == null) {
            return null;
        }

        actualizeDateTimeIfNeeded();

        return new byte[] {
                (byte) 0xF1,
                (byte) (settingsData.day           & 0xff),
                (byte) (settingsData.month         & 0xff),
                (byte) ((settingsData.year     )   & 0xff),
                (byte) ((settingsData.year >> 8)   & 0xff),
                (byte) (settingsData.hours         & 0xff),
                (byte) (settingsData.minutes       & 0xff),
                (byte) (settingsData.seconds       & 0xff),

                (byte) (settingsData.alarmHours & 0xff),
                (byte) (settingsData.alarmMinutes & 0xff),
                (byte) (settingsData.alarmEnabled & 0xff),
                (byte) (settingsData.ldrEnabled & 0xff)
        };
    }

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        settingsData = (SettingsData) intent.getSerializableExtra("SettingsData");
        return START_STICKY;
    }

    @Override
    public void onDeactivated(int reason) {
        Intent intent = new Intent(MainActivity.ACTION_NFC_TRANSFER_END);
        LocalBroadcastManager.getInstance(this).sendBroadcast(intent);
    }

    private void actualizeDateTimeIfNeeded() {
        if(settingsData.day < 0) {
            final Calendar c = Calendar.getInstance();

            settingsData.year = c.get(Calendar.YEAR);
            settingsData.month = (byte) (c.get(Calendar.MONTH) + 1);
            settingsData.day = (byte) c.get(Calendar.DAY_OF_MONTH);

            settingsData.hours = (byte) c.get(Calendar.HOUR_OF_DAY);
            settingsData.minutes = (byte) c.get(Calendar.MINUTE);
            settingsData.seconds = (byte) c.get(Calendar.SECOND);
        }
    }
}
