package com.mon1tor.clockconfigurator;

import android.annotation.SuppressLint;
import android.app.DatePickerDialog;
import android.app.TimePickerDialog;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.pm.PackageManager;
import android.nfc.NfcAdapter;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import android.widget.Toast;

import androidx.activity.EdgeToEdge;
import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.appcompat.widget.SwitchCompat;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.localbroadcastmanager.content.LocalBroadcastManager;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.Date;

public class MainActivity extends AppCompatActivity {

    public static final String ACTION_NFC_TRANSFER_END = "nfc-transfer-end";

    private EditText editTextDate, editTextTime, editTextAlarmTime;
    private Button buttonSelectDate, buttonSelectTime, buttonSelectAlarmTime, buttonWriteSettings;
    private SwitchCompat switchAutoTime, switchAlarm, switchLDR;
    private TextView textViewNFCMessage;

    private final BroadcastReceiver messageReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            MainActivity.this.onNFCTransferEnd();
        }
    };

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main);

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        editTextDate = findViewById(R.id.editTextDate);
        editTextTime = findViewById(R.id.editTextTime);
        editTextAlarmTime = findViewById(R.id.editTextAlarmTime);

        buttonSelectDate = findViewById(R.id.buttonSelectDate);
        buttonSelectTime = findViewById(R.id.buttonSelectTime);
        buttonSelectAlarmTime = findViewById(R.id.buttonSelectAlarmTime);
        buttonWriteSettings = findViewById(R.id.buttonWriteSettings);

        switchAutoTime = findViewById(R.id.switchAutoTime);
        switchAlarm = findViewById(R.id.switchAlarm);
        switchLDR = findViewById(R.id.switchLDR);

        textViewNFCMessage = findViewById(R.id.textViewNFCMessage);

        buttonSelectDate.setOnClickListener(this::onClickButtonSelectDate);
        buttonSelectTime.setOnClickListener(this::onClickButtonSelectTime);
        buttonSelectAlarmTime.setOnClickListener(this::onClickButtonSelectAlarmTime);
        buttonWriteSettings.setOnClickListener(this::onClickButtonWriteSettings);

        switchAutoTime.setOnClickListener(this::onClickSwitchAutoTime);
    }

    @Override
    protected void onPause() {
        LocalBroadcastManager.getInstance(this).unregisterReceiver(messageReceiver);
        super.onPause();
    }

    @Override
    public void onResume() {
        super.onResume();
        LocalBroadcastManager.getInstance(this)
                .registerReceiver(messageReceiver, new IntentFilter(ACTION_NFC_TRANSFER_END));
    }

    private void onClickButtonSelectDate(View v) {
        DatePickerDialog datePickerDialog = createDatePicker(
                (view, year, month, day) -> editTextDate.setText(getString(R.string.date_display, day, month + 1, year)));
        datePickerDialog.show();
    }

    private void onClickButtonSelectTime(View v) {
        TimePickerDialog timePickerDialog = createTimePicker(
                (view, hour, minute) -> {
                    int currentSecond = Calendar.getInstance().get(Calendar.SECOND);
                    editTextTime.setText(getString(R.string.time_display, hour, minute, currentSecond));
                });
        timePickerDialog.show();
    }

    private void onClickButtonSelectAlarmTime(View v) {
        TimePickerDialog timePickerDialog = createTimePicker(
                (view, hour, minute) -> editTextAlarmTime.setText(getString(R.string.alarm_time_display, hour, minute)));
        timePickerDialog.show();
    }

    @SuppressLint("SimpleDateFormat")
    private void onClickButtonWriteSettings(View v) {
        SettingsData value = new SettingsData();

        if(!switchAutoTime.isChecked()) {
            // Parse date
            SimpleDateFormat sdf = new SimpleDateFormat("dd.MM.yyyy");
            Date date;
            try {
                date = sdf.parse(String.valueOf(editTextDate.getText()));
                if(date == null) {
                    throw new ParseException("", 0);
                }
            } catch (ParseException e) {
                Toast.makeText(this, "Unable to parse date.", Toast.LENGTH_SHORT).show();
                return;
            }

            value.day = (byte) date.getDate();
            value.month = (byte) (date.getMonth() + 1);
            value.year = 1900 + date.getYear();

            // Parse time
            sdf = new SimpleDateFormat("hh:mm:ss");
            try {
                date = sdf.parse(String.valueOf(editTextTime.getText()));
                if(date == null) {
                    throw new ParseException("", 0);
                }
            } catch (ParseException e) {
                Toast.makeText(this, "Unable to parse time.", Toast.LENGTH_SHORT).show();
                return;
            }

            value.hours = (byte) date.getHours();
            value.minutes = (byte) date.getMinutes();
            value.seconds = (byte) date.getSeconds();
        } else {
            value.day = -1;
        }

        SimpleDateFormat sdf = new SimpleDateFormat("hh:mm");
        Date date;
        try {
            date = sdf.parse(String.valueOf(editTextAlarmTime.getText()));
            if(date == null) {
                throw new ParseException("", 0);
            }
        } catch (ParseException e) {
            Toast.makeText(this, "Unable to parse alarm time.", Toast.LENGTH_SHORT).show();
            return;
        }

        value.alarmHours = (byte) date.getHours();
        value.alarmMinutes = (byte) date.getMinutes();

        value.alarmEnabled = (byte) (switchAlarm.isChecked() ? 1 : 0);
        value.ldrEnabled = (byte) (switchLDR.isChecked() ? 1 : 0);

        setControlsState(false);

        getPackageManager().setComponentEnabledSetting(
                new ComponentName(MainActivity.this, MyHostApduService.class),
                PackageManager.COMPONENT_ENABLED_STATE_ENABLED,
                PackageManager.DONT_KILL_APP);

        Intent serviceIntent = new Intent(this, MyHostApduService.class);
        serviceIntent.putExtra("SettingsData", value);
        startService(serviceIntent);
    }

    private void onNFCTransferEnd() {
        setControlsState(true);

        Intent intent = new Intent(this, MyHostApduService.class);
        stopService(intent);

        getPackageManager().setComponentEnabledSetting(
                new ComponentName(MainActivity.this, MyHostApduService.class),
                PackageManager.COMPONENT_ENABLED_STATE_DISABLED,
                PackageManager.DONT_KILL_APP);
    }

    private void onClickSwitchAutoTime(View v) {
        setDateTimeFieldsState(!switchAutoTime.isChecked());
    }

    private void setControlsState(boolean enabled) {
        textViewNFCMessage.setVisibility(enabled ? View.INVISIBLE : View.VISIBLE);

        editTextAlarmTime.setEnabled(enabled);
        buttonSelectAlarmTime.setEnabled(enabled);

        buttonWriteSettings.setEnabled(enabled);
        switchAutoTime.setEnabled(enabled);
        switchAlarm.setEnabled(enabled);
        switchLDR.setEnabled(enabled);

        setDateTimeFieldsState(enabled && !switchAutoTime.isChecked());
    }

    private void setDateTimeFieldsState(boolean enabled) {
        editTextDate.setEnabled(enabled);
        editTextTime.setEnabled(enabled);
        buttonSelectDate.setEnabled(enabled);
        buttonSelectTime.setEnabled(enabled);
    }

    @NonNull
    private DatePickerDialog createDatePicker(DatePickerDialog.OnDateSetListener listener) {
        final Calendar c = Calendar.getInstance();
        int currentYear = c.get(Calendar.YEAR);
        int currentMonth = c.get(Calendar.MONTH);
        int currentDay = c.get(Calendar.DAY_OF_MONTH);

        return new DatePickerDialog(this,
                listener, currentYear, currentMonth, currentDay);
    }

    @NonNull
    private TimePickerDialog createTimePicker(TimePickerDialog.OnTimeSetListener listener) {
        final Calendar c = Calendar.getInstance();
        int currentHour = c.get(Calendar.HOUR_OF_DAY);
        int currentMinute = c.get(Calendar.MINUTE);

        return new TimePickerDialog(this,
                listener, currentHour, currentMinute, true);
    }
}