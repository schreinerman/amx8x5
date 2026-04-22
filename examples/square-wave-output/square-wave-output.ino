#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates the programmable Square Wave (SQW) output.
//
// The AM0805/AM1805 can generate a square wave on the FOUT/nIRQ pin with
// frequencies ranging from 32.768 kHz down to once per century, selectable
// via the SQFS field in the SQW register (0x13).
//
// Datasheet reference: Section 6.4.5 0x13 - SQW, Table 6-49
//
// SQFS value -> output frequency:
//   0        -> 1 per century
//   1        -> 32.768 kHz
//   2        -> 8.192 kHz
//   3        -> 4.096 kHz
//   4        -> 2.048 kHz
//   5        -> 1.024 kHz
//   6        -> 512 Hz  (default)
//   7        -> 256 Hz
//   8        -> 128 Hz
//   9        -> 64 Hz
//   10       -> 32 Hz
//   11       -> 16 Hz
//   12       -> 8 Hz
//   13       -> 4 Hz
//   14       -> 2 Hz
//   15       -> 1 Hz
//   16       -> 1/2 Hz  (1 pulse every 2 s)
//   ...      -> ...
//   31       -> 1 per year
//
// This example cycles through several frequencies every 5 seconds and prints
// the active setting to Serial.

AM0805 rtc;
stc_amx8x5_time_t stcTimeToSet;
stc_amx8x5_time_t* pMyTime;

// SQFS value, description
struct {
    uint8_t u8SQFS;
    const char* szDesc;
} aFrequencies[] = {
    { 1,  "32.768 kHz" },
    { 6,  "512 Hz"     },
    { 8,  "128 Hz"     },
    { 15, "1 Hz"       },
    { 16, "0.5 Hz"     },
};
const uint8_t u8NumFreq = sizeof(aFrequencies) / sizeof(aFrequencies[0]);
uint8_t u8FreqIdx = 0;

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

    // Enable Output Control: make sure FOUT/nIRQ is enabled as output
    rtc.enableOutput(AMX8X5_REG_OCTRL_O1EN_MSK, true);

    // Set initial square wave on FOUT/nIRQ (pin mask 1 = FOUT/nIRQ)
    rtc.setSquareWaveOutput(aFrequencies[u8FreqIdx].u8SQFS, 1);
    Serial.printf("SQW output started: %s on FOUT/nIRQ\n", aFrequencies[u8FreqIdx].szDesc);
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("%02d:%02d:%02d  |  SQW: %s (SQFS=%d)\n",
                  pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second,
                  aFrequencies[u8FreqIdx].szDesc, aFrequencies[u8FreqIdx].u8SQFS);

    delay(5000);

    // Switch to next frequency
    u8FreqIdx = (u8FreqIdx + 1) % u8NumFreq;
    rtc.setSquareWaveOutput(aFrequencies[u8FreqIdx].u8SQFS, 1);
    Serial.printf("  --> Switching SQW to: %s (SQFS=%d)\n",
                  aFrequencies[u8FreqIdx].szDesc, aFrequencies[u8FreqIdx].u8SQFS);
}
