# AMx8x5 API Reference

Complete API reference for the AMx8x5 RTC driver. For conceptual explanations and usage examples see [driver-guide.md](driver-guide.md).

---

## Table of Contents

1. [Error Codes (`en_result_t`)](#1-error-codes)
2. [Data Structures](#2-data-structures)
3. [Callback Types](#3-callback-types)
4. [Enumerations](#4-enumerations)
5. [Register Map](#5-register-map)
6. [Register Bit Masks](#6-register-bit-masks)
7. [C API](#7-c-api)
8. [C++ API — `AMx8x5` class](#8-c-api--amx8x5-class)
9. [Convenience Macros](#9-convenience-macros)

---

## 1. Error Codes

All C API functions return `en_result_t`. The C++ API uses the typedef `AMx8x5::enResult`.

```c
typedef enum en_result {
    Ok                       = 0,   // No error
    Error                    = 1,   // Non-specific error
    ErrorAddressAlignment    = 2,   // Address alignment mismatch
    ErrorAccessRights        = 3,   // Wrong mode (user/system)
    ErrorInvalidParameter    = 4,   // Parameter out of range
    ErrorOperationInProgress = 5,   // Conflicting operation in progress
    ErrorInvalidMode         = 6,   // Operation not allowed in current mode
    ErrorUninitialized       = 7,   // Module not properly initialised
    ErrorBufferFull          = 8,   // Circular buffer full
    ErrorTimeout             = 9,   // Time-out (I2C arbitration, etc.)
    ErrorNotReady            = 10,  // Requested state not yet reached
    OperationInProgress      = 11   // Operation in progress (non-error)
} en_result_t;
```

---

## 2. Data Structures

### `stc_amx8x5_handle_t` / `AMx8x5::stcHandle`

Driver handle. Must be populated before calling `Amx8x5_Init()`.

```c
typedef struct stc_amx8x5_handle {
    en_amx8x5_communication_mode_t enMode;   // AMx8x5ModeSPI or AMx8x5ModeI2C
    void*                          pHandle;  // Passed to callbacks; may be NULL
    en_amx8x5_rtc_type_t           enRtcType;
    union {
        uint32_t u32ChipSelect; // SPI: chip-select number
        uint32_t u32Address;    // I2C: 7-bit bus address (default 0x69)
    };
    union {
        pfn_spi_write_register pfnWriteSpi; // SPI write callback
        pfn_i2c_write_register pfnWriteI2C; // I2C write callback
    };
    union {
        pfn_spi_read_register pfnReadSpi;   // SPI read callback
        pfn_i2c_read_register pfnReadI2C;   // I2C read callback
    };
} stc_amx8x5_handle_t;
```

### `stc_amx8x5_time_t` / `AMx8x5::stcTime`

Time and date value. Fields are plain integers (not BCD).

```c
typedef struct stc_amx8x5_time {
    uint8_t u8Hundredth; // 0–99
    uint8_t u8Second;    // 0–59
    uint8_t u8Minute;    // 0–59
    uint8_t u8Hour;      // 0–23 (24-h) or 1–12 (12-h)
    uint8_t u8Date;      // 1–31
    uint8_t u8Weekday;   // 0–6  (0 = Sunday)
    uint8_t u8Month;     // 1–12
    uint8_t u8Year;      // 0–99 (years since 2000 or century boundary)
    uint8_t u8Century;   // century counter
    uint8_t u8Mode;      // 0 = 24-h, 1 = 12-h
} stc_amx8x5_time_t;
```

A global instance `stcSysTime` is provided by the driver and is updated by `Amx8x5_GetTime()`.

---

## 3. Callback Types

The driver does **not** access the bus directly. All bus I/O is performed through user-supplied callbacks registered in `stc_amx8x5_handle_t`.

### SPI write

```c
typedef int (*pfn_spi_write_register)(
    void*    pHandle,       // user context (from handle)
    uint32_t u32ChipSelect, // chip-select number
    uint8_t  u8Register,    // register address
    uint8_t* pu8Data,       // data to write
    uint32_t u32Len         // number of bytes
);
```

### SPI read

```c
typedef int (*pfn_spi_read_register)(
    void*    pHandle,
    uint32_t u32ChipSelect,
    uint8_t  u8Register,
    uint8_t* pu8Data,       // buffer to receive data
    uint32_t u32Len
);
```

### I2C write

```c
typedef int (*pfn_i2c_write_register)(
    void*    pHandle,
    uint32_t u32Address,    // 7-bit I2C address
    uint8_t  u8Register,
    uint8_t* pu8Data,
    uint32_t u32Len
);
```

### I2C read

```c
typedef int (*pfn_i2c_read_register)(
    void*    pHandle,
    uint32_t u32Address,
    uint8_t  u8Register,
    uint8_t* pu8Data,
    uint32_t u32Len
);
```

Return `0` on success, any non-zero value on error.

---

## 4. Enumerations

### `en_amx8x5_communication_mode_t` / `AMx8x5::enCommunicationMode`

```c
typedef enum en_amx8x5_communication_mode {
    AMx8x5ModeSPI = 0, // SPI bus
    AMx8x5ModeI2C = 1, // I2C bus
} en_amx8x5_communication_mode_t;
```

### `en_amx8x5_rtc_type_t` / `AMx8x5::enRtcType`

Identifies the physical device. Bit 4 selects SPI (1) or I2C (0). The upper nibble indicates power-management capability.

```c
typedef enum en_amx8x5_rtc_type {
    AMx8x5Type0805 = 0x0805, // AM0805AQ / AB0805          – no PSW, I2C
    AMx8x5Type0815 = 0x0815, // AM0815AQ / AB0815          – no PSW, SPI
    AMx8x5Type1805 = 0x1805, // AM1805AQ / AB1805 / RV-1805-C3 – PSW, I2C
    AMx8x5Type1815 = 0x1815, // AM1815AQ / AB1815          – PSW, SPI

    // Aliases
    AMx8x5TypeI2C                = 0x0805, // same as AMx8x5Type0805
    AMx8x5TypeSPI                = 0x0815, // same as AMx8x5Type0815
    AMx8x5TypeRV1805             = 0x1805, // same as AMx8x5Type1805
    AMx8x5TypeI2CPowerManagement = 0x1805, // same as AMx8x5Type1805
    AMx8x5TypeSPIPowerManagement = 0x1815, // same as AMx8x5Type1815
} en_amx8x5_rtc_type_t;
```

### `en_amx8x5_calibration_mode_t` / `AMx8x5::enCalibrationMode`

```c
typedef enum en_amx8x5_calibration_mode {
    AMx8x5ModeCalibrateXT = 0, // Crystal (XT) oscillator calibration
    AMx8x5ModeCalibrateRC = 1, // RC oscillator calibration
} en_amx8x5_calibration_mode_t;
```

### `en_amx8x5_alarm_repeat_t` / `AMx8x5::enAlarmRepeat`

Controls how often the alarm fires.

```c
typedef enum en_amx8x5_alarm_repeat {
    AMx8x5AlarmDisabled      = 0, // Alarm disabled
    AMx8x5AlarmYear          = 1, // Once per year
    AMx8x5AlarmMonth         = 2, // Once per month
    AMx8x5AlarmWeek          = 3, // Once per week
    AMx8x5AlarmDay           = 4, // Once per day
    AMx8x5AlarmHour          = 5, // Once per hour
    AMx8x5AlarmMinute        = 6, // Once per minute
    AMx8x5AlarmSecond        = 7, // Once per second
    AMx8x5Alarm10thSecond    = 8, // Every 1/10 second
    AMx8x5Alarm100thSecond   = 9, // Every 1/100 second
} en_amx8x5_alarm_repeat_t;
```

### `en_amx8x5_interrupt_mode_t` / `AMx8x5::enInterruptMode`

Duration of the nAIRQ interrupt pulse.

```c
typedef enum en_amx8x5_interrupt_mode {
    AMx8x5InterruptModeLevel        = 0, // Static level; cleared when flag cleared
    AMx8x5InterruptModePulseShort   = 1, // 1/8192 s (XT), 1/64 s (RC)
    AMx8x5InterruptModePulseMedium  = 2, // 1/64 s (XT and RC)
    AMx8x5InterruptModePulseLong    = 3, // 1/4 s (XT and RC)
} en_amx8x5_interrupt_mode_t;
```

Use `AMx8x5InterruptModePulseLong` to minimise standby current.

### `en_amx8x5_interrupt_pin_t` / `AMx8x5::enInterruptPin`

Selects which output pin the alarm interrupt is routed to.

```c
typedef enum en_amx8x5_interrupt_pin {
    AMx8x5InterruptPinInternal = 0, // Internal flag only (no pin)
    AMx8x5InterruptIrq         = 1, // FOUT/nIRQ
    AMx8x5InterruptIrq2        = 2, // PSW/nIRQ2
} en_amx8x5_interrupt_pin_t;
```

### `en_amx8x5_out1_mode_t` / `AMx8x5::enOut1Mode`

FOUT/nIRQ pin operating mode (bits OUT1S of CONTROL_2).

```c
typedef enum en_amx8x5_out1_mode {
    AMx8x5Out1nIRQAtIrqElseOut                  = 0, // nIRQ if interrupt enabled, else OUT
    AMx8x5Out1SwqIfSqweElseOut                  = 1, // SQW if SQWE=1, else OUT
    AMx8x5Out1SwqIfSqweElsenIRQAtIrqElseOut     = 2, // SQW if SQWE=1; nIRQ if interrupt; else OUT
    AMx8x5Out1nAIRQIfAieElseOut                 = 3, // nAIRQ if AIE set, else OUT
} en_amx8x5_out1_mode_t;
```

### `en_amx8x5_out2_mode_t` / `AMx8x5::enOut2Mode`

PSW/nIRQ2 pin operating mode (bits OUT2S of CONTROL_2).

```c
typedef enum en_amx8x5_out2_mode {
    AMx8x5Out2nIRQAtIrqElseOutB    = 0, // nIRQ if interrupt enabled, else OUTB
    AMx8x5Out2SwqIfSqweElseOutB    = 1, // SQW if SQWE=1, else OUTB
    AMx8x5Out2nAIRQIfAieElseOutB   = 3, // nAIRQ if AIE set, else OUTB
    AMx8x5Out2TIRQIfTieElseOutB    = 4, // TIRQ if TIE set, else OUTB
    AMx8x5Out2nTIRQIfTieElseOutB   = 5, // nTIRQ if TIE set, else OUTB
    AMx8x5Out2Sleep                = 6, // SLEEP signal
    AMx8x5Out2OutB                 = 7, // OUTB
} en_amx8x5_out2_mode_t;
```

### `en_amx8x5_sleep_mode_t` / `AMx8x5::enSleepMode`  *(AM18x5 only)*

```c
typedef enum en_amx8x5_sleep_mode {
    AMx8x5nRstLowInSleep           = 0, // nRST pulled low in sleep
    AMx8x5PswIrq2HighInSleep       = 1, // PSW/nIRQ2 pulled high in sleep
    AMx8x5nRstLoPswIrq2HighInSleep = 2, // Both nRST low and PSW/nIRQ2 high
} en_amx8x5_sleep_mode_t;
```

### `en_amx8x5_osc_select_t` / `AMx8x5::enOscSelect`

```c
typedef enum en_amx8x5_osc_select {
    AMx8x5Xt32KHzNoSwitch        = 0, // 32 kHz XT, no automatic switching
    AMx8x5Xt32KHzSwitchRcOnBat   = 1, // 32 kHz XT, switch to RC on VCC loss
    AMx8x5nRc128Hz               = 2, // 128 Hz RC oscillator
} en_amx8x5_osc_select_t;
```

### `en_amx8x5_periode_range_t` / `AMx8x5::enPeriodeRange`

Unit for countdown timer period.

```c
typedef enum en_amx8x5_periode_range {
    AMx8x5PeriodeUs      = 0, // Period in microseconds
    AMx8x5PeriodeSeconds = 1, // Period in seconds
} en_amx8x5_periode_range_t;
```

### `en_amx8x5_countdown_interrupt_output_t` / `AMx8x5::enCountdownInterruptOutput`

```c
typedef enum en_amx8x5_countdown_interrupt_output {
    AMx8x5RepeatModeSingleLevel          = 0, // Single level interrupt
    AMx8x5RepeatModeRepeatedPulseShort   = 1, // Repeated pulse 1/4096 s (XT) / 1/128 s (RC); range must be Us
    AMx8x5RepeatModeSinglePulseShort     = 2, // Single   pulse 1/4096 s (XT) / 1/128 s (RC); range must be Us
    AMx8x5RepeatModeRepeatedPulseMedium  = 3, // Repeated pulse 1/128 s; range must be Us
    AMx8x5RepeatModeSinglePulseMedium    = 4, // Single   pulse 1/128 s; range must be Us
    AMx8x5RepeatModeRepeatedPulseLong    = 5, // Repeated pulse 1/64 s; range must be Seconds
    AMx8x5RepeatModeSinglePulseLong      = 6, // Single   pulse 1/64 s; range must be Seconds
} en_amx8x5_countdown_interrupt_output_t;
```

### `en_amx8x5_countdown_interrupt_pin_t` / `AMx8x5::enCountdownInterruptPin`

```c
typedef enum en_amx8x5_countdown_interrupt_pin {
    AMx8x5CountdownInterruptPinDisable                      = 0, // Disable timer
    AMx8x5CountdownInterruptPinnTIRQLow                     = 1, // nTIRQ only (low)
    AMx8x5CountdownInterruptPinFOUTnIRQLownTIRQLow          = 2, // FOUT/nIRQ and nTIRQ (both low)
    AMx8x5CountdownInterruptPinPSWnIRQ2LownTIRQLow          = 3, // PSW/nIRQ2 and nTIRQ (both low)
    AMx8x5CountdownInterruptPinCLKOUTnIRQ3LownTIRQLow       = 4, // CLKOUT/nIRQ3 and nTIRQ (both low)
    AMx8x5CountdownInterruptPinCLKOUTnIRQ3HighnTIRQLow      = 5, // CLKOUT/nIRQ3 (high) and nTIRQ (low)
} en_amx8x5_countdown_interrupt_pin_t;
```

### `en_amx8x5_watchdog_interrupt_pin_t` / `AMx8x5::enWatchdogInterruptPin`

```c
typedef enum en_amx8x5_watchdog_interrupt_pin {
    AMx8x5WatchdogInterruptPinDisable  = 0, // WDT disabled
    AMx8x5WatchdogInterruptPinFOUTnIRQ = 1, // Interrupt on FOUT/nIRQ
    AMx8x5WatchdogInterruptPinPSWnIRQ2 = 2, // Interrupt on PSW/nIRQ2
    AMx8x5WatchdogInterruptPinnRST     = 3, // Hardware reset on nRST (AM18xx only)
} en_amx8x5_watchdog_interrupt_pin_t;
```

### `en_amx8x5_autocalibration_period_t` / `AMx8x5::enAutocalibrationPeriod`

```c
typedef enum en_amx8x5_autocalibration_period {
    AMx8x5AutoCalibrationPeriodDisable         = 0, // Autocalibration off
    AMx8x5AutoCalibrationPeriodSingleCycle      = 1, // Run one calibration cycle immediately
    AMx8x5AutoCalibrationPeriodCycleSecods1024  = 2, // Calibrate every 1024 s (~17 min)
    AMx8x5AutoCalibrationPeriodCycleSecods512   = 3, // Calibrate every  512 s (~8.5 min)
} en_amx8x5_autocalibration_period_t;
```

### `en_amx8x5_trickle_diode_t` / `AMx8x5::enTrickleDiode`

```c
typedef enum en_amx8x5_trickle_diode {
    AMx8x5TrickleDiodeDisabled = 0, // Trickle charger disabled
    AMx8x5TrickleDiodeSchottky = 1, // Schottky diode (Vdrop ≈ 0.3 V)
    AMx8x5TrickleDiodeNormal   = 2, // Standard diode (Vdrop ≈ 0.6 V)
} en_amx8x5_trickle_diode_t;
```

### `en_amx8x5_trickle_resistor_t` / `AMx8x5::enTrickleResistor`

```c
typedef enum en_amx8x5_trickle_resistor {
    AMx8x5TrickleResistorDisabled = 0, // Trickle charger disabled
    AMx8x5TrickleResistor3K       = 1, // 3 kΩ series resistor
    AMx8x5TrickleResistor6K       = 2, // 6 kΩ series resistor
    AMx8x5TrickleResistor11K      = 3, // 11 kΩ series resistor
} en_amx8x5_trickle_resistor_t;
```

### `en_amx8x5_bat_reference_t` / `AMx8x5::enBatReference`

VBAT threshold voltages (falling / rising).

```c
typedef enum en_amx8x5_bat_reference {
    AMx8x5BatReferenceFalling25V_Rising30V = 0x7, // 2.5 V fall / 3.0 V rise
    AMx8x5BatReferenceFalling21V_Rising25V = 0xB, // 2.1 V fall / 2.5 V rise
    AMx8x5BatReferenceFalling18V_Rising22V = 0xD, // 1.8 V fall / 2.2 V rise
    AMx8x5BatReferenceFalling14V_Rising16V = 0xF, // 1.4 V fall / 1.6 V rise
} en_amx8x5_bat_reference_t;
```

---

## 5. Register Map

| Address | Constant | Description |
|---------|----------|-------------|
| 0x00 | `AMX8X5_REG_HUNDREDTHS` | Hundredths counter |
| 0x01 | `AMX8X5_REG_SECONDS` | Seconds counter |
| 0x02 | `AMX8X5_REG_MINUTES` | Minutes counter |
| 0x03 | `AMX8X5_REG_HOURS` | Hours counter |
| 0x04 | `AMX8X5_REG_DATE` | Date (day of month) |
| 0x05 | `AMX8X5_REG_MONTH` | Month |
| 0x06 | `AMX8X5_REG_YEARS` | Year (0–99) |
| 0x07 | `AMX8X5_REG_WEEKDAY` | Day of week (0=Sunday) |
| 0x08 | `AMX8X5_REG_ALARM_HUNDREDTHS` | Alarm hundredths |
| 0x09 | `AMX8X5_REG_ALARM_SECONDS` | Alarm seconds |
| 0x0A | `AMX8X5_REG_ALARM_MINUTES` | Alarm minutes |
| 0x0B | `AMX8X5_REG_ALARM_HOURS` | Alarm hours |
| 0x0C | `AMX8X5_REG_ALARM_DATE` | Alarm date |
| 0x0D | `AMX8X5_REG_ALARM_MONTH` | Alarm month |
| 0x0E | `AMX8X5_REG_ALARM_WEEKDAY` | Alarm weekday |
| 0x0F | `AMX8X5_REG_STATUS` | Status (read-only) |
| 0x10 | `AMX8X5_REG_CONTROL_1` | Control 1 |
| 0x11 | `AMX8X5_REG_CONTROL_2` | Control 2 |
| 0x12 | `AMX8X5_REG_INT_MASK` | Interrupt mask |
| 0x13 | `AMX8X5_REG_SQW` | Square wave output |
| 0x14 | `AMX8X5_REG_CAL_XT` | XT calibration |
| 0x15 | `AMX8X5_REG_CAL_RC_HI` | RC calibration high byte |
| 0x16 | `AMX8X5_REG_CAL_RC_LOW` | RC calibration low byte |
| 0x17 | `AMX8X5_REG_SLEEP_CTRL` | Sleep control |
| 0x18 | `AMX8X5_REG_TIMER_CTRL` | Countdown timer control |
| 0x19 | `AMX8X5_REG_TIMER` | Countdown timer value |
| 0x1A | `AMX8X5_REG_TIMER_INITIAL` | Countdown timer initial value |
| 0x1B | `AMX8X5_REG_WDT` | Watchdog timer |
| 0x1C | `AMX8X5_REG_OSC_CONTROL` | Oscillator control |
| 0x1D | `AMX8X5_REG_OSC_STATUS` | Oscillator status |
| 0x1F | `AMX8X5_REG_CONFIG_KEY` | Configuration key |
| 0x20 | `AMX8X5_REG_TRICKLE` | Trickle charger |
| 0x21 | `AMX8X5_REG_BREF_CTRL` | Battery reference control |
| 0x26 | `AMX8X5_REG_ACAL_FLT` | Autocalibration filter |
| 0x27 | `AMX8X5_REG_BATMODE_IO` | BATMODE I/O control |
| 0x28 | `AMX8X5_REG_ID0` | Device ID byte 0 |
| 0x29–0x2E | `AMX8X5_REG_ID1`–`ID6` | Device ID bytes 1–6 |
| 0x2F | `AMX8X5_REG_ASTAT` | Analog status (read-only) |
| 0x30 | `AMX8X5_REG_OCTRL` | Output control |
| 0x3F | `AMX8X5_REG_EXTENDED_ADDR` | Extended RAM address |
| 0x40–0x7F | `AMX8X5_REG_RAM` | Standard user RAM (64 bytes) |
| 0x80–0xFF | `AMX8X5_REG_ALT_RAM` | Alternate / extended RAM |

### Config-key values

| Constant | Value | Unlocks |
|----------|-------|---------|
| `AMX8X5_REG_CONFIG_KEY_VAL_OSC` | 0xA1 | `AMX8X5_REG_OSC_CONTROL` |
| `AMX8X5_REG_CONFIG_KEY_VAL_OTHER` | 0x9D | `AMX8X5_REG_TRICKLE`, `AMX8X5_REG_BREF_CTRL`, `AMX8X5_REG_BATMODE_IO` |

---

## 6. Register Bit Masks

### STATUS register (`AMX8X5_REG_STATUS` = 0x0F)

| Mask | Pos | Description |
|------|-----|-------------|
| `AMX8X5_REG_STATUS_CB_MSK`  | 7 | Century bit |
| `AMX8X5_REG_STATUS_BAT_MSK` | 6 | Running on battery |
| `AMX8X5_REG_STATUS_WDT_MSK` | 5 | Watchdog timer fired |
| `AMX8X5_REG_STATUS_BL_MSK`  | 4 | Battery low |
| `AMX8X5_REG_STATUS_TIM_MSK` | 3 | Countdown timer interrupt |
| `AMX8X5_REG_STATUS_ALM_MSK` | 2 | Alarm interrupt |
| `AMX8X5_REG_STATUS_EX2_MSK` | 1 | External interrupt 2 (WDI) |
| `AMX8X5_REG_STATUS_EX1_MSK` | 0 | External interrupt 1 (EXTI) |

### CONTROL_1 register (`AMX8X5_REG_CONTROL_1` = 0x10)

| Mask | Pos | Description |
|------|-----|-------------|
| `AMX8X5_REG_CONTROL_1_STOP_MSK`  | 7 | Stop the RTC counters |
| `AMX8X5_REG_CONTROL_1_12_24_MSK` | 6 | 12/24-hour mode selection |
| `AMX8X5_REG_CONTROL_1_OUTB_MSK`  | 5 | OUTB value when PSW/nIRQ2 is not driven by interrupt |
| `AMX8X5_REG_CONTROL_1_OUT_MSK`   | 4 | OUT value when FOUT/nIRQ is not driven by interrupt |
| `AMX8X5_REG_CONTROL_1_RSP_MSK`   | 3 | nRST polarity (1=assert high) |
| `AMX8X5_REG_CONTROL_1_ARST_MSK`  | 2 | Auto-reset status on status register read |
| `AMX8X5_REG_CONTROL_1_PWR2_MSK`  | 1 | Enable PSW/nIRQ2 pin for power management |
| `AMX8X5_REG_CONTROL_1_WRTC_MSK`  | 0 | Write RTC (must be set before writing time) |

### CONTROL_2 register (`AMX8X5_REG_CONTROL_2` = 0x11)

| Mask | Pos | Description |
|------|-----|-------------|
| `AMX8X5_REG_CONTROL_2_RS1E_MSK`  | 5 | Enable RS1 (bit alias) |
| `AMX8X5_REG_CONTROL_2_OUT2S_MSK` | 5 | OUT2S field (3 bits) – PSW/nIRQ2 mode |
| `AMX8X5_REG_CONTROL_2_OUT1S_MSK` | 2 | OUT1S field (2 bits) – FOUT/nIRQ mode |

### INT_MASK register (`AMX8X5_REG_INT_MASK` = 0x12)

| Mask | Pos | Description |
|------|-----|-------------|
| `AMX8X5_REG_INT_MASK_CEB_MSK`  | 7 | Century enable |
| `AMX8X5_REG_INT_MASK_IM_MSK`   | 5 | Interrupt mode (2 bits) |
| `AMX8X5_REG_INT_MASK_BLIE_MSK` | 4 | Battery low interrupt enable |
| `AMX8X5_REG_INT_MASK_TIE_MSK`  | 3 | Timer interrupt enable |
| `AMX8X5_REG_INT_MASK_AIE_MSK`  | 2 | Alarm interrupt enable |
| `AMX8X5_REG_INT_MASK_EX2E_MSK` | 1 | External interrupt 2 enable |
| `AMX8X5_REG_INT_MASK_EX1E_MSK` | 0 | External interrupt 1 enable |

### OCTRL register (`AMX8X5_REG_OCTRL` = 0x30)

| Mask | Pos | Description |
|------|-----|-------------|
| `AMX8X5_REG_OCTRL_WDBM_MSK` | 7 | WDT/BREF output mask |
| `AMX8X5_REG_OCTRL_EXBM_MSK` | 6 | External/BREF output mask |
| `AMX8X5_REG_OCTRL_WDDS_MSK` | 5 | WDT disable in sleep |
| `AMX8X5_REG_OCTRL_EXDS_MSK` | 4 | EXT disable in sleep |
| `AMX8X5_REG_OCTRL_RSEN_MSK` | 3 | Enable nRST output |
| `AMX8X5_REG_OCTRL_O4EN_MSK` | 2 | Enable CLKOUT/nIRQ3 output |
| `AMX8X5_REG_OCTRL_O3EN_MSK` | 1 | Enable nTIRQ output |
| `AMX8X5_REG_OCTRL_O1EN_MSK` | 0 | Enable FOUT/nIRQ output |

---

## 7. C API

All functions take a pointer to an initialised `stc_amx8x5_handle_t` as their first argument (`pstcHandle`).

### Initialisation & Reset

| Function | Description |
|----------|-------------|
| `en_result_t Amx8x5_Init(stc_amx8x5_handle_t* pstcHandle)` | Initialise the RTC and verify communication. Must be called first. |
| `en_result_t Amx8x5_Reset(stc_amx8x5_handle_t* pstcHandle)` | Software reset of the RTC. |

### Time Read

| Function | Returns | Description |
|----------|---------|-------------|
| `Amx8x5_GetTime(pstcHandle, stc_amx8x5_time_t** ppstcTime)` | `en_result_t` | Read all time fields into `stcSysTime`; `*ppstcTime` points to it. |
| `Amx8x5_GetHundredth(pstcHandle)` | `int16_t` | Hundredths (0–99), or negative on error. |
| `Amx8x5_GetSecond(pstcHandle)` | `int16_t` | Seconds. |
| `Amx8x5_GetMinute(pstcHandle)` | `int16_t` | Minutes. |
| `Amx8x5_GetHour(pstcHandle)` | `int16_t` | Hours. |
| `Amx8x5_GetDay(pstcHandle)` | `int16_t` | Day of month. |
| `Amx8x5_GetWeekday(pstcHandle)` | `int16_t` | Day of week (0=Sunday). |
| `Amx8x5_GetMonth(pstcHandle)` | `int16_t` | Month. |
| `Amx8x5_GetYear(pstcHandle)` | `int16_t` | Year (0–99). |
| `Amx8x5_GetCentury(pstcHandle)` | `int16_t` | Century counter. |

The `int16_t` getters return the value on success, or a negative `en_result_t` cast on error.

### Time Write

```c
en_result_t Amx8x5_SetTime(
    stc_amx8x5_handle_t* pstcHandle,
    stc_amx8x5_time_t*   pstcTime,
    bool                 bProtect  // true = re-enable WRTC protection after write
);
```

Sets the RTC to the supplied time. Temporarily clears WRTC in CONTROL_1, then optionally re-enables it.

### Alarm

```c
en_result_t Amx8x5_SetAlarm(
    stc_amx8x5_handle_t*       pstcHandle,
    stc_amx8x5_time_t*         pstcTime,        // alarm time (fields used depend on enModeRepeat)
    en_amx8x5_alarm_repeat_t   enModeRepeat,    // repeat granularity
    en_amx8x5_interrupt_mode_t enModeIrq,       // pulse duration
    en_amx8x5_interrupt_pin_t  enModePin        // output pin
);
```

### Oscillator Control

| Function | Description |
|----------|-------------|
| `Amx8x5_Stop(pstcHandle, bool bStop)` | Start (`false`) or stop (`true`) the RTC counters. |
| `Amx8x5_SelectOscillatorMode(pstcHandle, enSelect)` | Choose XT/RC oscillator and battery switching behaviour. |
| `Amx8x5_SetCalibrationValue(pstcHandle, enMode, int32_t iAdjust)` | Set calibration trim value for XT or RC clock. |
| `Amx8x5_SetAutocalibration(pstcHandle, enPeriod)` | Configure automatic RC→XT calibration period. |

### Countdown Timer

```c
en_result_t Amx8x5_SetCountdown(
    stc_amx8x5_handle_t*                    pstcHandle,
    en_amx8x5_periode_range_t               enRange,   // AMx8x5PeriodeUs or AMx8x5PeriodeSeconds
    int32_t                                 iPeriod,   // countdown value in selected units
    en_amx8x5_countdown_interrupt_output_t  enRepeat,  // pulse/level, single/repeated
    en_amx8x5_countdown_interrupt_pin_t     enPin      // output pin(s)
);
```

Pass `enPin = AMx8x5CountdownInterruptPinDisable` to disable the timer.

### Watchdog  *(AM18x5)*

```c
en_result_t Amx8x5_SetWatchdog(
    stc_amx8x5_handle_t*              pstcHandle,
    uint32_t                          u32Period,  // watchdog timeout in milliseconds
    en_amx8x5_watchdog_interrupt_pin_t enPin      // output pin / reset
);
```

Pass `enPin = AMx8x5WatchdogInterruptPinDisable` to disable. The WDT must be refreshed before `u32Period` expires.

### Sleep  *(AM18x5)*

```c
en_result_t Amx8x5_SetSleepMode(
    stc_amx8x5_handle_t*     pstcHandle,
    uint8_t                  ui8Timeout,  // sleep duration in seconds (7-bit, max 127)
    en_amx8x5_sleep_mode_t   enMode
);
```

### Square Wave Output

```c
en_result_t Amx8x5_SetSquareWaveOutput(
    stc_amx8x5_handle_t* pstcHandle,
    uint8_t              u8SQFS,    // frequency select (0–31); see SQW register table
    uint8_t              u8PinMsk   // pin mask: bit 0 = FOUT/nIRQ, bit 1 = PSW/nIRQ2
);
```

Pass `u8PinMsk = 0` to disable. SQWE in INT_MASK is set/cleared automatically.

### Output Pin Control

| Function | Description |
|----------|-------------|
| `Amx8x5_EnableOutput(pstcHandle, uint8_t u8Mask, bool bEnable)` | Set/clear bits in the OCTRL register. Use `AMX8X5_REG_OCTRL_*_MSK` constants. |
| `Amx8x5_CtrlOutB(pstcHandle, bool bOnOff)` | Set OUTB bit in CONTROL_1. |
| `Amx8x5_CtrlOut(pstcHandle, bool bOnOff)` | Set OUT bit in CONTROL_1. |
| `Amx8x5_SetOut1Mode(pstcHandle, enMode)` | Configure FOUT/nIRQ pin function (OUT1S). |
| `Amx8x5_SetOut2Mode(pstcHandle, enMode)` | Configure PSW/nIRQ2 pin function (OUT2S). |
| `Amx8x5_SetResetPolarity(pstcHandle, bool bAssertHigh)` | Active-high (`true`) or active-low (`false`) nRST. |
| `Amx8x5_SetPswHighCurrent(pstcHandle, bool bEnabled)` | Enable high-drive on PSW/nIRQ2. |
| `Amx8x5_UsenExtrAsReset(pstcHandle, bool bEnabled)` | Use nEXTR as hardware reset input. |

### Interrupt Control

| Function | Description |
|----------|-------------|
| `Amx8x5_EnableInterrupt(pstcHandle, uint8_t u8IrqMask)` | Set bits in INT_MASK. |
| `Amx8x5_DisableInterrupt(pstcHandle, uint8_t u8IrqMask)` | Clear bits in INT_MASK. |
| `Amx8x5_ClearInterrupts(pstcHandle)` | Clear all status flags. |
| `Amx8x5_ClearInterrupt(pstcHandle, uint8_t u8IrqMask)` | Clear specific status flags. |
| `Amx8x5_GetInterruptStatus(pstcHandle, uint8_t* pu8Status)` | Read STATUS register. |
| `Amx8x5_EnableIrqXt1OnExti(pstcHandle, bool bEnabled)` | Enable external interrupt 1 (XT1/EXTI). |
| `Amx8x5_EnableIrqXt2OnWdi(pstcHandle, bool bEnabled)` | Enable external interrupt 2 (XT2/WDI). |
| `Amx8x5_EnableIrqAlarm(pstcHandle, bool bEnabled)` | Enable alarm interrupt (AIE). |
| `Amx8x5_EnableIrqTimer(pstcHandle, bool bEnabled)` | Enable countdown timer interrupt (TIE). |
| `Amx8x5_EnableIrqBatteryLow(pstcHandle, bool bEnabled)` | Enable battery-low interrupt (BLIE). |
| `Amx8x5_EnableIrqOscillatorFail(pstcHandle, bool bEnabled)` | Enable oscillator-fail interrupt. |
| `Amx8x5_EnableIrqAutocalibFail(pstcHandle, bool bEnabled)` | Enable autocalibration-fail interrupt. |
| `Amx8x5_AutoResetStatus(pstcHandle, bool bEnabled)` | Auto-clear STATUS on read (ARST bit). |

### Power Management  *(AM18x5)*

| Function | Description |
|----------|-------------|
| `Amx8x5_EnableTrickleCharger(pstcHandle, enDiode, enResistor, bool bEnable)` | Configure and enable/disable the trickle charger. |
| `Amx8x5_SetBatteryReferenceVoltage(pstcHandle, enBref)` | Set VBAT threshold voltages. |
| `Amx8x5_SetBatmodeIO(pstcHandle, bool bIoEnabled)` | Enable I/O when running on battery. |
| `Amx8x5_GetAnalogStatus(pstcHandle, uint8_t* pu8Status)` | Read ASTAT register. |

### RAM Access

```c
en_result_t Amx8x5_RamRead(stc_amx8x5_handle_t* pstcHandle,
                            uint8_t u8Address,      // 0x00–0x3F (maps to 0x40–0x7F)
                            uint8_t* pu8Data);

en_result_t Amx8x5_RamWrite(stc_amx8x5_handle_t* pstcHandle,
                             uint8_t u8Address,
                             uint8_t u8Data);
```

For extended RAM (0x80–0xFF) use `Amx8x5_GetExtensionAddress()` first.

```c
en_result_t Amx8x5_GetExtensionAddress(stc_amx8x5_handle_t* pstcHandle,
                                        uint8_t  u8Address,
                                        uint8_t* pu8ExtensionAddress);
```

### Raw Register Access

| Function | Description |
|----------|-------------|
| `Amx8x5_ReadByte(pstcHandle, u8Register, uint8_t* pu8Value)` | Read one byte. |
| `Amx8x5_ReadBytes(pstcHandle, u8Register, uint8_t* pu8Data, uint32_t u32Length)` | Read multiple bytes. |
| `Amx8x5_WriteByte(pstcHandle, u8Register, uint8_t u8Value)` | Write one byte. |
| `Amx8x5_WriteBytes(pstcHandle, u8Register, uint8_t* pu8Data, uint32_t u32Length)` | Write multiple bytes. |
| `Amx8x5_SetRegister(pstcHandle, u8Address, uint8_t u8Mask)` | Set (OR) bits in a register. |
| `Amx8x5_ClearRegister(pstcHandle, u8Address, uint8_t u8Mask)` | Clear (AND-NOT) bits in a register. |

---

## 8. C++ API — `AMx8x5` class

All C enums and structs are re-exported as member typedefs inside `AMx8x5` so that code can use the unqualified name (`AMx8x5::enResult`, etc.) or the global C name interchangeably.

### Convenience subclasses

| Class | Equivalent |
|-------|-----------|
| `AM0805` | `AMx8x5(AMx8x5Type0805)` — I2C, no PSW |
| `AM1805` | `AMx8x5(AMx8x5Type1805)` — I2C, with PSW |
| `AM0815` | `AMx8x5(AMx8x5Type0815)` — SPI, no PSW |
| `AM1815` | `AMx8x5(AMx8x5Type1815)` — SPI, with PSW |

### Constructors

```cpp
AMx8x5()                              // I2C, AMx8x5Type1805, address 0x69
AMx8x5(enRtcType enType)              // interface derived from enType (bit 4)
AMx8x5(enRtcType enType, void* pHandle) // same + user context pointer
AMx8x5(enCommunicationMode enMode)    // explicit mode, type 1805, address 0x69
```

### Arduino initialisation methods

```cpp
bool begin(void);               // I2C: uses global Wire / SPI via Arduino APIs
bool begin(uint8_t u8CsPin);    // SPI: uses global SPI, u8CsPin = chip-select pin
bool end(void);                 // Release peripheral
void update(void);              // Refresh stcSysTime from RTC (calls getTime internally)
```

`begin()` calls `Amx8x5_Init()` internally, sets up the low-level callbacks using the Arduino `Wire` or `SPI` library, and returns `true` on success.

### Low-level init (non-Arduino / custom callback)

```cpp
AMx8x5::enResult init(AMx8x5::stcHandle* pstcHandle);
```

### Time

```cpp
AMx8x5::enResult getTime(AMx8x5::stcTime** ppstcTime);
int16_t getHundredth(void);
int16_t getSecond(void);
int16_t getMinute(void);
int16_t getHour(void);
int16_t getDay(void);
int16_t getWeekday(void);
int16_t getMonth(void);
int16_t getYear(void);

AMx8x5::enResult setTime(AMx8x5::stcTime* pstcTime, bool bProtect);
```

### Alarm

```cpp
AMx8x5::enResult setAlarm(
    AMx8x5::stcTime*        pstcTime,
    AMx8x5::enAlarmRepeat   enModeRepeat,
    AMx8x5::enInterruptMode enModeIrq,
    AMx8x5::enInterruptPin  enModePin
);
```

### Oscillator

```cpp
AMx8x5::enResult reset(void);
AMx8x5::enResult stop(bool bStop);
AMx8x5::enResult selectOscillatorMode(AMx8x5::enOscSelect enSelect);
AMx8x5::enResult setCalibrationValue(AMx8x5::enCalibrationMode enMode, int32_t iAdjust);
AMx8x5::enResult setAutocalibration(AMx8x5::enAutocalibrationPeriod enPeriod);
```

### Countdown Timer

```cpp
AMx8x5::enResult setCountdown(
    AMx8x5::enPeriodeRange              enRange,
    int32_t                             iPeriod,
    AMx8x5::enCountdownInterruptOutput  enRepeat,
    AMx8x5::enCountdownInterruptPin     enPin
);
```

### Watchdog

```cpp
AMx8x5::enResult setWatchdog(
    uint32_t                       u32Period,
    AMx8x5::enWatchdogInterruptPin enPin
);
```

### Sleep  *(AM18x5)*

```cpp
AMx8x5::enResult setSleepMode(uint8_t ui8Timeout, AMx8x5::enSleepMode enMode);
```

### Square Wave

```cpp
AMx8x5::enResult setSquareWaveOutput(uint8_t u8SQFS, uint8_t u8PinMsk);
```

### Output Pins

```cpp
AMx8x5::enResult enableOutput(uint8_t u8Mask, bool bEnable);
AMx8x5::enResult ctrlOutB(bool bOnOff);
AMx8x5::enResult ctrlOut(bool bOnOff);
AMx8x5::enResult setOut1Mode(AMx8x5::enOut1Mode enMode);
AMx8x5::enResult setOut2Mode(AMx8x5::enOut2Mode enMode);
AMx8x5::enResult setResetPolarity(bool bAssertHigh);
AMx8x5::enResult setPswHighCurrent(bool bEnabled);
AMx8x5::enResult usenExtrAsReset(bool bEnabled);
```

### Interrupts

```cpp
AMx8x5::enResult enableInterrupt(uint8_t u8IrqMask);
AMx8x5::enResult disableInterrupt(uint8_t u8IrqMask);
AMx8x5::enResult clearInterrupts(void);
AMx8x5::enResult clearInterrupt(uint8_t u8IrqMask);
AMx8x5::enResult getInterruptStatus(uint8_t* pu8Status);
AMx8x5::enResult enableIrqXt1OnExti(bool bEnabled);
AMx8x5::enResult enableIrqXt2OnWdi(bool bEnabled);
AMx8x5::enResult enableIrqAlarm(bool bEnabled);
AMx8x5::enResult enableIrqTimer(bool bEnabled);
AMx8x5::enResult enableIrqBatteryLow(bool bEnabled);
AMx8x5::enResult enableIrqOscillatorFail(bool bEnabled);
AMx8x5::enResult enableIrqAutocalibFail(bool bEnabled);
AMx8x5::enResult autoResetStatus(bool bEnabled);
```

### Power Management  *(AM18x5)*

```cpp
AMx8x5::enResult enableTrickleCharger(
    AMx8x5::enTrickleDiode    enDiode,
    AMx8x5::enTrickleResistor enResistor,
    bool                      bEnable
);
AMx8x5::enResult setBatteryReferenceVoltage(AMx8x5::enBatReference enBref);
AMx8x5::enResult setBatmodeIO(bool bIoEnabled);
AMx8x5::enResult getAnalogStatus(uint8_t* pu8Status);
```

### RAM

```cpp
AMx8x5::enResult ramRead(uint8_t u8Address, uint8_t* pu8Data);
AMx8x5::enResult ramWrite(uint8_t u8Address, uint8_t u8Data);
AMx8x5::enResult getExtensionAddress(uint8_t u8Address, uint8_t* pu8ExtensionAddress);
```

### Raw Register Access

```cpp
AMx8x5::enResult clearRegister(uint8_t u8Address, uint8_t u8Mask);
AMx8x5::enResult setRegister(uint8_t u8Address, uint8_t u8Mask);
AMx8x5::enResult readByte(uint8_t u8Register, uint8_t* pu8Value);
AMx8x5::enResult readBytes(uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length);
AMx8x5::enResult writeByte(uint8_t u8Register, uint8_t u8Value);
AMx8x5::enResult writeBytes(uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length);
```

---

## 9. Convenience Macros

Predefined macros wrap `Amx8x5_EnableOutput()` for the four switchable output pins:

```c
// Enable/disable FOUT/nIRQ output
Amx8x5_EnableOutput_FOUT_nIRQ(pstcHandle, bEnable)
//  → Amx8x5_EnableOutput(pstcHandle, AMX8X5_REG_OCTRL_O1EN_MSK, bEnable)

// Enable/disable nTIRQ output
Amx8x5_EnableOutput_nTIRQ(pstcHandle, bEnable)
//  → Amx8x5_EnableOutput(pstcHandle, AMX8X5_REG_OCTRL_O3EN_MSK, bEnable)

// Enable/disable CLKOUT/nIRQ3 output
Amx8x5_EnableOutput_CLKOUT_nIRQ3(pstcHandle, bEnable)
//  → Amx8x5_EnableOutput(pstcHandle, AMX8X5_REG_OCTRL_O4EN_MSK, bEnable)

// Enable/disable nRST output
Amx8x5_EnableOutput_nRST(pstcHandle, bEnable)
//  → Amx8x5_EnableOutput(pstcHandle, AMX8X5_REG_OCTRL_RSEN_MSK, bEnable)
```
