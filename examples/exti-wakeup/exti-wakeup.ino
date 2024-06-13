#include <amx8x5.h>
#include <Wire.h>

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
    stcTimeToSet.u8Weekday = 0;
    stcTimeToSet.u8Century = 1; //year is 20xx, 0 would be 19xx or 21xx

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
      rtc.setTime(&stcTimeToSet,false);
    }

    rtc.enableTrickleCharger(AMx8x5::enTrickleDiode::AMx8x5TrickleDiodeSchottky,AMx8x5::enTrickleResistor::AMx8x5TrickleResistor3K,true);
    rtc.enableIrqXt1OnExti(true);

    // use power switch to possibly power-off the whole system
    rtc.setPswHighCurrent(true);

    // unlock configuration
    rtc.writeByte(AMX8X5_REG_CONFIG_KEY, AMX8X5_CONFIG_KEY_VAL); //enable AMX8X5_REG_OSC_CONTROL register
    rtc.setRegister(AMX8X5_REG_OSC_CONTROL,AMX8X5_REG_OSC_CONTROL_PWGT_MSK); //disable I2C interface during sleep

    // use EXTI to wakeup the whole system
    rtc.enableIrqXt1OnExti(true);
    
    rtc.getTime(&pMyTime);

    // try to sleep / power-off the whole system
    en_result_t res = rtc.setSleepMode(0,AMx8x5::enSleepMode::AMx8x5PswIrq2HighInSleep);
    if (res != Ok)
    {
        Serial.printf("Error: Could not sleep... reason: %d\n",res);
        while(1);
    }
}

// loop is never reached if the RTC is also used to switch-off power
void loop(void)
{
  uint8_t u8Status;
  rtc.readByte(AMX8X5_REG_STATUS,&u8Status);
  if (u8Status == 0xFF)
  {
    Serial.println("RTC not initialized... maybe sleeping?");
    Serial.println("Please press one of the buttons SW1..SW6!");
    while(u8Status == 0xFF)
    {
      rtc.readByte(AMX8X5_REG_STATUS,&u8Status);
    }
  } else
  {
    rtc.getTime(&pMyTime);
    Serial.printf("\n%d:%d:%d.%d\n",pMyTime->u8Hour,pMyTime->u8Minute,pMyTime->u8Second,pMyTime->u8Hundredth);
    Serial.printf("20%d-%d-%d Weekday: %d\n",pMyTime->u8Year,pMyTime->u8Month,pMyTime->u8Date,pMyTime->u8Weekday);
    Serial.printf("Status: 0x%02x\n",u8Status);
    if (u8Status & AMX8X5_REG_STATUS_EX1_MSK)
    {
        Serial.println("Wakeup was generated!");
        rtc.clearRegister(AMX8X5_REG_STATUS,AMX8X5_REG_STATUS_EX1_MSK);
    }

  }

}

