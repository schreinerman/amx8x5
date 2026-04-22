#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates reading and writing the 256-byte general purpose RAM
// of the AM0805/AM1805.
//
// The RAM is split into two regions:
//   Standard RAM:  0x00 - 0x3F  (64 bytes, directly accessible at registers 0x40-0x7F)
//   Alternate RAM: 0x40 - 0xFF  (192 bytes, accessible via extension address register 0x3F)
//
// Datasheet reference: Section 6.12 Ram Registers
//
// This example:
//   - Stores a counter value and a string in RAM at startup using standard and alternate RAM
//   - Reads them back and prints the result
//   - Uses the GP bits in the Seconds register to detect a true Power-On-Reset:
//     GP0 (bit 7 of Seconds register 0x01) is 0 after power-up, so we use it as an
//     "initialized" marker. If it is 0, we set the time and write initial data to RAM.

AM0805 rtc;
stc_amx8x5_time_t stcTimeToSet;
stc_amx8x5_time_t* pMyTime;

// RAM addresses (0x00 .. 0xFF, mapped via Amx8x5_RamRead/Write)
#define RAM_ADDR_COUNTER    0x00  // 1 byte: boot counter (standard RAM)
#define RAM_ADDR_STRING     0x40  // 16 bytes: message string (alternate RAM)

const char* szMessage = "Hello from RTC!";

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

    // Check GP0 bit (bit 7 of Seconds register): 0 = Power-On-Reset occurred
    uint8_t u8Seconds;
    rtc.readByte(AMX8X5_REG_SECONDS, &u8Seconds);
    bool bFirstBoot = !(u8Seconds & AMX8X5_REG_SECONDS_GP0_MSK);

    if (bFirstBoot)
    {
        Serial.println("First boot detected (GP0=0). Setting time and initializing RAM...");
        rtc.setTime(&stcTimeToSet, false);

        // Mark as initialized by setting GP0 = 1
        rtc.setRegister(AMX8X5_REG_SECONDS, AMX8X5_REG_SECONDS_GP0_MSK);

        // Write initial counter = 1 to standard RAM
        rtc.ramWrite(RAM_ADDR_COUNTER, 1);

        // Write message string to alternate RAM (byte by byte)
        for (uint8_t i = 0; i < strlen(szMessage) + 1; i++)
        {
            rtc.ramWrite(RAM_ADDR_STRING + i, (uint8_t)szMessage[i]);
        }

        Serial.println("RAM initialized.");
    }
    else
    {
        // Increment boot counter in RAM
        uint8_t u8Counter = 0;
        rtc.ramRead(RAM_ADDR_COUNTER, &u8Counter);
        u8Counter++;
        rtc.ramWrite(RAM_ADDR_COUNTER, u8Counter);
        Serial.printf("Boot counter incremented to: %d\n", u8Counter);
    }
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("\n%02d:%02d:%02d  |  ", pMyTime->u8Hour, pMyTime->u8Minute, pMyTime->u8Second);

    // Read boot counter from standard RAM
    uint8_t u8Counter = 0;
    rtc.ramRead(RAM_ADDR_COUNTER, &u8Counter);
    Serial.printf("Boot count (RAM[0x%02x]): %d  |  ", RAM_ADDR_COUNTER, u8Counter);

    // Read message string from alternate RAM
    char szBuf[17] = {0};
    for (uint8_t i = 0; i < 16; i++)
    {
        uint8_t u8Byte = 0;
        rtc.ramRead(RAM_ADDR_STRING + i, &u8Byte);
        szBuf[i] = (char)u8Byte;
        if (u8Byte == 0) break;
    }
    Serial.printf("RAM string: \"%s\"\n", szBuf);

    delay(2000);
}
