# AMx8x5 RTC Driver Guide

A complete reference for using the AMx8x5 real-time clock driver in **C**, **C++**, and **Arduino** environments.

---

## Table of Contents

1. [Hardware Overview](#1-hardware-overview)
2. [Chip Variants](#2-chip-variants)
3. [Getting Started — Arduino](#3-getting-started--arduino)
   - [I2C (AM0805 / AM1805)](#i2c-am0805--am1805)
   - [SPI (AM0815 / AM1815)](#spi-am0815--am1815)
4. [Getting Started — C API](#4-getting-started--c-api)
   - [I2C with custom callbacks](#i2c-with-custom-callbacks)
   - [SPI with custom callbacks](#spi-with-custom-callbacks)
5. [Getting Started — C++ API](#5-getting-started--c-api-1)
6. [Reading and Writing Time](#6-reading-and-writing-time)
7. [Alarms](#7-alarms)
8. [Countdown Timer](#8-countdown-timer)
9. [Watchdog Timer](#9-watchdog-timer)
10. [Square Wave Output](#10-square-wave-output)
11. [Oscillator Selection and Calibration](#11-oscillator-selection-and-calibration)
12. [Output Pin Control](#12-output-pin-control)
13. [Interrupt Handling](#13-interrupt-handling)
14. [Power Management (AM18x5 only)](#14-power-management-am18x5-only)
15. [Battery and Trickle Charger](#15-battery-and-trickle-charger)
16. [Non-volatile RAM](#16-non-volatile-ram)
17. [Raw Register Access](#17-raw-register-access)
18. [Error Codes](#18-error-codes)
19. [Full C API Reference](#19-full-c-api-reference)
20. [Full C++ / Arduino API Reference](#20-full-c--arduino-api-reference)

---

## 1. Hardware Overview

The AM0805 / AM0815 / AM1805 / AM1815 are ultra-low-power real-time clocks from Ambiq Micro (also sold as AB08x5 by Abracon and RV-1805 by Micro Crystal). They provide:

- Time-of-day with 1/100 s resolution (hundredths, seconds, minutes, hours, date, month, year, weekday, century)
- 12- or 24-hour mode
- Calendar alarm with programmable repeat interval
- Countdown timer / periodic interrupt
- Watchdog timer
- Square-wave output
- Crystal (XT) and internal RC oscillator with automatic failover
- Digital calibration (XT: ±610 ppm, RC: ±65 536 ppm)
- Autocalibration against the XT reference
- 256 bytes non-volatile SRAM (64 accessible via the RAM API, the rest are registers)
- I2C (AM08x5, 400 kHz) or SPI (AM08x5, up to 2 MHz) interface
- AM18x5 additionally: power-switch output (PSW/nIRQ2), sleep mode, trickle charger, battery reference comparator

---

## 2. Chip Variants

| C/C++ type constant      | Class (Arduino/C++) | Interface | Power mgmt | Compatible parts              |
|--------------------------|---------------------|-----------|------------|-------------------------------|
| `AMx8x5Type0805`         | `AM0805`            | I2C       | no         | AM0805AQ, AB0805              |
| `AMx8x5Type0815`         | `AM0815`            | SPI       | no         | AM0815AQ, AB0815              |
| `AMx8x5Type1805`         | `AM1805`            | I2C       | **yes**    | AM1805AQ, AB1805, RV-1805-C3  |
| `AMx8x5Type1815`         | `AM1815`            | SPI       | **yes**    | AM1815AQ, AB1815              |

Aliases: `AMx8x5TypeI2C` = 0805, `AMx8x5TypeSPI` = 0815, `AMx8x5TypeRV1805` = 1805.

---

## 3. Getting Started — Arduino

Include the library and `<Wire.h>` (I2C) or `<SPI.h>` (SPI). Instantiate the matching class.

### I2C (AM0805 / AM1805)

```cpp
#include <amx8x5.h>
#include <Wire.h>

AM0805 rtc;                         // AM1805 for the power-management variant

void setup()
{
    Wire.begin();                   // start I2C bus before calling begin()
    Serial.begin(115200);
    while (!rtc.begin())            // returns true once the chip ID matches
    {
        Serial.println("RTC not found");
        delay(1000);
    }
}
```

On RP2040, set the SDA/SCL pins before `Wire.begin()`:

```cpp
Wire.setSDA(4);
Wire.setSCL(5);
Wire.begin();
```

On ESP32, pass the pins directly:

```cpp
Wire.begin(21, 22);   // SDA, SCL
```

### SPI (AM0815 / AM1815)

```cpp
#include <amx8x5.h>
#include <SPI.h>

#define RTC_CS_PIN 10

AM0815 rtc;                         // AM1815 for the power-management variant

void setup()
{
    SPI.begin();
    Serial.begin(115200);
    while (!rtc.begin(RTC_CS_PIN))  // pass the chip-select pin
    {
        Serial.println("RTC not found");
        delay(1000);
    }
}
```

---

## 4. Getting Started — C API

The C API requires two callback functions for bus access (read and write). Implement them for your platform and store them in the `stc_amx8x5_handle_t` structure.

### I2C with custom callbacks

```c
#include "amx8x5.h"

/* ---- Platform I2C callbacks ---- */
static int myI2cWrite(void* pHandle, uint32_t u32Addr, uint8_t u8Reg,
                       uint8_t* pu8Data, uint32_t u32Len)
{
    /* Your platform I2C write implementation */
    /* Returns 0 on success, non-zero on error */
    return platform_i2c_write((uint8_t)u32Addr, u8Reg, pu8Data, u32Len);
}

static int myI2cRead(void* pHandle, uint32_t u32Addr, uint8_t u8Reg,
                      uint8_t* pu8Data, uint32_t u32Len)
{
    return platform_i2c_read((uint8_t)u32Addr, u8Reg, pu8Data, u32Len);
}

/* ---- RTC handle (static or global) ---- */
static stc_amx8x5_handle_t stcRtc = {
    .enMode      = AMx8x5ModeI2C,
    .pHandle     = NULL,                /* optional user context pointer */
    .enRtcType   = AMx8x5Type0805,      /* change for your variant */
    .u32Address  = AMX8X5_I2C_ADDRESS,  /* 0x69 default */
    .pfnWriteI2C = myI2cWrite,
    .pfnReadI2C  = myI2cRead,
};

int main(void)
{
    en_result_t res = Amx8x5_Init(&stcRtc);
    if (res != Ok)
    {
        /* error — chip not responding or wrong type */
    }
    /* ... */
}
```

The default I2C address (`AMX8X5_I2C_ADDRESS`) is `0x69`. If the ADDR pin is pulled low, use `0x68`.

### SPI with custom callbacks

```c
#include "amx8x5.h"

static int mySpiWrite(void* pHandle, uint32_t u32CS, uint8_t u8Reg,
                       uint8_t* pu8Data, uint32_t u32Len)
{
    /* The AM08x5 SPI protocol: register address byte with MSB=0 (write), then data */
    platform_spi_cs_low((uint8_t)u32CS);
    platform_spi_transfer(u8Reg & 0x7F);    /* MSB clear = write */
    for (uint32_t i = 0; i < u32Len; i++)
        platform_spi_transfer(pu8Data[i]);
    platform_spi_cs_high((uint8_t)u32CS);
    return 0;
}

static int mySpiRead(void* pHandle, uint32_t u32CS, uint8_t u8Reg,
                      uint8_t* pu8Data, uint32_t u32Len)
{
    /* Register address byte with MSB=1 (read) */
    platform_spi_cs_low((uint8_t)u32CS);
    platform_spi_transfer(u8Reg | 0x80);    /* MSB set = read */
    for (uint32_t i = 0; i < u32Len; i++)
        pu8Data[i] = platform_spi_transfer(0xFF);
    platform_spi_cs_high((uint8_t)u32CS);
    return 0;
}

static stc_amx8x5_handle_t stcRtc = {
    .enMode        = AMx8x5ModeSPI,
    .pHandle       = NULL,
    .enRtcType     = AMx8x5Type0815,
    .u32ChipSelect = 10,                /* CS pin or channel number */
    .pfnWriteSpi   = mySpiWrite,
    .pfnReadSpi    = mySpiRead,
};
```

---

## 5. Getting Started — C++ API

The `AMx8x5` base class and its concrete subclasses mirror the C API exactly. The C++ `init()` method accepts a preconfigured `AMx8x5::stcHandle` to supply custom callbacks; `begin()` (Arduino only) wires the Arduino `Wire`/`SPI` libraries automatically.

```cpp
#include "amx8x5.h"

AM0805 rtc;   // or AM1805 / AM0815 / AM1815

// Supply custom I2C callbacks instead of Arduino Wire:
AMx8x5::stcHandle handle;
handle.pfnWriteI2C = myI2cWrite;
handle.pfnReadI2C  = myI2cRead;
handle.u32Address  = AMX8X5_I2C_ADDRESS;

rtc.init(&handle);
```

All C++ methods return `AMx8x5::enResult`, which is `AMx8x5::Ok` (= `0`) on success.

---

## 6. Reading and Writing Time

### Time structure

```c
typedef struct {
    uint8_t u8Hundredth;  /* 0–99          */
    uint8_t u8Second;     /* 0–59          */
    uint8_t u8Minute;     /* 0–59          */
    uint8_t u8Hour;       /* 0–23 (24 h) or 1–12 (12 h) */
    uint8_t u8Date;       /* 1–31          */
    uint8_t u8Month;      /* 1–12          */
    uint8_t u8Year;       /* 0–99 (offset from century) */
    uint8_t u8Weekday;    /* 0–6 (0 = Sunday) */
    uint8_t u8Century;    /* 0 = 19xx / 21xx, 1 = 20xx */
    uint8_t u8Mode;       /* 2 = 24 h, 0 = 12 h AM, 1 = 12 h PM */
} stc_amx8x5_time_t;
```

The `AMx8x5_24HR_MODE` / `AMx8x5_12HR_MODE` macros set `u8Mode`.

### Arduino / C++

```cpp
stc_amx8x5_time_t  stcSet;
stc_amx8x5_time_t* pTime;

// Set time
stcSet.u8Second  = 0;
stcSet.u8Minute  = 30;
stcSet.u8Hour    = 14;
stcSet.u8Date    = 23;
stcSet.u8Month   = 4;
stcSet.u8Year    = 26;    // 2026
stcSet.u8Century = 1;
stcSet.u8Mode    = AMX8X5_24HR_MODE;
rtc.setTime(&stcSet, false);   // false = keep counters writable

// Read time
rtc.getTime(&pTime);
Serial.printf("%02d:%02d:%02d\n", pTime->u8Hour, pTime->u8Minute, pTime->u8Second);
```

### C API

```c
stc_amx8x5_time_t  stcSet = { .u8Second=0, .u8Minute=30, .u8Hour=14,
                               .u8Date=23, .u8Month=4, .u8Year=26,
                               .u8Century=1, .u8Mode=AMX8X5_24HR_MODE };
Amx8x5_SetTime(&stcRtc, &stcSet, false);

stc_amx8x5_time_t* pTime;
Amx8x5_GetTime(&stcRtc, &pTime);
```

Individual field getters (return value directly, `-1` on error):

```c
int16_t sec  = Amx8x5_GetSecond(&stcRtc);
int16_t min  = Amx8x5_GetMinute(&stcRtc);
int16_t hour = Amx8x5_GetHour(&stcRtc);
int16_t day  = Amx8x5_GetDay(&stcRtc);
int16_t mon  = Amx8x5_GetMonth(&stcRtc);
int16_t year = Amx8x5_GetYear(&stcRtc);
```

---

## 7. Alarms

The alarm fires when all enabled alarm registers match the current time.

```cpp
// Alarm every day at 07:00:00
stc_amx8x5_time_t stcAlarm = {};
stcAlarm.u8Second  = 0;
stcAlarm.u8Minute  = 0;
stcAlarm.u8Hour    = 7;

rtc.setAlarm(
    &stcAlarm,
    AMx8x5AlarmDay,                  // repeat: once per day
    AMx8x5InterruptModeLevel,        // level interrupt (static until cleared)
    AMx8x5InterruptIrq               // signal on FOUT/nIRQ pin
);
```

**Repeat modes** (`en_amx8x5_alarm_repeat_t`):

| Value                      | Description            |
|----------------------------|------------------------|
| `AMx8x5AlarmDisabled`      | Disable alarm          |
| `AMx8x5AlarmYear`          | Once per year          |
| `AMx8x5AlarmMonth`         | Once per month         |
| `AMx8x5AlarmWeek`          | Once per week          |
| `AMx8x5AlarmDay`           | Once per day           |
| `AMx8x5AlarmHour`          | Once per hour          |
| `AMx8x5AlarmMinute`        | Once per minute        |
| `AMx8x5AlarmSecond`        | Once per second        |
| `AMx8x5Alarm10thSecond`    | Once per 1/10 second   |
| `AMx8x5Alarm100thSecond`   | Once per 1/100 second  |

**Interrupt modes** (`en_amx8x5_interrupt_mode_t`):

| Value                              | Pulse width              |
|------------------------------------|--------------------------|
| `AMx8x5InterruptModeLevel`         | Static level             |
| `AMx8x5InterruptModePulseShort`    | 1/8192 s (XT) / 1/64 s (RC) |
| `AMx8x5InterruptModePulseMedium`   | 1/64 s                   |
| `AMx8x5InterruptModePulseLong`     | 1/4 s                    |

**Interrupt pins** (`en_amx8x5_interrupt_pin_t`):

| Value                       | Pin          |
|-----------------------------|--------------|
| `AMx8x5InterruptPinInternal`| Internal flag only |
| `AMx8x5InterruptIrq`        | FOUT/nIRQ    |
| `AMx8x5InterruptIrq2`       | PSW/nIRQ2    |

---

## 8. Countdown Timer

Generates a periodic interrupt at a fixed interval.

```cpp
// Interrupt every 500 ms on FOUT/nIRQ, repeat continuously
rtc.setCountdown(
    AMx8x5PeriodeRangeMs,            // range: milliseconds
    500,                             // period value
    AMx8x5CountdownSingleLevelIrq,   // single-shot level interrupt
    AMx8x5CountdownPinFoutIrq        // output pin
);
```

**Period ranges** (`en_amx8x5_periode_range_t`):

| Value                    | Unit        |
|--------------------------|-------------|
| `AMx8x5PeriodeRangeUs`   | Microseconds |
| `AMx8x5PeriodeRangeMs`   | Milliseconds |
| `AMx8x5PeriodeRangeS`    | Seconds      |
| `AMx8x5PeriodeRangeMin`  | Minutes      |
| `AMx8x5PeriodeRangeH`    | Hours        |

---

## 9. Watchdog Timer

Resets or generates an interrupt if the application does not service the WDI pin within the configured period.

```cpp
// 4-second watchdog, assert on FOUT/nIRQ
rtc.setWatchdog(4000, AMx8x5WatchdogInterruptPinFoutIrq);

// Disable watchdog
rtc.setWatchdog(0, AMx8x5WatchdogInterruptPinFoutIrq);
```

**Watchdog pins** (`en_amx8x5_watchdog_interrupt_pin_t`):

| Value                            | Output          |
|----------------------------------|-----------------|
| `AMx8x5WatchdogInterruptPinnRst` | nRST            |
| `AMx8x5WatchdogInterruptPinFoutIrq` | FOUT/nIRQ   |

---

## 10. Square Wave Output

Outputs a calibrated clock signal on FOUT/nIRQ or PSW/nIRQ2.

```cpp
// 32.768 kHz square wave on FOUT/nIRQ (SQFS=0)
rtc.setSquareWaveOutput(0, 1);   // SQFS=0, pinMsk=1 (FOUT/nIRQ)

// 512 Hz on PSW/nIRQ2 (SQFS=6)
rtc.setSquareWaveOutput(6, 2);   // pinMsk=2 = PSW/nIRQ2

// Disable
rtc.setSquareWaveOutput(0, 0);   // pinMsk=0 = disable
```

SQFS values (selected common ones):

| SQFS | Frequency    |
|------|--------------|
| 0    | 32 768 Hz    |
| 1    | 16 384 Hz    |
| 2    | 8 192 Hz     |
| 6    | 512 Hz       |
| 10   | 32 Hz        |
| 14   | 2 Hz         |
| 15   | 1 Hz         |
| 16   | 1/2 Hz       |
| 31   | 1/128 Hz     |

See register definition `AMX8X5_REG_SQW` for the full table.

---

## 11. Oscillator Selection and Calibration

### Oscillator mode

```cpp
// Use 32 kHz XT, switch to RC on battery power
rtc.selectOscillatorMode(AMx8x5Xt32KHzSwitchRcOnBat);

// Force 128 Hz RC oscillator (ultra-low power, less accurate)
rtc.selectOscillatorMode(AMx8x5nRc128Hz);
```

| Value                         | Description                                    |
|-------------------------------|------------------------------------------------|
| `AMx8x5Xt32KHzNoSwitch`       | 32 kHz crystal, no automatic switching         |
| `AMx8x5Xt32KHzSwitchRcOnBat`  | 32 kHz crystal, auto-switch to RC on battery   |
| `AMx8x5nRc128Hz`              | 128 Hz RC oscillator only                      |

### Manual calibration

Fine-tune the oscillator frequency in ppm:

```cpp
// Advance the XT oscillator by +5 ppm
rtc.setCalibrationValue(AMx8x5CalibrateModeXT, 5);

// Reset calibration
rtc.setCalibrationValue(AMx8x5CalibrateModeXT, 0);
```

Limits: XT: −610 to +242 ppm · RC: −65536 to +65520 ppm.

### Autocalibration

Automatically adjusts the RC oscillator to match the XT reference:

```cpp
rtc.setAutocalibration(AMx8x5AutocalibrationPeriod1Min);
```

| Value                               | Period            |
|-------------------------------------|-------------------|
| `AMx8x5AutocalibrationPeriodDisabled` | Disabled        |
| `AMx8x5AutocalibrationPeriod1Min`   | Every 1 minute    |
| `AMx8x5AutocalibrationPeriod10Min`  | Every 10 minutes  |
| `AMx8x5AutocalibrationPeriodOnce`   | Once only         |

---

## 12. Output Pin Control

### Enable / disable output pins (OCTRL register)

Controls whether a pin is driven during normal and power-down states.

```cpp
// Enable FOUT/nIRQ output
rtc.enableOutput(AMX8X5_REG_OCTRL_O1EN_MSK, true);

// Enable both FOUT/nIRQ and PSW/nIRQ2
rtc.enableOutput(AMX8X5_REG_OCTRL_O1EN_MSK | AMX8X5_REG_OCTRL_O3EN_MSK, true);

// Enable nRST output
rtc.enableOutput(AMX8X5_REG_OCTRL_RSEN_MSK, true);
```

| Mask constant                | Pin          |
|------------------------------|--------------|
| `AMX8X5_REG_OCTRL_O1EN_MSK`  | FOUT/nIRQ    |
| `AMX8X5_REG_OCTRL_O3EN_MSK`  | PSW/nIRQ2 / nTIRQ |
| `AMX8X5_REG_OCTRL_O4EN_MSK`  | CLKOUT/nIRQ3 |
| `AMX8X5_REG_OCTRL_RSEN_MSK`  | nRST         |

### Static output level

```cpp
rtc.ctrlOut(true);     // drive FOUT/nIRQ high (static OUT bit)
rtc.ctrlOutB(true);    // drive PSW/nIRQ2 high (OUTB bit, AM18x5 only)
```

### OUT1 / OUT2 signal routing

```cpp
// Route nIRQ signal to FOUT/nIRQ when any interrupt is active
rtc.setOut1Mode(AMx8x5Out1nIRQAtIrqElseOut);

// Route nTIRQ to PSW/nIRQ2 when the timer interrupt is active
rtc.setOut2Mode(AMx8x5Out2TIRQIfTieElseOutB);
```

**OUT1 modes** (`en_amx8x5_out1_mode_t`):

| Value                                | Description                         |
|--------------------------------------|-------------------------------------|
| `AMx8x5Out1nIRQAtIrqElseOut`         | nIRQ when interrupt active, else OUT |
| `AMx8x5Out1SwqIfSqweElseOut`         | Square wave if SQWE set, else OUT   |
| `AMx8x5Out1SwqIfSqweElsenIRQAtIrqElseOut` | SQW or nIRQ, else OUT          |
| `AMx8x5Out1nAIRQIfAieElseOut`        | nAIRQ if alarm enabled, else OUT    |

**OUT2 modes** (`en_amx8x5_out2_mode_t`):

| Value                             | Description                                  |
|-----------------------------------|----------------------------------------------|
| `AMx8x5Out2nIRQAtIrqElseOutB`     | nIRQ when interrupt active, else OUTB        |
| `AMx8x5Out2SwqIfSqweElseOutB`     | Square wave if SQWE, else OUTB               |
| `AMx8x5Out2nAIRQIfAieElseOutB`    | nAIRQ if alarm enabled, else OUTB            |
| `AMx8x5Out2TIRQIfTieElseOutB`     | TIRQ if timer enabled, else OUTB             |
| `AMx8x5Out2nTIRQIfTieElseOutB`    | nTIRQ if timer enabled, else OUTB            |
| `AMx8x5Out2Sleep`                 | Sleep mode output                            |
| `AMx8x5Out2OutB`                  | Static OUTB                                  |

---

## 13. Interrupt Handling

### Enable / disable individual interrupts

```cpp
rtc.enableIrqAlarm(true);           // alarm interrupt
rtc.enableIrqTimer(true);           // countdown timer interrupt
rtc.enableIrqBatteryLow(true);      // battery low
rtc.enableIrqOscillatorFail(true);  // oscillator failure
rtc.enableIrqAutocalibFail(true);   // autocalibration failure
rtc.enableIrqXt1OnExti(true);       // XT1 interrupt on EXTn pin
rtc.enableIrqXt2OnWdi(true);        // XT2 interrupt on WDI pin
```

### Raw interrupt mask

```cpp
// Enable alarm and timer interrupts simultaneously
rtc.enableInterrupt(AMX8X5_REG_INT_MASK_AIE_MSK | AMX8X5_REG_INT_MASK_TIE_MSK);

// Disable all
rtc.disableInterrupt(0xFF);
```

### Read and clear interrupt status

```cpp
uint8_t status;
rtc.getInterruptStatus(&status);

if (status & AMX8X5_REG_STATUS_ALM_MSK)
{
    // Alarm fired
    rtc.clearInterrupt(AMX8X5_REG_STATUS_ALM_MSK);
}

// Or clear all at once
rtc.clearInterrupts();
```

### Auto-clear on status read

Enable automatic clearing of all interrupt flags whenever the Status register is read:

```cpp
rtc.autoResetStatus(true);
```

---

## 14. Power Management (AM18x5 only)

These functions are only available on AM1805 / AM1815.

### Sleep mode

Put the RTC into sleep mode, asserting PSW/nIRQ2 or nRST as a power switch to the host:

```cpp
// After 2 × 7.8 ms delay, pull PSW/nIRQ2 high (turns off host supply)
rtc.setSleepMode(2, AMx8x5PswIrq2HighInSleep);
```

Wake-up is triggered by the first enabled interrupt (alarm, timer, or external pin).

| Sleep mode value                    | Effect on pins                           |
|-------------------------------------|------------------------------------------|
| `AMx8x5nRstLowInSleep`              | Pull nRST low                            |
| `AMx8x5PswIrq2HighInSleep`          | Pull PSW/nIRQ2 high (power switch)       |
| `AMx8x5nRstLoPswIrq2HighInSleep`    | Both                                     |

### Battery mode I/O

Control whether the I2C/SPI bus remains active when the device is on battery power:

```cpp
rtc.setBatmodeIO(true);    // keep I/O active on VBAT (useful for battery-backed access)
rtc.setBatmodeIO(false);   // disable I/O on VBAT (default — lower power)
```

### High-current PSW output

```cpp
rtc.setPswHighCurrent(true);   // ~1 Ω pull-down on PSW/nIRQ2 (power switch mode)
rtc.setPswHighCurrent(false);  // normal open-drain
```

### nEXTR as reset

```cpp
rtc.usenExtrAsReset(true);   // nEXTR pulse generates nRST
```

### Reset polarity

```cpp
rtc.setResetPolarity(false);  // nRST asserted low (default)
rtc.setResetPolarity(true);   // nRST asserted high
```

### Stop and restart

```cpp
rtc.stop(true);    // freeze the counter
rtc.stop(false);   // resume counting
```

---

## 15. Battery and Trickle Charger

### Trickle charger (AM18x5 only)

Slowly charges a connected supercap or rechargeable cell via VBAT:

```cpp
// Enable: Schottky diode (0.3 V drop), 3 kΩ resistor
rtc.enableTrickleCharger(
    AMx8x5TrickleDiodeSchottky,
    AMx8x5TrickleResistor3K,
    true
);

// Disable
rtc.enableTrickleCharger(AMx8x5TrickleDiodeSchottky, AMx8x5TrickleResistor3K, false);
```

**Diode options** (`en_amx8x5_trickle_diode_t`):

| Value                       | Drop    |
|-----------------------------|---------|
| `AMx8x5TrickleDiodeDisabled`| —       |
| `AMx8x5TrickleDiodeSchottky`| 0.3 V   |
| `AMx8x5TrickleDiodeNormal`  | 0.6 V   |

**Resistor options** (`en_amx8x5_trickle_resistor_t`):

| Value                        | Resistance |
|------------------------------|------------|
| `AMx8x5TrickleResistorDisabled` | —       |
| `AMx8x5TrickleResistor3K`    | 3 kΩ      |
| `AMx8x5TrickleResistor6K`    | 6 kΩ      |
| `AMx8x5TrickleResistor11K`   | 11 kΩ     |

### Battery reference voltage

Sets the threshold for the battery brownout detector (BBOD signal):

```cpp
rtc.setBatteryReferenceVoltage(AMx8x5BatReferenceFalling25V_Rising30V);
```

| Value                                    | Falling | Rising |
|------------------------------------------|---------|--------|
| `AMx8x5BatReferenceFalling25V_Rising30V` | 2.5 V   | 3.0 V  |
| `AMx8x5BatReferenceFalling21V_Rising25V` | 2.1 V   | 2.5 V  |
| `AMx8x5BatReferenceFalling18V_Rising22V` | 1.8 V   | 2.2 V  |
| `AMx8x5BatReferenceFalling14V_Rising16V` | 1.4 V   | 1.6 V  |

### Analog status register

Read power-supply voltage levels:

```cpp
uint8_t astat;
rtc.getAnalogStatus(&astat);
// Bit fields defined in AMX8X5_REG_ASTAT
```

---

## 16. Non-volatile RAM

The AM08x5/AM18x5 provides 64 bytes of user RAM accessible at addresses `0x00`–`0x3F` within the RAM space (mapped to register addresses `0x40`–`0x7F`).

```cpp
// Write a byte
rtc.ramWrite(0x00, 0xAB);

// Read it back
uint8_t val;
rtc.ramRead(0x00, &val);   // val == 0xAB
```

Each address stores one byte. Data persists as long as VCC or VBAT is present.

---

## 17. Raw Register Access

For direct register manipulation not covered by the high-level API:

```cpp
// Set individual bits
rtc.setRegister(AMX8X5_REG_OSC_CONTROL, AMX8X5_REG_OSC_CONTROL_FOS_MSK);

// Clear individual bits
rtc.clearRegister(AMX8X5_REG_OSC_CONTROL, AMX8X5_REG_OSC_CONTROL_FOS_MSK);

// Read / write single byte
uint8_t val;
rtc.readByte(AMX8X5_REG_STATUS, &val);
rtc.writeByte(AMX8X5_REG_CONFIG_KEY, AMX8X5_REG_CONFIG_KEY_VAL_OSC);

// Multi-byte read / write
uint8_t buf[8];
rtc.readBytes(AMX8X5_REG_HUNDREDTHS, buf, 8);
```

> **Note:** Some registers (OSC_CONTROL, TRICKLE, BREF_CTRL, BATMODE_IO) require writing the Configuration Key register first. The high-level API handles this automatically. When using raw access, write `AMX8X5_REG_CONFIG_KEY` with value `AMX8X5_REG_CONFIG_KEY_VAL_OSC` (0xA1) or `AMX8X5_REG_CONFIG_KEY_VAL_OTHER` (0x9D) as required by the datasheet.

---

## 18. Error Codes

All functions returning `en_result_t` (C) or `AMx8x5::enResult` (C++) use:

| Value                   | Meaning                                      |
|-------------------------|----------------------------------------------|
| `Ok` (0)                | Success                                      |
| `Error`                 | Generic error                                |
| `ErrorInvalidParameter` | NULL pointer or out-of-range argument        |
| `ErrorUninitialized`    | Handle is NULL or callbacks not set          |
| `ErrorInvalidMode`      | Chip ID mismatch during `Init()`             |
| `OperationInProgress`   | Sleep request declined (interrupt pending)   |
| `ErrorTimeout`          | Communication timeout                        |

Individual getters (`Amx8x5_GetSecond`, etc.) return `-1` on error.

---

## 19. Full C API Reference

| Function | Description |
|---|---|
| `Amx8x5_Init(handle)` | Initialize, verify chip ID |
| `Amx8x5_Reset(handle)` | Software reset |
| `Amx8x5_GetTime(handle, &pTime)` | Read full time struct |
| `Amx8x5_GetHundredth(handle)` | Read hundredths field |
| `Amx8x5_GetSecond(handle)` | Read seconds field |
| `Amx8x5_GetMinute(handle)` | Read minutes field |
| `Amx8x5_GetHour(handle)` | Read hours field |
| `Amx8x5_GetDay(handle)` | Read day field |
| `Amx8x5_GetWeekday(handle)` | Read weekday field |
| `Amx8x5_GetMonth(handle)` | Read month field |
| `Amx8x5_GetYear(handle)` | Read year field |
| `Amx8x5_GetCentury(handle)` | Read century bit |
| `Amx8x5_SetTime(handle, pTime, bProtect)` | Write full time struct |
| `Amx8x5_SetCalibrationValue(handle, mode, ppm)` | Set oscillator calibration |
| `Amx8x5_SetAlarm(handle, pTime, repeat, irqMode, pin)` | Configure alarm |
| `Amx8x5_SetCountdown(handle, range, period, repeat, pin)` | Configure countdown timer |
| `Amx8x5_SetWatchdog(handle, ms, pin)` | Configure watchdog |
| `Amx8x5_SetSquareWaveOutput(handle, sqfs, pinMsk)` | Configure square wave |
| `Amx8x5_SelectOscillatorMode(handle, mode)` | Select XT/RC oscillator |
| `Amx8x5_SetAutocalibration(handle, period)` | Configure autocalibration |
| `Amx8x5_EnableOutput(handle, mask, enable)` | Control OCTRL output pins |
| `Amx8x5_SetOut1Mode(handle, mode)` | Set FOUT/nIRQ routing |
| `Amx8x5_SetOut2Mode(handle, mode)` | Set PSW/nIRQ2 routing |
| `Amx8x5_CtrlOut(handle, on)` | Static OUT bit |
| `Amx8x5_CtrlOutB(handle, on)` | Static OUTB bit |
| `Amx8x5_SetResetPolarity(handle, high)` | nRST polarity |
| `Amx8x5_AutoResetStatus(handle, en)` | Auto-clear status on read |
| `Amx8x5_UsenExtrAsReset(handle, en)` | nEXTR → nRST |
| `Amx8x5_SetPswHighCurrent(handle, en)` | High-current PSW |
| `Amx8x5_EnableIrqXt1OnExti(handle, en)` | XT1 / EXTn interrupt |
| `Amx8x5_EnableIrqXt2OnWdi(handle, en)` | XT2 / WDI interrupt |
| `Amx8x5_EnableIrqAlarm(handle, en)` | Alarm interrupt |
| `Amx8x5_EnableIrqTimer(handle, en)` | Timer interrupt |
| `Amx8x5_EnableIrqBatteryLow(handle, en)` | Battery-low interrupt |
| `Amx8x5_EnableIrqOscillatorFail(handle, en)` | Oscillator-fail interrupt |
| `Amx8x5_EnableIrqAutocalibFail(handle, en)` | Autocalib-fail interrupt |
| `Amx8x5_EnableInterrupt(handle, mask)` | Set bits in INT_MASK |
| `Amx8x5_DisableInterrupt(handle, mask)` | Clear bits in INT_MASK |
| `Amx8x5_ClearInterrupts(handle)` | Clear all status flags |
| `Amx8x5_ClearInterrupt(handle, mask)` | Clear selected status flags |
| `Amx8x5_GetInterruptStatus(handle, &status)` | Read STATUS register |
| `Amx8x5_GetAnalogStatus(handle, &status)` | Read ASTAT register |
| `Amx8x5_SetBatmodeIO(handle, en)` | I/O on battery power |
| `Amx8x5_EnableTrickleCharger(handle, diode, res, en)` | Trickle charger |
| `Amx8x5_SetBatteryReferenceVoltage(handle, ref)` | VBAT comparator threshold |
| `Amx8x5_SetSleepMode(handle, timeout, mode)` | Enter sleep mode |
| `Amx8x5_Stop(handle, stop)` | Freeze / unfreeze counter |
| `Amx8x5_RamRead(handle, addr, &data)` | Read user RAM byte |
| `Amx8x5_RamWrite(handle, addr, data)` | Write user RAM byte |
| `Amx8x5_GetExtensionAddress(handle, addr, &ext)` | Read extension register |
| `Amx8x5_ReadByte(handle, reg, &val)` | Read one register byte |
| `Amx8x5_ReadBytes(handle, reg, buf, len)` | Read multiple bytes |
| `Amx8x5_WriteByte(handle, reg, val)` | Write one register byte |
| `Amx8x5_WriteBytes(handle, reg, buf, len)` | Write multiple bytes |
| `Amx8x5_SetRegister(handle, reg, mask)` | Set bits in register |
| `Amx8x5_ClearRegister(handle, reg, mask)` | Clear bits in register |

---

## 20. Full C++ / Arduino API Reference

All methods below are members of `AMx8x5` (base class). The concrete classes `AM0805`, `AM1805`, `AM0815`, `AM1815` inherit all methods.

| Method | Description |
|---|---|
| `begin()` | Init with Arduino Wire (I2C) |
| `begin(csPin)` | Init with Arduino SPI, chip-select pin |
| `init(pHandle)` | Init with custom callbacks |
| `reset()` | Software reset |
| `getTime(&pTime)` | Read full time struct |
| `setTime(pTime, protect)` | Write full time struct |
| `setCalibrationValue(mode, ppm)` | Oscillator calibration |
| `setAlarm(pTime, repeat, irqMode, pin)` | Alarm configuration |
| `setCountdown(range, period, repeat, pin)` | Countdown timer |
| `setWatchdog(ms, pin)` | Watchdog timer |
| `setSquareWaveOutput(sqfs, pinMsk)` | Square wave output |
| `selectOscillatorMode(mode)` | XT / RC oscillator selection |
| `setAutocalibration(period)` | RC autocalibration |
| `enableOutput(mask, enable)` | OCTRL output pin control |
| `setOut1Mode(mode)` | FOUT/nIRQ routing |
| `setOut2Mode(mode)` | PSW/nIRQ2 routing |
| `ctrlOut(on)` | Static OUT bit |
| `ctrlOutB(on)` | Static OUTB bit |
| `setResetPolarity(high)` | nRST polarity |
| `autoResetStatus(en)` | Auto-clear on status read |
| `usenExtrAsReset(en)` | nEXTR → nRST |
| `setPswHighCurrent(en)` | High-current PSW |
| `enableIrqXt1OnExti(en)` | XT1 interrupt |
| `enableIrqXt2OnWdi(en)` | XT2 interrupt |
| `enableIrqAlarm(en)` | Alarm interrupt |
| `enableIrqTimer(en)` | Timer interrupt |
| `enableIrqBatteryLow(en)` | Battery-low interrupt |
| `enableIrqOscillatorFail(en)` | Oscillator-fail interrupt |
| `enableIrqAutocalibFail(en)` | Autocalib-fail interrupt |
| `enableInterrupt(mask)` | Set INT_MASK bits |
| `disableInterrupt(mask)` | Clear INT_MASK bits |
| `clearInterrupts()` | Clear all status flags |
| `clearInterrupt(mask)` | Clear selected status flags |
| `getInterruptStatus(&status)` | Read STATUS register |
| `getAnalogStatus(&status)` | Read ASTAT register |
| `setBatmodeIO(en)` | I/O on battery power |
| `enableTrickleCharger(diode, res, en)` | Trickle charger |
| `setBatteryReferenceVoltage(ref)` | VBAT comparator threshold |
| `setSleepMode(timeout, mode)` | Enter sleep mode |
| `stop(stop)` | Freeze / unfreeze counter |
| `ramRead(addr, &data)` | Read user RAM byte |
| `ramWrite(addr, data)` | Write user RAM byte |
| `readByte(reg, &val)` | Read one register byte |
| `readBytes(reg, buf, len)` | Read multiple bytes |
| `writeByte(reg, val)` | Write one register byte |
| `writeBytes(reg, buf, len)` | Write multiple bytes |
| `setRegister(reg, mask)` | Set bits in register |
| `clearRegister(reg, mask)` | Clear bits in register |
| `getExtensionAddress(addr, &ext)` | Read extension register |
