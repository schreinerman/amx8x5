#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates the Trickle Charger for keeping a backup battery
// or supercapacitor on the VBAT pin charged from VCC.
//
// The trickle charger is enabled by setting:
//   - TCS field = 0b1010 (enable charging)
//   - DIODE field: Schottky diode (0.3V drop) or normal diode (0.6V drop)
//   - ROUT field: series resistor (3k, 6k, or 11k Ohm)
//
// Datasheet reference: Section 5.15 Trickle Charger
//
// This example also:
//   - Sets the battery reference voltage threshold for low-battery detection
//   - Enables the battery-low interrupt
//   - Reads the ASTAT register to show current VCC/VBAT levels
//   - Clears the battery flag when detected
//
// Typical use case: supercapacitor (e.g. 0.1F / 3.3V) on VBAT pin.
// Requires AM1805 or AM1815 (VBAT pin available).

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

    // Check if last run detected a VBAT switchover
    uint8_t u8Status;
    rtc.readByte(AMX8X5_REG_STATUS, &u8Status);
    if (u8Status & AMX8X5_REG_STATUS_BAT_MSK)
    {
        Serial.println("*** System was running on VBAT since last power cycle! ***");
        rtc.clearRegister(AMX8X5_REG_STATUS, AMX8X5_REG_STATUS_BAT_MSK);
    }

    // Enable trickle charger:
    //   - Schottky diode (0.3V drop) in charging path
    //   - 3k Ohm series resistor
    en_result_t res = rtc.enableTrickleCharger(
        AMx8x5::enTrickleDiode::AMx8x5TrickleDiodeSchottky,
        AMx8x5::enTrickleResistor::AMx8x5TrickleResistor3K,
        true);
    if (res != Ok)
    {
        Serial.printf("Error: Could not enable trickle charger: %d\n", res);
    }
    else
    {
        Serial.println("Trickle charger enabled (Schottky diode, 3k resistor).");
    }

    // Set battery reference voltage: flag BL when VBAT drops below 2.5V
    rtc.setBatteryReferenceVoltage(
        AMx8x5::enBatReference::AMx8x5BatReferenceFalling25V_Rising30V);

    // Enable battery-low interrupt on FOUT/nIRQ
    rtc.enableIrqBatteryLow(true);

    Serial.println("Battery-low interrupt enabled (threshold: 2.5V falling / 3.0V rising).");
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("\n%02d:%02d:%02d  |  ", pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second);

    // Read Analog Status Register for VCC/VBAT levels
    uint8_t u8AStatus;
    rtc.getAnalogStatus(&u8AStatus);
    Serial.printf("ASTAT=0x%02x", u8AStatus);

    // Read Status Register for battery-low and switchover flags
    uint8_t u8Status;
    rtc.readByte(AMX8X5_REG_STATUS, &u8Status);
    if (u8Status & AMX8X5_REG_STATUS_BL_MSK)
    {
        Serial.println("  --> BATTERY LOW detected! (VBAT < 2.5V)");
        rtc.clearRegister(AMX8X5_REG_STATUS, AMX8X5_REG_STATUS_BL_MSK);
    }
    else if (u8Status & AMX8X5_REG_STATUS_BAT_MSK)
    {
        Serial.println("  --> Running on VBAT (VCC lost)!");
        rtc.clearRegister(AMX8X5_REG_STATUS, AMX8X5_REG_STATUS_BAT_MSK);
    }
    else
    {
        Serial.println("  --> VCC present, VBAT OK.");
    }

    delay(2000);
}
