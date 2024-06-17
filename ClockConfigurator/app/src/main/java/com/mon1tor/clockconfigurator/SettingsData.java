package com.mon1tor.clockconfigurator;

import java.io.Serializable;

public final class SettingsData implements Serializable {

    public byte day;
    public byte month;
    public int year;
    public byte hours;
    public byte minutes;
    public byte seconds;

    public byte alarmHours;
    public byte alarmMinutes;
    public byte alarmEnabled;
    public byte ldrEnabled;
}
