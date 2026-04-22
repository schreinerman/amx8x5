#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates:
//   1. Software Reset (datasheet section 5.14)
//   2. Oscillator Fail Detection and interrupt (datasheet section 5.11)
//   3. FOS bit: automatic fallback to RC oscillator on XT failure
//
// Software Reset:
//   Writing 0x3C to the Configuration Key register (0x1F) triggers a software
//   reset identical to a hardware power-on reset. All registers return to reset
//   values. This is recommended after power-up when VBAT was connected and
//   the analog comparator may have disturbed register contents.
//
// Oscillator Fail Detection:
//   If the XT oscillator is selected but fails (e.g. crystal broken, no crystal
//   fitted), the OF flag in the Oscillator Status register (0x1D) is set.
//   When OFIE (bit 1 of OSC_CONTROL) is set, an IRQ is generated as well.
//   The FOS bit (bit 3 of OSC_CONTROL) makes the RTC automatically switch to
//   the RC oscillator on oscillator failure - crucial for fail-safe clocking.
//
// This example:
//   - Issues a software reset on first boot (detected via GP0 marker in RAM)
//   - Enables the oscillator fail interrupt and automatic fallback to RC (FOS)
//   - Enables the oscillator fail interrupt output
//   - Polls OF and OMODE flags every second and reports any failure
//
// Works with AM0805 and AM1805.

AM0805 rtc;
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

    // Check GP0 bit (bit 7 of Seconds register) as power-on marker
    uint8_t u8Seconds;
    rtc.readByte(AMX8X5_REG_SECONDS, &u8Seconds);
    bool bFirstBoot = !(u8Seconds & AMX8X5_REG_SECONDS_GP0_MSK);

    if (bFirstBoot)
    {
        Serial.println("First boot: issuing Software Reset...");

        // Software Reset: write 0x3C to Configuration Key (0x1F)
        // This resets all registers to their power-on default values.
        // Note: re-initialize the RTC after reset.
        rtc.writeByte(AMX8X5_REG_CONFIG_KEY, 0x3C);

        // Re-initialize after reset (re-run begin())
        delay(10);
        while (!rtc.begin())
        {
            Serial.println("RTC not responding after reset...");
            delay(1000);
        }
        Serial.println("Software Reset complete. Setting time...");
        rtc.setTime(&stcTimeToSet, false);

        // Set GP0 = 1 to mark initialization done
        rtc.setRegister(AMX8X5_REG_SECONDS, AMX8X5_REG_SECONDS_GP0_MSK);
    }
    else
    {
        Serial.println("Normal boot (no reset needed).");
    }

    // Select XT oscillator without automatic switching
    rtc.selectOscillatorMode(AMx8x5::enOscSelect::AMx8x5Xt32KHzNoSwitch);

    // Enable Oscillator Fail Interrupt (OFIE bit in OSC_CONTROL)
    rtc.enableIrqOscillatorFail(true);

    // Enable automatic fallback to RC oscillator on XT failure (FOS bit)
    // Note: enableIrqOscillatorFail() already unlocks OSC_CONTROL via Config Key
    rtc.writeByte(AMX8X5_REG_CONFIG_KEY, AMX8X5_REG_CONFIG_KEY_VAL_OSC);
    rtc.setRegister(AMX8X5_REG_OSC_CONTROL, AMX8X5_REG_OSC_CONTROL_FOS_MSK);

    // Enable FOUT/nIRQ output for the oscillator fail interrupt
    rtc.enableOutput(AMX8X5_REG_OCTRL_O1EN_MSK, true);
    rtc.setOut1Mode(AMx8x5::enOut1Mode::AMx8x5Out1nIRQAtIrqElseOut);

    Serial.println("Oscillator fail detection active.");
    Serial.println("  - OFIE: interrupt on oscillator failure");
    Serial.println("  - FOS:  auto-switch to RC on failure");
    Serial.println("Monitoring OSC_STATUS every second...");
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("\n%02d:%02d:%02d  |  ", pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second);

    uint8_t u8OscStatus;
    rtc.readByte(AMX8X5_REG_OSC_STATUS, &u8OscStatus);
    Serial.printf("OSC_STATUS=0x%02x  [%s]",
                  u8OscStatus,
                  (u8OscStatus & AMX8X5_REG_OSC_STATUS_OMODE_MSK) ? "RC" : "XT");

    if (u8OscStatus & AMX8X5_REG_OSC_STATUS_OF_MSK)
    {
        Serial.println("\n  *** OSCILLATOR FAILURE DETECTED! Running on RC fallback. ***");
        // Clear the OF flag (write 0 to clear, requires config key)
        rtc.writeByte(AMX8X5_REG_CONFIG_KEY, AMX8X5_REG_CONFIG_KEY_VAL_OSC);
        rtc.clearRegister(AMX8X5_REG_OSC_STATUS, AMX8X5_REG_OSC_STATUS_OF_MSK);
    }
    else
    {
        Serial.println("  OK");
    }

    // Check and clear the oscillator fail interrupt flag in STATUS register
    uint8_t u8Status;
    rtc.readByte(AMX8X5_REG_STATUS, &u8Status);
    if (u8Status != 0 && u8Status != 0xFF)
    {
        Serial.printf("  STATUS=0x%02x\n", u8Status);
        rtc.clearRegister(AMX8X5_REG_STATUS, 0xFF);
    }

    delay(1000);
}
