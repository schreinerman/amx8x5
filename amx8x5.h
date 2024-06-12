/******************************************************************************
 ** Created by Manuel Schreiner
 **
 ** Copyright © 2024 io-expert.com. All rights reserved.
 **
 ** 1. Redistributions of source code must retain the above copyright notice,
 **    this condition and the following disclaimer.
 **
 ** This software is provided by the copyright holder and contributors "AS IS"
 ** and any warranties related to this software are DISCLAIMED.
 ** The copyright owner or contributors be NOT LIABLE for any damages caused
 ** by use of this software.
 ******************************************************************************/
/******************************************************************************/
/** \file amx8x5.h
 **
 ** Driver for RTCs from 
 ** Ambiq 
 ** - AM0805
 ** - AM0815
 ** - AM0805
 ** - AM0815
 **
 ** Abracom
 ** - AB0805
 ** - AB0815
 ** - AB0805
 ** - AB0815
 **
 ** Microcrystal
 ** - RV1805
 **
 ** A detailed description is available at 
 ** @link AMx8x5Group AMx8x5 Driver description @endlink
 **
 ** History:
 **   - 2024-05-27  V1.0  Manuel Schreiner  First Version
 **
 *****************************************************************************/
#ifndef __AMX8X5_H__
#define __AMX8X5_H__

/* C binding of definitions if building with C++ compiler */
//#ifdef __cplusplus
//extern "C"
//{
//#endif
    
/**
 ******************************************************************************
 ** \defgroup AMx8x5Group AMx8x5 Driver
 **
 ** Provided functions of AMx8x5:
 **
 ** Basic functionality:
 ** - Amx8x5_Init()
 ** - Amx8x5_Reset()
 ** - Amx8x5_GetTime()
 ** - Amx8x5_SetTime()
 ** - Amx8x5_SetCalibrationValue()
 ** - Amx8x5_SetAlarm()
 ** - Amx8x5_Stop()
 **
 ** Advanced functionality:
 ** - Amx8x5_SetSleepMode()
 ** - Amx8x5_SetWatchdog()
 ** - Amx8x5_GetExtensionAddress()
 ** - Amx8x5_SetSquareWaveOutput()
 ** - Amx8x5_SelectOscillatorMode()
 ** - Amx8x5_SetCountdown()
 ** - Amx8x5_SetAutocalibration()
 ** - Amx8x5_EnableTrickleCharger() 
 ** - Amx8x5_SetBatteryReferenceVoltage()
 ** - Amx8x5_RamRead()
 ** - Amx8x5_RamWrite()
 ** - Amx8x5_CtrlOutB()
 ** - Amx8x5_CtrlOut()
 ** - Amx8x5_SetResetPolarity()
 ** - Amx8x5_AutoResetStatus()
 ** - Amx8x5_SetPswHighCurrent()
 ** - Amx8x5_UsenExtrAsReset()
 ** - Amx8x5_SetOut1Mode()
 ** - Amx8x5_SetOut2Mode()
 ** - Amx8x5_EnableIrqXt1OnExti()
 ** - Amx8x5_EnableIrqXt2OnWdi()
 ** - Amx8x5_EnableIrqAlarm()
 ** - Amx8x5_EnableIrqTimer()
 ** - Amx8x5_EnableIrqBatteryLow()
 ** - Amx8x5_EnableOutput_CLKOUT_nIRQ3()
 ** - Amx8x5_EnableOutput_FOUT_nIRQ()
 ** - Amx8x5_EnableOutput_nRST()
 ** - Amx8x5_EnableOutput_nTIRQ()
 ** 
 ** Provided direct register access functions (valid for I2C and SPI):
 ** - Amx8x5_ClearRegister() - clear bits in a register
 ** - Amx8x5_SetRegister() - set bits in register
 ** - Amx8x5_ReadByte() - read one byte
 ** - Amx8x5_ReadBytes() - read multiple bytes
 ** - Amx8x5_WriteByte() - write one byte
 ** - Amx8x5_WriteBytes() - read multiple bytes
 ** 
 **   
 ******************************************************************************/
//@{

/**
 ******************************************************************************
 ** \defgroup amx8x5_module_examples Code Examples
 ** 
 ** In this section different code examples can be found
 **/
///@{
    
/**
 ******************************************************************************    
 ** \page amx8x5_module_includes Example Implementation: Required includes in main application
 ** \brief Following includes are required
 ** @code   
 ** #include "amx8x5.h"   
 ** @endcode
 **
 ******************************************************************************/

/**
 ******************************************************************************    
 ** \page amx8x5_module_i2ctransfer Example Implementation: I2C transfer routines
 **
 ** Following includes are required
 ** @code   
 ** #include "amx8x5.h"   
 ** @endcode
 **
 ** For writing I2C data, following wrapper function has to be implemented:
 ** @code 
 ** static int I2CWrite(void* pHandle, uint32_t u32Address,    uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
 ** {
 **     int result = 0;
 **
 **     result = HAL_I2CWrite(
 **                   (PERIPHERAL_I2C*)pHandle, //pHandle is the peripheral pointer to the used I2C peripheral as defined in CMSIS
 **                   u32Address,               //u32Address defines the I2C device address
 **                   u8Register,               //u8Register defines the register to address
 **                   pu8Data,                  //pu8Data defines the data to write
 **                   u32Len                    //u32Len defines the length of data
 **                  );
 **
 **     return result;
 ** }
 ** @endcode
 **
 ** For reading I2C data, following wrapper function has to be implemented:
 ** @code 
 ** static int I2CRead(void* pHandle, uint32_t u32Address,    uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
 ** {
 **     int result = 0;
 **
 **     result = HAL_I2CRead(
 **                   (PERIPHERAL_I2C*)pHandle, //pHandle is the peripheral pointer to the used I2C peripheral as defined in CMSIS
 **                   u32Address,               //u32Address defines the I2C device address
 **                   u8Register,               //u8Register defines the register to address
 **                   pu8Data,                  //pu8Data defines the data to write
 **                   u32Len                    //u32Len defines the length of data
 **                  );
 **
 **     return result;
 ** }
 ** @endcode
 **
 ******************************************************************************/

/**
 ******************************************************************************    
 ** \page amx8x5_module_i2cinit Example Implementation: I2C Initialization
 **
 ** The initialization is done like the following example implementation (with use of the previous @link amx8x5_module_i2ctransfer I2C transfer routines @endlink)
 ** @code    
 ** const static stc_amx8x5_handle_t stcRtcConfig = {
 **                                                    AMx8x5ModeI2C,          //enMode specifies SPI or I2C mode: AMx8x5ModeSPI or AMx8x5ModeI2C
 **                                                    (void*)I2C0,            //pHandle is the peripheral pointer to the used I2C peripheral as defined in CMSIS  
 **                                                    AMx8x5Type1805,         //enRtcType defines the type of used RTC: #AMx8x5Type0805, #AMx8x5Type0815, #AMx8x5Type1805 or #AMx8x5Type1815
 **                                                    AMX8X5_I2C_ADDRESS,     //u32Address defines the I2C address
 **                                                    I2CWrite,               //pfnWriteI2C defines the I2C write procedure
 **                                                    I2CRead                 //pfnReadI2C defines the I2C read procedure
 **                                                 };
 ** int main(void)
 ** {
 **      Amx8x5_Init((stc_amx8x5_handle_t*)&stcRtcConfig);
 **      ...
 ** }     
 ** @endcode     
 ******************************************************************************/





/**
 ******************************************************************************    
 ** \page amx8x5_module_spitransfer Example Implementation: SPI transfer routines
 **
 ** Following includes are required
 ** @code   
 ** #include "amx8x5.h"   
 ** @endcode
 **
 ** For writing SPI data, following wrapper function has to be implemented:
 ** @code 
 ** static int SPIWrite(void* pHandle, uint32_t u32Chipselect, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
 ** {
 **     int result = 0;
 **     CS_LOW(u32Chipselect);
 **     result = HAL_SPIWrite(
 **                           (PERIPHERAL_SPI*)pHandle, //pHandle is the peripheral pointer to the used SPI peripheral as defined in CMSIS
 **                           &u8Register,               //u8Register defines the register to address
 **                           1                          //datalen = 1
 **                          );
 **
 **     if (result != 0) return result;
 **
 **     result = HAL_SPIWrite(
 **                           (PERIPHERAL_SPI*)pHandle, //pHandle is the peripheral pointer to the used SPI peripheral as defined in CMSIS
 **                           pu8Data,                  //pu8Data defines the data to write
 **                           u32Len                    //u32Len defines the length of data
 **                  );
 **
 **     CS_HIGH(u32Chipselect);
 **     return result;
 ** }
 ** @endcode
 **
 ** For reading SPI data, following wrapper function has to be implemented:
 ** @code 
 ** static int SPIRead(void* pHandle, uint32_t u32Chipselect, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
 ** {
 **     int result = 0;
 **     CS_LOW(u32Chipselect);
 **     result = HAL_SPIWrite(
 **                           (PERIPHERAL_SPI*)pHandle, //pHandle is the peripheral pointer to the used SPI peripheral as defined in CMSIS
 **                           &u8Register,               //u8Register defines the register to address
 **                           1                          //datalen = 1
 **                          );
 **
 **     if (result != 0) return result;
 **
 **     result = HAL_SPIRead(
 **                           (PERIPHERAL_SPI*)pHandle, //pHandle is the peripheral pointer to the used SPI peripheral as defined in CMSIS
 **                           pu8Data,                  //pu8Data defines the data to write
 **                           u32Len                    //u32Len defines the length of data
 **                  );
 **
 **     CS_HIGH(u32Chipselect);
 **     return result;
 ** }
 ** @endcode
 **
 ******************************************************************************/

/**
 ******************************************************************************    
 ** \page amx8x5_module_spiinit Example Implementation: SPI Initialization
 **
 ** The initialization is done like the following example implementation (with use of the previous @link amx8x5_module_spitransfer SPI transfer routines @endlink)
 ** @code    
 ** const static stc_amx8x5_handle_t stcRtcConfig = {
 **                                                    AMx8x5ModeSPI,          //enMode specifies SPI or I2C mode: AMx8x5ModeSPI or AMx8x5ModeI2C
 **                                                    (void*)SPI0,            //pHandle is the peripheral pointer to the used I2C peripheral as defined in CMSIS  
 **                                                    AMx8x5Type1815,         //enRtcType defines the type of used RTC: #AMx8x5Type0805, #AMx8x5Type0815, #AMx8x5Type1805 or #AMx8x5Type1815
 **                                                    AMX8X5_I2C_ADDRESS,     //u32Address defines the I2C address
 **                                                    I2CWrite,               //pfnWriteI2C defines the I2C write procedure
 **                                                    I2CRead                 //pfnReadI2C defines the I2C read procedure
 **                                                 };
 ** int main(void)
 ** {
 **      Amx8x5_Init((stc_amx8x5_handle_t*)&stcRtcConfig);
 **      ...
 ** }     
 ** @endcode     
 ******************************************************************************/
///@} //amx8x5_module_examples
    
/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

/*****************************************************************************/
/* Global pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/
    
#define AMX8X5_DEBUG                         0

// see https://stackoverflow.com/questions/11697820/how-to-use-date-and-time-predefined-macros-in-as-two-integers-then-stri

#define BUILD_YEAR_CH0 (__DATE__[ 7])
#define BUILD_YEAR_CH1 (__DATE__[ 8])
#define BUILD_YEAR_CH2 (__DATE__[ 9])
#define BUILD_YEAR_CH3 (__DATE__[10])

#define BUILD_MONTH_IS_JAN (__DATE__[0] == 'J' && __DATE__[1] == 'a' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_FEB (__DATE__[0] == 'F')
#define BUILD_MONTH_IS_MAR (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'r')
#define BUILD_MONTH_IS_APR (__DATE__[0] == 'A' && __DATE__[1] == 'p')
#define BUILD_MONTH_IS_MAY (__DATE__[0] == 'M' && __DATE__[1] == 'a' && __DATE__[2] == 'y')
#define BUILD_MONTH_IS_JUN (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'n')
#define BUILD_MONTH_IS_JUL (__DATE__[0] == 'J' && __DATE__[1] == 'u' && __DATE__[2] == 'l')
#define BUILD_MONTH_IS_AUG (__DATE__[0] == 'A' && __DATE__[1] == 'u')
#define BUILD_MONTH_IS_SEP (__DATE__[0] == 'S')
#define BUILD_MONTH_IS_OCT (__DATE__[0] == 'O')
#define BUILD_MONTH_IS_NOV (__DATE__[0] == 'N')
#define BUILD_MONTH_IS_DEC (__DATE__[0] == 'D')

#define BUILD_MONTH_CH0 \
    ((BUILD_MONTH_IS_OCT || BUILD_MONTH_IS_NOV || BUILD_MONTH_IS_DEC) ? '1' : '0')

#define BUILD_MONTH_CH1 \
    ( \
        (BUILD_MONTH_IS_JAN) ? '1' : \
        (BUILD_MONTH_IS_FEB) ? '2' : \
        (BUILD_MONTH_IS_MAR) ? '3' : \
        (BUILD_MONTH_IS_APR) ? '4' : \
        (BUILD_MONTH_IS_MAY) ? '5' : \
        (BUILD_MONTH_IS_JUN) ? '6' : \
        (BUILD_MONTH_IS_JUL) ? '7' : \
        (BUILD_MONTH_IS_AUG) ? '8' : \
        (BUILD_MONTH_IS_SEP) ? '9' : \
        (BUILD_MONTH_IS_OCT) ? '0' : \
        (BUILD_MONTH_IS_NOV) ? '1' : \
        (BUILD_MONTH_IS_DEC) ? '2' : \
        /* error default */    '?' \
    )

#define BUILD_DAY_CH0 ((__DATE__[4] >= '0') ? (__DATE__[4]) : '0')
#define BUILD_DAY_CH1 (__DATE__[ 5])

#define BUILD_HOUR_CH0 (__TIME__[0])
#define BUILD_HOUR_CH1 (__TIME__[1])

#define BUILD_MIN_CH0 (__TIME__[3])
#define BUILD_MIN_CH1 (__TIME__[4])

#define BUILD_SEC_CH0 (__TIME__[6])
#define BUILD_SEC_CH1 (__TIME__[7])

#define BUILD_SECOND ((BUILD_SEC_CH0-'0') * 10 + (BUILD_SEC_CH1-'0'))
#define BUILD_HOUR ((BUILD_HOUR_CH0-'0') * 10 + (BUILD_HOUR_CH1-'0'))
#define BUILD_MINUTE ((BUILD_MIN_CH0-'0') * 10 +(BUILD_MIN_CH1-'0'))
#define BUILD_YEAR ((BUILD_YEAR_CH0-'0')*1000+(BUILD_YEAR_CH1-'0')*100+(BUILD_YEAR_CH2-'0')*10+(BUILD_YEAR_CH3-'0'))
#define BUILD_MONTH ((BUILD_MONTH_CH0-'0') * 10 +(BUILD_MONTH_CH1-'0'))
#define BUILD_DAY ((BUILD_DAY_CH0-'0') * 10 + (BUILD_DAY_CH1-'0'))

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions Register Definitions
 ** 
 ** \brief Registers are accessed by selecting a register address and then performing read or write operations.
 ** Multiple reads or writes may be executed in a single access, with the address automatically incrementing
 ** after each byte. Table 18 and Table 19 summarize the function of each register. In Table 18, the GPx bits
 ** (where x is between 0 and 27) are 28 register bits which may be used as general purpose storage. These
 ** bits are described in the sections below. All of the GPx bits are cleared when the AM18X5 powers up and
 ** they can therefore be used to allow software to determine if a true Power On Reset has occurred or hold
 ** other initialization data.
 **
 ** Following Registers are defined in this section:
 **
 ** | Offset | Register         | Define / More Information |
 ** |--------|------------------|---------------------------|
 ** | 0x00   | Hundredths       | #AMX8X5_REG_HUNDREDTHS    |
 ** | 0x01   | Seconds          | #AMX8X5_REG_SECONDS       |
 ** | 0x02   | Minutes          | #AMX8X5_REG_MINUTES       |
 ** | 0x03   | Hours            | #AMX8X5_REG_HOURS         |
 ** | 0x04   | Date             | #AMX8X5_REG_DATE          |
 ** | 0x05   | Months           | #AMX8X5_REG_MONTH         |
 ** | 0x06   | Years            | #AMX8X5_REG_YEARS         |
 ** | 0x07   | Weekdays         | #AMX8X5_REG_WEEKDAY       |
 ** | 0x08   | Hundredths Alarm | #AMX8X5_REG_ALARM_HUNDRS  |
 ** | 0x09   | Seconds Alarm    | #AMX8X5_REG_ALARM_SECONDS |
 ** | 0x0A   | Minutes Alarm    | #AMX8X5_REG_ALARM_MINUTES |
 ** | 0x0B   | Hours Alarm      | #AMX8X5_REG_ALARM_HOURS   |
 ** | 0x0C   | Date Alarm       | #AMX8X5_REG_ALARM_DATE    |
 ** | 0x0D   | Months Alarm     | #AMX8X5_REG_ALARM_MONTH   |
 ** | 0x0E   | Weekdays Alarm   | #AMX8X5_REG_ALARM_WEEKDAY |
 ** | 0x0F   | Status           | #AMX8X5_REG_STATUS        |
 ** | 0x10   | Control1         | #AMX8X5_REG_CONTROL_1     |
 ** | 0x11   | Control2         | #AMX8X5_REG_CONTROL_2     |
 ** | 0x12   | IntMask          | #AMX8X5_REG_INT_MASK      |
 ** | 0x13   | SQW              | #AMX8X5_REG_SQW           |
 ** | 0x14   | Cal_XT           | #AMX8X5_REG_CAL_XT        |
 ** | 0x15   | Cal_RC_Hi        | #AMX8X5_REG_CAL_RC_HI     |
 ** | 0x16   | Cal_RC_Low       | #AMX8X5_REG_CAL_RC_LOW    |
 ** | 0x17   | Sleep_Control    | #AMX8X5_REG_SLEEP_CTRL    |
 ** | 0x18   | Timer Control    | #AMX8X5_REG_TIMER_CTRL    |
 ** | 0x19   | Timer            | #AMX8X5_REG_TIMER         |
 ** | 0x1A   | Timer_Initial    | #AMX8X5_REG_TIMER_INITIAL |
 ** | 0x1B   | WDT              | #AMX8X5_REG_WDT           |
 ** | 0x1C   | Osc. Control     | #AMX8X5_REG_OSC_CONTROL   |
 ** | 0x1D   | Osc. Status      | #AMX8X5_REG_OSC_STATUS    |
 ** | 0x1E   | RESERVED         |                           |
 ** | 0x1F   | Configuration Key| #AMX8X5_REG_CONFIG_KEY    |
 ** | 0x20   | Trickle          | #AMX8X5_REG_TRICKLE       |
 ** | 0x21   | BREF Control     | #AMX8X5_REG_BREF_CTRL     |
 ** | 0x22   | RESERVED         |                           |
 ** | 0x23   | RESERVED         |                           |
 ** | 0x24   | RESERVED         |                           |
 ** | 0x25   | RESERVED         |                           |
 ** | 0x26   | AFCTRL           | #AMX8X5_REG_ACAL_FLT      |
 ** | 0x27   | BATMODE I/O      | #AMX8X5_REG_BATMODE_IO    |
 ** | 0x28   | ID0              | #AMX8X5_REG_ID0           |
 ** | 0x29   | ID1              | #AMX8X5_REG_ID1           |
 ** | 0x2A   | ID2              | #AMX8X5_REG_ID2           |
 ** | 0x2B   | ID3              | #AMX8X5_REG_ID3           |
 ** | 0x2C   | ID4              | #AMX8X5_REG_ID4           |
 ** | 0x2D   | ID5              | #AMX8X5_REG_ID5           |
 ** | 0x2E   | ID6              | #AMX8X5_REG_ID6           |
 ** | 0x2F   | ASTAT            | #AMX8X5_REG_ASTAT         |
 ** | 0x30   | OCTRL            | #AMX8X5_REG_OCTRL         |
 ** | 0x3F   | Extension Address| #AMX8X5_REG_EXTENDED_ADDR |
 ** | 0x40-7F | RAM             | #AMX8X5_REG_RAM           |
 ** | 0x80-FF | RAM             | #AMX8X5_REG_ALT_RAM       |
 **/   
 //@{
    
/**
 *****************************************************************************
 **
 ** \defgroup amx8x5_register_definitions00 Register 0x00 - Hundredths (AMX8X5_REG_HUNDREDTHS)
 ** 
 ** This register holds the count of hundredths of seconds, in two binary coded decimal (BCD) digits. Values
 ** will be from 00 to 99. Note that in order to divide from 32.786 kHz, the hundredths register will not be fully
 ** accurate at all times but will be correct every 500 ms. Maximum jitter of this register will be less than 1 ms.
 ** The Hundredths Counter is not valid if the 128 Hz RC Oscillator is selected.
 **/
 //@{    
/**
 ** \brief Register 0x00 - Hundredths (AMX8X5_REG_HUNDREDTHS)
 ** This register holds the count of hundredths of seconds, in two binary coded decimal (BCD) digits. Values
 ** will be from 00 to 99. Note that in order to divide from 32.786 kHz, the hundredths register will not be fully
 ** accurate at all times but will be correct every 500 ms. Maximum jitter of this register will be less than 1 ms.
 ** The Hundredths Counter is not valid if the 128 Hz RC Oscillator is selected.     
 **
 ** Hundredths Register
 ** -------------------
 **
 ** |   Bits      |       [7:4]       |         [3:0]         |
 ** |-------------|-------------------|-----------------------|
 ** | Description |  Seconds - Tenths | Seconds - Hundredths  |
 **
 ** Hundredths Register Bits
 ** ------------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** | 7:4 | Seconds - Tenths | Holds the tenths place in the hundredths counter.      |
 ** | 3:0 | Seconds - Tenths | Holds the hundredths place in the hundredths counter.  |
 **
 **/
#define AMX8X5_REG_HUNDREDTHS                0x00 
//@}
    
/**
 *****************************************************************************
 ** \ingroup amx8x5_register_definitions
 **
 ** \defgroup amx8x5_register_definitions01 Register 0x01 - Seconds (AMX8X5_REG_SECONDS)
 ** 
 ** This register holds the count of seconds, in two binary coded decimal (BCD) digits. Values will be from 00 to 59.
 **/
 //@{
    
/**
 ** \brief Register 0x01 - Seconds 
 **
 ** This register holds the count of seconds, in two binary coded decimal (BCD) digits. Values will be from 00 to 59.
 **
 ** Seconds Register
 ** ----------------
 **
 ** |   Bits      |  7   |       [6:4]      |         [3:0]         |
 ** |-------------|------|------------------|-----------------------|
 ** | Description | GP0  | Seconds - Tens   | Seconds - Ones        |
 **
 ** Seconds Register Bits
 ** ---------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP0        | Register bit for general purpose use.                  |
 ** | 6:4 | Seconds - Tens   | Holds the tens place in the seconds counter.           |
 ** | 3:0 | Seconds - Ones   | Holds the ones place in the seconds counter.           |
 **
 **/
#define AMX8X5_REG_SECONDS                   0x01

#define AMX8X5_REG_SECONDS_GP0_POS           (7)                               ///<see #AMX8X5_REG_SECONDS for more information
#define AMX8X5_REG_SECONDS_GP0_MSK           (1 << AMX8X5_REG_SECONDS_GP0_POS) ///<see #AMX8X5_REG_SECONDS for more information

//@}

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions02 Register 0x02 - Minutes (AMX8X5_REG_MINUTES)
 ** 
 ** This register holds the count of minutesin two binary coded decimal (BCD) digits. Values will be from 00 to 59.
 **/
 //@{
    
/**
 ** \brief Register 0x02 - Minutes
 **
 ** This register holds the count of minutesin two binary coded decimal (BCD) digits. Values will be from 00 to 59.
 **
 ** Minutes Register
 ** ----------------
 **
 ** |   Bits      |  7   |       [6:4]      |         [3:0]         |
 ** |-------------|------|------------------|-----------------------|
 ** | Description | GP1  | Minutes - Tens   | Minutes - Ones        |
 **
 ** Minutes Register Bits
 ** ---------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP1        | Register bit for general purpose use.                  |
 ** | 6:4 | Minutes - Tens   | Holds the tens place in the minutes counter.           |
 ** | 3:0 | Minutes - Ones   | Holds the ones place in the minutes counter.           |
 **
 **/
#define AMX8X5_REG_MINUTES                   0x02 
    
#define AMX8X5_REG_MINUTES_GP1_POS           (7)                                 ///<see #AMX8X5_REG_MINUTES for more information
#define AMX8X5_REG_MINUTES_GP1_MSK           (1 << AMX8X5_REG_MINUTES_GP1_POS)   ///<see #AMX8X5_REG_MINUTES for more information
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions03 Register 0x03 - Hours (AMX8X5_REG_HOURS)
 **
 ** This register holds the count of hours, in two binary coded decimal (BCD) digits. Values will be from 00 to
 ** 23 if the 12/24 bit (see 0x10 - Control1) is clear. If the 12/24 bit is set, the AM/PM bit will be 0 for AM hours
 ** and 1 for PM hours, and hour values will range from 1 to 12.
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x03 - Hours
 **
 ** This register holds the count of hours, in two binary coded decimal (BCD) digits. Values will be from 00 to
 ** 23 if the 12/24 bit (see 0x10 - Control1) is clear. If the 12/24 bit is set, the AM/PM bit will be 0 for AM hours
 ** and 1 for PM hours, and hour values will range from 1 to 12.
 **
 ** 12 Hour Mode
 ** ============
 **
 ** Hours Register
 ** --------------
 **
 ** |   Bits      |  7   |   6  |   5   |    4          |       [3:0]        |
 ** |-------------|------|------|-------|---------------|--------------------|
 ** | Description | GP3  |  GP2 | AM/PM | Hours - Tens  | Hours - Ones       |
 **
 ** Hours Register Bits
 ** -------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP3        | Register bit for general purpose use.                  |
 ** |  6  |       GP2        | Register bit for general purpose use.                  |
 ** |  5  |      AM/PM       | 0 = AM hours. 1 = PM hours.                            |
 ** |  4  | Hours - Tens     | Holds the tens place in the hours counter.             |
 ** | 3:0 | Hours - Hours    | Holds the ones place in the hours counter.             |
 **
 ** 24 Hour Mode
 ** ============
 **
 ** Hours Register
 ** --------------
 **
 ** |   Bits      |  7   |   6  |  [5:4]        |       [3:0]        |
 ** |-------------|------|------|---------------|--------------------|
 ** | Description | GP3  |  GP2 | Hours - Tens  | Hours - Ones       |
 **
 ** Hours Register Bits
 ** -------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP3        | Register bit for general purpose use.                  |
 ** |  6  |       GP2        | Register bit for general purpose use.                  |
 ** | 5:4 | Hours - Tens     | Holds the tens place in the hours counter.             |
 ** | 3:0 | Hours - Hours    | Holds the ones place in the hours counter.             |
 **
 **/ 
#define AMX8X5_REG_HOURS                     0x03
    
#define AMX8X5_REG_HOURS_GP3_POS             (7)                               ///<see #AMX8X5_REG_HOURS for more information
#define AMX8X5_REG_HOURS_GP3_MSK             (1 << AMX8X5_REG_HOURS_GP3_POS)   ///<see #AMX8X5_REG_HOURS for more information
#define AMX8X5_REG_HOURS_GP2_POS             (6)                               ///<see #AMX8X5_REG_HOURS for more information
#define AMX8X5_REG_HOURS_GP2_MSK             (1 << AMX8X5_REG_HOURS_GP2_POS)   ///<see #AMX8X5_REG_HOURS for more information
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions04 Register 0x04 - Date (AMX8X5_REG_DATE)
 ** 
 ** This register holds the current day of the month, in two binary coded decimal (BCD) digits. Values will
 ** range from 01 to 31. Leap years are correctly handled from 1900 to 2199.
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x04 - Date
 **
 ** This register holds the current day of the month, in two binary coded decimal (BCD) digits. Values will
 ** range from 01 to 31. Leap years are correctly handled from 1900 to 2199.
 **
 ** Date Register
 ** -------------
 **
 ** |   Bits      |  7   |   6  |  [5:4]        |       [3:0]        |
 ** |-------------|------|------|---------------|--------------------|
 ** | Description | GP5  |  GP4 | Date - Tens   | Date - Ones        |
 **
 ** Date Register Bits
 ** ------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP5        | Register bit for general purpose use.                  |
 ** |  6  |       GP4        | Register bit for general purpose use.                  |
 ** | 5:4 | Date - Tens      | Holds the tens place in the date counter.              |
 ** | 3:0 | Date - Hours     | Holds the ones place in the date counter.              |
 **
 **/ 
#define AMX8X5_REG_DATE                      0x04
    
#define AMX8X5_REG_DATE_GP5_POS              (7)                               ///<see #AMX8X5_REG_DATE for more information
#define AMX8X5_REG_DATE_GP5_MSK              (1 << AMX8X5_REG_DATE_GP5_POS)    ///<see #AMX8X5_REG_DATE for more information
#define AMX8X5_REG_DATE_GP4_POS              (6)                               ///<see #AMX8X5_REG_DATE for more information
#define AMX8X5_REG_DATE_GP4_MSK              (1 << AMX8X5_REG_DATE_GP4_POS)    ///<see #AMX8X5_REG_DATE for more information
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions05 Register 0x05 - Months (AMX8X5_REG_MONTH)
 ** 
 ** This register holds the current day of the month, in two binary coded decimal (BCD) digits. Values will
 ** range from 01 to 31. Leap years are correctly handled from 1900 to 2199.
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x05 - Months
 **
 ** This register holds the current day of the month, in two binary coded decimal (BCD) digits. Values will
 ** range from 01 to 31. Leap years are correctly handled from 1900 to 2199.
 **
 ** Months Register
 ** ---------------
 **
 ** |   Bits      |  7   |   6  |  5   |   4            |       [3:0]        |
 ** |-------------|------|------|------|----------------|--------------------|
 ** | Description | GP8  |  GP7 |  GP6 | Months - Tens  | Months - Ones      |
 **
 ** Months Register Bits
 ** --------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP8        | Register bit for general purpose use.                  |
 ** |  6  |       GP7        | Register bit for general purpose use.                  |
 ** |  5  |       GP6        | Register bit for general purpose use.                  |
 ** | 5:4 | Months - Tens    | Holds the tens place in the months counter.            |
 ** | 3:0 | Months - Hours   | Holds the ones place in the months counter.            |
 **
 **/ 
#define AMX8X5_REG_MONTH                     0x05
    
#define AMX8X5_REG_MONTH_GP8_POS              (7)                               ///<see #AMX8X5_REG_MONTH  for more information
#define AMX8X5_REG_MONTH_GP8_MSK              (1 << AMX8X5_REG_MONTH_GP8_POS)   ///<see #AMX8X5_REG_MONTH  for more information
#define AMX8X5_REG_MONTH_GP7_POS              (6)                               ///<see #AMX8X5_REG_MONTH  for more information
#define AMX8X5_REG_MONTH_GP7_MSK              (1 << AMX8X5_REG_MONTH_GP7_POS)   ///<see #AMX8X5_REG_MONTH  for more information
#define AMX8X5_REG_MONTH_GP6_POS              (5)                               ///<see #AMX8X5_REG_MONTH  for more information
#define AMX8X5_REG_MONTH_GP6_MSK              (1 << AMX8X5_REG_MONTH_GP6_POS)   ///<see #AMX8X5_REG_MONTH  for more information
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions06 Register 0x06 - Years  (AMX8X5_REG_YEARS)
 ** 
 ** This register holds the current year, in two binary coded decimal (BCD) digits. Values will range from 00 to 99.   
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x06 - Years 
 ** 
 ** This register holds the current year, in two binary coded decimal (BCD) digits. Values will range from 00 to 99.   
 **
 ** Years Register
 ** --------------
 **
 ** |   Bits      |       [7:4]       |         [3:0]         |
 ** |-------------|-------------------|-----------------------|
 ** | Description |  Years - Tens     | Years - Ones          |
 **
 ** Years Register Bits
 ** -------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** | 3:0 | Years - Tens     | Holds the tens place in the years counter.             |
 ** | 3:0 | Years - Ones     | Holds the ones place in the years counter.             |
 **
 **/
#define AMX8X5_REG_YEARS                     0x06
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions07 Register 0x07 - Weekday (AMX8X5_REG_WEEKDAY)
 ** 
 ** This register holds the current day of the week. Values will range from 0 to 6.
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x07 - Weekday
 ** 
 ** This register holds the current day of the week. Values will range from 0 to 6. 
 **
 ** Weekdays Register
 ** -----------------
 **
 ** |   Bits      |  7   |  6   |  5   |  4   |  3   |         [2:0]         |
 ** |-------------|------|------|------|------|------|-----------------------|
 ** | Description | GP13 | GP12 | GP11 | GP10 | GP9  |     Weekdays          |
 **
 ** Weekdays Register Bits
 ** ----------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP13       | Register bit for general purpose use.                  |
 ** |  6  |       GP12       | Register bit for general purpose use.                  |
 ** |  5  |       GP11       | Register bit for general purpose use.                  |
 ** |  4  |       GP10       | Register bit for general purpose use.                  |
 ** |  3  |       GP9        | Register bit for general purpose use.                  |
 ** | 2:0 |     Weekdays     | Holds the weekday counter value.                       |
 **
 **/
#define AMX8X5_REG_WEEKDAY                   0x07

#define AMX8X5_REG_WEEKDAY_GP13_POS              (7)                                ///<see #AMX8X5_REG_WEEKDAY  for more information
#define AMX8X5_REG_WEEKDAY_GP13_MSK              (1 << AMX8X5_REG_WEEKDAY_GP13_POS) ///<see #AMX8X5_REG_WEEKDAY  for more information
#define AMX8X5_REG_WEEKDAY_GP12_POS              (6)                                ///<see #AMX8X5_REG_WEEKDAY  for more information
#define AMX8X5_REG_WEEKDAY_GP12_MSK              (1 << AMX8X5_REG_WEEKDAY_GP12_POS) ///<see #AMX8X5_REG_WEEKDAY  for more information
#define AMX8X5_REG_WEEKDAY_GP11_POS              (5)                                ///<see #AMX8X5_REG_WEEKDAY  for more information
#define AMX8X5_REG_WEEKDAY_GP11_MSK              (1 << AMX8X5_REG_WEEKDAY_GP11_POS) ///<see #AMX8X5_REG_WEEKDAY  for more information
#define AMX8X5_REG_WEEKDAY_GP10_POS              (4)                                ///<see #AMX8X5_REG_WEEKDAY  for more information
#define AMX8X5_REG_WEEKDAY_GP10_MSK              (1 << AMX8X5_REG_WEEKDAY_GP9_POS)  ///<see #AMX8X5_REG_WEEKDAY  for more information
#define AMX8X5_REG_WEEKDAY_GP9_POS               (3)                                ///<see #AMX8X5_REG_WEEKDAY  for more information
#define AMX8X5_REG_WEEKDAY_GP9_MSK               (1 << AMX8X5_REG_WEEKDAY_GP9_POS)  ///<see #AMX8X5_REG_WEEKDAY  for more information
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions08 Register 0x08 - Hundredths Alarm (AMX8X5_REG_ALARM_HUNDRS)
 ** 
 ** This register holds the alarm value for hundredths of seconds, in two binary coded decimal (BCD) digits.
 ** Values will range from 00 to 99.    
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x08 - Hundredths Alarm
 ** 
 ** This register holds the alarm value for hundredths of seconds, in two binary coded decimal (BCD) digits.
 ** Values will range from 00 to 99.     
 **
 ** Hundredths Alarm Register
 ** -------------------------
 **
 ** |   Bits      |       [7:4]             |         [3:0]               |
 ** |-------------|-------------------------|-----------------------------|
 ** | Description |  Seconds Alarm - Tenths | Seconds Alarm - Hundredths  |
 **
 ** Hundredths Alarm Register Bits
 ** ------------------------------
 **
 ** | Bit |       Name                 | Function                                              |
 ** |-----|----------------------------|-------------------------------------------------------|
 ** | 7:4 | Seconds Alarm - Tenths     | Holds the tenths place for the hundredths alarm.      |
 ** | 3:0 | Seconds Alarm - Hundredths | Holds the hundredths place for the hundredths alarm.  |
 **
 **/
#define AMX8X5_REG_ALARM_HUNDRS              0x08
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions09 Register 0x09 - Seconds Alarm (AMX8X5_REG_ALARM_SECONDS)
 ** 
 ** This register holds the alarm value for seconds, in two binary coded decimal (BCD) digits. Values will
 ** range from 00 to 59.
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x09 - Seconds Alarm
 **
 ** This register holds the alarm value for seconds, in two binary coded decimal (BCD) digits. Values will
 ** range from 00 to 59.
 **
 ** Seconds Alarm Register
 ** ----------------------
 **
 ** |   Bits      |  7   |       [6:4]            |         [3:0]         |
 ** |-------------|------|------------------------|-----------------------|
 ** | Description | GP14 | Seconds Alarm - Tens   | Seconds Alarm - Ones  |
 **
 ** Seconds Alarm Register Bits
 ** ---------------------------
 **
 ** | Bit |       Name             | Function                                               |
 ** |-----|------------------------|--------------------------------------------------------|
 ** |  7  |       GP14             | Register bit for general purpose use.                  |
 ** | 6:4 | Seconds Alarm - Tens   | Holds the tens place for the seconds alarm.            |
 ** | 3:0 | Seconds Alarm - Ones   | Holds the ones place for the seconds alarm.            |
 **
 **/
#define AMX8X5_REG_ALARM_SECONDS             0x09
    
#define AMX8X5_REG_ALARM_SECONDS_GP14_POS              (7)                                      ///<see #AMX8X5_REG_ALARM_SECONDS  for more information
#define AMX8X5_REG_ALARM_SECONDS_GP14_MSK              (1 << AMX8X5_REG_ALARM_SECONDS_GP14_POS) ///<see #AMX8X5_REG_ALARM_SECONDS  for more information
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions0A Register 0x0A - Minutes Alarm (AMX8X5_REG_ALARM_MINUTES)
 ** 
 ** This register holds the alarm value for minutes, in two binary coded decimal (BCD) digits. Values will range
 ** from 00 to 59.
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x0A - Minutes Alarm
 **
 ** This register holds the alarm value for minutes, in two binary coded decimal (BCD) digits. Values will range
 ** from 00 to 59.
 **
 ** Minutes Alarm Register
 ** ----------------------
 **
 ** |   Bits      |  7   |       [6:4]          |         [3:0]         |
 ** |-------------|------|----------------------|-----------------------|
 ** | Description | GP15 | Minutes Alarm - Tens | Minutes Alarm - Ones  |
 **
 ** Minutes Alarm Register Bits
 ** ---------------------------
 **
 ** | Bit |       Name             | Function                                               |
 ** |-----|------------------------|--------------------------------------------------------|
 ** |  7  |       GP15             | Register bit for general purpose use.                  |
 ** | 6:4 | Minutes Alarm - Tens   | Holds the tens place for the minutes alarm.            |
 ** | 3:0 | Minutes Alarm - Ones   | Holds the ones place for the minutes alarm.            |
 **
 **/
#define AMX8X5_REG_ALARM_MINUTES             0x0A  

#define AMX8X5_REG_ALARM_MINUTES_GP15_POS              (7)                                      ///<see #AMX8X5_REG_ALARM_MINUTES  for more information
#define AMX8X5_REG_ALARM_MINUTES_GP15_MSK              (1 << AMX8X5_REG_ALARM_MINUTES_GP15_POS) ///<see #AMX8X5_REG_ALARM_MINUTES  for more information
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions0B Register 0x0B - Hours Alarm (AMX8X5_REG_ALARM_HOURS)
 ** 
 ** This register holds the alarm value for hours, in two binary coded decimal (BCD) digits. Values will range
 ** from 00 to 23 if the 12/24 bit (see 0x10 - Control1) is clear. If the 12/24 bit is set, the AM/PM bit will be 0 for
 ** AM hours and 1 for PM hours, and hour values will be from 1 to 12.
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x0B - Hours Alarm
 **
 ** This register holds the alarm value for hours, in two binary coded decimal (BCD) digits. Values will range
 ** from 00 to 23 if the 12/24 bit (see 0x10 - Control1) is clear. If the 12/24 bit is set, the AM/PM bit will be 0 for
 ** AM hours and 1 for PM hours, and hour values will be from 1 to 12.
 **
 ** 12 Hour Mode
 ** ============
 **
 ** Hours Alarm Register
 ** --------------------
 **
 ** |   Bits      |  7   |   6   |   5   |    4                |       [3:0]         |
 ** |-------------|------|-------|-------|---------------------|---------------------|
 ** | Description | GP17 |  GP16 | AM/PM | Hours Alarm - Tens  | Hours Alarm - Ones  |
 **
 ** Hours Alarm Register Bits
 ** -------------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP17       | Register bit for general purpose use.                  |
 ** |  6  |       GP16       | Register bit for general purpose use.                  |
 ** |  5  |      AM/PM       | 0 = AM hours. 1 = PM hours.                            |
 ** |  4  | Hours - Tens     | Holds the tens place for the hours alarm.              |
 ** | 3:0 | Hours - Hours    | Holds the ones place for the hours alarm.              |
 **
 ** 24 Hour Mode
 ** ============
 **
 ** Hours Alarm Register
 ** --------------
 **
 ** |   Bits      |  7   |   6  |  [5:4]              |       [3:0]              |
 ** |-------------|------|------|---------------------|--------------------------|
 ** | Description | GP17 | GP16 | Hours Alarm - Tens  | Hours Alarm - Ones       |
 **
 ** Hours Alarm Register Bits
 ** -------------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP17       | Register bit for general purpose use.                  |
 ** |  6  |       GP16       | Register bit for general purpose use.                  |
 ** | 5:4 | Hours - Tens     | Holds the tens place for the hours alarm.              |
 ** | 3:0 | Hours - Hours    | Holds the ones place for the hours alarm.              |
 **
 **/   
#define AMX8X5_REG_ALARM_HOURS               0x0B
    
#define AMX8X5_REG_ALARM_HOURS_GP17_POS              (7)                                    ///<see #AMX8X5_REG_ALARM_HOURS  for more information
#define AMX8X5_REG_ALARM_HOURS_GP17_MSK              (1 << AMX8X5_REG_ALARM_HOURS_GP17_POS) ///<see #AMX8X5_REG_ALARM_HOURS  for more information
#define AMX8X5_REG_ALARM_HOURS_GP16_POS              (6)                                    ///<see #AMX8X5_REG_ALARM_HOURS  for more information
#define AMX8X5_REG_ALARM_HOURS_GP16_MSK              (1 << AMX8X5_REG_ALARM_HOURS_GP16_POS) ///<see #AMX8X5_REG_ALARM_HOURS  for more information
    
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions0C Register 0x0C - Date Alarm (AMX8X5_REG_ALARM_DATE)
 ** 
 ** This register holds alarm value for the date, in two binary coded decimal (BCD) digits. Values will range
 ** from 01 to 31. Leap years are correctly handled from 1900 to 2199.
 **/
 //@{

/**
 *****************************************************************************
 ** \brief Register 0x0C - Date Alarm (AMX8X5_REG_ALARM_DATE)
 **
 ** This register holds alarm value for the date, in two binary coded decimal (BCD) digits. Values will range
 ** from 01 to 31. Leap years are correctly handled from 1900 to 2199.
 **
 ** Date Alarm Register
 ** -------------------
 **
 ** |   Bits      |  7   |   6   |          [5:4]              |       [3:0]         |
 ** |-------------|------|-------|-----------------------------|---------------------|
 ** | Description | GP19 |  GP18 |      Date Alarm - Tens      |  Date Alarm - Ones  |
 **
 ** Date Alarm Register Bits
 ** ------------------------
 **
 ** | Bit |       Name       | Function                                               |
 ** |-----|------------------|--------------------------------------------------------|
 ** |  7  |       GP19       | Register bit for general purpose use.                  |
 ** |  6  |       GP18       | Register bit for general purpose use.                  |
 ** | 5:4 |Date Alarm - Tens | Holds the tens place for the date alarm.               |
 ** | 3:0 |Date Alarm - Ones | Holds the ones place for the date alarm.               |
 **/    
#define AMX8X5_REG_ALARM_DATE               0x0C

#define AMX8X5_REG_ALARM_DATE_GP19_POS              (7)                                    ///<see #AMX8X5_REG_ALARM_DATE  for more information
#define AMX8X5_REG_ALARM_DATE_GP19_MSK              (1 << AMX8X5_REG_ALARM_DATE_GP19_POS)  ///<see #AMX8X5_REG_ALARM_DATE  for more information
#define AMX8X5_REG_ALARM_DATE_GP18_POS              (6)                                    ///<see #AMX8X5_REG_ALARM_DATE  for more information
#define AMX8X5_REG_ALARM_DATE_GP18_MSK              (1 << AMX8X5_REG_ALARM_DATE_GP18_POS)  ///<see #AMX8X5_REG_ALARM_DATE  for more information
    
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions0D Register 0x0D - Months Alarm(AMX8X5_REG_ALARM_MONTH)
 ** 
 ** This register holds alarm value for months, in two binary coded decimal (BCD) digits. Values will range
 ** from 01 to 12.
 **/
 //@{
    
/**
 *****************************************************************************
 ** \brief Register 0x0C - Months Alarm (AMX8X5_REG_ALARM_MONTH)
 **
 ** This register holds alarm value for months, in two binary coded decimal (BCD) digits. Values will range
 ** from 01 to 12.
 **
 ** Months Alarm Register
 ** ---------------------
 **
 ** |   Bits      |  7   |  6   |  5   |             4         |       [3:0]           |
 ** |-------------|------|------|------|-----------------------|-----------------------|
 ** | Description | GP22 | GP21 | GP20 |  Months Alarm - Tens  |  Months Alarm - Ones  |
 **
 ** Months Alarm Register Bits
 ** --------------------------
 **
 ** | Bit |       Name          | Function                                               |
 ** |-----|---------------------|--------------------------------------------------------|
 ** |  7  |       GP22          | Register bit for general purpose use.                  |
 ** |  6  |       GP21          | Register bit for general purpose use.                  |
 ** |  5  |       GP20          | Register bit for general purpose use.                  |
 ** |  4  | Months Alarm - Tens | Holds the tens place for the months alarm.             |
 ** | 3:0 | Months Alarm - Ones | Holds the ones place for the months alarm.             |
 **/
#define AMX8X5_REG_ALARM_MONTH               0x0D
    
#define AMX8X5_REG_ALARM_MONTH_GP22_POS              (7)                                     ///<see #AMX8X5_REG_ALARM_MONTH  for more information
#define AMX8X5_REG_ALARM_MONTH_GP22_MSK              (1 << AMX8X5_REG_ALARM_MONTH_GP22_POS)  ///<see #AMX8X5_REG_ALARM_MONTH  for more information
#define AMX8X5_REG_ALARM_MONTH_GP21_POS              (6)                                     ///<see #AMX8X5_REG_ALARM_MONTH  for more information
#define AMX8X5_REG_ALARM_MONTH_GP21_MSK              (1 << AMX8X5_REG_ALARM_MONTH_GP21_POS)  ///<see #AMX8X5_REG_ALARM_MONTH  for more information
#define AMX8X5_REG_ALARM_MONTH_GP20_POS              (5)                                     ///<see #AMX8X5_REG_ALARM_MONTH  for more information
#define AMX8X5_REG_ALARM_MONTH_GP20_MSK              (1 << AMX8X5_REG_ALARM_MONTH_GP20_POS)  ///<see #AMX8X5_REG_ALARM_MONTH  for more information
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions0E Register 0x0E - Weekday Alarm (AMX8X5_REG_ALARM_WEEKDAY)
 ** 
 ** This register holds the alarm value for the day of the week. Values will range from 0 to 6.
 **/
 //@{    
/**
 *****************************************************************************
 ** \brief Register 0x0C - Weekday Alarm (AMX8X5_REG_ALARM_WEEKDAY)
 **
 ** This register holds the alarm value for the day of the week. Values will range from 0 to 6.
 **
 ** Weekdays Alarm Register
 ** -----------------------
 **
 ** |   Bits      |  7  |  6  |  5  |  4  |  3  |      [2:0]      |
 ** |-------------|-----|-----|-----|-----|-----|-----------------|
 ** | Description | GP27|�GP26| GP25| GP24| GP23| Weekdays Alarm  |
 **
 ** Weekdays Alarm Register Bits
 ** ----------------------------
 **
 ** | Bit |      Name      | Function                                                                                                  |
 ** |-----|----------------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  |      GP27      | Register bit for general purpose use.                                                                     |
 ** |  6  |      GP26      | Register bit for general purpose use.                                                                     |
 ** |  5  |      GP25      | Register bit for general purpose use.                                                                     |
 ** |  4  |      GP24      | Register bit for general purpose use.                                                                     |
 ** |  3  |      GP23      | Register bit for general purpose use.                                                                     |
 ** | 2:0 | Weekdays Alarm | Holds the weekdays alarm value.                                                                           |
 **/

#define AMX8X5_REG_ALARM_WEEKDAY             0x0E
#define AMX8X5_REG_ALARM_WEEKDAY_GP27_POS              (7)                                       ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information
#define AMX8X5_REG_ALARM_WEEKDAY_GP27_MSK              (1 << AMX8X5_REG_ALARM_WEEKDAY_GP27_POS)  ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information
#define AMX8X5_REG_ALARM_WEEKDAY_GP26_POS              (7)                                       ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information
#define AMX8X5_REG_ALARM_WEEKDAY_GP26_MSK              (1 << AMX8X5_REG_ALARM_WEEKDAY_GP26_POS)  ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information
#define AMX8X5_REG_ALARM_WEEKDAY_GP25_POS              (7)                                       ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information
#define AMX8X5_REG_ALARM_WEEKDAY_GP25_MSK              (1 << AMX8X5_REG_ALARM_WEEKDAY_GP25_POS)  ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information
#define AMX8X5_REG_ALARM_WEEKDAY_GP24_POS              (7)                                       ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information
#define AMX8X5_REG_ALARM_WEEKDAY_GP24_MSK              (1 << AMX8X5_REG_ALARM_WEEKDAY_GP24_POS)  ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information
#define AMX8X5_REG_ALARM_WEEKDAY_GP23_POS              (7)                                       ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information
#define AMX8X5_REG_ALARM_WEEKDAY_GP23_MSK              (1 << AMX8X5_REG_ALARM_WEEKDAY_GP23_POS)  ///<see #AMX8X5_REG_ALARM_WEEKDAY  for more information    
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions0F Register 0x0F - Status (read only) (AMX8X5_REG_STATUS)
 ** 
 ** This register holds a variety of status bits. The register may be written at any time to clear or set any status
 ** flag. If the ARST bit is set, any read of the Status Register will clear all of the bits except the CB bit.
 **/
 //@{    
    
/**
 *****************************************************************************
 ** \brief Register 0x0F - Status (read only)
 **
 ** This register holds a variety of status bits. The register may be written at any time to clear or set any status
 ** flag. If the ARST bit is set, any read of the Status Register will clear all of the bits except the CB bit.
 **
 ** Status Register
 ** ---------------
 **
 ** |   Bits      |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 ** |-------------|-----|-----|-----|-----|-----|-----|-----|-----|
 ** | Description |  CB | BAT | WDT | BL  | TIM | ALM | EX2 | EX1 |
 **
 ** Status Register Bits
 ** --------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  |  CB  | Century. This bit will be toggled when the Years register rolls over from 99 to 00 if the CEB bit is a 1. |
 ** |  ^  |  ^   | A 0 assumes the century is 19xx or 21xx, and a 1 assumes it is 20xx for leap year calculations.           |
 ** |  6  |  BAT | Set when the system switches to the VBAT Power state.                                                     |
 ** |  5  |  WDT | Set when the Watchdog Timer is enabled and is triggered, and the WDS bit is 0.                            |
 ** |  4  |  BL  | Set if the battery voltage VBAT crosses the reference voltage selected by BREF in the direction           |
 ** |  ^  |  ^   | selected by BPOL.                                                                                         |
 ** |  3  |  TIM | Set when the Countdown Timer is enabled and reaches zero.                                                 |
 ** |  2  |  ALM | Set when the Alarm function is enabled and all selected Alarm registers match their respective            |
 ** |  ^  |  ^   | counters.                                                                                                 |
 ** |  1  |  EX2 | Set when an external trigger is detected on the WDI pin. The EX2E bit must be set in order for this       |
 ** |  ^  |  ^   | interrupt to occur, but subsequently clearing EX2E will not automatically clear this flag.                |
 ** |  0  |  EX1 | Set when an external trigger is detected on the EXTI pin. The EX1E bit must be set in order for this      |
 ** |  ^  |  ^   | interrupt to occur, but subsequently clearing EX1E will not automatically clear this flag.                |
 **
 **/    
#define AMX8X5_REG_STATUS                    0x0F
    
#define AMX8X5_REG_STATUS_CB_POS             (7)                                   ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_CB_MSK             (1 << AMX8X5_REG_STATUS_CB_POS)       ///<see #AMX8X5_REG_STATUS for more information   
#define AMX8X5_REG_STATUS_BAT_POS            (6)                                   ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_BAT_MSK            (1 << AMX8X5_REG_STATUS_BAT_POS)      ///<see #AMX8X5_REG_STATUS for more information 
#define AMX8X5_REG_STATUS_WDT_POS            (5)                                   ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_WDT_MSK            (1 << AMX8X5_REG_STATUS_WDT_POS)      ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_BL_POS             (4)                                   ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_BL_MSK             (1 << AMX8X5_REG_STATUS_BL_POS)       ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_TIM_POS            (3)                                   ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_TIM_MSK            (1 << AMX8X5_REG_STATUS_TIM_POS)      ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_ALM_POS            (2)                                   ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_ALM_MSK            (1 << AMX8X5_REG_STATUS_ALM_POS)      ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_EX2_POS            (1)                                   ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_EX2_MSK            (1 << AMX8X5_REG_STATUS_EX2_POS)      ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_EX1_POS            (0)                                   ///<see #AMX8X5_REG_STATUS for more information
#define AMX8X5_REG_STATUS_EX1_MSK            (1 << AMX8X5_REG_STATUS_EX1_POS)      ///<see #AMX8X5_REG_STATUS for more information
    
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions10 Register 0x10 - Control1 (AMX8X5_REG_CONTROL_1)
 ** 
 ** This register holds some major control signals.
 **/
 //@{    
    
/**
 *****************************************************************************
 ** \brief Register 0x10 - Control1
 **
 ** This register holds some major control signals.
 **
 ** Control1 Register
 ** -----------------
 **
 ** |   Bits      |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 ** |-------------|-----|-----|-----|-----|-----|-----|-----|-----|
 ** | Description |STOP |12/24|OUTB | OUT | RSP |ARST |PWR2 |WRTC |
 **
 ** Control1 Register Bits
 ** ----------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  | STOP | When 1, stops the clocking system. The XT and RC Oscillators are not stopped. In XT Mode                  |
 ** |  ^  |  ^   | the 32.786 kHz clock output will continue to run. In RC Mode, the 128 Hz clock output will continue       |
 ** |  ^  |  ^   | to run. Other clock output selections will produce static outputs. This bit allows the clock              |
 ** |  ^  |  ^   | system to be precisely started, by setting it to 1 and back to 0.                                         |
 ** |  6  |12/24 | When 0, the Hours register operates in 24 hour mode. When 1, the Hours register operates in               |
 ** |  ^  |  ^   | 12 hour mode.                                                                                             |    
 ** |  5  | OUTB | A static value which may be driven on the PSW/nIRQ2 pin. The OUTB bit cannot be set to 1 if               |
 ** |  ^  |  ^   | the LKO2 bit is 1.                                                                                        |
 ** |  4  |  OUT | A static value which may be driven on the FOUT/nIRQ pin. This bit also defines the default                |
 ** |  ^  |  ^   | value for the Square Wave output when SQWE is not asserted.                                               |
 ** |  3  |  RSP | Reset Polarity. When 1, the nRST pin is asserted high. When 0, the nRST pin is asserted low.              |
 ** |  2  | ARST | Auto reset enable. When 1, a read of the Status register will cause any interrupt bits (TIM, BL,          |
 ** |  ^  |  ^   | ALM, WDT, XT1, XT2) to be cleared. When 0, the bits must be explicitly cleared by writing the             |
 ** |  ^  |  ^   | Status register.                                                                                          |
 ** |  1  | PWR2 | When 1, the PSW/nIRQ2 pin is driven by an approximately 1? pull-down which allows the                     |
 ** |  ^  |  ^   | AM18X5 to switch power to other system devices through this pin. When 0, the PSW/nIRQ2pin                 |
 ** |  ^  |  ^   | is a normal open drain output.                                                                            |
 ** |  0  | WRTC | Write RTC. This bit must be set in order to write any of the Counter registers (Hundredths, Seconds,      |
 ** |  ^  |  ^   | Minutes, Hours, Date, Months, Years or Weekdays).                                                         |
 **
 **/
#define AMX8X5_REG_CONTROL_1                 0x10
    
#define AMX8X5_REG_CONTROL_1_STOP_POS        (7)                                   ///<see #AMX8X5_REG_CONTROL_1 for more information
#define AMX8X5_REG_CONTROL_1_STOP_MSK        (1 << AMX8X5_REG_CONTROL_1_STOP_POS)  ///<see #AMX8X5_REG_CONTROL_1 for more information   
#define AMX8X5_REG_CONTROL_1_12_24_POS       (6)                                   ///<see #AMX8X5_REG_CONTROL_1 for more information
#define AMX8X5_REG_CONTROL_1_12_24_MSK       (1 << AMX8X5_REG_CONTROL_1_12_24_POS) ///<see #AMX8X5_REG_CONTROL_1 for more information   
#define AMX8X5_REG_CONTROL_1_OUTB_POS        (5)                                   ///<see #AMX8X5_REG_CONTROL_1 for more information
#define AMX8X5_REG_CONTROL_1_OUTB_MSK        (1 << AMX8X5_REG_CONTROL_1_OUTB_POS)  ///<see #AMX8X5_REG_CONTROL_1 for more information  
#define AMX8X5_REG_CONTROL_1_OUT_POS         (4)                                   ///<see #AMX8X5_REG_CONTROL_1 for more information
#define AMX8X5_REG_CONTROL_1_OUT_MSK         (1 << AMX8X5_REG_CONTROL_1_OUT_POS)   ///<see #AMX8X5_REG_CONTROL_1 for more information      
#define AMX8X5_REG_CONTROL_1_RSP_POS         (3)                                   ///<see #AMX8X5_REG_CONTROL_1 for more information
#define AMX8X5_REG_CONTROL_1_RSP_MSK         (1 << AMX8X5_REG_CONTROL_1_RSP_POS)   ///<see #AMX8X5_REG_CONTROL_1 for more information  
#define AMX8X5_REG_CONTROL_1_ARST_POS        (2)                                   ///<see #AMX8X5_REG_CONTROL_1 for more information
#define AMX8X5_REG_CONTROL_1_ARST_MSK        (1 << AMX8X5_REG_CONTROL_1_ARST_POS)  ///<see #AMX8X5_REG_CONTROL_1 for more information      
#define AMX8X5_REG_CONTROL_1_PWR2_POS        (1)                                   ///<see #AMX8X5_REG_CONTROL_1 for more information
#define AMX8X5_REG_CONTROL_1_PWR2_MSK        (1 << AMX8X5_REG_CONTROL_1_PWR2_POS)  ///<see #AMX8X5_REG_CONTROL_1 for more information
#define AMX8X5_REG_CONTROL_1_WRTC_POS        (0)                                   ///<see #AMX8X5_REG_CONTROL_1 for more information
#define AMX8X5_REG_CONTROL_1_WRTC_MSK        (1 << AMX8X5_REG_CONTROL_1_WRTC_POS)  ///<see #AMX8X5_REG_CONTROL_1 for more information
    
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions11 Register 0x11 - Control2 (AMX8X5_REG_CONTROL_2)
 ** 
 ** This register holds additional control and configuration signals for the flexible output pins FOUT/nIRQ and
 ** PSW/nIRQ2. Note that PSW/nIRQ2 and FOUT/nIRQ are open drain outputs.
 **/
 //@{    
    
/**
 *****************************************************************************
 ** \brief Register 0x11 - Control2
 **
 ** This register holds additional control and configuration signals for the flexible output pins FOUT/nIRQ and
 ** PSW/nIRQ2. Note that PSW/nIRQ2 and FOUT/nIRQ are open drain outputs.
 **
 ** Control1 Register
 ** -----------------
 **
 ** |   Bits      |   [7:6]  |  5  |  [4:2]  |  [1:0]  |
 ** |-------------|----------|-----|---------|---------|
 ** | Description | RESERVED |RS1E |  OUT2S  |  OUT1S  |
 **
 ** Control2 Register Bits
 ** ----------------------
 **
 ** | Bit | Name     | Function                                                                                                  |
 ** |-----|----------|-----------------------------------------------------------------------------------------------------------|
 ** | 7:6 | RESERVED | RESERVED                                                                                                  |
 ** |  5  |   RS1E   | When 1, enable the nEXTR pin to generate nRST.                                                            |
 ** | 4:2 |   OUT2S  | Controls the function of the PSW/nIRQ2 pin                                                                | 
 ** | ^   |   ^      | 000: nIRQ if at least one interrupt is enabled, else OUTB                                                 |   
 ** | ^   |   ^      | 001: SQW if SQWE = 1, else OUTB                                                                           | 
 ** | ^   |   ^      | 010: RESERVED                                                                                             | 
 ** | ^   |   ^      | 011: nAIRQ if AIE is set, else OUTB                                                                       | 
 ** | ^   |   ^      | 100: TIRQ if TIE is set, else OUTB                                                                        | 
 ** | ^   |   ^      | 101: nTIRQ if TIE is set, else OUTB                                                                       | 
 ** | ^   |   ^      | 110: SLEEP                                                                                                | 
 ** | ^   |   ^      | 111: OUTB                                                                                                 | 
 ** | ^   |   ^      | see also #en_amx8x5_out1_mode_t                                                                           |
 ** | 1:0 |   OUT1S  | Controls the function of the FOUT/NIRQ pin                                                                |
 ** | ^   |   ^      | 00: nIRQ if at least one interrupt is enabled, else OUT                                                   |
 ** | ^   |   ^      | 01: SQW if SQWE = 1, else OUT                                                                             |
 ** | ^   |   ^      | 10: SQW if SQWE = 1, else nIRQ if at least one interrupt is enabled, else OUT                             |
 ** | ^   |   ^      | 11: nAIRQ if AIE is set, else OUT                                                                         |
 ** | ^   |   ^      | see also #en_amx8x5_out2_mode_t                                                                           |
 **
 **/
#define AMX8X5_REG_CONTROL_2                 0x11

#define AMX8X5_REG_CONTROL_2_RS1E_POS        (5)                                      ///<see #AMX8X5_REG_CONTROL_2 for more information
#define AMX8X5_REG_CONTROL_2_RS1E_MSK        (1 << AMX8X5_REG_CONTROL_2_OUT2S_POS)    ///<see #AMX8X5_REG_CONTROL_2 for more information   
#define AMX8X5_REG_CONTROL_2_OUT2S_POS       (2)                                      ///<see #AMX8X5_REG_CONTROL_2 for more information
#define AMX8X5_REG_CONTROL_2_OUT2S_MSK       (0x7 << AMX8X5_REG_CONTROL_2_OUT2S_POS)  ///<see #AMX8X5_REG_CONTROL_2 for more information   
#define AMX8X5_REG_CONTROL_2_OUT1S_POS       (0)                                      ///<see #AMX8X5_REG_CONTROL_2 for more information
#define AMX8X5_REG_CONTROL_2_OUT1S_MSK       (0x3 << AMX8X5_REG_CONTROL_2_OUT1S_POS)  ///<see #AMX8X5_REG_CONTROL_2 for more information 
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions12 Register 0x12 - Interrupt Mask (AMX8X5_REG_INT_MASK)
 ** 
 ** This register holds the interrupt enable bits and other configuration information.
 **/
 //@{    
    
/**
 *****************************************************************************
 ** \brief Register 0x12 - Interrupt Mask
 **
 ** This register holds the interrupt enable bits and other configuration information.
 **
 ** Interrupt Mask Register
 ** -----------------------
 **
 ** |   Bits      |  7  |   [6:5]   |  4  |  3  |  2  |  1  |  0  |
 ** |-------------|-----|-----------|-----|-----|-----|-----|-----|
 ** | Description | CEB |    IM     | BLIE| TIE | AIE |EX2E |EX1E |
 **
 ** Interrupt Mask Register Bits
 ** ----------------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  |  CEB | Century Enable.                                                                                           |
 ** |  ^  |  ^   | 0: The CB bit will never be automatically updated.                                                        |
 ** |  ^  |  ^   | 1: The CB bit will toggle when the Years register rolls over from 99 to 00.                               |
 ** | 6:5 |  IM  | Interrupt Mode.                                                                                           |
 ** |  ^  |  ^   | This controls the duration of the nAIRQ interrupt as shown below. The interrupt output always goes        |
 ** |  ^  |  ^   | high when the corresponding flag in the Status Register is cleared. In order to minimize current          |
 ** |  ^  |  ^   | drawn by the AM18X5 this field should be kept at 0x3.                                                     |
 ** |  ^  |  ^   | 00: Level (static) for both XT mode and RC mode.                                                          |
 ** |  ^  |  ^   | 01: 1/8192 seconds for XT mode. 1/64 seconds for RC mode.                                                 |
 ** |  ^  |  ^   | 10: 1/64 seconds for both XT mode and RC mode.                                                            |
 ** |  ^  |  ^   | 11: 1/4 seconds for both XT mode and RC mode.                                                             |
 ** |  ^  |  ^   | see also #en_amx8x5_interrupt_mode_t                                                                      |
 ** |  4  |  BLIE| Battery Low Interrupt Enable.                                                                             |
 ** |  ^  |  ^   | 0: Disable the battery low interrupt                                                                      |
 ** |  ^  |  ^   | 1: The battery low detection will generate an interrupt.                                                  |
 ** |  3  |  TIE | Timer Interrupt Enable.                                                                                   |
 ** |  ^  |  ^   | 0: Disable the timer interrupt                                                                            |
 ** |  ^  |  ^   | 1: The Countdown Timer will generate an IRQ interrupt signal and set the TIM flag when the timer          |
 ** |  ^  |  ^   |    reaches 0.                                                                                             |
 ** |  2  |  AIE | Alarm Interrupt Enable.                                                                                   |
 ** |  ^  |  ^   | 0: Disable the alarm interrupt.                                                                           |
 ** |  ^  |  ^   | 1: A match of all the enabled alarm registers will generate an IRQ interrupt signal.                      |
 ** |  1  |  EX2E| XT2 Interrupt Enable.                                                                                     |
 ** |  ^  |  ^   | 0: Disable the XT2 interrupt.                                                                             |
 ** |  ^  |  ^   | 1: The WDI input pin will generate the XT2 interrupt when the edge specified by EX2P occurs.              |
 ** |  0  |  EX1E| XT1 Interrupt Enable.                                                                                     |
 ** |  ^  |  ^   | 0: Disable the XT1 interrupt.                                                                             |
 ** |  ^  |  ^   | 1: The EXTI input pin will generate the XT1 interrupt when the edge specified by EX1P occurs.             |
 **
 **/
#define AMX8X5_REG_INT_MASK                  0x12
    
#define AMX8X5_REG_INT_MASK_CEB_POS          (7)                                   ///<see #AMX8X5_REG_INT_MASK for more information
#define AMX8X5_REG_INT_MASK_CEB_MSK          (1 << AMX8X5_REG_INT_MASK_CEB_POS)    ///<see #AMX8X5_REG_INT_MASK for more information   
#define AMX8X5_REG_INT_MASK_IM_POS           (5)                                   ///<see #AMX8X5_REG_INT_MASK for more information
#define AMX8X5_REG_INT_MASK_IM_MSK           (0x3 << AMX8X5_REG_INT_MASK_IM_POS)   ///<see #AMX8X5_REG_INT_MASK for more information   
#define AMX8X5_REG_INT_MASK_BLIE_POS         (4)                                   ///<see #AMX8X5_REG_INT_MASK for more information
#define AMX8X5_REG_INT_MASK_BLIE_MSK         (0x1 << AMX8X5_REG_INT_MASK_BLIE_POS) ///<see #AMX8X5_REG_INT_MASK for more information   
#define AMX8X5_REG_INT_MASK_TIE_POS          (3)                                   ///<see #AMX8X5_REG_INT_MASK for more information
#define AMX8X5_REG_INT_MASK_TIE_MSK          (0x1 << AMX8X5_REG_INT_MASK_TIE_POS)  ///<see #AMX8X5_REG_INT_MASK for more information 
#define AMX8X5_REG_INT_MASK_AIE_POS          (2)                                   ///<see #AMX8X5_REG_INT_MASK for more information
#define AMX8X5_REG_INT_MASK_AIE_MSK          (0x1 << AMX8X5_REG_INT_MASK_AIE_POS) ///<see #AMX8X5_REG_INT_MASK for more information      
#define AMX8X5_REG_INT_MASK_EX2E_POS         (1)                                   ///<see #AMX8X5_REG_INT_MASK for more information
#define AMX8X5_REG_INT_MASK_EX2E_MSK         (0x1 << AMX8X5_REG_INT_MASK_EX2E_POS) ///<see #AMX8X5_REG_INT_MASK for more information    
#define AMX8X5_REG_INT_MASK_EX1E_POS         (0)                                   ///<see #AMX8X5_REG_INT_MASK for more information
#define AMX8X5_REG_INT_MASK_EX1E_MSK         (0x1 << AMX8X5_REG_INT_MASK_EX1E_POS) ///<see #AMX8X5_REG_INT_MASK for more information  
    
//@}

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions13 Register 0x13 - SWQ (AMX8X5_REG_SQW)
 ** 
 ** This register holds the control signals for the square wave output. Note that some frequency selections are
 ** not valid if the 128 Hz RC Oscillator is selected
 **
 **/
    
//@{     
/**
 *****************************************************************************
 ** \brief Register 0x13 - SWQ (AMX8X5_REG_SQW)
 **
 ** This register holds the control signals for the square wave output. Note that some frequency selections are
 ** not valid if the 128 Hz RC Oscillator is selected
 **
 ** SQW Register
 ** ------------
 **
 ** |   Bits      |  7  |   [6:5]   |  [4:0]  |
 ** |-------------|-----|-----------|---------|
 ** | Description | SQWE| RESERVED  |  SQFS   |
 **
 ** Interrupt Mask Register Bits
 ** ----------------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  | SQWE | When 1, the square wave output is enabled. When 0, the square wave output is held at the                  |
 ** |  ^  |  ^   | value of OUT.                                                                                             |
 ** | 6:5 |  IM  | RESERVED                                                                                                  |
 ** | 4:0 | SQFS | Selects the frequency of the square wave output, as shown in Table 66. Note that some selections          |
 ** |  ^  |  ^   | are not valid if the 128 Hz oscillator is selected. Some selections also produce short                    |
 ** |  ^  |  ^   | pulses rather than square waves, and are intended primarily for test usage.                               |
 ** |  ^  |  ^   | 00000: 1 century (2)                                                                                      |
 ** |  ^  |  ^   | 00001: 32.768 kHz (1)                                                                                     |
 ** |  ^  |  ^   | 00010: 8.192 kHz (1)                                                                                      |
 ** |  ^  |  ^   | 00011: 4.096 kHz(1)                                                                                       |
 ** |  ^  |  ^   | 00100: 2.048 kHz(1)                                                                                       |
 ** |  ^  |  ^   | 00101: 1.024 kHz(1)                                                                                       |
 ** |  ^  |  ^   | 00110: 512 Hz(1) � Default value                                                                          |
 ** |  ^  |  ^   | 00111: 256 Hz(1)                                                                                          |
 ** |  ^  |  ^   | 01000: 128 Hz                                                                                             |
 ** |  ^  |  ^   | 01001: 64 Hz                                                                                              |
 ** |  ^  |  ^   | 01010: 32 Hz                                                                                              |
 ** |  ^  |  ^   | 01011: 16 Hz                                                                                              |
 ** |  ^  |  ^   | 01100: 8 Hz                                                                                               |
 ** |  ^  |  ^   | 01101: 4 Hz                                                                                               |
 ** |  ^  |  ^   | 01110: 2 Hz                                                                                               |
 ** |  ^  |  ^   | 01111: 1 Hz                                                                                               |
 ** |  ^  |  ^   | 10000: 1/2 Hz                                                                                             |
 ** |  ^  |  ^   | 10001: 1/4 Hz                                                                                             |
 ** |  ^  |  ^   | 10010: 1/8 Hz                                                                                             |
 ** |  ^  |  ^   | 10011: 1/16 Hz                                                                                            |
 ** |  ^  |  ^   | 10100: 1/32 Hz                                                                                            |
 ** |  ^  |  ^   | 10101: 1/609 Hz (1 minute)                                                                                |
 ** |  ^  |  ^   | 10110: 16.384 kHz (1)                                                                                     |
 ** |  ^  |  ^   | 10111: 00 Hz (1)(2)                                                                                       |
 ** |  ^  |  ^   | 11000: 1 hour(2)                                                                                          |
 ** |  ^  |  ^   | 11001: 1 day(2)                                                                                           |
 ** |  ^  |  ^   | 11010: TIRQ                                                                                               |
 ** |  ^  |  ^   | 11011: NOT TIRQ                                                                                           |
 ** |  ^  |  ^   | 11100: 1 year(2)                                                                                          |
 ** |  ^  |  ^   | 11101: 1 Hz to Counters(2)                                                                                |
 ** |  ^  |  ^   | 11110: 1/32 Hz from Acal(2)                                                                               |
 ** |  ^  |  ^   | 11111: 1/8 Hz from Acal (2)                                                                               |
 ** |  ^  |  ^   | (1) NA if 128 Hz Oscillator selected, (2) Pulses for Test Usage.                                          |
 **
 **/
    
#define AMX8X5_REG_SQW                       0x13
#define AMX8X5_REG_SQW_SQEW_POS              (7)                                   ///<see #AMX8X5_REG_SQW for more information
#define AMX8X5_REG_SQW_SQEW_MSK              (1 << AMX8X5_REG_SQW_SQEW_POS)        ///<see #AMX8X5_REG_SQW for more information 
#define AMX8X5_REG_SQW_SQFS_POS              (0)                                   ///<see #AMX8X5_REG_SQW for more information
#define AMX8X5_REG_SQW_SQFS_MSK              (0x1F << AMX8X5_REG_SQW_SQFS_POS)     ///<see #AMX8X5_REG_SQW for more information 
    
//@}

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions14 Register 0x14 - Calibration XT (AMX8X5_REG_CAL_XT)
 ** 
 ** This register holds the control signals for a digital calibration function of the XT Oscillator.
 **/

/**
 *****************************************************************************
 ** \brief Register 0x14 - Calibration XT (AMX8X5_REG_CAL_XT)
 **
 ** This register holds the control signals for a digital calibration function of the XT Oscillator.
 **
 ** Calibration XT Register 
 ** -----------------------
 **
 ** |   Bits      |  7  |   [6:0]  |
 ** |-------------|-----|----------|
 ** | Description | CMDX| OFFSETX  |
 **
 ** Calibration XT Register Bits
 ** ----------------------------
 **
 ** | Bit |  Name   | Function                                                                                                  |
 ** |-----|---------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  |  CMDX   | The calibration adjust mode. When 0 (Normal Mode), each adjustment step is +/- 2 ppm. When                |
 ** |  ^  |  ^      | 1 (Coarse Mode), each adjustment step is +/- 4 ppm.                                                       |
 ** | 6:0 | OFFSETX | The amount to adjust the effective time. This is a two's complement number with a range of -64            |
 ** |  ^  |  ^      | to +63 adjustment steps.                                                                                  |    
 **/
 //@{      
#define AMX8X5_REG_CAL_XT                       0x14
#define AMX8X5_REG_CAL_XT_CMDX_POS              (7)                                      ///<see #AMX8X5_REG_CAL_XT for more information
#define AMX8X5_REG_CAL_XT_CMDX_MSK              (1 << AMX8X5_REG_CAL_XT_CMDX_POS)        ///<see #AMX8X5_REG_CAL_XT for more information 
#define AMX8X5_REG_CAL_XT_OFFSETX_POS           (0)                                      ///<see #AMX8X5_REG_CAL_XT for more information
#define AMX8X5_REG_CAL_XT_OFFSETX_MSK           (7F << AMX8X5_REG_CAL_XT_OFFSETX_POS)    ///<see #AMX8X5_REG_CAL_XT for more information    
//@}

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions15 Register 0x15 - Calibration RC Upper (AMX8X5_REG_CAL_RC_HI)
 ** 
 **/
 //@{  
#define AMX8X5_REG_CAL_RC_HI                 0x15
//@}

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions16 Register 0x16 - Calibration RC Lower (AMX8X5_REG_CAL_RC_LOW)
 ** 
 **/
 //@{ 
#define AMX8X5_REG_CAL_RC_LOW                0x16
//@}

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions17 Register 0x17  - Sleep Control (AMX8X5_REG_SLEEP_CTRL)
 ** 
 ** This register controls the Sleep function of the Power Control system.
 **/
//@{ 
       
/**
 *****************************************************************************
 ** \brief Register 0x17 - Sleep Control (AMX8X5_REG_SLEEP_CTRL)
 **
 ** This register controls the Sleep function of the Power Control system.
 **
 ** Status Register
 ** ---------------
 **
 ** |   Bits      |  7  |  6  |  5  |  4  |  3  |      [2:0]      |
 ** |-------------|-----|-----|-----|-----|-----|-----------------|
 ** | Description |  SLP|SLRES| EX2P| EX1P| SLST|      SLTO       |
 **
 ** Status Register Bits
 ** --------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  |  SLP | When 1, the Power Control SM will transition to the SWAIT state. This bit will be cleared when            |
 ** |  ^  |  ^   | the SM returns to the RUN state. If either STOP is 1 or no interrupt is enabled, SLP will remain          |
 ** |  ^  |  ^   | at 0 even after an attempt to set it to 1.                                                                |
 ** |  6  | SLRES| When 1, assert nRST low when the Power Control SM is in the SLEEP state.                                  |
 ** |  5  | EX2P | When 1, the external interrupt XT2 will trigger on a rising edge of the WDI pin. When 0, the              |
 ** |  ^  |  ^   | external interrupt XT2 will trigger on a falling edge of the WDI pin.                                     |
 ** |  4  | EX1P | When 1, the external interrupt XT1 will trigger on a rising edge of the EXTI pin. When 0, the             |
 ** |  ^  |  ^   | external interrupt XT1 will trigger on a falling edge of the EXTI pin.                                    |
 ** |  3  | SLST | Set when the AM18X5 enters Sleep Mode. This allows software to determine if a SLEEP has                   |
 ** |  ^  |  ^   | occurred since the last time this bit was read.                                                           |
 ** | 2:0 | SLTO | The number of 7.8 ms periods after SLP is set until the Power Control SM goes into the SLEEP              |
 ** |  ^  |  ^   | state. If SLTO is not 0, the actual delay is guaranteed to be between SLTO and (SLTO + 1) periods.        |
 ** |  ^  |  ^   | If SLTO is 0, the transition will occur with no delay.                                                    |
 **
 **/
#define AMX8X5_REG_SLEEP_CTRL                0x17
//@}
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions18 Register 0x18 - Countdown Timer Control (AMX8X5_REG_TIMER_CTRL)
 ** 
 ** This register controls the Countdown Timer function. Note that the 00 frequency selection is slightly
 ** different depending on whether the 32.786 kHz XT Oscillator or the 128 Hz RC Oscillator is selected. In
 ** some RC Oscillator modes, the interrupt pulse output is specified as RCPLS. In these cases the interrupt
 ** output will be a short negative going pulse which is typically between 100 and 400 us. This allows control
 ** of external devices which require pulses shorter than the minimum 7.8 ms pulse created directly by the RC
 **/
//@{ 
    
/**
 *****************************************************************************
 ** \brief Register 0x18 - Countdown Timer Control (AMX8X5_REG_TIMER_CTRL)
 **
 ** This register controls the Countdown Timer function. Note that the 00 frequency selection is slightly
 ** different depending on whether the 32.786 kHz XT Oscillator or the 128 Hz RC Oscillator is selected. In
 ** some RC Oscillator modes, the interrupt pulse output is specified as RCPLS. In these cases the interrupt
 ** output will be a short negative going pulse which is typically between 100 and 400 us. This allows control
 ** of external devices which require pulses shorter than the minimum 7.8 ms pulse created directly by the RC
 ** Oscillator.
 **
 ** Countdown Timer Control Register
 ** --------------------------------
 **
 ** |   Bits      |  7  |  6  |  5  |   [4:2]   |  [1:0]  |
 ** |-------------|-----|-----|-----|-----------|---------|
 ** | Description |  TE |  TM | TRPT|    RPT    |   TFS   |
 **
 ** Countdown Timer Control Register Bits
 ** -------------------------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  |  TE  | Timer Enable. When 1, the Countdown Timer will count down. When 0, the Countdown Timer retains            |
 ** |  ^  |  ^   | the current value. If TE is 0, the clock to the Timer is disabled for power minimization.                 |
 ** |  6  |  TM  | Timer Interrupt Mode. Along with TRPT, this controls the Timer Interrupt function as shown in Table       |
 ** |  ^  |  ^   | 28. A Level Interrupt will cause the nIRQ signal to be driven low by a Countdown Timer interrupt until    |
 ** |  ^  |  ^   | the associated flag is cleared. A Pulse interrupt will cause the nIRQ signal to be driven low for the     |
 ** |  ^  |  ^   | time shown in Table 79 in DS or until the flag is cleared.                                                |
 ** |  5  | TRPT | Along with TM, this controls the repeat function of the Countdown Timer. If Repeat is selected, the       |
 ** |  ^  |  ^   | Countdown Timer reloads the value from the Timer_Initial register upon reaching 0, and continues          |
 ** |  ^  |  ^   | counting. If Single is selected, the Countdown Timer will halt when it reaches zero. This allows the      |
 ** |  ^  |  ^   | generation of periodic interrupts of virtually any frequency.                                             |
 ** | 4:2 |  RPT | These bits enable the Alarm Interrupt repeat function, as shown in Table 78. HA is the Hundredths-        |
 ** |  ^  |  ^   | Alarm register value.                                                                                     |
 ** | 1:0 |  TFS | Select the clock frequency and interrupt pulse width of the Countdown Timer, as defined in Table 79 in DS.|
 ** |  ^  |  ^   | RCPLS is a 100-400 us pulse.                                                                              |
 **
 **/
#define AMX8X5_REG_TIMER_CTRL                0x18

#define AMX8X5_REG_TIMER_CTRL_TE_POS              (7)                                    ///<see #AMX8X5_REG_TIMER_CTRL  for more information
#define AMX8X5_REG_TIMER_CTRL_TE_MSK              (1 << AMX8X5_REG_TIMER_CTRL_TE_POS)    ///<see #AMX8X5_REG_TIMER_CTRL  for more information
#define AMX8X5_REG_TIMER_CTRL_TM_POS              (6)                                    ///<see #AMX8X5_REG_TIMER_CTRL  for more information
#define AMX8X5_REG_TIMER_CTRL_TM_MSK              (1 << AMX8X5_REG_TIMER_CTRL_TM_POS)    ///<see #AMX8X5_REG_TIMER_CTRL  for more information
#define AMX8X5_REG_TIMER_CTRL_TRPT_POS            (5)                                    ///<see #AMX8X5_REG_TIMER_CTRL  for more information
#define AMX8X5_REG_TIMER_CTRL_TRPT_MSK            (1 << AMX8X5_REG_TIMER_CTRL_TRPT_POS)  ///<see #AMX8X5_REG_TIMER_CTRL  for more information
#define AMX8X5_REG_TIMER_CTRL_RPT_POS             (2)                                    ///<see #AMX8X5_REG_TIMER_CTRL  for more information
#define AMX8X5_REG_TIMER_CTRL_RPT_MSK             (0x7 << AMX8X5_REG_TIMER_CTRL_RPT_POS) ///<see #AMX8X5_REG_TIMER_CTRL  for more information
#define AMX8X5_REG_TIMER_CTRL_TFS_POS             (0)                                    ///<see #AMX8X5_REG_TIMER_CTRL  for more information
#define AMX8X5_REG_TIMER_CTRL_TFS_MSK             (0x3 << AMX8X5_REG_TIMER_CTRL_TFS_POS) ///<see #AMX8X5_REG_TIMER_CTRL  for more information
     
//@}
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions19 Register 0x19 - Countdown Timer (AMX8X5_REG_TIMER)
 ** 
 **/
//@{ 
/**
 *****************************************************************************
 ** \brief Register 0x19 - Countdown Timer (AMX8X5_REG_TIMER)
 **
 ** This register holds the current value of the Countdown Timer. It may be loaded with the desired starting
 ** value when the Countdown Timer is stopped.
 **
 ** Countdown Timer Control Register
 ** --------------------------------
 **
 ** |   Bits      |        [7:0]      |
 ** |-------------|-------------------|
 ** | Description |  Countdown Timer  |
 **                                                       
 **
 **/
#define AMX8X5_REG_TIMER                     0x19
//@}
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions1A Register 0x1A - Timer Initial Value (AMX8X5_REG_TIMER_INITIAL)
 ** 
 **/
//@{  
/**
 *****************************************************************************
 ** \brief Register 0x1A - Timer Initial Value (AMX8X5_REG_TIMER_INITIAL)
 **
 ** This register holds the current value of the Countdown Timer. It may be loaded with the desired starting
 ** value when the Countdown Timer is stopped.
 **
 ** The value reloaded into the Countdown Timer when it reaches zero if the TRPT bit is a 1.
 **
 ** Countdown Timer Control Register
 ** --------------------------------
 **
 ** |   Bits      |          [7:0]        |
 ** |-------------|-----------------------|
 ** | Description |  Timer Initial Value  |
 **                                                
 **
 **/
#define AMX8X5_REG_TIMER_INITIAL             0x1A
//@}
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions1B Register 0x1B - Watchdog Timer (AMX8X5_REG_WDT)
 ** 
 **/
//@{  
   
/**
 *****************************************************************************
 ** \brief Register 0x1B - Watchdog Timer (AMX8X5_REG_WDT)
 **
 ** This register controls the Watchdog Timer function.
 **
 ** Countdown Timer Control Register
 ** --------------------------------
 **
 ** |   Bits      |  7  |  [6:2]  |  [1:0]  |
 ** |-------------|-----|---------|---------|
 ** | Description | WDS |   BMB   |   WRB   |
 **
 ** Countdown Timer Control Register Bits
 ** -------------------------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  |  WDS | Watchdog Steering. When 0, the Watchdog Timer will generate WIRQ when it times out. When 1,               |
 ** |  ^  |  ^   | the Watchdog Timer will generate a reset when it times out.                                               |
 ** | 6:2 |  BMB | The number of clock cycles which must occur before the Watchdog Timer times out. A value of               |
 ** |  ^  |  ^   | 00000 disables the Watchdog Timer function.                                                               |
 ** | 1:0 |  WRB | The clock frequency of the Watchdog Timer                                                                 |
 ** |  ^  |  ^   | 00: 16Hz                                                                                                  |
 ** |  ^  |  ^   | 01:  4Hz                                                                                                  |
 ** |  ^  |  ^   | 10:  1Hz                                                                                                  |
 ** |  ^  |  ^   | 11: 1/4Hz                                                                                                 |
 **
 **/
#define AMX8X5_REG_WDT                       0x1B
//@}
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions1C Register 0x1C - Oscillator Control (AMX8X5_REG_OSC_CONTROL)
 ** 
 **/
//@{  
   
/**
 *****************************************************************************
 ** \brief Register 0x1C - Oscillator Control (AMX8X5_REG_OSC_CONTROL)
 **
 ** This register controls the overall Oscillator
 **
 ** Oscillator Control Register
 ** ---------------------------
 **
 ** |   Bits      |  7  |  [6:5]  |  4  |  3  |  2  |  1  |  0  |
 ** |-------------|-----|---------|-----|-----|-----|-----|-----|
 ** | Description |OSEL |   ACAL  | AOS | FOS |PWGT |OFIE |ACIE |
 **
 ** Oscillator Control Register Bits
 ** --------------------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  | OSEL | When 1, request the RC Oscillator to generate a 128 Hz clock for the timer circuits. When 0,              |
 ** |  ^  |  ^   | request the XT Oscillator to generate a 32.786 kHz clock to the timer circuit. Note that if the XT        |
 ** |  ^  |  ^   | Oscillator is not operating, the oscillator switch will not occur. The OMODE field in the Oscillator      |
 ** |  ^  |  ^   | Status register indicates the actual oscillator which is selected.                                        |
 ** | 6:5 | ACAL | Controls the automatic calibration function, as described in Autocalibration.                             |    
 ** |  4  | AOS  | When 1, the oscillator will automatically switch to RC oscillator mode when the system is powered         |
 ** |  ^  |  ^   | from the battery. When 0, no automatic switching occurs.                                                  |
 ** |  3  | FOS  | When 1, the oscillator will automatically switch to RC oscillator mode when an oscillator failure is      |
 ** |  ^  |  ^   | detected. When 0, no automatic switching occurs.                                                          |
 ** |  2  | PWGT | When 1, the I/O interface will be disabled when the power switch is active and disabled (PWR2 is          |
 ** |  ^  |  ^   | a 1 and the OUT2 output is a 1). In order for the I/O interface to be disabled, the PSW pin must          |
 ** |  ^  |  ^   | be configured for the sleep function by setting the OUT2S field to a value of 6.                          |
 ** |  1  | OFIE | Oscillator Fail interrupt enable. When 1, an Oscillator Failure will generate an IRQ signal.              |
 ** |  0  | ACIE | When 1, an Autocalibration Failure will generate an interrupt.                                            |
 **
 **/
#define AMX8X5_REG_OSC_CONTROL               0x1C
#define AMX8X5_REG_OSC_CONTROL_OSEL_POS      (7)
#define AMX8X5_REG_OSC_CONTROL_OSEL_MSK      (1 << AMX8X5_REG_OSC_CONTROL_OSEL_POS)
#define AMX8X5_REG_OSC_CONTROL_ACAL_POS      (5)
#define AMX8X5_REG_OSC_CONTROL_ACAL_MSK      (3 << AMX8X5_REG_OSC_CONTROL_ACAL_POS)
#define AMX8X5_REG_OSC_CONTROL_AOS_POS       (4)
#define AMX8X5_REG_OSC_CONTROL_AOS_MSK       (1 << AMX8X5_REG_OSC_CONTROL_AOS_POS)
#define AMX8X5_REG_OSC_CONTROL_FOS_POS       (3)
#define AMX8X5_REG_OSC_CONTROL_FOS_MSK       (1 << AMX8X5_REG_OSC_CONTROL_FOS_POS)
#define AMX8X5_REG_OSC_CONTROL_PWGT_POS      (2)
#define AMX8X5_REG_OSC_CONTROL_PWGT_MSK      (1 << AMX8X5_REG_OSC_CONTROL_PWGT_POS)
#define AMX8X5_REG_OSC_CONTROL_OFIE_POS      (1)
#define AMX8X5_REG_OSC_CONTROL_OFIE_MSK      (1 << AMX8X5_REG_OSC_CONTROL_OFIE_POS)
#define AMX8X5_REG_OSC_CONTROL_ACIE_POS      (0)
#define AMX8X5_REG_OSC_CONTROL_ACIE_MSK      (0 << AMX8X5_REG_OSC_CONTROL_ACIE_POS)

//@}
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions1D Register 0x1D - Oscillator Status (AMX8X5_REG_OSC_STATUS)
 ** 
 **/
//@{  
    
/**
 *****************************************************************************
 ** \brief Register 0x1D - Oscillator Status (AMX8X5_REG_OSC_STATUS) 
 **
 ** This register holds several miscellaneous bits used to control and observe the oscillators.
 **
 ** Oscillator Status Register
 ** --------------------------
 **
 ** |   Bits      |    [7.6]  |  5  |  4  |    [3:2]  |  1  |  0  |
 ** |-------------|-----------|-----|-----|-----------|-----|-----|
 ** | Description |    XTCAL  |LKO2 |OMODE|  RESERVED |  OF |ACF  |
 **
 ** Control1 Register Bits
 ** ----------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** | 7:6 | XTCAL| Extended Crystal Calibration. This field defines a value by which the Crystal Oscillator is adjusted      |
 ** |  ^  |  ^   | to compensate for low capacitance crystals, independent of the normal Crystal Calibration function        |
 ** |  ^  |  ^   | controlled by the Calibration XT Register. The frequency generated by the Crystal Oscillator              |
 ** |  ^  |  ^   | is slowed by 122 ppm times the value in the XTCAL field (0, -122,-244 or -366 ppm).                       |
 ** |  5  | LKO2 | Lock OUT2. If this bit is a 1, the OUTB register bit (see Section 7.3.2) cannot be set to 1. This is      |
 ** |  ^  |  ^   | typically used when OUT2 is configured as a power switch, and setting OUTB to a 1 would turn off          |
 ** |  ^  |  ^   | the switch.                                                                                               |
 ** |  4  | OMODE| (read only) � Oscillator Mode. This bit is a 1 if the RC Oscillator is selected to drive the internal     |
 ** |  ^  |  ^   | clocks, and a 0 if the Crystal Oscillator is selected. If the STOP bit is set, the OMODE bit is invalid.  |
 ** | 3:2 | RES  | RESERVED                                                                                                  |
 ** |  1  |  OF  | Oscillator Failure. This bit is set on a power on reset, when both the system and battery voltages        |
 ** |  ^  |  ^   | have dropped below acceptable levels. It is also set if an Oscillator Failure occurs, indicating that     |
 ** |  ^  |  ^   | the crystal oscillator is running at less than 8 kHz. It can be cleared by writing a 0 to the bit.        |
 ** |  0  | ACF  | Set when an Autocalibration Failure occurs, indicating that either the RC Oscillator frequency is         |
 ** |  ^  |  ^   | too different from 128 Hz to be correctly calibrated or the XT Oscillator did not start.                  |
 **
 **/
#define AMX8X5_REG_OSC_STATUS                0x1D
     
#define AMX8X5_REG_OSC_STATUS_XTCAL_POS              (6)                                       ///<see #AMX8X5_REG_OSC_STATUS  for more information
#define AMX8X5_REG_OSC_STATUS_XTCAL_MSK              (0x3 << AMX8X5_REG_OSC_STATUS_XTCAL_POS)  ///<see #AMX8X5_REG_OSC_STATUS  for more information
#define AMX8X5_REG_OSC_STATUS_LKO2_POS               (5)                                       ///<see #AMX8X5_REG_OSC_STATUS  for more information
#define AMX8X5_REG_OSC_STATUS_LKO2_MSK               (1 << AMX8X5_REG_OSC_STATUS_LKO2_POS)     ///<see #AMX8X5_REG_OSC_STATUS  for more information
#define AMX8X5_REG_OSC_STATUS_OMODE_POS              (4)                                       ///<see #AMX8X5_REG_OSC_STATUS  for more information
#define AMX8X5_REG_OSC_STATUS_OMODE_MSK              (1 << AMX8X5_REG_OSC_STATUS_OMODE_POS)    ///<see #AMX8X5_REG_OSC_STATUS  for more information   
#define AMX8X5_REG_OSC_STATUS_OF_POS                 (1)                                       ///<see #AMX8X5_REG_OSC_STATUS  for more information
#define AMX8X5_REG_OSC_STATUS_OF_MSK                 (1 << AMX8X5_REG_OSC_STATUS_OF_POS)       ///<see #AMX8X5_REG_OSC_STATUS  for more information 
#define AMX8X5_REG_OSC_STATUS_ACF_POS                (0)                                       ///<see #AMX8X5_REG_OSC_STATUS  for more information
#define AMX8X5_REG_OSC_STATUS_ACF_MSK                (1 << AMX8X5_REG_OSC_STATUS_ACF_POS)       ///<see #AMX8X5_REG_OSC_STATUS  for more information 
     
//@}
    
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions1F Register 0x1F - Config Key (AMX8X5_REG_CONFIG_KEY)
 ** 
 **/
//@{  
    
/**
 *****************************************************************************
 ** \brief Register 0x1F - Config Key (AMX8X5_REG_CONFIG_KEY)
 **
 ** This register contains the Configuration Key, which must be written with specific values in order to access
 ** some registers and functions. The Configuration Key is reset to 0x00 on any register write.
 **
 ** 1. Writing a value of 0xA1 enables write access to the Oscillator Control register
 **
 ** 2. Writing a value of 0x3C does not update the Configuration Key register, but generates a Software
 **    Reset (see Software Reset).
 **
 ** 3. Writing a value of 0x9D enables write access to the Trickle Register (0x20), the BREF Register
 **    (0x21), the AFCTRL Register (0x26), the Batmode I/O Register (0x27) and the Output Control Register
 **    (0x30).
 **
 ** Config Key Register
 ** -------------------
 **
 ** |   Bits      |          [7:0]        |
 ** |-------------|-----------------------|
 ** | Description |  Configuration Key    |
 **/
#define AMX8X5_REG_CONFIG_KEY                0x1F
//@}
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions20 Register 0x20 - Trickle (AMX8X5_REG_TRICKLE)
 ** 
 ** The devices supporting the VBAT pin include a trickle charging circuit which allows a battery or
 ** supercapacitor connected to the VBAT pin to be charged from the power supply connected to the VCC pin.
 ** The circuit of the Trickle Charger is shown in Figure 40. The Trickle Charger configuration is controlled by
 ** the Trickle register. The Trickle Charger is enabled if a) the TCS field is 1010, b) the
 ** DIODE field is 01 or 10 and c) the ROUT field is not 00. A diode, with a typical voltage drop of 0.6V, is
 ** inserted in the charging path if DIODE is 10. A Schottky diode, with a typical voltage drop of 0.3V, is
 ** inserted in the charging path if DIODE is 01. The series current limiting resistor is selected by the ROUT
 ** field as shown in the figure.
 **   
 ** \image html ./img/trickle.png
 ** \image latex ./img/trickle.png
 **
 ** The Trickle register controls the Trickle Charger. The Key Register must be written with the value 0x9D in order to
 ** enable access to this register.
 **/   
//@{  

/**
 *****************************************************************************
 ** \brief Register 0x20 - Trickle (AMX8X5_REG_TRICKLE) 
 **
 ** This register holds several miscellaneous bits used to control and observe the oscillators.
 **
 ** Trickle Register
 ** ----------------
 **
 ** |   Bits      |    [7:4]  |    [3:2]  | [1:0]  |
 ** |-------------|-----------|-----------|--------|
 ** | Description |    TCS    |   DIODE   |  ROUT  |
 **
 ** Trickle Register Bits
 ** ---------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** | 7:4 | TCS  | A value of 1010 enables the trickle charge function. All other values disable the Trickle Charger.        |
 ** | 3:2 | DIODE| Diode Select. A value of 10 inserts a standard diode into the trickle charge circuit, with a voltage      |
 ** |  ^  |  ^   | drop of 0.6V. A value of 01 inserts a schottky diode into the trickle charge circuit, with a voltage drop |
 ** |  ^  |  ^   | of 0.3V. Other values disable the Trickle Charger.                                                        |
 ** | 1:0 | ROUT | Output Resistor. This selects the output resistor of the trickle charge circuit                           |
 ** |  ^  |  ^   | 00: Disable                                                                                               |
 ** |  ^  |  ^   | 01: 3K Ohm                                                                                                |
 ** |  ^  |  ^   | 10: 6K Ohm                                                                                                |
 ** |  ^  |  ^   | 11: 11K Ohm                                                                                               |
 **   
 **/
    
#define AMX8X5_REG_TRICKLE                      0x20

#define AMX8X5_REG_TRICKLE_TCS_ENABLE_VALUE     0xA                                  ///<see A value of 1010 enables the trickle charge function. All other values disable the Trickle Charger.
    
#define AMX8X5_REG_TRICKLE_TCS_POS              (4)                                  ///<see #AMX8X5_REG_TRICKLE  for more information
#define AMX8X5_REG_TRICKLE_TCS_MSK              (0xF << AMX8X5_REG_TRICKLE_TCS_POS)  ///<see #AMX8X5_REG_TRICKLE  for more information
#define AMX8X5_REG_TRICKLE_DIODE_POS            (2)                                  ///<see #AMX8X5_REG_TRICKLE  for more information
#define AMX8X5_REG_TRICKLE_DIODE_MSK            (0x3 << AMX8X5_REG_TRICKLE_DIODE_POS)///<see #AMX8X5_REG_TRICKLE  for more information
#define AMX8X5_REG_TRICKLE_ROUT_POS             (0)                                  ///<see #AMX8X5_REG_TRICKLE  for more information
#define AMX8X5_REG_TRICKLE_ROUT_MSK             (0x3 << AMX8X5_REG_TRICKLE_ROUT_POS) ///<see #AMX8X5_REG_TRICKLE  for more information
    
//@}

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions21 Register 0x21 - BREF Control (AMX8X5_REG_BREF_CTRL)
 ** 
 ** This register controls the reference voltages used in the Wakeup Control system. The Key Register must
 ** be written with the value 0x9D in order to enable access to this register.
 **/
//@{
     
/**
 *****************************************************************************
 ** \brief Register 0x21 - BREF Control (AMX8X5_REG_BREF_CTRL)
 **
 ** This register controls the reference voltages used in the Wakeup Control system. The Key Register must
 ** be written with the value 0x9D in order to enable access to this register.
 **
 ** BREF Control Register
 ** ---------------------
 **
 ** |   Bits      |    [7:4]  |   [3:0]   |
 ** |-------------|-----------|-----------|
 ** | Description |    BREF   | RESERVED  |
 **
 ** BREF Control Register Bits
 ** --------------------------
 **
 ** | Bit | Name     | Function                                                                                                  |
 ** |-----|----------|-----------------------------------------------------------------------------------------------------------|
 ** | 7:4 | BREF     | A value of 1010 enables the trickle charge function. All other values disable the Trickle Charger.        |
 ** |  ^  |  ^       | 0111: 2.5V VBAT Falling Voltage (TYP), 3.0V VBAT Rising Voltage (TYP)                                     |
 ** |  ^  |  ^       | 1011: 2.1V VBAT Falling Voltage (TYP), 2.5V VBAT Rising Voltage (TYP)                                     |
 ** |  ^  |  ^       | 1101: 1.8V VBAT Falling Voltage (TYP), 2.2V VBAT Rising Voltage (TYP)                                     |
 ** |  ^  |  ^       | 1111: 1.4V VBAT Falling Voltage (TYP), 1.6V VBAT Rising Voltage (TYP)                                     |
 ** | 3:0 | RESERVED | RESERVED                                                                                                  |
 **   
 **/
    
#define AMX8X5_REG_BREF_CTRL                 0x21
#define AMX8X5_REG_BREF_CTRL_BREF_POS        (4)                                  ///<see #AMX8X5_REG_BREF_CTRL  for more information
#define AMX8X5_REG_BREF_CTRL_BREF_MSK        (0xF << AMX8X5_REG_TRICKLE_TCS_POS)  ///<see #AMX8X5_REG_BREF_CTRL  for more information
     
//@}

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions26 Register 0x26 - AFCTRL (AMX8X5_REG_ACAL_FLT)
 ** 
 ** This register holds the enable code for the Autocalibration Filter (AF) filter capacitor connected to the AF
 ** pin. Writing the value 0xA0 to this register enables the AF pin. Writing the value 0x00 to this register
 ** disables the AF pin. No other value may be written to this register. The Configuration Key Register must be
 ** written with the value 0x9D prior to writing the AFCTRL Register.
 **/
//@{       
#define AMX8X5_REG_ACAL_FLT                  0x26 ///< This register holds the enable code for the Autocalibration Filter
//@}

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions27 Register 0x27 - Batmode IO (AMX8X5_REG_BATMODE_IO)
 ** 
 ** This register holds the IOBM bit which controls the enabling and disabling of the I/O interface when a
 ** Brownout Detection occurs. It may only be written if the Configuration Key register contains the value
 ** 0x9D. All undefined bits must be written with 0.
 **/
//@{      
#define AMX8X5_REG_BATMODE_IO                0x27 ///< This register holds the IOBM bit which controls the enabling and disabling of the I/O interface when a Brownout Detection occurs.
//@}
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions28 Register 0x28 - 0x2E: ID Regsiters (AMX8X5_REG_ID0..6)
 ** 
 **/
//@{
#define AMX8X5_REG_ID0                       0x28  ///< ID0 register
#define AMX8X5_REG_ID1                       0x29  ///< ID1 register
#define AMX8X5_REG_ID2                       0x2A  ///< ID2 register
#define AMX8X5_REG_ID3                       0x2B  ///< ID3 register
#define AMX8X5_REG_ID4                       0x2C  ///< ID4 register 
#define AMX8X5_REG_ID5                       0x2D  ///< ID5 register
#define AMX8X5_REG_ID6                       0x2E  ///< ID6 register
//@}
    
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions2F Register 0x2F - Analog Status Register (Read Only) (AMX8X5_REG_OCTRL)
 ** 
 ** This register holds eight status bits which indicate the voltage levels of the VCC and VBAT power inputs.
 **/
//@{
#define AMX8X5_REG_ASTAT                     0x2F ///< This register holds eight status bits which indicate the voltage levels of the VCC and VBAT power inputs.
//@}     
     
/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions30 Register 0x30 - Output Control (AMX8X5_REG_OCTRL)
 ** 
 **/
//@{
    
/**
 ** \brief Register 0x30 - Output Control
 **
 ** This register holds some major control signals.
 **
 ** Output Control Register 
 ** -----------------------
 **
 ** |   Bits      |  7  |  6  |  5  |  4  |  3  |  2  |  1  |  0  |
 ** |-------------|-----|-----|-----|-----|-----|-----|-----|-----|
 ** | Description |WDBM |EXBM |WDDS |EXDS |RSEN |O4EN |O3EN |O1EN |
 **
 ** Output Control Register Bits
 ** ----------------------------
 **
 ** | Bit | Name | Function                                                                                                  |
 ** |-----|------|-----------------------------------------------------------------------------------------------------------|
 ** |  7  | WDBM | If 1, the WDI input is enabled when the AM18X5 is powered from VBAT. If 0, the WDI input is disabled      |
 ** |  ^  |  ^   | when the AM18X5 is powered from VBAT.                                                                     |
 ** |  6  | EXBM | If 1, the EXTI input is enabled when the AM18X5 is powered from VBAT. If 0, the EXTI input is disabled    |
 ** |  ^  |  ^   | when the AM18X5 is powered from VBAT.                                                                     |    
 ** |  5  | WDDS | If 1, the WDI input is disabled when the AM18X5 is in Sleep Mode. If 0, the WDI input is enabled          |
 ** |  ^  |  ^   | when the AM18X5 is in Sleep Mode If WDI is disabled, it will appear as a 1 to the internal logic.         |
 ** |  4  | EXDS | If 1, the EXTI input is disabled when the AM18X5 is in Sleep Mode. If 0, the EXTI input is enabled        |
 ** |  ^  |  ^   | when the AM18X5 is in Sleep Mode. If EXTI is disabled, it will appear as a 1 to the internal logic.       |
 ** |  3  | RSEN | If 1, the nRST output is enabled when the AM18X5 is in Sleep Mode. If 0, the nRST output is completely    |
 ** |  ^  |  ^   | disconnected when the AM18X5 is in Sleep Mode.                                                            |
 ** |  2  | O4EN | If 1, the CLKOUT/nIRQ3 output is enabled when the AM18X5 is in Sleep Mode. If 0, the CLKOUT/              |
 ** |  ^  |  ^   | nIRQ3 output is completely disconnected when the AM18X5 is in Sleep Mode.                                 |
 ** |  1  | O3EN | If 1, the nTIRQ output is enabled when the AM18X5 is in Sleep Mode. If 0, the nTIRQ output is completely  |
 ** |  ^  |  ^   | disconnected when the AM18X5 is in Sleep Mode.                                                            |
 ** |  0  | WRTC | If 1, the FOUT/nIRQ output is enabled when the AM18X5 is in Sleep Mode. If 0, the FOUT/nIRQ output        |
 ** |  ^  |  ^   | is completely disconnected when the AM18X5 is in Sleep Mode.                                              |
 **
 **/
#define AMX8X5_REG_OCTRL                     0x30

#define AMX8X5_REG_OCTRL_WDBM_POS        (7)                                ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_WDBM_MSK        (1 << AMX8X5_REG_OCTRL_WDBM_POS)   ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_EXBM_POS        (6)                                ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_EXBM_MSK        (1 << AMX8X5_REG_OCTRL_EXBM_POS)   ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_WDDS_POS        (5)                                ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_WDDS_MSK        (1 << AMX8X5_REG_OCTRL_WDDS_POS)   ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_EXDS_POS        (4)                                ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_EXDS_MSK        (1 << AMX8X5_REG_OCTRL_EXDS_POS)   ///<see #AMX8X5_REG_OCTRL for more information   
#define AMX8X5_REG_OCTRL_RSEN_POS        (3)                                ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_RSEN_MSK        (1 << AMX8X5_REG_OCTRL_RSEN_POS)   ///<see #AMX8X5_REG_OCTRL for more information 
#define AMX8X5_REG_OCTRL_O4EN_POS        (2)                                ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_O4EN_MSK        (1 << AMX8X5_REG_OCTRL_O4EN_POS)   ///<see #AMX8X5_REG_OCTRL for more information  
#define AMX8X5_REG_OCTRL_O3EN_POS        (1)                                ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_O3EN_MSK        (1 << AMX8X5_REG_OCTRL_O3EN_POS)   ///<see #AMX8X5_REG_OCTRL for more information  
#define AMX8X5_REG_OCTRL_O1EN_POS        (0)                                ///<see #AMX8X5_REG_OCTRL for more information
#define AMX8X5_REG_OCTRL_O1EN_MSK        (1 << AMX8X5_REG_OCTRL_O1EN_POS)   ///<see #AMX8X5_REG_OCTRL for more information   
   
//@} 

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions3F Register 0x3F - Extension Address (AMX8X5_REG_EXTENDED_ADDR)
 ** 
 **/
//@{     
#define AMX8X5_REG_EXTENDED_ADDR             0x3F
//@} 

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions40 Register 0x40 - 0x7F - RAM (AMX8X5_REG_RAM)
 ** 
 ** 64 bytes of RAM space which may be accessed in either I2C or SPI interface mode. The data in the RAM
 ** is held when using battery power. The upper 2 bits of the RAM address are taken from the XADS field, and
 ** the lower 6 bits are taken from the address offset, supporting a total RAM of 256 bytes. The initial values of
 ** the RAM locations are undefined.
 **/
//@{ 
#define AMX8X5_REG_RAM                       0x40
#define AMX8X5_REG_RAM_START                 0x40
#define AMX8X5_REG_RAM_END                   0x7F
//@} 

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_definitions80 Register 0x80 - 0xFF - Alternate RAM (AMX8X5_REG_ALT_RAM)
 ** 
 ** 128 bytes of RAM which may be accessed only in I2C interface mode. The data in the RAM is held when
 ** using battery power. The upper bit of the RAM address is taken from the XADA field, and the lower 7 bits
 ** are taken from the address offset, supporting a total RAM of 256 bytes. The initial values of the RAM
 ** locations are undefined.   
 **/
//@{
#define AMX8X5_REG_ALT_RAM                   0x80
#define AMX8X5_REG_ALT_RAM_START             0x80
#define AMX8X5_REG_ALT_RAM_END               0xFF
//@}
    
//@} // amx8x5_register_definitions
    
// Keys.
#define AMX8X5_CONFIG_KEY_VAL            0xA1 ///<configuration key value

// Modes
#define AMX8X5_12HR_MODE                 0x01 ///<12h mode value
#define AMX8X5_24HR_MODE                 0x02 ///<24h mode value

/**
 *****************************************************************************
 ** \brief BCD format to decimal number conversion
 **
 **/
#define AMX8X5_BCD_TO_DEC(x)             (((0xF0 & (x)) >> 4) * 10 + (0x0F & (x)))
     
#if !defined(ENUM_TO_UINT8)
#define ENUM_TO_UINT8(x)  ((uint8_t)x)
#endif

#if !defined(ENUM_TO_UINT16)
#define ENUM_TO_UINT16(x)  ((uint16_t)x)
#endif   
     
#if !defined(ENUM_TO_UINT32)
#define ENUM_TO_UINT32(x)  ((uint32_t)x)
#endif     

/**
 *****************************************************************************
 ** \brief decimal format to BCD number conversion
 **
 **/
#define AMX8X5_DEC_TO_BCD(x)             ((0xF0 & (((x) / 10) << 4)) | (0x0F & ((x) % 10)))
#define AMX8X5_I2C_ADDRESS               (0xD2 >> 1)

#if AMX8X5_DEBUG == 1
  #if defined(ARDUINO)
    #include <Arduino.h>
    #define AMX8X5_DEBUG_PRINTF(...)        Serial.printf(__VA_ARGS__)
  #else
    #define AMX8X5_DEBUG_PRINTF(...)        printf(__VA_ARGS__)
  #endif
  #define AMX8X5_DEBUG_PRINT_HEX_U8(x)    AMX8X5_DEBUG_PRINTF("0x%02x",x)
  #define AMX8X5_DEBUG_PRINT_HEX_U16(x)   AMX8X5_DEBUG_PRINTF("0x%04x",x)
  #define AMX8X5_DEBUG_PRINT_HEX_U32(x)   AMX8X5_DEBUG_PRINTF("0x%08x",x)
  #define AMX8X5_DEBUG_PRINT_REG(reg,val) AMX8X5_DEBUG_PRINTF("register 0x%02x: 0x%02x, ",reg,val); AMX8X5_DEBUG_PRINT_BIN_U8(val); if (reg <= 0x30) { AMX8X5_DEBUG_PRINTF(" (%s)",astrRegNames[reg]); }; AMX8X5_DEBUG_PRINTF("\r\n")

#else
  #define AMX8X5_DEBUG_PRINTF(...)  
  #define AMX8X5_DEBUG_FUNC_START(func_name);
  #define AMX8X5_DEBUG_PRINT_BIN_U8(x)
  #define AMX8X5_DEBUG_PRINT_BIN_U16(x)
  #define AMX8X5_DEBUG_PRINT_BIN_U32(x)
  #define AMX8X5_DEBUG_PRINT_HEX_U8(x)    
  #define AMX8X5_DEBUG_PRINT_HEX_U16(x)  
  #define AMX8X5_DEBUG_PRINT_HEX_U32(x)  
  #define AMX8X5_DEBUG_PRINT_REG(reg,val) 
  #define AMX8X5_DEBUG_PRINT_BUFFER(x,y,z)
  #define AMX8X5_DEBUG_PRINT_LEVEL()
  #define AMX8X5_FUNC_END(err) err
#endif

/**
 *****************************************************************************
 ** \defgroup amx8x5_register_functions Function Definitions
 ** 
 **/   
 //@{
    
/**
 ******************************************************************************
 ** \brief  This function is controlling the behavior of the OUT/nIRQ pin under 
 **         various power down conditions.
 **
 ** see also #AMX8X5_REG_OCTRL definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnable        If true, the FOUT/nIRQ output is enabled when the AM18X5 
 **                        is in Sleep Mode. If false, the FOUT/nIRQ output
 **                        is completely disconnected when the AM18X5 is in Sleep Mode.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableOutput_FOUT_nIRQ(&stcRtcConfig,true);  //set FOUT/nIRQ as output
 **
 ** Amx8x5_EnableOutput_FOUT_nIRQ(&stcRtcConfig,false); //dont use FOUT/nIRQ as output
 ** @endcode
 **
 **/     
#define Amx8x5_EnableOutput_FOUT_nIRQ(pstcHandle, bEnable) Amx8x5_EnableOutput(pstcHandle, AMX8X5_REG_OCTRL_O1EN_MSK, bEnable)

/**
 ******************************************************************************
 ** \brief  This function is controlling the behavior of the nTIRQ pin under 
 **         various power down conditions.
 **
 ** see also #AMX8X5_REG_OCTRL definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnable        If true, the nTIRQ output is enabled when the AM18X5 
 **                        is in Sleep Mode. If false, the nTIRQ output is completely
 **                        disconnected when the AM18X5 is in Sleep Mode.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableOutput_nTIRQ(&stcRtcConfig,true);  //set nTIRQ as output
 **
 ** Amx8x5_EnableOutput_nTIRQ(&stcRtcConfig,false); //dont use nTIRQ as output
 ** @endcode
 **
 **/    
#define Amx8x5_EnableOutput_nTIRQ(pstcHandle, bEnable) Amx8x5_EnableOutput(pstcHandle, AMX8X5_REG_OCTRL_O3EN_MSK, bEnable)

/**
 ******************************************************************************
 ** \brief  This function is controlling the behavior of the CLKOUT/nIRQ3 pin under 
 **         various power down conditions.
 **
 ** see also #AMX8X5_REG_OCTRL definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnable        If true, the CLKOUT/nIRQ3 output is enabled when the AM18X5 
 **                        is in Sleep Mode. If false, the CLKOUT/nIRQ3 output 
 **                        is completely disconnected when the AM18X5 is in Sleep Mode.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableOutput_CLKOUT_nIRQ3(&stcRtcConfig,true);  //set CLKOUT/nIRQ3 as output
 **
 ** Amx8x5_EnableOutput_CLKOUT_nIRQ3(&stcRtcConfig,false); //dont use CLKOUT/nIRQ3 as output
 ** @endcode
 **
 **/    
#define Amx8x5_EnableOutput_CLKOUT_nIRQ3(pstcHandle, bEnable) Amx8x5_EnableOutput(pstcHandle, AMX8X5_REG_OCTRL_O4EN_MSK, bEnable)

/**
 ******************************************************************************
 ** \brief  This function is controlling the behavior of the nRST pin under 
 **         various power down conditions.
 **
 ** see also #AMX8X5_REG_OCTRL definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnable        If true, the nRST output is enabled when the AM18X5 
 **                        is in Sleep Mode. If false, the nRST output is completely
 **                        disconnected when the AM18X5 is in Sleep Mode.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableOutput_nRST(&stcRtcConfig,true);  //set nRST as output
 **
 ** Amx8x5_EnableOutput_nRST(&stcRtcConfig,false); //dont use nRST as output
 ** @endcode
 **
 **/
#define Amx8x5_EnableOutput_nRST(pstcHandle, bEnable) Amx8x5_EnableOutput(pstcHandle, AMX8X5_REG_OCTRL_RSEN_MSK, bEnable)
 
//@}
    
/*****************************************************************************/
/* Global type definitions ('typedef')                                        */
/*****************************************************************************/

#if !defined(__EN_RESULT_T__)
#define __EN_RESULT_T__
/** generic error codes */
typedef enum en_result
{
    Ok                          = 0,  ///< No error
    Error                       = 1,  ///< Non-specific error code
    ErrorAddressAlignment       = 2,  ///< Address alignment does not match
    ErrorAccessRights           = 3,  ///< Wrong mode (e.g. user/system) mode is set
    ErrorInvalidParameter       = 4,  ///< Provided parameter is not valid
    ErrorOperationInProgress    = 5,  ///< A conflicting or requested operation is still in progress
    ErrorInvalidMode            = 6,  ///< Operation not allowed in current mode
    ErrorUninitialized          = 7,  ///< Module (or part of it) was not initialized properly
    ErrorBufferFull             = 8,  ///< Circular buffer can not be written because the buffer is full
    ErrorTimeout                = 9,  ///< Time Out error occurred (e.g. I2C arbitration lost, Flash time-out, etc.)
    ErrorNotReady               = 10, ///< A requested final state is not reached
    OperationInProgress         = 11  ///< Indicator for operation in progress (e.g. ADC conversion not finished, DMA channel used, etc.)
} en_result_t;
#endif

/**
 ******************************************************************************
 ** \brief RTC communication mode (I2C/SPI)
 **
 ** used to set the RTC communication mode
 **
 ******************************************************************************/
typedef enum en_amx8x5_communication_mode
{
    AMx8x5ModeSPI = 0, ///< SPI mode
    AMx8x5ModeI2C = 1, ///< I2C mode
} en_amx8x5_communication_mode_t;

/**
 ******************************************************************************
 ** \brief RTC type
 **
 ** used to set the type of used RTC (with/without power management)
 **
 ******************************************************************************/
typedef enum en_amx8x5_rtc_type
{
    AMx8x5Type0805 = 0x0805, ///< RTC type AM0805AQ without power management (I2C)
    AMx8x5Type0815 = 0x0815, ///< RTC type AM0815AQ without power management (SPI)
    AMx8x5Type1805 = 0x1805, ///< RTC type AM1805AQ with power management (I2C)
    AMx8x5Type1815 = 0x1815,  ///< RTC type AM1815AQ with power management (SPI)
    AMx8x5TypeI2C = 0x0805, ///< RTC type AM0805AQ without power management (I2C)
    AMx8x5TypeSPI = 0x0815, ///< RTC type AM0815AQ without power management (SPI)
    AMx8x5TypeRV1805 = 0x1805, ///< RTC type RV1805 with power management (I2C)
    AMx8x5TypeI2CPowerManagement = 0x1805, ///< RTC type AM1805AQ with power management (I2C)
    AMx8x5TypeSPIPowerManagement = 0x1815  ///< RTC type AM1815AQ with power management (SPI)
} en_amx8x5_rtc_type_t;

/**
 ******************************************************************************
 ** \brief Calibration mode
 **
 ******************************************************************************/
typedef enum en_amx8x5_calibration_mode
{
    AMx8x5ModeCalibrateXT = 0, ///< Calibration mode crystal
    AMx8x5ModeCalibrateRC = 1, ///< Calibration mode RC clock
} en_amx8x5_calibration_mode_t;

/**
 ******************************************************************************
 ** \brief Alarm repeat configurations
 **
 ******************************************************************************/
typedef enum en_amx8x5_alarm_repeat
{
    AMx8x5AlarmDisabled = 0,    ///< Alarm disabled
    AMx8x5AlarmYear = 1,        ///< Alarm by year
    AMx8x5AlarmMonth = 2,       ///< Alarm by month
    AMx8x5AlarmWeek = 3,        ///< Alarm by week
    AMx8x5AlarmDay = 4,         ///< Alarm by day
    AMx8x5AlarmHour = 5,        ///< Alarm by hour
    AMx8x5AlarmMinute = 6,      ///< Alarm by minute
    AMx8x5AlarmSecond = 7,      ///< Alarm by second
    AMx8x5Alarm10thSecond = 8,  ///< Alarm by 10th second
    AMx8x5Alarm100thSecond = 9, ///< Alarm by 100th second
} en_amx8x5_alarm_repeat_t;

/**
 ******************************************************************************
 ** \brief Interrupt Mode.
 **
 ** This controls the duration of the nAIRQ interrupt as shown below. The interrupt output always goes
 ** high when the corresponding flag in the Status Register is cleared. In order to minimize current
 ** drawn by the AM18X5 this field should be kept at 0x3.
 ** Used in the Interrupt Mask register (#AMX8X5_REG_INT_MASK)
 **
 ******************************************************************************/
typedef enum en_amx8x5_interrupt_mode
{
    AMx8x5InterruptModeLevel = 0,        ///<Level (static) for both XT mode and RC mode.
    AMx8x5InterruptModePulseShort = 1,   ///<1/8192 seconds pulse for XT mode. 1/64 seconds for RC mode.
    AMx8x5InterruptModePulseMedium = 2,  ///<1/64 seconds pulse for both XT mode and RC mode
    AMx8x5InterruptModePulseLong = 3     ///<1/4 seconds pulse for both XT mode and RC mode.
} en_amx8x5_interrupt_mode_t;

/**
 ******************************************************************************
 ** \brief Interrupt pin setting
 **
 ******************************************************************************/
typedef enum en_amx8x5_interrupt_pin
{
    AMx8x5InterruptPinInternal = 0,      ///<internal flag only
    AMx8x5InterruptIrq = 1,              ///<FOUT/nIRQ
    AMx8x5InterruptIrq2 = 2              ///<PSW/nIRQ2
} en_amx8x5_interrupt_pin_t;


/**
 ******************************************************************************
 ** \brief FOUT/nIRQ Pin Control
 **
 ** used to set out1 mode in Control2 register (#AMX8X5_REG_CONTROL_2)
 **
 ******************************************************************************/
typedef enum en_amx8x5_out1_mode
{
    AMx8x5Out1nIRQAtIrqElseOut = 0,              ///<nIRQ if at least one interrupt is enabled, else OUT
    AMx8x5Out1SwqIfSqweElseOut = 1,              ///<SQW if SQWE = 1, else OUT
    AMx8x5Out1SwqIfSqweElsenIRQAtIrqElseOut = 2, ///<SQW if SQWE = 1, else nIRQ if at least one interrupt is enabled, else OUT
    AMx8x5Out1nAIRQIfAieElseOut = 3,             ///<nAIRQ if AIE is set, else OUT
} en_amx8x5_out1_mode_t;


/**
 ******************************************************************************
 ** \brief PSW/nIRQ2 Pin Control
 **
 ** used to set out2 mode in Control2 register (#AMX8X5_REG_CONTROL_2)
 **
 ******************************************************************************/
typedef enum en_amx8x5_out2_mode
{
    AMx8x5Out2nIRQAtIrqElseOutB = 0,              ///<nIRQ if at least one interrupt is enabled, else OUTB
    AMx8x5Out2SwqIfSqweElseOutB = 1,              ///<SQW if SQWE = 1, else OUTB
    AMx8x5Out2nAIRQIfAieElseOutB = 3,             ///<nAIRQ if AIE is set, else OUTB
    AMx8x5Out2TIRQIfTieElseOutB = 4,              ///<TIRQ if TIE is set, else OUTB
    AMx8x5Out2nTIRQIfTieElseOutB = 5,             ///<nTIRQ if TIE is set, else OUTB
    AMx8x5Out2Sleep = 6,                          ///<SLEEP
    AMx8x5Out2OutB = 7                            ///<OUTB
} en_amx8x5_out2_mode_t;

/**
 ******************************************************************************
 ** \brief Sleep mode (AM18x5 only).
 **
 ******************************************************************************/
typedef enum en_amx8x5_sleep_mode
{
    AMx8x5nRstLowInSleep = 0,              ///< nRST is pulled low in sleep mode
    AMx8x5PswIrq2HighInSleep = 1,          ///< PSW/nIRQ2 is pulled high on a sleep
    AMx8x5nRstLoPswIrq2HighInSleep = 2,    ///< nRST pulled low and PSW/nIRQ2 pulled high on sleep
} en_amx8x5_sleep_mode_t;

/**
 ******************************************************************************
 ** \brief Select an oscillator mode.
 **
 ******************************************************************************/
typedef enum en_amx8x5_osc_select
{
    AMx8x5Xt32KHzNoSwitch = 0,               ///< 32 KHz XT oscillator, no automatic oscillator switching
    AMx8x5Xt32KHzSwitchRcOnBat = 1,          ///< 32 KHz XT oscillator, automatic oscillator switching to RC on switch to battery power
    AMx8x5nRc128Hz = 2,                      ///< 128 Hz RC oscillator
} en_amx8x5_osc_select_t;

/**
 ******************************************************************************
 ** \brief Periode range
 **
 ******************************************************************************/
typedef enum en_amx8x5_periode_range
{
    AMx8x5PeriodeUs = 0,        ///<periode in us
    AMx8x5PeriodeSeconds = 1,   ///<periode in seconds
} en_amx8x5_periode_range_t;

/**
 ******************************************************************************
 ** \brief Countdown Interrupt Output Type
 **
 ******************************************************************************/
typedef enum en_amx8x5_countdown_interrupt_output
{
    AMx8x5RepeatModeSingleLevel = 0,        ///<generate a single level interrupt
    AMx8x5RepeatModeRepeatedPulseShort = 1, ///<generate a repeated pulsed interrupt, 1/4096 s (XT mode), 1/128 s (RC mode), en8Range must be AMx8x5PeriodeUs
    AMx8x5RepeatModeSinglePulseShort = 2,   ///<generate a single pulsed interrupt, 1/4096 s (XT mode), 1/128 s (RC mode), en8Range must be AMx8x5PeriodeUs
    AMx8x5RepeatModeRepeatedPulseMedium = 3,///<generate a repeated pulsed interrupt, 1/128 s, en8Range must be AMx8x5PeriodeUs
    AMx8x5RepeatModeSinglePulseMedium = 4,  ///<generate a single pulsed interrupt, 1/128 s, en8Range must be AMx8x5PeriodeUs
    AMx8x5RepeatModeRepeatedPulseLong = 5,  ///<generate a repeated pulsed interrupt, 1/64 s, en8Range must be AMx8x5PeriodeSeconds
    AMx8x5RepeatModeSinglePulseLong = 6     ///<generate a single pulsed interrupt, 1/64 s, en8Range must be AMx8x5PeriodeSeconds
} en_amx8x5_countdown_interrupt_output_t;


/**
 ******************************************************************************
 ** \brief Countdown Interrupt Output Pin
 **
 ******************************************************************************/
typedef enum en_amx8x5_countdown_interrupt_pin
{
    AMx8x5CountdownInterruptPinDisable = 0,                     ///<disable the countdown timer
    AMx8x5CountdownInterruptPinnTIRQLow = 1,                    ///<generate an interrupt on nTIRQ only, asserted low
    AMx8x5CountdownInterruptPinFOUTnIRQLownTIRQLow = 2,         ///<generate an interrupt on FOUT/nIRQ and nTIRQ, both asserted low
    AMx8x5CountdownInterruptPinPSWnIRQ2LownTIRQLow = 3,         ///<generate an interrupt on PSW/nIRQ2 and nTIRQ, both asserted low
    AMx8x5CountdownInterruptPinCLKOUTnIRQ3LownTIRQLow = 4,      ///<generate an interrupt on CLKOUT/nIRQ3 and nTIRQ, both asserted low
    AMx8x5CountdownInterruptPinCLKOUTnIRQ3HighnTIRQLow = 5,     ///<generate an interrupt on CLKOUT/nIRQ3 (asserted high) and nTIRQ (asserted low)
} en_amx8x5_countdown_interrupt_pin_t;

/**
 ******************************************************************************
 ** \brief pin to generate the watchdog signal
**
 ******************************************************************************/
typedef enum en_amx8x5_watchdog_interrupt_pin
{
    AMx8x5WatchdogInterruptPinDisable = 0,   ///<disable WDT
    AMx8x5WatchdogInterruptPinFOUTnIRQ = 1,  ///<generate an interrupt on FOUT/nIRQ
    AMx8x5WatchdogInterruptPinPSWnIRQ2 = 2,  ///<generate an interrupt on PSW/nIRQ2
    AMx8x5WatchdogInterruptPinnRST = 3,      ///<generate a reset on nRST (AM18xx only)
} en_amx8x5_watchdog_interrupt_pin_t;

/**
 ******************************************************************************
 ** \brief Auto Calibration Period
**
 ******************************************************************************/
typedef enum en_amx8x5_autocalibration_period
{
    AMx8x5AutoCalibrationPeriodDisable = 0,         ///<disable autocalibration
    AMx8x5AutoCalibrationPeriodSingleCycle = 1,     ///<execute a single autocalibration cycle
    AMx8x5AutoCalibrationPeriodCycleSecods1024 = 2, ///<execute a cycle every 1024 seconds (~17 minutes)
    AMx8x5AutoCalibrationPeriodCycleSecods512 = 3,  ///<execute a cycle every 512 seconds (~8.5 minutes)
} en_amx8x5_autocalibration_period_t;

/**
 ******************************************************************************
 ** \brief Diode Select
 **
 ** see also field DIODE of #AMX8X5_REG_TRICKLE
 **
 ******************************************************************************/
typedef enum en_amx8x5_trickle_diode
{
    AMx8x5TrickleDiodeDisabled = 0, ///< disable the Trickle Charger.
    AMx8x5TrickleDiodeSchottky = 1, ///< inserts a schottky diode into the trickle charge circuit, with a voltage drop of 0.3V
    AMx8x5TrickleDiodeNormal = 2,   ///< inserts a standard diode into the trickle charge circuit, with a voltage drop of 0.6V
} en_amx8x5_trickle_diode_t;

/**
 ******************************************************************************
 ** \brief Output Resistor
 **
 ** see also field ROUT of #AMX8X5_REG_TRICKLE
 **
 ******************************************************************************/
typedef enum en_amx8x5_trickle_resistor
{
    AMx8x5TrickleResistorDisabled = 0, ///< disable the Trickle Charger.
    AMx8x5TrickleResistor3K = 1,       ///< this selects 3K Ohm as the output resistor of the trickle charge circuit
    AMx8x5TrickleResistor6K = 2,       ///< this selects 6K Ohm as the output resistor of the trickle charge circuit
    AMx8x5TrickleResistor11K = 3,      ///< this selects 11K Ohm as the output resistor of the trickle charge circuit
} en_amx8x5_trickle_resistor_t;

/**
 ******************************************************************************
 ** \brief VBAT Reference Voltage
 **
 ** see also field BREF of #AMX8X5_REG_BREF_CTRL
 **
 ******************************************************************************/
typedef enum en_amx8x5_bat_reference
{
     AMx8x5BatReferenceFalling25V_Rising30V = 0x7, ///< VBAT Falling Voltage 2.5V, VBAT Rising Voltage 3.0V
     AMx8x5BatReferenceFalling21V_Rising25V = 0xB, ///< VBAT Falling Voltage 2.1V, VBAT Rising Voltage 2.5V
     AMx8x5BatReferenceFalling18V_Rising22V = 0xD, ///< VBAT Falling Voltage 1.8V, VBAT Rising Voltage 2.2V
     AMx8x5BatReferenceFalling14V_Rising16V = 0xF, ///< VBAT Falling Voltage 1.4V, VBAT Rising Voltage 1.6V
} en_amx8x5_bat_reference_t;

/**
 ******************************************************************************
 ** \brief Function type for SPI write
 **
 ** For writing SPI data, following wrapper function has to be implemented:
 ** @code 
 ** static int SPIWrite(void* pHandle, uint32_t u32Chipselect, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
 ** {
 **     ...
 ** }
 ** @endcode
 **
 ** see also @link amx8x5_module_spitransfer SPI transfer routines example @endlink or @link amx8x5_module_spiinit SPI initialization example @endlink
 **/
typedef int (*pfn_spi_write_register) (void* pHandle, uint32_t u32ChipSelect, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len); 


/**
 ******************************************************************************
 ** \brief Function type for I2C write
 **
 ** For writing I2C data, following wrapper function has to be implemented:
 ** @code 
 ** static int I2CWrite(void* pHandle, uint32_t u32Address, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
 ** {
 **     ...
 ** }
 ** @endcode
 **
 ** see also @link amx8x5_module_i2ctransfer I2C transfer routines example @endlink or @link amx8x5_module_i2cinit I2C initialization example @endlink
 **/
typedef int (*pfn_i2c_write_register) (void* pHandle, uint32_t u32Address,    uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len); 


/**
 ******************************************************************************
 ** \brief Function type for SPI read
 **
 ** For reading SPI data, following wrapper function has to be implemented:
 ** @code 
 ** static int SPIRead(void* pHandle, uint32_t u32Chipselect, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
 ** {
 **     ...
 ** }
 ** @endcode
 **
 ** see also @link amx8x5_module_spitransfer SPI transfer routines example @endlink or @link amx8x5_module_spiinit SPI initialization example @endlink
 **/
typedef int (*pfn_spi_read_register)  (void* pHandle, uint32_t u32ChipSelect, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len); 

/**
 ******************************************************************************
 ** \brief Function type for I2C read
 **
 ** For reading I2C data, following wrapper function has to be implemented:
 ** @code 
 ** static int I2CRead(void* pHandle, uint32_t u32Chipselect, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
 ** {
 **     ...
 ** }
 ** @endcode
 **
 ** see also @link amx8x5_module_i2ctransfer I2C transfer routines example @endlink or @link amx8x5_module_i2cinit I2C initialization example @endlink
 **/
typedef int (*pfn_i2c_read_register)  (void* pHandle, uint32_t u32Address,    uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len); 

/* ========================================  Start of section using anonymous unions  ======================================== */
#if defined (__CC_ARM)
  #pragma push
  #pragma anon_unions
#elif defined (__ICCARM__)
  #pragma language=extended
#elif defined(__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wc11-extensions"
  #pragma clang diagnostic ignored "-Wreserved-id-macro"
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning 586
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#else
  #warning Not supported compiler type
#endif

/**
 ******************************************************************************
 ** \brief RTC Handle and configuration
 **
 ** used as RTC handle, can be const data
 **
 ******************************************************************************/
typedef struct stc_amx8x5_handle
{
    en_amx8x5_communication_mode_t enMode; ///<SPI or I2C mode
    void* pHandle;                         ///<Peripheral handle
    en_amx8x5_rtc_type_t enRtcType;        ///<RTC type
    /// \brief SPI Chipselect Number or I2C address
    union 
    {
        uint32_t u32ChipSelect;                ///<Chip Select number (unused in I2C mode).
        uint32_t u32Address;                   ///<Address for I2C communication. (unused in SPI mode)
    };
    /// \brief SPI write or I2C write function pointer
    union 
    {
        pfn_spi_write_register pfnWriteSpi; ///<SPI write routine
        pfn_i2c_write_register pfnWriteI2C; ///<I2C write routine
    };
    /// \brief SPI read or I2C read function pointer
    union 
    {
        pfn_spi_read_register pfnReadSpi; ///<SPI read routine
        pfn_i2c_read_register pfnReadI2C; ///<I2C read routine
    };
} stc_amx8x5_handle_t;

/* =========================================  End of section using anonymous unions  ========================================= */
#if defined (__CC_ARM)
  #pragma pop
#elif defined (__ICCARM__)
  /* leave anonymous unions enabled */
#elif (__ARMCC_VERSION >= 6010050)
  #pragma clang diagnostic pop
#elif defined (__GNUC__)
  /* anonymous unions are enabled by default */
#elif defined (__TMS470__)
  /* anonymous unions are enabled by default */
#elif defined (__TASKING__)
  #pragma warning restore
#elif defined (__CSMC__)
  /* anonymous unions are enabled by default */
#endif

/**
 ******************************************************************************
 ** \brief RTC time structure
 **
 **
 ******************************************************************************/
typedef struct stc_amx8x5_time
{
    uint8_t u8Hundredth; ///< Hundedth
    uint8_t u8Second;    ///< Second
    uint8_t u8Minute;    ///< Minute
    uint8_t u8Hour;      ///< Hour
    uint8_t u8Date;      ///< Day / Date
    uint8_t u8Weekday;   ///< Weekday
    uint8_t u8Month;     ///< Month
    uint8_t u8Year;      ///< Year
    uint8_t u8Century;   ///< Century
    uint8_t u8Mode;      ///< Mode
} stc_amx8x5_time_t;



/*****************************************************************************/
/* Global variable declarations ('extern', definition in C source)           */
/*****************************************************************************/

extern stc_amx8x5_time_t stcSysTime;

/*****************************************************************************/
/* Global function prototypes ('extern', definition in C source)             */
/*****************************************************************************/

/**
 *****************************************************************************
 ** \addtogroup amx8x5_register_functions Function Definitions
 **
 ** Provided functions of AMx8x5:
 **
 ** Basic functionality:
 ** - Amx8x5_Init()
 ** - Amx8x5_Reset()
 ** - Amx8x5_GetTime()
 ** - Amx8x5_SetTime()
 ** - Amx8x5_SetCalibrationValue()
 ** - Amx8x5_SetAlarm()
 ** - Amx8x5_Stop()
 **
 ** Advanced functionality:
 ** - Amx8x5_SetSleepMode()
 ** - Amx8x5_SetWatchdog()
 ** - Amx8x5_GetExtensionAddress()
 ** - Amx8x5_SetSquareWaveOutput()
 ** - Amx8x5_SelectOscillatorMode()
 ** - Amx8x5_SetCountdown()
 ** - Amx8x5_SetAutocalibration()
 ** - Amx8x5_EnableTrickleCharger() 
 ** - Amx8x5_SetBatteryReferenceVoltage()
 ** - Amx8x5_RamRead()
 ** - Amx8x5_RamWrite()
 ** - Amx8x5_CtrlOutB()
 ** - Amx8x5_CtrlOut()
 ** - Amx8x5_SetResetPolarity()
 ** - Amx8x5_AutoResetStatus()
 ** - Amx8x5_SetPswHighCurrent()
 ** - Amx8x5_UsenExtrAsReset()
 ** - Amx8x5_SetOut1Mode()
 ** - Amx8x5_SetOut2Mode()
 ** - Amx8x5_EnableIrqXt1OnExti()
 ** - Amx8x5_EnableIrqXt2OnWdi()
 ** - Amx8x5_EnableIrqAlarm()
 ** - Amx8x5_EnableIrqTimer()
 ** - Amx8x5_EnableIrqBatteryLow()
 ** - Amx8x5_EnableOutput_CLKOUT_nIRQ3()
 ** - Amx8x5_EnableOutput_FOUT_nIRQ()
 ** - Amx8x5_EnableOutput_nRST()
 ** - Amx8x5_EnableOutput_nTIRQ()
 **
 ** Provided direct register access functions (valid for I2C and SPI):
 ** - Amx8x5_ClearRegister() - clear bits in a register
 ** - Amx8x5_SetRegister() - set bits in register
 ** - Amx8x5_ReadByte() - read one byte
 ** - Amx8x5_ReadBytes() - read multiple bytes
 ** - Amx8x5_WriteByte() - write one byte
 ** - Amx8x5_WriteBytes() - read multiple bytes
 **/   
 //@{

en_result_t Amx8x5_Init(stc_amx8x5_handle_t* pstcHandle);
en_result_t Amx8x5_Reset(stc_amx8x5_handle_t* pstcHandle);
en_result_t Amx8x5_GetTime(stc_amx8x5_handle_t* pstcHandle, stc_amx8x5_time_t** ppstcTime);
int16_t Amx8x5_GetHundredth(stc_amx8x5_handle_t* pstcHandle);
int16_t Amx8x5_GetSecond(stc_amx8x5_handle_t* pstcHandle);
int16_t Amx8x5_GetMinute(stc_amx8x5_handle_t* pstcHandle);
int16_t Amx8x5_GetHour(stc_amx8x5_handle_t* pstcHandle);
int16_t Amx8x5_GetDay(stc_amx8x5_handle_t* pstcHandle);
int16_t Amx8x5_GetWeekday(stc_amx8x5_handle_t* pstcHandle);
int16_t Amx8x5_GetMonth(stc_amx8x5_handle_t* pstcHandle);
int16_t Amx8x5_GetYear(stc_amx8x5_handle_t* pstcHandle);
int16_t Amx8x5_GetCentury(stc_amx8x5_handle_t* pstcHandle);

en_result_t Amx8x5_SetTime(stc_amx8x5_handle_t* pstcHandle, stc_amx8x5_time_t* pstcTime, bool bProtect);
en_result_t Amx8x5_SetCalibrationValue(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_calibration_mode_t enMode, int32_t iAdjust);
en_result_t Amx8x5_SetAlarm(stc_amx8x5_handle_t* pstcHandle, stc_amx8x5_time_t* pstcTime, en_amx8x5_alarm_repeat_t enModeRepeat, en_amx8x5_interrupt_mode_t enModeIrq, en_amx8x5_interrupt_pin_t enModePin);
en_result_t Amx8x5_Stop(stc_amx8x5_handle_t* pstcHandle, bool bStop);

en_result_t Amx8x5_CtrlOutB(stc_amx8x5_handle_t* pstcHandle, bool bOnOff);
en_result_t Amx8x5_CtrlOut(stc_amx8x5_handle_t* pstcHandle, bool bOnOff);
en_result_t Amx8x5_SetResetPolarity(stc_amx8x5_handle_t* pstcHandle, bool bAssertHigh);
en_result_t Amx8x5_AutoResetStatus(stc_amx8x5_handle_t* pstcHandle, bool bEnabled);
en_result_t Amx8x5_SetPswHighCurrent(stc_amx8x5_handle_t* pstcHandle, bool bEnabled);
en_result_t Amx8x5_UsenExtrAsReset(stc_amx8x5_handle_t* pstcHandle, bool bEnabled);
en_result_t Amx8x5_SetOut1Mode(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_out1_mode_t enMode);
en_result_t Amx8x5_SetOut2Mode(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_out2_mode_t enMode);
en_result_t Amx8x5_EnableIrqXt1OnExti(stc_amx8x5_handle_t* pstcHandle, bool bEnabled);
en_result_t Amx8x5_EnableIrqXt2OnWdi(stc_amx8x5_handle_t* pstcHandle, bool bEnabled);
en_result_t Amx8x5_EnableIrqAlarm(stc_amx8x5_handle_t* pstcHandle, bool bEnabled);
en_result_t Amx8x5_EnableIrqTimer(stc_amx8x5_handle_t* pstcHandle, bool bEnabled);
en_result_t Amx8x5_EnableIrqBatteryLow(stc_amx8x5_handle_t* pstcHandle, bool bEnabled);
en_result_t Amx8x5_EnableTrickleCharger(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_trickle_diode_t enDiode, en_amx8x5_trickle_resistor_t enResistor, bool bEnable);
en_result_t Amx8x5_SetBatteryReferenceVoltage(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_bat_reference_t enBref);


en_result_t Amx8x5_SetWatchdog(stc_amx8x5_handle_t* pstcHandle, uint32_t u32Period, en_amx8x5_watchdog_interrupt_pin_t enPin);
en_result_t Amx8x5_SetSleepMode(stc_amx8x5_handle_t* pstcHandle, uint8_t ui8Timeout, en_amx8x5_sleep_mode_t enMode);
en_result_t Amx8x5_GetExtensionAddress(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Address, uint8_t* pu8ExtensionAddress);
en_result_t Amx8x5_SetSquareWaveOutput(stc_amx8x5_handle_t* pstcHandle, uint8_t u8SQFS, uint8_t u8PinMsk);
en_result_t Amx8x5_SelectOscillatorMode(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_osc_select_t enSelect);
en_result_t Amx8x5_SetCountdown(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_periode_range_t enRange, int32_t iPeriod, en_amx8x5_countdown_interrupt_output_t enRepeat, en_amx8x5_countdown_interrupt_pin_t enPin);
en_result_t Amx8x5_SetAutocalibration(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_autocalibration_period_t enPeriod);
en_result_t Amx8x5_RamRead(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Address, uint8_t* pu8Data);
en_result_t Amx8x5_RamWrite(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Address, uint8_t u8Data);

en_result_t Amx8x5_ClearRegister(stc_amx8x5_handle_t*, uint8_t u8Address, uint8_t u8Mask);
en_result_t Amx8x5_SetRegister(stc_amx8x5_handle_t*, uint8_t u8Address, uint8_t u8Mask);
en_result_t Amx8x5_ReadByte(stc_amx8x5_handle_t*, uint8_t u8Register, uint8_t* pu8Value);
en_result_t Amx8x5_ReadBytes(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length);
en_result_t Amx8x5_WriteByte(stc_amx8x5_handle_t*, uint8_t u8Register, uint8_t u8Value);
en_result_t Amx8x5_WriteBytes(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length);


//@}

//#ifdef __cplusplus
//}
//#endif

#ifdef __cplusplus

class AMx8x5
  {
    public:
      typedef stc_amx8x5_time_t stcTime;
      typedef en_amx8x5_calibration_mode_t enCalibrationMode;
      typedef en_amx8x5_alarm_repeat_t enAlarmRepeat;
      typedef en_amx8x5_interrupt_mode_t enInterruptMode;
      typedef en_amx8x5_interrupt_pin_t enInterruptPin;
      typedef en_amx8x5_trickle_diode_t enTrickleDiode;
      typedef en_amx8x5_trickle_resistor_t enTrickleResistor;
      typedef en_amx8x5_bat_reference_t enBatReference;
      typedef en_amx8x5_watchdog_interrupt_pin_t enWatchdogInterruptPin;
      typedef en_amx8x5_sleep_mode_t enSleepMode;
      typedef en_amx8x5_osc_select_t enOscSelect;
      typedef en_amx8x5_periode_range_t enPeriodeRange;
      typedef en_amx8x5_countdown_interrupt_output_t enCountdownInterruptOutput;
      typedef en_amx8x5_countdown_interrupt_pin_t enCountdownInterruptPin;
      typedef en_amx8x5_autocalibration_period_t enAutocalibrationPeriod;
      typedef en_amx8x5_out1_mode_t enOut1Mode;
      typedef en_amx8x5_out2_mode_t enOut2Mode;
      typedef stc_amx8x5_handle_t stcHandle;
      typedef en_amx8x5_communication_mode_t enCommunicationMode;
      typedef en_amx8x5_rtc_type_t enRtcType;
      typedef en_result_t enResult;

      AMx8x5()
      {
        stcRtcConfig.enMode = AMx8x5ModeI2C;
        stcRtcConfig.enRtcType = AMx8x5Type1805;
        stcRtcConfig.pHandle = (void*)-1;
        stcRtcConfig.u32Address = 0x69;
      }

      AMx8x5(AMx8x5::enRtcType enType)
      {
        if (enType & 0x0010)
        {
          stcRtcConfig.enMode = AMx8x5ModeSPI;
        }
        else
        {
          stcRtcConfig.enMode = AMx8x5ModeI2C;
        }
        stcRtcConfig.enRtcType = enType;
        stcRtcConfig.pHandle = (void*)-1;
        stcRtcConfig.u32Address = 0x69;
      }

      AMx8x5(AMx8x5::enRtcType enType, void* pHandle)
      {
        if (enType & 0x0010)
        {
          stcRtcConfig.enMode = AMx8x5ModeSPI;
        }
        else
        {
          stcRtcConfig.enMode = AMx8x5ModeI2C;
        }
        stcRtcConfig.enRtcType = enType;
        stcRtcConfig.pHandle = pHandle;
        stcRtcConfig.u32Address = 0x69;
      }

      AMx8x5(AMx8x5::enCommunicationMode enMode)
      {
        stcRtcConfig.enMode = enMode;
        stcRtcConfig.enRtcType = AMx8x5Type1805;
        stcRtcConfig.pHandle = (void*)-1;
        stcRtcConfig.u32Address = 0x69;
      }

      /**
      * @brief Enable RTC
      * @return whether success.
      */
      bool begin(void);

      bool end(void);

      void update(void);

      AMx8x5::enResult init(AMx8x5::stcHandle* pstcHandle);
      AMx8x5::enResult reset(void);
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
      AMx8x5::enResult setCalibrationValue(AMx8x5::enCalibrationMode enMode, int32_t iAdjust);
      AMx8x5::enResult setAlarm(AMx8x5::stcTime* pstcTime, AMx8x5::enAlarmRepeat enModeRepeat, AMx8x5::enInterruptMode enModeIrq, AMx8x5::enInterruptPin enModePin);
      AMx8x5::enResult stop(bool bStop);

      AMx8x5::enResult ctrlOutB(bool bOnOff);
      AMx8x5::enResult ctrlOut(bool bOnOff);
      AMx8x5::enResult setResetPolarity(bool bAssertHigh);
      AMx8x5::enResult autoResetStatus(bool bEnabled);
      AMx8x5::enResult setPswHighCurrent(bool bEnabled);
      AMx8x5::enResult usenExtrAsReset(bool bEnabled);
      AMx8x5::enResult setOut1Mode(AMx8x5::enOut1Mode enMode);
      AMx8x5::enResult setOut2Mode(AMx8x5::enOut2Mode enMode);
      AMx8x5::enResult enableIrqXt1OnExti(bool bEnabled);
      AMx8x5::enResult enableIrqXt2OnWdi(bool bEnabled);
      AMx8x5::enResult enableIrqAlarm(bool bEnabled);
      AMx8x5::enResult enableIrqTimer(bool bEnabled);
      AMx8x5::enResult enableIrqBatteryLow(bool bEnabled);
      AMx8x5::enResult enableTrickleCharger(AMx8x5::enTrickleDiode enDiode, AMx8x5::enTrickleResistor enResistor, bool bEnable);
      AMx8x5::enResult setBatteryReferenceVoltage(AMx8x5::enBatReference enBref);

      AMx8x5::enResult setWatchdog(uint32_t u32Period, AMx8x5::enWatchdogInterruptPin enPin);
      AMx8x5::enResult setSleepMode(uint8_t ui8Timeout, AMx8x5::enSleepMode enMode);
      AMx8x5::enResult getExtensionAddress(uint8_t u8Address, uint8_t* pu8ExtensionAddress);
      AMx8x5::enResult setSquareWaveOutput(uint8_t u8SQFS, uint8_t u8PinMsk);
      AMx8x5::enResult selectOscillatorMode(AMx8x5::enOscSelect enSelect);
      AMx8x5::enResult setCountdown(AMx8x5::enPeriodeRange enRange, int32_t iPeriod, AMx8x5::enCountdownInterruptOutput enRepeat, AMx8x5::enCountdownInterruptPin enPin);
      AMx8x5::enResult setAutocalibration(AMx8x5::enAutocalibrationPeriod enPeriod);
      AMx8x5::enResult ramRead(uint8_t u8Address, uint8_t* pu8Data);
      AMx8x5::enResult ramWrite(uint8_t u8Address, uint8_t u8Data);
      AMx8x5::enResult clearRegister(uint8_t u8Address, uint8_t u8Mask);
      AMx8x5::enResult setRegister(uint8_t u8Address, uint8_t u8Mask);
      AMx8x5::enResult readByte(uint8_t u8Register, uint8_t* pu8Value);
      AMx8x5::enResult readBytes(uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length);
      AMx8x5::enResult writeByte(uint8_t u8Register, uint8_t u8Value);
      AMx8x5::enResult writeBytes(uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length);

    private:
      AMx8x5::stcHandle stcRtcConfig;
  };


class AM0805:public AMx8x5
  {
    public:
      AM0805(void) : AMx8x5(AMx8x5Type0805)
      {

      };
      AM0805(void* pHandle):AMx8x5(AMx8x5Type0805, pHandle)
      {

      };
  };

class AM1805:public AMx8x5
  {
    public:
      AM1805(void):AMx8x5(AMx8x5Type1805)
      {

      };
      AM1805(void* pHandle):AMx8x5(AMx8x5Type1805, pHandle)
      {

      };
  };
class AM0815:public AMx8x5
  {
    public:
      AM0815(void):AMx8x5(AMx8x5Type0815)
      {

      };
      AM0815(void* pHandle):AMx8x5(AMx8x5Type0815, pHandle)
      {

      };
  };

class AM1815:public AMx8x5
  {
    public:
      AM1815(void):AMx8x5(AMx8x5Type1815)
      {

      };
      AM1815(void* pHandle):AMx8x5(AMx8x5Type1815, pHandle)
      {

      };
  };



class ABx8x5:public AMx8x5
{

};

class AB0805:public AMx8x5
  {
    public:
      AB0805(void):AMx8x5(AMx8x5Type0805)
      {

      };
      AB0805(void* pHandle):AMx8x5(AMx8x5Type0805, pHandle)
      {

      };
  };

class AB1805:public AMx8x5
  {
    public:
      AB1805(void):AMx8x5(AMx8x5Type1805)
      {

      };
      AB1805(void* pHandle):AMx8x5(AMx8x5Type1805, pHandle)
      {

      };
  };
class AB0815:public AMx8x5
  {
    public:
      AB0815(void):AMx8x5(AMx8x5Type0815)
      {

      };
      AB0815(void* pHandle):AMx8x5(AMx8x5Type0815, pHandle)
      {

      };
  };

class AB1815:public AMx8x5
  {
    public:
      AB1815(void):AMx8x5(AMx8x5Type1815)
      {

      };
      AB1815(void* pHandle):AMx8x5(AMx8x5Type1815, pHandle)
      {

      };
  };

class RV1805:public AMx8x5
  {
    public:
      RV1805(void):AMx8x5(AMx8x5Type1805)
      {

      };
      RV1805(void* pHandle):AMx8x5(AMx8x5Type1805, pHandle)
      {

      };
  };
#endif




//@} // AMx8x5Group

#endif /*__AMX8X5_H__*/

/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

