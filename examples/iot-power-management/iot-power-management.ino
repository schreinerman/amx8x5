#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates a real-world low-power IoT workflow as used on a
// hackable IoT name badge (RP2040 + AM1805 RTC at I2C address 0x69).
//
// Features shown:
//   - Smart time validation against the firmware build timestamp
//     (more robust than just checking for year == 0)
//   - RC oscillator to save power (no external crystal required)
//   - Trickle charger for a backup super-cap / rechargeable cell
//   - EXTI wakeup on nIRQ pin (e.g. button press)
//   - Periodic timer countdown (60 s) to wake the MCU from sleep
//   - PWGT: I2C disabled while RTC is sleeping (prevents bus errors)
//   - PSW/nIRQ2 high-current mode to power external circuitry during RUN
//   - Deep sleep via setSleepMode(AMx8x5PswIrq2HighInSleep)
//   - Detection and handling of "RTC not reachable" (status == 0xFF),
//     which happens when the RTC has cut I2C (PWGT) and the MCU woke via
//     a button rather than the RTC timer
//
// Pin assignment (adapt to your board):
//   SDA -> GPIO4   (I2C data)
//   SCL -> GPIO5   (I2C clock)
//
// Datasheet references:
//   Section 5.4  RC oscillator
//   Section 5.9  PSW/nIRQ2 output
//   Section 5.10 Trickle charger
//   Section 5.13 Sleep mode

#define I2C_SDA_PIN   4    // <-- adapt to your board
#define I2C_SCL_PIN   5    // <-- adapt to your board

// Wakeup interval: every 60 seconds the RTC fires a timer IRQ
#define SLEEP_INTERVAL_S  60

AM1805 rtc;

static stc_amx8x5_time_t stcTimeToSet;
static stc_amx8x5_time_t* pstcTime;

// --------------------------------------------------------------------------
// validateTime()
//
// Checks whether the time stored in the RTC is plausible by comparing it
// against the firmware build timestamp. Returns false if the time was never
// set or if it is older than the build — in both cases we reset to build time.
// --------------------------------------------------------------------------
static bool validateTime(stc_amx8x5_time_t* pTime)
{
    if (pTime->u8Year == 0)                        return false;
    if (pTime->u8Year  < (BUILD_YEAR - 2000))      return false;
    if (pTime->u8Year  > (BUILD_YEAR - 2000))      return true;
    if (pTime->u8Month < BUILD_MONTH)              return false;
    if (pTime->u8Month > BUILD_MONTH)              return true;
    if (pTime->u8Date  < BUILD_DAY)                return false;
    if (pTime->u8Date  > BUILD_DAY)                return true;
    if (pTime->u8Hour  < BUILD_HOUR)               return false;
    if (pTime->u8Hour  > BUILD_HOUR)               return true;
    if (pTime->u8Minute < BUILD_MINUTE)            return false;
    if (pTime->u8Minute > BUILD_MINUTE)            return true;
    if (pTime->u8Second < BUILD_SECOND)            return false;
    return true;
}

// --------------------------------------------------------------------------
// initRtc()
//
// Full RTC setup:
//   1. begin() – initialise I2C callbacks, verify ID registers
//   2. RC oscillator (no external crystal needed)
//   3. Trickle charger for backup super-cap (Schottky diode, 3 kΩ)
//   4. EXTI on XT1 pin (e.g. connected to a user button)
//   5. PSW/nIRQ2 high-current output (powers external load during RUN)
//   6. PWGT: disable I2C bus while the RTC is sleeping
//   7. 60 s countdown timer -> nTIRQ pulsed output
//   8. Time validation / first-time set
// --------------------------------------------------------------------------
static void initRtc(void)
{
    stcTimeToSet.u8Hundredth = 0;
    stcTimeToSet.u8Second    = BUILD_SECOND;
    stcTimeToSet.u8Minute    = BUILD_MINUTE;
    stcTimeToSet.u8Hour      = BUILD_HOUR;
    stcTimeToSet.u8Date      = BUILD_DAY;
    stcTimeToSet.u8Month     = BUILD_MONTH;
    stcTimeToSet.u8Year      = BUILD_YEAR - 2000;
    stcTimeToSet.u8Weekday   = 0;
    stcTimeToSet.u8Century   = 1;  // 20xx
    stcTimeToSet.u8Mode      = AMX8X5_24HR_MODE;

    while (!rtc.begin())
    {
        // The RTC may be unreachable right after a power-on if the MCU woke
        // via a GPIO and the RTC is still driving PSW low (I2C disabled by
        // PWGT). A button press or a brief power cycle will release sleep.
        Serial.println("RTC not initialised — is it sleeping? Press a wakeup button!");
        delay(1000);
    }

    // Use RC oscillator (128 Hz base) — no external 32.768 kHz crystal needed
    rtc.selectOscillatorMode(AMx8x5::enOscSelect::AMx8x5nRc128Hz);

    // Trickle charger: Schottky diode + 3 kΩ resistor, charge enabled
    rtc.enableTrickleCharger(
        AMx8x5::enTrickleDiode::AMx8x5TrickleDiodeSchottky,
        AMx8x5::enTrickleResistor::AMx8x5TrickleResistor3K,
        true);

    // Allow an external event on XT1/EXTI to wake the MCU (e.g. button press)
    rtc.enableIrqXt1OnExti(true);

    // PSW/nIRQ2 in high-current mode: powers external load (display, radio…)
    // PSW is HIGH during RUN and LOW during sleep, gating the load supply.
    rtc.setPswHighCurrent(true);

    // Disable I2C while the RTC is sleeping (PWGT bit).
    // This prevents spurious bus activity when VCC is still present but the
    // MCU is in deep sleep or the load switch has cut the I2C bus.
    rtc.writeByte(AMX8X5_REG_CONFIG_KEY, AMX8X5_CONFIG_KEY_VAL);
    rtc.setRegister(AMX8X5_REG_OSC_CONTROL, AMX8X5_REG_OSC_CONTROL_PWGT_MSK);

    // 60 s periodic countdown timer -> repeated long pulse on nTIRQ
    rtc.setCountdown(
        AMx8x5::enPeriodeRange::AMx8x5PeriodeSeconds,
        SLEEP_INTERVAL_S,
        AMx8x5::enCountdownInterruptOutput::AMx8x5RepeatModeRepeatedPulseLong,
        AMx8x5::enCountdownInterruptPin::AMx8x5CountdownInterruptPinnTIRQLow);

    // Validate time; reset to build timestamp if stale or uninitialised
    rtc.getTime(&pstcTime);
    if (!validateTime(pstcTime))
    {
        Serial.println("RTC time invalid — resetting to firmware build timestamp.");
        rtc.setTime(&stcTimeToSet, false);
    }
    else
    {
        Serial.printf("RTC time OK: 20%02d-%02d-%02d  %02d:%02d:%02d\n",
                      pstcTime->u8Year, pstcTime->u8Month, pstcTime->u8Date,
                      pstcTime->u8Hour, pstcTime->u8Minute, pstcTime->u8Second);
    }
}

// --------------------------------------------------------------------------
// updateRtc()
//
// Called every main loop iteration. Reads the status register to:
//   - detect an EXTI wakeup (button press / external event)
//   - detect a timer countdown wakeup
//   - handle the case where the RTC is not yet reachable after sleep
// --------------------------------------------------------------------------
static void updateRtc(void)
{
    uint8_t u8Status = 0;
    rtc.readByte(AMX8X5_REG_STATUS, &u8Status);

    if (u8Status == 0xFF)
    {
        // 0xFF means the RTC is not responding — it is still sleeping with
        // PWGT active (I2C disabled). This can happen if the MCU woke from
        // a source other than the RTC (e.g. USB plug-in on RP2040).
        // Wait here until the RTC exits sleep and releases I2C.
        Serial.println("RTC sleeping (I2C disabled by PWGT) — waiting for wakeup...");
        do {
            delay(100);
            rtc.readByte(AMX8X5_REG_STATUS, &u8Status);
        } while (u8Status == 0xFF);
        Serial.println("RTC is awake again.");
        return;
    }

    // Refresh current time
    rtc.getTime(&pstcTime);
    Serial.printf("%02d:%02d:%02d  20%02d-%02d-%02d  Weekday: %d\n",
                  pstcTime->u8Hour,   pstcTime->u8Minute, pstcTime->u8Second,
                  pstcTime->u8Year,   pstcTime->u8Month,  pstcTime->u8Date,
                  pstcTime->u8Weekday);

    if (u8Status & AMX8X5_REG_STATUS_EX1_MSK)
    {
        Serial.println("-> External event (EXTI/button wakeup) detected.");
        rtc.clearRegister(AMX8X5_REG_STATUS, AMX8X5_REG_STATUS_EX1_MSK);
        // TODO: handle button action here
    }

    if (u8Status & AMX8X5_REG_STATUS_TIM_MSK)
    {
        Serial.println("-> Timer countdown expired (periodic wakeup).");
        rtc.clearRegister(AMX8X5_REG_STATUS, AMX8X5_REG_STATUS_TIM_MSK);
        // TODO: do periodic work (update display, send sensor data, …) here
    }
}

// --------------------------------------------------------------------------

void setup(void)
{
    Serial.begin(115200);
    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
    initRtc();
}

void loop(void)
{
    // --- Active phase: do work ---
    updateRtc();
    Serial.println("Active phase: doing work...");
    // TODO: update display, read sensors, transmit data, …
    delay(500);

    // --- Sleep phase ---
    // PSW/nIRQ2 will go LOW, cutting power to external load.
    // The RTC wakes the MCU after SLEEP_INTERVAL_S seconds (timer)
    // or immediately on an EXTI event (button press).
    // The second argument selects the sleep output behaviour:
    //   AMx8x5PswIrq2HighInSleep  -> PSW driven HIGH while sleeping
    //                                 (use to keep load ON during sleep)
    //   AMx8x5PswIrq2LowInSleep   -> PSW driven LOW while sleeping
    //                                 (use to cut load power during sleep)
    Serial.println("Going to sleep — bye!");
    Serial.flush();
    en_result_t res = rtc.setSleepMode(0, AMx8x5::enSleepMode::AMx8x5PswIrq2HighInSleep);
    if (res != Ok)
    {
        Serial.printf("setSleepMode failed: %d — continuing without sleep.\n", res);
        delay(SLEEP_INTERVAL_S * 1000UL);
    }
    // MCU resumes here after the RTC wakes it up (PSW toggles / nTIRQ fires).
}
