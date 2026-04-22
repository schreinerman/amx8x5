#include <amx8x5.h>
#include <Wire.h>

// This example demonstrates how to use the AM0805 / AM1805 via the I2C interface
// with manually supplied callbacks — analogous to the spi-basic example.
//
// Normally you simply call rtc.begin(), which internally wires Arduino's Wire
// library. This example shows how to do the same wiring explicitly via init(),
// which is useful when you need a custom I2C implementation (e.g. software I2C,
// a different TwoWire bus, or a bare-metal environment without the Arduino HAL).
//
// The AM0805 and AM1805 are the I2C variants of the AM0815/AM1815 (SPI).
// Default I2C address: 0x69  (pin ADDR/nCE = open or tied to VCC)
//                      0x68  (pin ADDR/nCE = tied to GND)
//
// Datasheet reference: Section 5.1 I2C Interface

#define RTC_I2C_ADDRESS   0x69   // <-- adapt if ADDR pin is pulled low (0x68)

AM0805 rtc;
stc_amx8x5_time_t stcTimeToSet;
stc_amx8x5_time_t* pMyTime;

// ---------- I2C callbacks ----------

static int i2cWrite(void* pHandle, uint32_t u32Address, uint8_t u8Register,
                    uint8_t* pu8Data, uint32_t u32Len)
{
    // The AM08X5 I2C write: for each byte, send register address then data.
    // The driver already handles multi-byte writes by calling this for each byte
    // or in burst mode; the wire protocol sends address + register + data.
    for (uint32_t i = 0; i < u32Len; i++)
    {
        Wire.beginTransmission((uint8_t)u32Address);
        Wire.write(u8Register + i);   // register address (auto-incremented)
        Wire.write(pu8Data[i]);       // data byte
        Wire.endTransmission();
    }
    return 0;
}

static int i2cRead(void* pHandle, uint32_t u32Address, uint8_t u8Register,
                   uint8_t* pu8Data, uint32_t u32Len)
{
    for (uint32_t i = 0; i < u32Len; i++)
    {
        Wire.beginTransmission((uint8_t)u32Address);
        Wire.write(u8Register + i);   // register address
        Wire.endTransmission();
        Wire.requestFrom((uint8_t)u32Address, (uint8_t)1);
        pu8Data[i] = Wire.read();
    }
    return 0;
}

// -----------------------------------

void setup()
{
    stcTimeToSet.u8Hundredth = 0;
    stcTimeToSet.u8Second    = BUILD_SECOND;
    stcTimeToSet.u8Minute    = BUILD_MINUTE;
    stcTimeToSet.u8Hour      = BUILD_HOUR;
    stcTimeToSet.u8Date      = BUILD_DAY;
    stcTimeToSet.u8Month     = BUILD_MONTH;
    stcTimeToSet.u8Year      = BUILD_YEAR - 2000;
    stcTimeToSet.u8Weekday   = 0;
    stcTimeToSet.u8Century   = 1; // year is 20xx; 0 would be 19xx or 21xx

    Serial.begin(115200);

    Wire.begin();

    // Build the I2C handle manually and pass it via init()
    stc_amx8x5_handle_t stcHandle;
    memset(&stcHandle, 0, sizeof(stcHandle));
    stcHandle.enMode       = AMx8x5ModeI2C;
    stcHandle.enRtcType    = AMx8x5Type0805;
    stcHandle.pHandle      = NULL;
    stcHandle.u32Address   = RTC_I2C_ADDRESS;
    stcHandle.pfnWriteI2C  = i2cWrite;
    stcHandle.pfnReadI2C   = i2cRead;

    rtc.init(&stcHandle);

    // Verify communication (Amx8x5_Init reads and checks the ID registers)
    while (Amx8x5_Init((stc_amx8x5_handle_t*)&stcHandle) != Ok)
    {
        Serial.println("RTC not initialized via I2C...");
        delay(1000);
    }
    Serial.println("RTC initialized via I2C.");

    // Only set time if the RTC has not been set before (year == 0 after POR)
    rtc.getTime(&pMyTime);
    if (pMyTime->u8Year == 0)
    {
        rtc.setTime(&stcTimeToSet, false);
    }
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("\n%02d:%02d:%02d.%02d\n",
                  pMyTime->u8Hour, pMyTime->u8Minute,
                  pMyTime->u8Second, pMyTime->u8Hundredth);
    Serial.printf("20%02d-%02d-%02d  Weekday: %d\n",
                  pMyTime->u8Year, pMyTime->u8Month,
                  pMyTime->u8Date, pMyTime->u8Weekday);
    delay(1000);
}
