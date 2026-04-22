#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates the ultra-low-power RC oscillator mode.
//
// The AM0805/AM1805 contains an integrated RC oscillator that consumes only
// 14 nA (vs. 55 nA for the crystal oscillator). It is less accurate but
// sufficient for many applications where precise timekeeping is not critical.
//
// Datasheet reference: Section 5.4 RC Oscillator, Table 5-1
//
// Three oscillator modes are available:
//   AMx8x5Xt32KHzNoSwitch     – XT always (most accurate, highest power)
//   AMx8x5Xt32KHzSwitchRcOnBat – XT on VCC, auto-switch to RC when on VBAT
//   AMx8x5nRc128Hz             – RC always (least accurate, 14 nA)
//
// This example:
//   - Starts in XT mode for accurate time setting
//   - Switches to pure RC mode
//   - Watches the OMODE bit to confirm the switch
//   - Demonstrates the RC calibration using setCalibrationValue()
//     (useful to compensate for the RC oscillator's temperature drift)
//
// Works with AM0805 (no power management) and AM1805.

AM0805 rtc;
stc_amx8x5_time_t stcTimeToSet;
stc_amx8x5_time_t* pMyTime;

// RC calibration offset in ppm (measure your device and adjust)
// Each step for RC = ~1.907 ppm in mode 0 (CMDR=00)
#define RC_CALIBRATION_PPM   0

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
        Serial.println("Time set from build timestamp.");
    }

    // Apply RC calibration if configured
    if (RC_CALIBRATION_PPM != 0)
    {
        int32_t iSteps = (int32_t)((float)RC_CALIBRATION_PPM / 1.90735f);
        rtc.setCalibrationValue(AMx8x5::enCalibrationMode::AMx8x5ModeCalibrateRC, iSteps);
        Serial.printf("RC calibration applied: %d ppm -> %d steps\n", RC_CALIBRATION_PPM, iSteps);
    }

    // Switch to RC oscillator (128 Hz, ~14 nA)
    // Note: Hundredths register is not valid in RC mode (datasheet section 5.4)
    en_result_t res = rtc.selectOscillatorMode(AMx8x5::enOscSelect::AMx8x5nRc128Hz);
    if (res != Ok)
    {
        Serial.printf("Error switching to RC oscillator: %d\n", res);
        while (1);
    }

    // Wait briefly for the oscillator to settle
    delay(10);

    // Confirm switch via OMODE bit in OSC_STATUS register
    uint8_t u8OscStatus;
    rtc.readByte(AMX8X5_REG_OSC_STATUS, &u8OscStatus);
    if (u8OscStatus & AMX8X5_REG_OSC_STATUS_OMODE_MSK)
    {
        Serial.println("RC oscillator active (OMODE=1). Ultra-low power mode.");
    }
    else
    {
        Serial.println("Warning: XT oscillator still active (OMODE=0).");
    }

    Serial.println("Running on RC oscillator. Checking every second...");
    Serial.println("Note: u8Hundredth is not valid in RC mode.");
}

void loop(void)
{
    rtc.getTime(&pMyTime);

    uint8_t u8OscStatus;
    rtc.readByte(AMX8X5_REG_OSC_STATUS, &u8OscStatus);

    Serial.printf("%02d:%02d:%02d  |  Osc: %s  |  OSC_STATUS=0x%02x\n",
                  pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second,
                  (u8OscStatus & AMX8X5_REG_OSC_STATUS_OMODE_MSK) ? "RC" : "XT",
                  u8OscStatus);

    delay(1000);
}
