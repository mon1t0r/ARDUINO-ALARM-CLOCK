#include <RtcDS1302.h>

#include "rtc_wrapper.h"
#include "config.h"

ThreeWire myWire(RTC_DAT_PIN, RTC_CLK_PIN, RTC_RST_PIN);
RtcDS1302<ThreeWire> rtc(myWire);

void clock_init() {
    rtc.Begin();
}

void set_date(const byte day, const byte month, const int year) {
    RtcDateTime dt_old = rtc.GetDateTime();

    RtcDateTime dt_new(year, month, day, dt_old.Hour(), dt_old.Minute(), dt_old.Second());

    return rtc.SetDateTime(dt_new);
}

void set_time(const byte hours, const byte minutes, const byte seconds) {
    RtcDateTime dt_old = rtc.GetDateTime();

    RtcDateTime dt_new(dt_old.Year(), dt_old.Month(), dt_old.Day(), hours, minutes, seconds);

    return rtc.SetDateTime(dt_new);
}

void set_datetime(const byte day, const byte month, const int year, const byte hours, const byte minutes, const byte seconds) {
    RtcDateTime dt_new(year, month, day, hours, minutes, seconds);

    return rtc.SetDateTime(dt_new);
}

byte get_day() {
    return rtc.GetDateTime().Day();
}

byte get_month() {
    return rtc.GetDateTime().Month();
}

int get_year() {
    return rtc.GetDateTime().Year();
}

byte get_hours() {
    return rtc.GetDateTime().Hour();
}

byte get_minutes() {
    return rtc.GetDateTime().Minute();
}

byte get_seconds() {
    return rtc.GetDateTime().Second();
}

struct dt now() {
    RtcDateTime dt_old = rtc.GetDateTime();

    struct dt dt;
    dt.seconds = dt_old.Second();
    dt.minutes = dt_old.Minute();
    dt.hours = dt_old.Hour();
    dt.day = dt_old.Day();
    dt.month = dt_old.Month();
    dt.year = dt_old.Year();

    return dt;
}