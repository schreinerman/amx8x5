#include <amx8x5.h>
#include <Wire.h>

// This example sets a one-time alarm that triggers 1 minute after boot.
// The alarm wakes up the system via PSW/nIRQ2 (sleep mode).
// After wakeup, the alarm flag is cleared and the time is printed.
//
// Requires AM1805 or AM1815 (with power management).

AM1805 rtc;
stc_amx8x5_time_t stcTimeToSet;
stc_amx8x5_time_t stcAlarmTime;
stc_amx8x5_time_t* pMyTime;

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

    rtc.enableTrickleCharger(AMx8x5::enTrickleDiode::AMx8x5TrickleDiodeSchottky,
                             AMx8x5::enTrickleResistor::AMx8x5TrickleResistor3K, true);

    // use power switch to possibly power-off the whole system
    rtc.setPswHighCurrent(true);

    // unlock configuration
    rtc.writeByte(AMX8X5_REG_CONFIG_KEY, AMX8X5_CONFIG_KEY_VAL); //enable AMX8X5_REG_OSC_CONTROL register
    rtc.setRegister(AMX8X5_REG_OSC_CONTROL, AMX8X5_REG_OSC_CONTROL_PWGT_MSK); //disable I2C interface during sleep

    // set alarm 1 minute after current time
    rtc.getTime(&pMyTime);
    stcAlarmTime           = *pMyTime;
    stcAlarmTime.u8Minute  = (pMyTime->u8Minute + 1) % 60;
    stcAlarmTime.u8Second  = 0;
    stcAlarmTime.u8Hundredth = 0;

    Serial.printf("Current time:  %02d:%02d:%02d\n", pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second);
    Serial.printf("Alarm set for: %02d:%02d:%02d\n", stcAlarmTime.u8Hour, stcAlarmTime.u8Minute, stcAlarmTime.u8Second);

    rtc.setAlarm(&stcAlarmTime,
                 AMx8x5::enAlarmRepeat::AMx8x5AlarmMinute,
                 AMx8x5::enInterruptMode::AMx8x5InterruptModePulseLong,
                 AMx8x5::enInterruptPin::AMx8x5InterruptIrq2);

    // try to sleep / power-off the whole system until alarm fires
    en_result_t res = rtc.setSleepMode(0, AMx8x5::enSleepMode::AMx8x5PswIrq2HighInSleep);
    if (res != Ok)
    {
        Serial.printf("Error: Could not sleep... reason: %d\n", res);
        while (1);
    }
}

// loop is never reached if the RTC is also used to switch-off power
void loop(void)
{
    uint8_t u8Status;
    rtc.readByte(AMX8X5_REG_STATUS, &u8Status);
    if (u8Status == 0xFF)
    {
        Serial.println("RTC not initialized... maybe sleeping?");
        while (u8Status == 0xFF)
        {
            rtc.readByte(AMX8X5_REG_STATUS, &u8Status);
        }
    } else
    {
        rtc.getTime(&pMyTime);
        Serial.printf("\n%02d:%02d:%02d.%d\n", pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second, pMyTime->u8Hundredth);
        Serial.printf("20%02d-%02d-%02d Weekday: %d\n", pMyTime->u8Year, pMyTime->u8Month, pMyTime->u8Date, pMyTime->u8Weekday);
        Serial.printf("Status: 0x%02x\n", u8Status);
        if (u8Status & AMX8X5_REG_STATUS_ALM_MSK)
        {
            Serial.println("Alarm wakeup was triggered!");
            rtc.clearRegister(AMX8X5_REG_STATUS, AMX8X5_REG_STATUS_ALM_MSK);
        }
    }
    delay(1000);
}
