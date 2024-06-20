#include <zephyr/kernel.h>
#include <zephyr/drivers/rtc.h>
#include <zephyr/logging/log.h>

const struct device *const rtc = DEVICE_DT_GET(DT_NODELABEL(rv_8263_c8));

LOG_MODULE_REGISTER(main, LOG_LEVEL_INF);

struct rtc_time time;

#if CONFIG_RTC_ALARM
static uint16_t mask;
#endif

#if CONFIG_RTC_UPDATE
void on_rtc_update(const struct device *dev, void *user_data)
{
    rtc_get_time(rtc, &time);
    LOG_INF("Seconds: %u", time.tm_sec);
    LOG_INF("Minute: %u", time.tm_min);
    LOG_INF("Hour: %u", time.tm_hour);
    LOG_INF("Weekday: %u", time.tm_wday);
    LOG_INF("Day of month: %u", time.tm_mday);
    LOG_INF("Month: %u", time.tm_mon);
    LOG_INF("Year: %u", time.tm_year);
}
#endif

#if CONFIG_RTC_ALARM
void on_rtc_alarm(const struct device *dev, uint16_t id, void *user_data)
{
    struct rtc_time alarm_time;

    LOG_INF("Alarm!");

    rtc_get_time(rtc, &alarm_time);
    alarm_time.tm_sec += 10;
    rtc_alarm_set_time(rtc, id, mask, &alarm_time);
}
#endif

int main(void)
{
    if (!device_is_ready(rtc)) {
        LOG_ERR("Device not ready!");
        return -1;
    }

    time.tm_hour = 00;
    time.tm_min = 1;
    time.tm_sec = 0;
    time.tm_wday = 0;
    time.tm_mday = 1;
    time.tm_mon = 1;
    time.tm_year = 124;
    rtc_set_time(rtc, &time);

#if CONFIG_RTC_ALARM
    uint16_t id = 0;
    struct rtc_time alarm_time;

    rtc_alarm_get_supported_fields(rtc, id, &mask);

    LOG_INF("Alarm fields: 0x%X", mask);
    LOG_INF("Alarm pending: %u", rtc_alarm_is_pending(rtc, id));

    rtc_get_time(rtc, &alarm_time);
    alarm_time.tm_sec += 10;
    rtc_alarm_set_time(rtc, id, mask, &alarm_time);
    rtc_alarm_set_callback(rtc, id, on_rtc_alarm, NULL);
#endif

#if CONFIG_RTC_UPDATE
    rtc_update_set_callback(rtc, on_rtc_update, NULL);
#endif

#if CONFIG_RTC_UPDATE
    int32_t calibration;

    rtc_set_calibration(rtc, 120000);
    rtc_get_calibration(rtc, &calibration);
    LOG_INF("Calibration: %i", calibration);
#endif

    while (1) {
#ifndef CONFIG_RTC_UPDATE
        rtc_get_time(rtc, &time);
        LOG_INF("Seconds: %u", time.tm_sec);
        LOG_INF("Minute: %u", time.tm_min);
        LOG_INF("Hour: %u", time.tm_hour);
        LOG_INF("Weekday: %u", time.tm_wday);
        LOG_INF("Day of month: %u", time.tm_mday);
        LOG_INF("Month: %u", time.tm_mon);
        LOG_INF("Year: %u", time.tm_year);
#endif
        k_msleep(20000);
    }

    return 0;
}