#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates manual XT oscillator digital calibration.
//
// The goal is to compensate for crystal frequency deviation and achieve
// ±2 ppm accuracy without autocalibration hardware.
//
// Procedure (from datasheet section 5.9.1):
//   1. Connect the FOUT/nIRQ pin to a frequency counter.
//   2. Configure a 32.768 kHz square wave output.
//   3. Measure the exact frequency (Fmeas) in Hz.
//   4. Compute:  PAdj = ((32768 - Fmeas) * 1000000) / 32768    [ppm]
//                Adj  = PAdj / 1.90735                          [steps]
//   5. Depending on Adj, set XTCAL, CMDX and OFFSETX accordingly.
//   6. Call Amx8x5_SetCalibrationValue() which handles all register writes.
//
// This sketch:
//   - Enables 32.768 kHz output on FOUT/nIRQ for external measurement.
//   - Shows how to apply a calibration value via setCalibrationValue().
//   - Prints the oscillator status and the calibration registers.
//
// After measuring your crystal, adjust CALIBRATION_PPM below and re-flash.
// Set CALIBRATION_PPM = 0 to disable calibration (reset to factory default).
//
// Requires any AM0805 or AM1805.

AM0805 rtc;
stc_amx8x5_time_t stcTimeToSet;
stc_amx8x5_time_t* pMyTime;

// *** Adjust this value to your measured crystal deviation in ppm. ***
// Positive value: crystal runs too slow  (add pulses)
// Negative value: crystal runs too fast  (remove pulses)
// Example: crystal measured at 32767.3 Hz -> PAdj = (32768-32767.3)*1e6/32768 = +21.4 ppm
#define CALIBRATION_PPM   0

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

    // Select XT oscillator (no automatic switching)
    rtc.selectOscillatorMode(AMx8x5::enOscSelect::AMx8x5Xt32KHzNoSwitch);

    // Enable 32.768 kHz square wave output on FOUT/nIRQ for measurement
    // setSquareWaveOutput with pinMsk=1 enables O1EN (FOUT/nIRQ) internally
    rtc.setSquareWaveOutput(1, 1); // SQFS=1 -> 32.768 kHz, pinMsk=1 = FOUT/nIRQ

    Serial.println("32.768 kHz output active on FOUT/nIRQ.");
    Serial.println("Measure frequency with a counter, then set CALIBRATION_PPM and re-flash.");

    // Apply calibration if non-zero
    if (CALIBRATION_PPM != 0)
    {
        // Convert ppm to adjustment steps: Adj = PPM / (1e6 / 2^19) = PPM / 1.90735
        int32_t iAdjSteps = (int32_t)((float)CALIBRATION_PPM / 1.90735f);
        Serial.printf("Applying calibration: %d ppm -> %d steps\n", CALIBRATION_PPM, iAdjSteps);

        en_result_t res = rtc.setCalibrationValue(
            AMx8x5::enCalibrationMode::AMx8x5ModeCalibrateXT,
            iAdjSteps);
        if (res != Ok)
        {
            Serial.printf("Error: Calibration failed: %d\n", res);
        }
        else
        {
            Serial.println("XT calibration applied successfully.");
        }
    }
    else
    {
        Serial.println("No calibration applied (CALIBRATION_PPM = 0).");
    }
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("\n%02d:%02d:%02d.%02d  |  ",
                  pMyTime->u8Hour, pMyTime->u8Minute,
                  pMyTime->u8Second, pMyTime->u8Hundredth);

    // Print calibration registers
    uint8_t u8CalXT, u8OscStatus;
    rtc.readByte(AMX8X5_REG_CAL_XT, &u8CalXT);
    rtc.readByte(AMX8X5_REG_OSC_STATUS, &u8OscStatus);

    Serial.printf("CAL_XT=0x%02x  OSC_STATUS=0x%02x  [%s]\n",
                  u8CalXT, u8OscStatus,
                  (u8OscStatus & AMX8X5_REG_OSC_STATUS_OMODE_MSK) ? "RC" : "XT");

    delay(1000);
}
