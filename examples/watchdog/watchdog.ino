#include <amx8x5.h>
#include <Wire.h>

// This example configures the watchdog timer of the AM1805.
// The watchdog must be retriggered (by calling rtc.setWatchdog() again) within
// the configured period, otherwise the watchdog fires and generates a hardware
// reset on the nRST pin.
//
// The loop prints the current time every second and retriggers the watchdog.
// To test the reset, comment out the setWatchdog() call in loop().
//
// Requires AM1805 or AM1815 (with power management, nRST pin available).

AM1805 rtc;
stc_amx8x5_time_t stcTimeToSet;
stc_amx8x5_time_t* pMyTime;

// Watchdog period in milliseconds (here: 4 seconds)
#define WATCHDOG_PERIOD_MS   4000

void setup () {
    stcTimeToSet.u8Hundredth = 0;
    stcTimeToSet.u8Second    = BUILD_SECOND;
    stcTimeToSet.u8Minute    = BUILD_MINUTE;
    stcTimeToSet.u8Hour      = BUILD_HOUR;
    stcTimeToSet.u8Date      = BUILD_DAY;
    stcTimeToSet.u8Month     = BUILD_MONTH;
    stcTimeToSet.u8Year      = BUILD_YEAR - 2000;
    stcTimeToSet.u8Weekday   = 0;
    stcTimeToSet.u8Century   = 1; //year is 20xx, 0 would be 19xx or 21xx

    Wire.begin();

    Serial.begin(115200);
    while (!rtc.begin())
    {
        Serial.println("RTC not initialized...");
        delay(1000);
    }

    rtc.getTime(&pMyTime);
    if (pMyTime->u8Year == 0)
    {
        rtc.setTime(&stcTimeToSet, false);
    }

    // Check if last reset was caused by watchdog
    uint8_t u8Status;
    rtc.readByte(AMX8X5_REG_STATUS, &u8Status);
    if (u8Status & AMX8X5_REG_STATUS_WDT_MSK)
    {
        Serial.println("*** System was reset by watchdog! ***");
        rtc.clearRegister(AMX8X5_REG_STATUS, AMX8X5_REG_STATUS_WDT_MSK);
    }

    // Enable watchdog: fires on nRST pin if not retriggered within WATCHDOG_PERIOD_MS
    en_result_t res = rtc.setWatchdog(WATCHDOG_PERIOD_MS,
                                      AMx8x5::enWatchdogInterruptPin::AMx8x5WatchdogInterruptPinnRST);
    if (res != Ok)
    {
        Serial.printf("Error: Could not set watchdog... reason: %d\n", res);
        while (1);
    }

    Serial.printf("Watchdog armed: %d ms timeout, reset via nRST.\n", WATCHDOG_PERIOD_MS);
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("\n%02d:%02d:%02d.%d\n", pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second, pMyTime->u8Hundredth);
    Serial.printf("20%02d-%02d-%02d Weekday: %d\n", pMyTime->u8Year, pMyTime->u8Month, pMyTime->u8Date, pMyTime->u8Weekday);

    // Retrigger watchdog by re-arming it within the timeout period.
    // Comment this out to test the watchdog reset!
    rtc.setWatchdog(WATCHDOG_PERIOD_MS,
                    AMx8x5::enWatchdogInterruptPin::AMx8x5WatchdogInterruptPinnRST);

    Serial.println("Watchdog retriggered.");
    delay(1000);
}
