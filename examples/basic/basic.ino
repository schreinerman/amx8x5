#include <amx8x5.h>
#include <Wire.h>


AM0805 rtc;
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
}

void loop(void)
{
    rtc.getTime(&pMyTime);
    Serial.printf("\n%d:%d:%d.%d\n",pMyTime->u8Hour,pMyTime->u8Minute,pMyTime->u8Second,pMyTime->u8Hundredth);
    Serial.printf("20%d-%d-%d Weekday: %d\n",pMyTime->u8Year,pMyTime->u8Month,pMyTime->u8Date,pMyTime->u8Weekday);
    delay(1000);
}

