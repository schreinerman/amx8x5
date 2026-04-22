#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates flexible output pin configuration.
//
// The AM0805/AM1805 has two multipurpose output pins:
//
//   FOUT/nIRQ  (Out1) – configured via setOut1Mode() and ctrlOut()
//   PSW/nIRQ2  (Out2) – configured via setOut2Mode() and ctrlOutB()
//
// Out1 (FOUT/nIRQ) modes:
//   AMx8x5Out1nIRQAtIrqElseOut            – nIRQ when any interrupt active, else static OUT level
//   AMx8x5Out1SwqIfSqweElseOut            – SQW output when SQWE=1, else static OUT level
//   AMx8x5Out1SwqIfSqweElsenIRQAtIrqElseOut – SQW when SQWE=1, nIRQ when interrupt, else OUT
//   AMx8x5Out1nAIRQIfAieElseOut           – nAIRQ (alarm only) when AIE=1, else OUT
//
// Out2 (PSW/nIRQ2) modes:
//   AMx8x5Out2nIRQAtIrqElseOutB           – nIRQ when any interrupt active, else static OUTB
//   AMx8x5Out2SwqIfSqweElseOutB           – SQW when SQWE=1, else static OUTB
//   AMx8x5Out2nAIRQIfAieElseOutB          – nAIRQ (alarm only) when AIE=1, else OUTB
//   AMx8x5Out2TIRQIfTieElseOutB           – TIRQ when TIE=1, else OUTB
//   AMx8x5Out2nTIRQIfTieElseOutB          – nTIRQ when TIE=1, else OUTB
//   AMx8x5Out2Sleep                       – SLEEP signal
//   AMx8x5Out2OutB                        – static OUTB level
//
// This example cycles through several Out1 and Out2 configurations every 5 s
// and uses ctrlOut() / ctrlOutB() to drive static levels where applicable.
//
// Datasheet reference: Section 6.4.3 0x11 - Control2, Table 6-43 / 6-44

AM0805 rtc;
stc_amx8x5_time_t stcTimeToSet;
stc_amx8x5_time_t* pMyTime;

uint8_t u8Step = 0;

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

    // Enable both output pins via Output Control register
    // O1EN (bit 0) = FOUT/nIRQ, O3EN (bit 1) = PSW/nIRQ2
    rtc.enableOutput(AMX8X5_REG_OCTRL_O1EN_MSK | AMX8X5_REG_OCTRL_O3EN_MSK, true);

    Serial.println("Output pin configuration demo started.");
    Serial.println("FOUT/nIRQ = Out1, PSW/nIRQ2 = Out2");
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("\n%02d:%02d:%02d  |  Step %d\n",
                  pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second, u8Step);

    switch (u8Step)
    {
        case 0:
            // Out1: nIRQ when any interrupt is active, else OUT=HIGH (static)
            rtc.setOut1Mode(AMx8x5::enOut1Mode::AMx8x5Out1nIRQAtIrqElseOut);
            rtc.ctrlOut(true);  // static OUT level = HIGH
            // Out2: static OUTB level = HIGH
            rtc.setOut2Mode(AMx8x5::enOut2Mode::AMx8x5Out2OutB);
            rtc.ctrlOutB(true);
            Serial.println("Out1: nIRQ | else static HIGH");
            Serial.println("Out2: static HIGH (OUTB)");
            break;

        case 1:
            // Out1: Square Wave on FOUT/nIRQ (512 Hz), else static LOW
            rtc.setSquareWaveOutput(6, 1); // SQFS=6 -> 512 Hz, enable on FOUT/nIRQ
            rtc.setOut1Mode(AMx8x5::enOut1Mode::AMx8x5Out1SwqIfSqweElseOut);
            rtc.ctrlOut(false);  // static OUT level = LOW when SQWE=0
            // Out2: nTIRQ (timer interrupt, active low) when TIE=1, else static LOW
            rtc.setOut2Mode(AMx8x5::enOut2Mode::AMx8x5Out2nTIRQIfTieElseOutB);
            rtc.ctrlOutB(false);
            Serial.println("Out1: SQW 512 Hz on FOUT/nIRQ");
            Serial.println("Out2: nTIRQ | else static LOW");
            break;

        case 2:
            // Out1: nAIRQ (alarm interrupt only) when AIE=1, else OUT=LOW
            rtc.setOut1Mode(AMx8x5::enOut1Mode::AMx8x5Out1nAIRQIfAieElseOut);
            rtc.ctrlOut(false);
            // Out2: nIRQ when any interrupt active, else static LOW
            rtc.setOut2Mode(AMx8x5::enOut2Mode::AMx8x5Out2nIRQAtIrqElseOutB);
            rtc.ctrlOutB(false);
            Serial.println("Out1: nAIRQ (alarm only)");
            Serial.println("Out2: nIRQ (any interrupt)");
            break;

        case 3:
            // Out1 and Out2: both driven to static LOW (all signals disabled)
            rtc.setOut1Mode(AMx8x5::enOut1Mode::AMx8x5Out1nIRQAtIrqElseOut);
            rtc.ctrlOut(false);
            rtc.setOut2Mode(AMx8x5::enOut2Mode::AMx8x5Out2OutB);
            rtc.ctrlOutB(false);
            // Stop SQW
            rtc.setSquareWaveOutput(0, 0);
            Serial.println("Out1: static LOW");
            Serial.println("Out2: static LOW");
            break;

        default:
            u8Step = 0;
            return;
    }

    u8Step = (u8Step + 1) % 4;
    delay(5000);
}
