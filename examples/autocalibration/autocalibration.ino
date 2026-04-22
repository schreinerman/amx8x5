#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates the RC Oscillator Autocalibration feature.
//
// The AM1805 uses the XT (crystal) oscillator for high accuracy, but automatically
// calibrates the internal RC oscillator to match the XT frequency. This allows
// extremely low-power operation on VBAT (RC only) while still achieving XT-like
// accuracy after a switchover.
//
// Datasheet reference: Section 5.10 Autocalibration
//
// Hardware note: Connect a 47 pF ceramic capacitor between the AF pin and GND
// for improved autocalibration accuracy (required for the AF filter below).
//
// This example:
//  - Selects XT oscillator with automatic switch to RC on battery power (AOS bit)
//  - Enables the AF filter capacitor via AFCTRL register
//  - Enables periodic autocalibration every 1024 s (~17 min)
//  - Enables the Autocalibration Fail interrupt so errors are reported
//  - Prints oscillator status every second

AM1805 rtc;
stc_amx8x5_time_t stcTimeToSet;
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

    // Select XT oscillator, automatically switch to RC oscillator when on VBAT
    rtc.selectOscillatorMode(AMx8x5::enOscSelect::AMx8x5Xt32KHzSwitchRcOnBat);

    // Enable the AF filter: write 0xA0 to AFCTRL register (0x26)
    // Requires Configuration Key 0x9D first (datasheet section 5.10.5)
    rtc.writeByte(AMX8X5_REG_CONFIG_KEY, AMX8X5_REG_CONFIG_KEY_VAL_OTHER);
    rtc.writeByte(AMX8X5_REG_ACAL_FLT, 0xA0);

    // Enable periodic autocalibration every 1024 seconds (~17 minutes)
    en_result_t res = rtc.setAutocalibration(
        AMx8x5::enAutocalibrationPeriod::AMx8x5AutoCalibrationPeriodCycleSecods1024);
    if (res != Ok)
    {
        Serial.printf("Error: Could not set autocalibration: %d\n", res);
        while (1);
    }

    // Enable interrupt when autocalibration fails (ACIE bit in OSC_CONTROL)
    rtc.enableIrqAutocalibFail(true);

    Serial.println("Autocalibration enabled (every 1024 s, AF filter active).");
    Serial.println("Printing oscillator status every second...");
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("\n%02d:%02d:%02d\n", pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second);

    // Read oscillator status register
    uint8_t u8OscStatus;
    rtc.readByte(AMX8X5_REG_OSC_STATUS, &u8OscStatus);
    Serial.printf("OSC Status (0x1D): 0x%02x  |  ", u8OscStatus);

    if (u8OscStatus & AMX8X5_REG_OSC_STATUS_OMODE_MSK)
    {
        Serial.print("[RC oscillator active]  ");
    } else
    {
        Serial.print("[XT oscillator active]  ");
    }
    if (u8OscStatus & AMX8X5_REG_OSC_STATUS_OF_MSK)
    {
        Serial.print("[OSCILLATOR FAILURE!]  ");
    }
    if (u8OscStatus & AMX8X5_REG_OSC_STATUS_ACF_MSK)
    {
        Serial.print("[AUTOCALIB FAIL!]  ");
    }
    if (u8OscStatus & AMX8X5_REG_OSC_STATUS_LKO2_MSK)
    {
        Serial.print("[LKO2]  ");
    }
    Serial.println();

    // Check status register for autocalib fail interrupt
    uint8_t u8Status;
    rtc.readByte(AMX8X5_REG_STATUS, &u8Status);
    if (u8Status & AMX8X5_REG_STATUS_BAT_MSK)
    {
        Serial.println("  --> Running on VBAT (VCC lost)!");
    }

    delay(1000);
}
