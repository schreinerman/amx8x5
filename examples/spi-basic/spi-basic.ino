#include <amx8x5.h>
#include <SPI.h>

// This example demonstrates how to use the AM0815 / AM1815 via the SPI interface
// (3-wire or 4-wire, up to 2 MHz).
//
// The AM0815 and AM1815 are the SPI variants of the AM0805/AM1805.
// Pin assignment (adapt to your board):
//   SCK  -> SPI clock
//   SDO  -> SPI MISO (Master In Slave Out)
//   SDI  -> SPI MOSI (Master Out Slave In)
//   nCE  -> Chip Select (driven manually, connect to a digital output pin)
//
// The driver requires two callbacks: spiWrite and spiRead.
// These are wired to Arduino's SPI library below.
//
// Datasheet reference: Section 5.2 SPI Interface

#define RTC_CS_PIN    10   // <-- adapt to your board

AM0815 rtc;
stc_amx8x5_time_t stcTimeToSet;
stc_amx8x5_time_t* pMyTime;

// ---------- SPI callbacks ----------

static SPISettings spiSettings(2000000, MSBFIRST, SPI_MODE0);

static int spiWrite(void* pHandle, uint32_t u32ChipSelect, uint8_t u8Register,
                    uint8_t* pu8Data, uint32_t u32Len)
{
    // The AM08X5 SPI write: send register address with MSB clear (write flag = 0),
    // then data bytes.
    SPI.beginTransaction(spiSettings);
    digitalWrite((uint8_t)u32ChipSelect, LOW);
    SPI.transfer(u8Register & 0x7F);  // MSB=0 -> write
    for (uint32_t i = 0; i < u32Len; i++)
    {
        SPI.transfer(pu8Data[i]);
    }
    digitalWrite((uint8_t)u32ChipSelect, HIGH);
    SPI.endTransaction();
    return 0;
}

static int spiRead(void* pHandle, uint32_t u32ChipSelect, uint8_t u8Register,
                   uint8_t* pu8Data, uint32_t u32Len)
{
    // The AM08X5 SPI read: send register address with MSB set (read flag = 1),
    // then read data bytes.
    SPI.beginTransaction(spiSettings);
    digitalWrite((uint8_t)u32ChipSelect, LOW);
    SPI.transfer(u8Register | 0x80);  // MSB=1 -> read
    for (uint32_t i = 0; i < u32Len; i++)
    {
        pu8Data[i] = SPI.transfer(0x00);
    }
    digitalWrite((uint8_t)u32ChipSelect, HIGH);
    SPI.endTransaction();
    return 0;
}

// -----------------------------------

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

    Serial.begin(115200);

    // Chip select pin setup
    pinMode(RTC_CS_PIN, OUTPUT);
    digitalWrite(RTC_CS_PIN, HIGH);

    SPI.begin();

    // Build the SPI handle manually and pass it via init()
    stc_amx8x5_handle_t stcHandle;
    memset(&stcHandle, 0, sizeof(stcHandle));
    stcHandle.enMode        = AMx8x5ModeSPI;
    stcHandle.enRtcType     = AMx8x5Type0815;
    stcHandle.pHandle       = NULL;
    stcHandle.u32ChipSelect = (uint32_t)RTC_CS_PIN;
    stcHandle.pfnWriteSpi   = spiWrite;
    stcHandle.pfnReadSpi    = spiRead;

    rtc.init(&stcHandle);

    // Verify communication (Init reads ID registers)
    while (Amx8x5_Init((stc_amx8x5_handle_t*)&stcHandle) != Ok)
    {
        Serial.println("RTC not initialized via SPI...");
        delay(1000);
    }
    Serial.println("RTC initialized via SPI.");

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
