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
/** \file amx8x5.cpp
 **
 ** A detailed description is available at 
 ** @link AMx8x5Group AMx8x5 Driver description @endlink
 **
 ** History:
 **   - 2024-05-27  V1.0  Manuel Schreiner  First Version
 **
 *****************************************************************************/
#define __AMX8X5_C__
/*****************************************************************************/
/* Include files                                                             */
/*****************************************************************************/

#include "amx8x5.h"
#include <stdint.h>
#include <string.h>

/*****************************************************************************/
/* Local pre-processor symbols/macros ('#define')                            */
/*****************************************************************************/

  #if defined(ARDUINO)
    #include <Arduino.h>
    #if AMX8X5_DEBUG == 1
        #undef AMX8X5_DEBUG_PRINTF
        #define AMX8X5_DEBUG_PRINTF(...)        Serial.printf(__VA_ARGS__)
    #endif
  #endif

/*****************************************************************************/
/* Global variable definitions (declared in header file with 'extern')       */
/*****************************************************************************/



/*****************************************************************************/
/* Local type definitions ('typedef')                                        */
/*****************************************************************************/


/*****************************************************************************/
/* Local variable definitions ('static')                                     */
/*****************************************************************************/

stc_amx8x5_time_t stcSysTime;

#if AMX8X5_DEBUG == 1
static volatile uint32_t u32DgbLevel = 0;
static const char* astrRegNames[] = {
                                        "HUNDREDTHS",
                                        "SECONDS",
                                        "MINUTES",
                                        "HOURS",
                                        "DATE",
                                        "MONTH",
                                        "YEARS",
                                        "WEEKDAY",
                                        "ALARM_HUNDRS",
                                        "ALARM_SECONDS",
                                        "ALARM_MINUTES",
                                        "ALARM_HOURS",
                                        "ALARM_DATE",
                                        "ALARM_MONTH",
                                        "ALARM_WEEKDAY",
                                        "STATUS",
                                        "CONTROL_1",
                                        "CONTROL_2",
                                        "INT_MASK",
                                        "SQW",
                                        "CAL_XT",
                                        "CAL_RC_HI",
                                        "CAL_RC_LOW",
                                        "SLEEP_CTRL",
                                        "TIMER_CTRL",
                                        "TIMER",
                                        "TIMER_INITIAL",
                                        "WDT",
                                        "OSC_CONTROL",
                                        "OSC_STATUS",
                                        "RESERVED",
                                        "CONFIG_KEY",
                                        "TRICKLE",
                                        "BREF_CTRL",
                                        "RESERVED",
                                        "RESERVED",
                                        "RESERVED",
                                        "RESERVED",
                                        "AFCTRL",
                                        "BATMODE I/O",
                                        "ID0",
                                        "ID1",
                                        "ID2",
                                        "ID3",
                                        "ID4",
                                        "ID5",
                                        "ID6",
                                        "ASTAT",
                                        "OCTRL"};
#endif

/*****************************************************************************/
/* Local function prototypes ('static')                                      */
/*****************************************************************************/

#if AMX8X5_DEBUG == 1
static void AMX8X5_DEBUG_FUNC_START(const char* name);
static en_result_t AMX8X5_FUNC_END(en_result_t enErrCode);
static void AMX8X5_DEBUG_PRINT_LEVEL(void);
static void AMX8X5_DEBUG_PRINT_BUFFER(uint8_t* pu8Data, uint32_t u32Len, uint32_t u32DisplayOffset);
static void AMX8X5_DEBUG_PRINT_BIN_U8(uint8_t u8Data);
static void AMX8X5_DEBUG_PRINT_BIN_U16(uint16_t u16Data);
static void AMX8X5_DEBUG_PRINT_BIN_U32(uint32_t u32Data);
#endif

/*****************************************************************************/
/* Function implementation - global ('extern') and local ('static')          */
/*****************************************************************************/

#if AMX8X5_DEBUG == 1

/**
 ******************************************************************************
 ** \brief  Print spaces recording the debug function level
 ** 
 ******************************************************************************/
static void AMX8X5_DEBUG_PRINT_LEVEL(void)
{
    uint8_t i;
    for(i = 0;i < u32DgbLevel;i++)
    {
        AMX8X5_DEBUG_PRINTF("  ");
    }
}

/**
 ******************************************************************************
 ** \brief  Function start
 **
 ** \param  name           Function name
 ** 
 ******************************************************************************/
static void AMX8X5_DEBUG_FUNC_START(const char* name)
{
    AMX8X5_DEBUG_PRINT_LEVEL(); 
    u32DgbLevel++; 
    AMX8X5_DEBUG_PRINTF("[f] "); 
    AMX8X5_DEBUG_PRINTF(name); 
    AMX8X5_DEBUG_PRINTF("() {\r\n");
}

/**
 ******************************************************************************
 ** \brief  Function end (return)
 **
 ** \param  enErrCode     RTC Handle
 **
 ** \return enErrCode
 ** 
 ******************************************************************************/
static en_result_t AMX8X5_FUNC_END(en_result_t enErrCode)
{
    if (u32DgbLevel > 0) u32DgbLevel--;
    AMX8X5_DEBUG_PRINT_LEVEL(); 
    AMX8X5_DEBUG_PRINTF("} //[f] end ");
    
    switch(enErrCode) 
    {
        case Ok:
        AMX8X5_DEBUG_PRINTF(", Ok");
        break;
        case Error:
        AMX8X5_DEBUG_PRINTF(", Error(1): Error");
        break;
        case ErrorAddressAlignment:
        AMX8X5_DEBUG_PRINTF(", Error(2): ErrorAddressAlignment");
        break;
        case ErrorAccessRights:
        AMX8X5_DEBUG_PRINTF(", Error(3): ErrorAccessRights");
        break;
        case ErrorInvalidParameter:
        AMX8X5_DEBUG_PRINTF(", Error(4): ErrorInvalidParameter");
        break;
        case ErrorOperationInProgress:
        AMX8X5_DEBUG_PRINTF(", Error(5): ErrorOperationInProgress");
        break;
        case ErrorInvalidMode:
        AMX8X5_DEBUG_PRINTF(", Error(6): ErrorInvalidMode");
        break;
        case ErrorUninitialized:
        AMX8X5_DEBUG_PRINTF(", Error(7): ErrorUninitialized");
        break;
        case ErrorBufferFull:
        AMX8X5_DEBUG_PRINTF(", Error(8): ErrorBufferFull");
        break;
        case ErrorTimeout:
        AMX8X5_DEBUG_PRINTF(", Error(9): ErrorTimeout");
        break;
        case ErrorNotReady:
        AMX8X5_DEBUG_PRINTF(", Error(10): ErrorNotReady");
        break;
        case OperationInProgress:
        AMX8X5_DEBUG_PRINTF(", Error(11): OperationInProgress");
        break;
        default:
        AMX8X5_DEBUG_PRINTF(", Error(N/A): Unknown");
        break;
    }
    AMX8X5_DEBUG_PRINTF("\r\n");
    if (u32DgbLevel == 0)
    {
        AMX8X5_DEBUG_PRINTF("\r\n");
    }
    return enErrCode;
}

/**
 ******************************************************************************
 ** \brief  Debug print register buffer
 **
 ** \param pu8Data          Register data
 **
 ** \param u32Len           Data length
 **
 ** \param u32DisplayOffset Offset
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
static void AMX8X5_DEBUG_PRINT_BUFFER(uint8_t* pu8Data, uint32_t u32Len, uint32_t u32DisplayOffset)
{
    uint32_t i;
    AMX8X5_DEBUG_PRINTF("size %d: { \r\n",u32Len);
    for(i = 0;i < u32Len;i++)
    {
         AMX8X5_DEBUG_PRINT_LEVEL();
         AMX8X5_DEBUG_PRINTF("  0x%02x: 0x%02x, ",i+u32DisplayOffset,pu8Data[i]); 
         AMX8X5_DEBUG_PRINT_BIN_U8(pu8Data[i]);
         if ((i+u32DisplayOffset) <= 0x30) { AMX8X5_DEBUG_PRINTF(" (%s)",astrRegNames[i+u32DisplayOffset]); }
         AMX8X5_DEBUG_PRINTF("\r\n");
    }
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("} //buffer end\r\n");
}

/**
 ******************************************************************************
 ** \brief  Debug print byte as binary
 **
 ** \param  u8Data     Byte
 ** 
 ******************************************************************************/
static void AMX8X5_DEBUG_PRINT_BIN_U8(uint8_t u8Data)
{
    uint8_t i;
    AMX8X5_DEBUG_PRINTF("0b");
    for(i = 0; i < 8;i++)
    {
        if (u8Data & (1 << (7-i)))
        {
            AMX8X5_DEBUG_PRINTF("1");
        }
        else
        {
            AMX8X5_DEBUG_PRINTF("0");
        }
    }
}

/**
 ******************************************************************************
 ** \brief  Debug print halfword as binary
 **
 ** \param  u8Data     Halfword
 ** 
 ******************************************************************************/
static void AMX8X5_DEBUG_PRINT_BIN_U16(uint16_t u16Data)
{
    uint8_t i;
    AMX8X5_DEBUG_PRINTF("0b");
    for(i = 0; i < 16;i++)
    {
        if (u16Data & (1 << (16-i)))
        {
            AMX8X5_DEBUG_PRINTF("1");
        }
        else
        {
            AMX8X5_DEBUG_PRINTF("0");
        }
    }
}

/**
 ******************************************************************************
 ** \brief  Debug print word as binary
 **
 ** \param  u8Data     word
 ** 
 ******************************************************************************/
static void AMX8X5_DEBUG_PRINT_BIN_U32(uint32_t u32Data)
{
    uint8_t i;
    AMX8X5_DEBUG_PRINTF("0b");
    for(i = 0; i < 32;i++)
    {
        if (u32Data & (1 << (32-i)))
        {
            AMX8X5_DEBUG_PRINTF("1");
        }
        else
        {
            AMX8X5_DEBUG_PRINTF("0");
        }
    }
}
#endif

/**
 ******************************************************************************
 ** \brief  Clear bits in register
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param u8Address       Register address
 **
 ** \param u8Mask          Bitmask
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
en_result_t Amx8x5_ClearRegister(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Address, uint8_t u8Mask)
{
    uint8_t u8Temp;
    en_result_t res;
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("[reg] -> clear bits in ");
    AMX8X5_DEBUG_PRINT_REG(u8Address,u8Mask); 
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("  +->");
    res = Amx8x5_ReadByte(pstcHandle, u8Address, &u8Temp);
    if (res != Ok) return res;
    u8Temp &= ~u8Mask;
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("  +->");
    res = Amx8x5_WriteByte(pstcHandle, u8Address, u8Temp);
    return res;
}

/**
 ******************************************************************************
 ** \brief  Set bits in register
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param u8Address       Register address
 **
 ** \param u8Mask          Bitmask
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
en_result_t Amx8x5_SetRegister(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Address, uint8_t u8Mask)
{
    uint8_t u8Temp;
    en_result_t res;
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("[reg] -> set   bits in ");
    AMX8X5_DEBUG_PRINT_REG(u8Address,u8Mask);
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("  +->");
    res = Amx8x5_ReadByte(pstcHandle, u8Address, &u8Temp);
    if (res != Ok) return res;
    u8Temp |= u8Mask;
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("  +->");
    res = Amx8x5_WriteByte(pstcHandle, u8Address, u8Temp);
    return res;
}

/**
 ******************************************************************************
 ** \brief  Read multiple bytes
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param u8Register      Register address
 **
 ** \param pu8Data         Data pointer to store the data
 **
 ** \param u32Length       Data length
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
en_result_t Amx8x5_ReadBytes(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length)
{
    int res = 0;
    if (pstcHandle == NULL) return ErrorUninitialized;
    if (pu8Data == NULL) return ErrorInvalidParameter;
    if (pstcHandle->enMode == AMx8x5ModeI2C)
    {
        if (pstcHandle->pfnReadI2C != NULL)
        {
            res = pstcHandle->pfnReadI2C(pstcHandle->pHandle,pstcHandle->u32Address,u8Register,pu8Data,u32Length);
            if (res != 0) return Error;
        }
        else
        {
            return ErrorUninitialized;
        }
    } else 
    {
        if (pstcHandle->pfnReadSpi != NULL)
        {
            res = pstcHandle->pfnReadSpi(pstcHandle->pHandle,pstcHandle->u32ChipSelect,u8Register,pu8Data,u32Length);
            if (res != 0) return Error;
        }
        else
        {
            return ErrorUninitialized;
        }
    }
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("[reg] -> read   bytes,  register 0x%02x, ",u8Register);
    AMX8X5_DEBUG_PRINT_BUFFER(pu8Data,u32Length,u8Register);

    return Ok;
}

/**
 ******************************************************************************
 ** \brief  Read one byte
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param u8Register      Register address
 **
 ** \param pu8Value        Data pointer to store the data
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
en_result_t Amx8x5_ReadByte(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Register, uint8_t* pu8Value)
{
    int res = 0;
    if (pstcHandle == NULL) return ErrorUninitialized;
    if (pu8Value == NULL) return ErrorInvalidParameter;
    if (pstcHandle->enMode == AMx8x5ModeI2C)
    {
        if (pstcHandle->pfnReadI2C != NULL)
        {
            res = pstcHandle->pfnReadI2C(pstcHandle->pHandle,pstcHandle->u32Address,u8Register,pu8Value,1);
            if (res != 0) return Error;
        }
        else
        {
            return ErrorUninitialized;
        }
    } else 
    {
        if (pstcHandle->pfnReadSpi != NULL)
        {
            res = pstcHandle->pfnReadSpi(pstcHandle->pHandle,pstcHandle->u32ChipSelect,u8Register,pu8Value,1);
            if (res != 0) return Error;
        }
        else
        {
            return ErrorUninitialized;
        }
    }
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("[reg] -> read  byte in ");
    AMX8X5_DEBUG_PRINT_REG(u8Register,*pu8Value); 
    return Ok;
}

/**
 ******************************************************************************
 ** \brief  Write multiple bytes
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param u8Register      Register address
 **
 ** \param pu8Data         Data pointer
 **
 ** \param u32Length       Data length
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
en_result_t Amx8x5_WriteBytes(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length)
{
    int res = 0;
    if (pstcHandle == NULL) return ErrorUninitialized;
    if (pu8Data == NULL) return ErrorInvalidParameter;
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("[reg] -> write  bytes,  register 0x%02x, ",u8Register);
    AMX8X5_DEBUG_PRINT_BUFFER(pu8Data,u32Length,u8Register);
    if (pstcHandle->enMode == AMx8x5ModeI2C)
    {
        if (pstcHandle->pfnWriteI2C != NULL)
        {
            res = pstcHandle->pfnWriteI2C(pstcHandle->pHandle,pstcHandle->u32Address,u8Register,pu8Data,u32Length);
            if (res != 0) return Error;
        }
        else
        {
            return ErrorUninitialized;
        }
    } else 
    {
        if (pstcHandle->pfnWriteSpi != NULL)
        {
            res = pstcHandle->pfnWriteSpi(pstcHandle->pHandle,pstcHandle->u32ChipSelect,u8Register,pu8Data,u32Length);
            if (res != 0) return Error;
        }
        else
        {
            return ErrorUninitialized;
        }
    }
    return Ok;
}

/**
 ******************************************************************************
 ** \brief  Write one byte
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  u8Register     Register address
 **
 ** \param u8Value         Value to write
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
en_result_t Amx8x5_WriteByte(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Register, uint8_t u8Value)
{
    int res = 0;
    if (pstcHandle == NULL) return ErrorUninitialized;
    
    AMX8X5_DEBUG_PRINT_LEVEL();
    AMX8X5_DEBUG_PRINTF("[reg] -> write byte in ");
    AMX8X5_DEBUG_PRINT_REG(u8Register,u8Value);
    
    if (pstcHandle->enMode == AMx8x5ModeI2C)
    {
        if (pstcHandle->pfnWriteI2C != NULL)
        {
            res = pstcHandle->pfnWriteI2C(pstcHandle->pHandle,pstcHandle->u32Address,u8Register,&u8Value,1);
            if (res != 0) return Error;
        }
        else
        {
            return ErrorUninitialized;
        }
    } else 
    {
        if (pstcHandle->pfnWriteSpi != NULL)
        {
            res = pstcHandle->pfnWriteSpi(pstcHandle->pHandle,pstcHandle->u32ChipSelect,u8Register,&u8Value,1);
            if (res != 0) return Error;
        }
        else
        {
            return ErrorUninitialized;
        }
    }
    return Ok;
}


/**
 ******************************************************************************
 ** \brief  This function is reset the RTC
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
en_result_t Amx8x5_Reset(stc_amx8x5_handle_t* pstcHandle)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_Reset");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }

    return AMX8X5_FUNC_END(Amx8x5_WriteByte(pstcHandle,AMX8X5_CONFIG_KEY_VAL, 0x3C));
}

/**
 ******************************************************************************
 ** \brief  This function is initializing the RTC module
 **
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \return Ok on success, else the Error as en_result_t
 **
 **
 **
 ** For correct initialization, the I2C/SPI transfer rotines has to be implemented.
 **
 ** Examples for SPI routines can be found here: 
 ** @link amx8x5_module_spitransfer SPI transfer routines example @endlink or @link amx8x5_module_spiinit SPI initialization example @endlink
 **
 ** Examples for I2C routines can be found here:
 ** @link amx8x5_module_i2ctransfer I2C transfer routines example @endlink or @link amx8x5_module_i2cinit I2C initialization example @endlink 
 ** 
 ** Example:
 ** @code 
 ** #include "amx8x5.h" 
 **
 ** const static stc_amx8x5_handle_t stcRtcConfig = 
 **            {
 **              AMx8x5ModeI2C,          //enMode specifies SPI or I2C mode: AMx8x5ModeSPI or AMx8x5ModeI2C
 **              (void*)I2C0,            //pHandle is the peripheral pointer to the used I2C peripheral as defined in CMSIS  
 **              AMx8x5Type1805,         //enRtcType defines the type of used RTC: #AMx8x5Type0805, #AMx8x5Type0815, #AMx8x5Type1805 or #AMx8x5Type1815
 **              AMX8X5_I2C_ADDRESS,     //u32Address defines the I2C address
 **              I2CWrite,               //pfnWriteI2C defines the I2C write procedure
 **              I2CRead                 //pfnReadI2C defines the I2C read procedure
 **            };
 **
 ** int main(void)
 ** {
 **      Amx8x5_Init((stc_amx8x5_handle_t*)&stcRtcConfig);
 **      ...
 ** } 
 ** @endcode
 ** 
 ******************************************************************************/
en_result_t Amx8x5_Init(stc_amx8x5_handle_t* pstcHandle)
{
    en_result_t res;
    uint32_t u32Temp = 0;
    uint8_t* pu8Buffer = (uint8_t*)&u32Temp;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_Init");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    
    res = Amx8x5_ReadBytes(pstcHandle, AMX8X5_REG_ID0, pu8Buffer, 2); 
    if (res != Ok) {
        return AMX8X5_FUNC_END(res);
    }
    switch(pstcHandle->enRtcType)
    {
      case AMx8x5Type0805:
          if ((u32Temp & 0xFFFF) == 0x000508)
          {
              return AMX8X5_FUNC_END(Ok);
          }
          return AMX8X5_FUNC_END(ErrorInvalidMode);
      case AMx8x5Type0815:
          if ((u32Temp & 0xFFFF) == 0x001508)
          {
              return AMX8X5_FUNC_END(Ok);
          }
          return AMX8X5_FUNC_END(ErrorInvalidMode);
      case AMx8x5Type1805:
          if ((u32Temp & 0xFFFF) == 0x000518)
          {
              return AMX8X5_FUNC_END(Ok);
          }
          return AMX8X5_FUNC_END(ErrorInvalidMode);
      case AMx8x5Type1815:
          if ((u32Temp & 0xFFFF) == 0x001518)
          {
              return AMX8X5_FUNC_END(Ok);
          }
          return AMX8X5_FUNC_END(ErrorInvalidMode);
      default:
        return AMX8X5_FUNC_END(Error);
    }
}

/**
 ******************************************************************************
 ** \brief  This function is getting the time of the RTC
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  ppstcTime      returns the pointer of the time structure stc_amx8x5_time_t in a pointer of the pointer
 **
 ** \return Ok on success, else the Error as en_result_t
 **
 ** Example:
 ** @code
 ** stc_amx8x5_time_t* pstcTime;
 **
 ** Amx8x5_GetTime(&stcRtcConfig,&pstcTime);
 **
 ** printf("It is: %d:%d:%d \r\n",pstcTime->u8Hour,pstcTime->u8Minute,pstcTime->u8Second);
 ** @endcode
 ** 
 ******************************************************************************/
en_result_t Amx8x5_GetTime(stc_amx8x5_handle_t* pstcHandle, stc_amx8x5_time_t** ppstcTime)
{
    uint8_t u8Temp;
    en_result_t res;
    uint32_t au32Buffer[3] = {0,0,0};
    uint8_t* pu8Buffer = (uint8_t*)&au32Buffer[0];
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_GetTime");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    res = Amx8x5_ReadBytes(pstcHandle,AMX8X5_REG_HUNDREDTHS,pu8Buffer,8);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    stcSysTime.u8Hundredth = AMX8X5_BCD_TO_DEC(pu8Buffer[0]);
    stcSysTime.u8Second = AMX8X5_BCD_TO_DEC(pu8Buffer[1]);
    stcSysTime.u8Minute = AMX8X5_BCD_TO_DEC(pu8Buffer[2]);
    stcSysTime.u8Hour = pu8Buffer[3];
    stcSysTime.u8Date = AMX8X5_BCD_TO_DEC(pu8Buffer[4]);
    stcSysTime.u8Month = AMX8X5_BCD_TO_DEC(pu8Buffer[5]);
    stcSysTime.u8Year = AMX8X5_BCD_TO_DEC(pu8Buffer[6]);
    stcSysTime.u8Weekday = AMX8X5_BCD_TO_DEC(pu8Buffer[7]);
    
    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_CONTROL_1,&u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    if ((u8Temp & 0x40) == 0)
    {
        //
        // 24-hour mode.
        //
        stcSysTime.u8Mode = 2;
        stcSysTime.u8Hour =stcSysTime.u8Hour & 0x3F;
    }
    else
    {
        //
        // 12-hour mode.  Get PM:AM.
        //

        stcSysTime.u8Mode = (stcSysTime.u8Hour & 0x20) ? 1 : 0;
        stcSysTime.u8Hour &= 0x1F;
    }
    
    stcSysTime.u8Hour = AMX8X5_BCD_TO_DEC(stcSysTime.u8Hour );

    //
    // Get the century bit.
    //
    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_STATUS,&u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    stcSysTime.u8Century = (u8Temp & AMX8X5_REG_STATUS_CB_MSK) ? 1 : 0;
    
    if (ppstcTime != NULL) *ppstcTime = &stcSysTime;
    
    return AMX8X5_FUNC_END(Ok);
}

/**
 ******************************************************************************
 ** \brief  This function is setting the time of the RTC
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  pstcTime       pointer of the new time stc_amx8x5_time_t to write
 **
 ** \param  bProtect       false to leave counters writable, true to leave counters unwritable
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
en_result_t Amx8x5_SetTime(stc_amx8x5_handle_t* pstcHandle, stc_amx8x5_time_t* pstcTime, bool bProtect)
{
    uint8_t u8Temp;
    uint32_t au32Buffer[2];
    uint8_t* pu8Buffer = (uint8_t*)&au32Buffer[0];
    en_result_t res;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetTime");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (pstcTime == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorInvalidParameter);
    }
    
    AMX8X5_DEBUG_PRINTF("[f] Amx8x5_SetTime\r\n");
    
    pu8Buffer[0] = AMX8X5_DEC_TO_BCD(pstcTime->u8Hundredth);
    pu8Buffer[1] = AMX8X5_DEC_TO_BCD(pstcTime->u8Second);
    pu8Buffer[2] = AMX8X5_DEC_TO_BCD(pstcTime->u8Minute);
    pu8Buffer[3] = AMX8X5_DEC_TO_BCD(pstcTime->u8Hour);
    pu8Buffer[4] = AMX8X5_DEC_TO_BCD(pstcTime->u8Date);
    pu8Buffer[5] = AMX8X5_DEC_TO_BCD(pstcTime->u8Month);
    pu8Buffer[6] = AMX8X5_DEC_TO_BCD(pstcTime->u8Year);
    pu8Buffer[7] = AMX8X5_DEC_TO_BCD(pstcTime->u8Weekday);
    
    
    //
    // Determine whether 12 or 24-hour timekeeping mode is being used and set
    // the 1224 bit appropriately.
    // 24-hour day.
    //
    if (pstcTime->u8Mode == AMX8X5_24HR_MODE)
    {
        res = Amx8x5_ClearRegister(pstcHandle, AMX8X5_REG_CONTROL_1, AMX8X5_REG_CONTROL_1_12_24_MSK);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }

    //
    // 12-hour day PM.
    //
    else if (pstcTime->u8Mode == AMX8X5_12HR_MODE)
    {
        //
        // Set AM/PM.
        //
        pu8Buffer[3] |= 0x20;
        res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_CONTROL_1, AMX8X5_REG_CONTROL_1_12_24_MSK);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }

    //
    // 12-hour day AM.
    //
    else
    {
        res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_CONTROL_1, AMX8X5_REG_CONTROL_1_12_24_MSK);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }

    //
    // Set the WRTC bit to enable counter writes.
    //
    res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_CONTROL_1, AMX8X5_REG_CONTROL_1_WRTC_MSK);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }

    //
    // Set the correct century.
    //
    if (pstcTime->u8Century == 0)
    {
        res = Amx8x5_ClearRegister(pstcHandle, AMX8X5_REG_STATUS, AMX8X5_REG_STATUS_CB_MSK);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }
    else
    {
        res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_STATUS, AMX8X5_REG_STATUS_CB_MSK);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }
    
    res = Amx8x5_WriteBytes(pstcHandle,AMX8X5_REG_HUNDREDTHS,pu8Buffer,8);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    

    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_CONTROL_1,&u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    u8Temp &= 0x7E;
    if (false == bProtect)
    {
         u8Temp |= AMX8X5_REG_CONTROL_1_WRTC_MSK;
    } 
    
    res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_CONTROL_1,u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    

    
    memcpy(&stcSysTime,pstcTime,sizeof(stcSysTime));
    //alternative to memcpy validate via GetTime: Amx8x5_GetTime(pstcHandle,NULL); //update SysTime
    return AMX8X5_FUNC_END(Ok);
}

/**
 ******************************************************************************
 ** \brief  Set the alarm value
 **
 ** This function sets the alarm value and configures the correct pin (if
 ** necessary).
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  pstcTime       Time
 **
 ** \param enModeRepeat   Repeat mode as defined in #en_amx8x5_alarm_repeat_t
 **        AMx8x5AlarmDisabled => disable alarm
 **        AMx8x5AlarmYear => once per year
 **        AMx8x5AlarmMonth => once per month
 **        AMx8x5AlarmWeek => once per week
 **        AMx8x5AlarmDay => once per day
 **        AMx8x5AlarmHour => once per hour
 **        AMx8x5AlarmMinute => once per minute
 **        AMx8x5AlarmSecond => once per second
 **        AMx8x5Alarm10thSecond => once per 10th of a second
 **        AMx8x5Alarm100thSecond => once per 100th of a second
 **        NOTE: year and century are not used
 **        NOTE: mode must match current 12/24 selection
 **
 ** \param enModeIrq       Interrupt mode as defined in #en_amx8x5_interrupt_mode_t
 **        AMx8x5InterruptModeLevel => Level (static) for both XT mode and RC mode.
 **        AMx8x5InterruptModePulseShort => 1/8192 seconds pulse for XT mode. 1/64 seconds for RC mode.
 **        AMx8x5InterruptModePulseMedium => 1/64 seconds pulse for both XT mode and RC mode
 **        AMx8x5InterruptModePulseLong => 1/4 seconds pulse for both XT mode and RC mode.
 **
 ** \param enModePin pin on which to generate the interrupt as defined in #en_amx8x5_interrupt_pin_t
 **        AMx8x5InterruptPinInternal => internal flag only
 **        AMx8x5InterruptIrq => FOUT/nIRQ
 **        AMx8x5InterruptIrq2 => PSW/nIRQ2
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ******************************************************************************/
en_result_t Amx8x5_SetAlarm(stc_amx8x5_handle_t* pstcHandle, stc_amx8x5_time_t* pstcTime, en_amx8x5_alarm_repeat_t enModeRepeat, en_amx8x5_interrupt_mode_t enModeIrq, en_amx8x5_interrupt_pin_t enModePin)
{
    uint8_t u8Temp;
    uint32_t au32Buffer[2];
    uint8_t* pu8Buffer = (uint8_t*)&au32Buffer[0];
    en_result_t res;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetAlarm");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (pstcTime == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorInvalidParameter);
    }
    
    pu8Buffer[0] = AMX8X5_DEC_TO_BCD(pstcTime->u8Hundredth);
    pu8Buffer[1] = AMX8X5_DEC_TO_BCD(pstcTime->u8Second);
    pu8Buffer[2] = AMX8X5_DEC_TO_BCD(pstcTime->u8Minute);
    pu8Buffer[3] = AMX8X5_DEC_TO_BCD(pstcTime->u8Hour);
    pu8Buffer[4] = AMX8X5_DEC_TO_BCD(pstcTime->u8Date);
    pu8Buffer[5] = AMX8X5_DEC_TO_BCD(pstcTime->u8Month);
    pu8Buffer[6] = AMX8X5_DEC_TO_BCD(pstcTime->u8Weekday);
    
    //
    // Determine whether 12 or 24-hour timekeeping mode is being used 
    //
    
    if (pstcTime->u8Mode == AMX8X5_12HR_MODE)
    {
        //
        // Set AM/PM.
        //
        pu8Buffer[3] |= 0x20;
    }
    
    //
    // Clear the RPT field.
    // Clear the AIE bit IM field.
    // Clear the ALM flag.
    //
    res = Amx8x5_ClearRegister(pstcHandle, AMX8X5_REG_TIMER_CTRL, 0x1C);
    if (res != Ok)
    {
        return AMX8X5_FUNC_END(res);
    }
    
    res = Amx8x5_ClearRegister(pstcHandle, AMX8X5_REG_INT_MASK, 0x64);
    if (res != Ok)
    {
        return AMX8X5_FUNC_END(res);
    }
    
    res = Amx8x5_ClearRegister(pstcHandle, AMX8X5_REG_STATUS, 0x04);
    if (res != Ok)
    {
        return AMX8X5_FUNC_END(res);
    }
    
    switch(enModePin)
    {
            case AMx8x5InterruptIrq:
                //
                // Interrupt on FOUT/nIRQ.
                // Get the Control2 Register.
                //
                res = Amx8x5_ReadByte(pstcHandle, AMX8X5_REG_CONTROL_2,&u8Temp);
                if (res != Ok)
                {
                    return AMX8X5_FUNC_END(res);
                }
                //
                // Extract the OUT1S field.
                //
                u8Temp = (u8Temp & AMX8X5_REG_CONTROL_2_OUT1S_MSK);

                //
                // Not already selecting nIRQ.
                //
                if (u8Temp != 0)
                {
                    //
                    // Set OUT1S to AMx8x5Out1nAIRQIfAieElseOut (3)
                    //
                    res = Amx8x5_SetOut1Mode(pstcHandle,AMx8x5Out1nAIRQIfAieElseOut);
                    if (res != Ok)
                    {
                        return AMX8X5_FUNC_END(res);
                    }
                }
                break;
            case AMx8x5InterruptIrq2:
                //
                // Interrupt on PSW/nIRQ2.
                // Get the Control2 Register.
                //
                res = Amx8x5_ReadByte(pstcHandle, AMX8X5_REG_CONTROL_2, &u8Temp);
                if (res != Ok)
                {
                    return AMX8X5_FUNC_END(res);
                }
                //
                // Extract the OUT2S field.
                //
                u8Temp = (u8Temp & AMX8X5_REG_CONTROL_2_OUT2S_MSK);

                //
                // Not already selecting nIRQ.
                //
                if (u8Temp != 0)
                {
                    //
                    // Set OUT2S to AMx8x5Out2nAIRQIfAieElseOutB (3)
                    //
                    res = Amx8x5_SetOut2Mode(pstcHandle,AMx8x5Out2nAIRQIfAieElseOutB);
                    if (res != Ok)
                    {
                        return AMX8X5_FUNC_END(res);
                    }
                }
                break;
      default:
        return AMX8X5_FUNC_END(ErrorInvalidParameter);
        
    }
    if (enModeRepeat == AMx8x5Alarm10thSecond)
    {
        //
        // 10ths interrupt.
        // Select correct RPT value.
        //
        pu8Buffer[0] |= 0xF0;
        enModeRepeat = AMx8x5AlarmSecond;
    }
    if (enModeRepeat == AMx8x5Alarm100thSecond)
    {
        //
        // 100ths interrupt.
        // Select correct RPT value.
        //
        pu8Buffer[0] = 0xFF;
        enModeRepeat = AMx8x5AlarmSecond;
    }
    
    //
    // Don't initiate if enModeRepeat is AMx8x5AlarmDisabled.
    //
    if (enModeRepeat != AMx8x5AlarmDisabled)
    {
        //
        // Set the RPT field to the value of u8Repeat.
        //
        res = Amx8x5_ClearRegister(pstcHandle, AMX8X5_REG_INT_MASK, AMX8X5_REG_INT_MASK_IM_MSK);
        if (res != Ok)
        {
            return AMX8X5_FUNC_END(res);
        }
        
        switch(enModeIrq)
        {
            case AMx8x5InterruptModePulseShort:
               res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_INT_MASK, (0x01 << AMX8X5_REG_INT_MASK_IM_POS));
                if (res != Ok)
                {
                    return AMX8X5_FUNC_END(res);
                }
                break;
            case AMx8x5InterruptModePulseMedium:
               res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_INT_MASK, (0x02 << AMX8X5_REG_INT_MASK_IM_POS));
                if (res != Ok)
                {
                    return AMX8X5_FUNC_END(res);
                }
                break;
            case AMx8x5InterruptModePulseLong:
               res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_INT_MASK, (0x03 << AMX8X5_REG_INT_MASK_IM_POS));
                if (res != Ok)
                {
                    return AMX8X5_FUNC_END(res);
                }
                break;
          default:
            return AMX8X5_FUNC_END(ErrorInvalidParameter);
        }
        
        switch(enModeRepeat)
        {
              case AMx8x5AlarmYear:        // Alarm by year
                    res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_TIMER_CTRL, (0x01 << AMX8X5_REG_TIMER_CTRL_RPT_POS));
                    if (res != Ok)
                    {
                        return AMX8X5_FUNC_END(res);
                    }
                    break;
              case AMx8x5AlarmMonth:       // Alarm by month
                    res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_TIMER_CTRL, (0x02 << AMX8X5_REG_TIMER_CTRL_RPT_POS));
                    if (res != Ok)
                    {
                        return AMX8X5_FUNC_END(res);
                    }
                    break;
              case AMx8x5AlarmWeek:        // Alarm by week
                   res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_TIMER_CTRL, (0x03 << AMX8X5_REG_TIMER_CTRL_RPT_POS));
                    if (res != Ok)
                    {
                        return AMX8X5_FUNC_END(res);
                    }
                    break;
              case AMx8x5AlarmDay:         // Alarm by day
                    res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_TIMER_CTRL, (0x04 << AMX8X5_REG_TIMER_CTRL_RPT_POS));
                    if (res != Ok)
                    {
                        return AMX8X5_FUNC_END(res);
                    }
                    break;
              case AMx8x5AlarmHour:        // Alarm by hour
                    res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_TIMER_CTRL, (0x05 << AMX8X5_REG_TIMER_CTRL_RPT_POS));
                    if (res != Ok)
                    {
                        return AMX8X5_FUNC_END(res);
                    }
                    break;
              case AMx8x5AlarmMinute:      // Alarm by minute
                    res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_TIMER_CTRL, (0x06 << AMX8X5_REG_TIMER_CTRL_RPT_POS));
                    if (res != Ok)
                    {
                        return AMX8X5_FUNC_END(res);
                    }
                    break;
              case AMx8x5AlarmSecond:      // Alarm by second
                    res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_TIMER_CTRL, (0x07 << AMX8X5_REG_TIMER_CTRL_RPT_POS));
                    if (res != Ok)
                    {
                        return AMX8X5_FUNC_END(res);
                    }
                    break;
          default:
            return AMX8X5_FUNC_END(ErrorInvalidParameter);
        }
        res = Amx8x5_WriteBytes(pstcHandle,AMX8X5_REG_ALARM_HUNDRS,pu8Buffer,7);
        if (res != Ok)
        {
            return AMX8X5_FUNC_END(res);
        }
        res = Amx8x5_EnableIrqAlarm(pstcHandle,true);
        if (res != Ok)
        {
            return AMX8X5_FUNC_END(res);
        }
    }
    else
    {
        //
        // Set IM field to 0x3 (reset value) to minimize current draw.
        //
         res = Amx8x5_SetRegister(pstcHandle, AMX8X5_REG_INT_MASK, 0x60);
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
    }
    
    return AMX8X5_FUNC_END(Ok);
}


/**
 ******************************************************************************
 ** \brief  Set the calibration.
 **
 ** This function loads the AMX8XX counter registers with the last read
 ** values of Amx8x5_GetTime()
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  enMode         AMx8x5ModeCalibrateXT calibrate the XT oscillator or AMx8x5ModeCalibrateRC calibrate the RC oscillator
 **
 ** \param   iAdjust: Adjustment in ppm. 
 **          Adjustment limits are:
 **              enMode = AMx8x5ModeCalibrateXT => (-610 to +242)
 **              enMode = AMx8x5ModeCalibrateRC => (-65536 to +65520)
 **              An iAdjust value of zero resets the selected oscillator calibration
 **              value to 0.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** enMode(&stcRtcConfig,AMx8x5ModeCalibrateXT,5);
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_SetCalibrationValue(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_calibration_mode_t enMode, int32_t iAdjust)
{
    en_result_t res;
    int32_t iAdjint;
    uint8_t u8Adjreg;
    uint8_t u8Adjregu;
    uint8_t u8XTcal;

    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetCalibrationValue");
    
    //
    // Calculate current calibration value:
    //    adjval = (double)iAdjust;
    //    iAdjint = (int16_t)round(adjval*1000/1907)
    //
    if (iAdjust < 0 )
    {
        iAdjint = ((iAdjust)*1000 - 953);
    }
    else
    {
        iAdjint = ((iAdjust)*1000 + 953);
    }
    iAdjint = iAdjint / 1907;

    if (enMode == AMx8x5ModeCalibrateXT)
    {
        //
        // XT Adjust
        //
        if (iAdjint > 63 )
        {
            //
            // 64 to 127.
            // CMDX = 1.
            //
            u8XTcal = 0;
            u8Adjreg = ((iAdjint >> 1) & 0x3F) | 0x80;
        }
        else if (iAdjint > -65)
        {
            //
            // -64 to 63.
            // CMDX = 0.
            //
            u8XTcal = 0;
            u8Adjreg = (iAdjint & 0x7F);
        }
        else if (iAdjint > -129)
        {
            //
            // -128 to -65.
            // CMDX = 0.
            //
            u8XTcal = 1;
            u8Adjreg = ((iAdjint + 64) & 0x7F);
        }
        else if (iAdjint > -193)
        {
            //
            // -192 to -129.
            // CMDX = 0.
            //
            u8XTcal = 2;
            u8Adjreg = ((iAdjint + 128) & 0x7F);
        }
        else if (iAdjint > -257)
        {
            //
            // -256 to -193.
            // CMDX = 0.
            //
            u8XTcal = 3;
            u8Adjreg = ((iAdjint + 192) & 0x7F);
        }
        else
        {
            //
            // -320 to -257.
            // CMDX = 1.
            //
            u8XTcal = 3;
            u8Adjreg = ((iAdjint + 192) >> 1) & 0xFF;
        }
        //
        // Load the CALX register.
        //
        res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_CAL_XT, u8Adjreg);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        
        //
        // Mask u8XTcal.
        //
        res = Amx8x5_ReadByte(pstcHandle, AMX8X5_REG_OSC_STATUS,&u8Adjreg);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        
        u8Adjreg = u8Adjreg & 0x3F;
        
        //
        // Add u8XTcal field.
        //
        u8Adjreg = u8Adjreg | (u8XTcal << 6);

        //
        // Write it back.
        //
        res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_OSC_STATUS, u8Adjreg);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }
    else
    {
        //
        // RC Adjust.
        //
        if (iAdjint > 32767 )
        {
            //
            // 32768 to 65535.
            // Lower 8 bits.
            // CMDR = 3.
            //
            u8Adjreg = ((iAdjint >> 3) & 0xFF);
            u8Adjregu = ((iAdjint >> 11) | 0xC0);
        }
        else if (iAdjint > 16383 )
        {
            //
            // 16384 to 32767.
            // Lower 8 bits.
            // CMDR = 2.
            //
            u8Adjreg = ((iAdjint >> 2) & 0xFF);
            u8Adjregu = ((iAdjint >> 10) | 0x80);
        }
        else if (iAdjint > 8191 )
        {
            //
            // 8192 to 16383.
            // Lower 8 bits.
            // CMDR = 2.
            //
            u8Adjreg = ((iAdjint >> 1) & 0xFF);
            u8Adjregu = ((iAdjint >> 9) | 0x40);
        }
        else if (iAdjint >= 0 )
        {
            //
            // 0 to 1023.
            // Lower 8 bits.
            // CMDR = 0.
            //
            u8Adjreg = ((iAdjint) & 0xFF);
            u8Adjregu = (iAdjint >> 8);
        }
        else if (iAdjint > -8193 )
        {
            //
            // -8192 to -1.
            // Lower 8 bits.
            // CMDR = 0.
            //
            u8Adjreg = ((iAdjint) & 0xFF);
            u8Adjregu = (iAdjint >> 8) & 0x3F;
        }
        else if (iAdjint > -16385 )
        {
            //
            // -16384 to -8193.
            // Lower 8 bits.
            // CMDR = 1.
            //
            u8Adjreg = ((iAdjint >> 1) & 0xFF);
            u8Adjregu = (iAdjint >> 9) & 0x7F;
        }
        else if (iAdjint > -32769 )
        {
            //
            // -32768 to -16385.
            // Lower 8 bits.
            // CMDR = 2.
            //
            u8Adjreg = ((iAdjint >> 2) & 0xFF);
            u8Adjregu = (iAdjint >> 10) & 0xBF;
        }
        else
        {
            //
            // -65536 to -32769.
            // Lower 8 bits.
            // CMDR = 3.
            //
            u8Adjreg = ((iAdjint >> 3) & 0xFF);
            u8Adjregu = (iAdjint >> 11) & 0xFF;
        }

        //
        // Load the CALRU register.
        //
        res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_CAL_RC_HI, u8Adjregu);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }

        //
        // Load the CALRL register.
        //
        res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_CAL_RC_LOW, u8Adjreg);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }
    return AMX8X5_FUNC_END(Ok);
}

/**
 ******************************************************************************
 ** \brief  This function is stopping / releasing stop of the RTC
 **
 ** Dets the STOP bit in the Control1 register, 
 ** see also #AMX8X5_REG_CONTROL_1 definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bStop          true to stop, false to release stop
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_Stop(&stcRtcConfig,true); //stop the RTC
 **
 ** Amx8x5_Stop(&stcRtcConfig,false); //release stop for the RTC
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_Stop(stc_amx8x5_handle_t* pstcHandle, bool bStop)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_Stop");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bStop)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_STOP_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_STOP_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  This function controlling a static value which may be driven 
 **         on the PSW/nIRQ2 pin
 **
 ** The OUTB bit cannot be set to 1 if the @link AMX8X5_REG_OSC_STATUS LKO2 bit @endlink 
 ** is 1. see also #AMX8X5_REG_OSC_STATUS for LKO2.
 **
 ** Sets the OUTB bit in the Control1 register, 
 ** see also #AMX8X5_REG_CONTROL_1 definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bOnOff          true for ON, false for OFF
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_CtrlOutB(&stcRtcConfig,true);  //set PSW/nIRQ2 pin on
 **                                       //(The OUTB bit cannot be set to 1 if the LKO2 bit is 1)
 **
 ** Amx8x5_CtrlOutB(&stcRtcConfig,false); //set PSW/nIRQ2 pin off
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_CtrlOutB(stc_amx8x5_handle_t* pstcHandle, bool bOnOff)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_CtrlOutB");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bOnOff)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_OUTB_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_OUTB_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  This function controlling a static value which may be driven 
 **         on the FOUT/nIRQ pin
 **
 ** This bit also defines the default value for the Square Wave output 
 ** when @link AMX8X5_REG_SQW SQWE @endlink is not asserted
 **
 ** Sets the OUT bit in the Control1 register, see also #AMX8X5_REG_CONTROL_1 definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bOnOff         true for ON, false for OFF
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_CtrlOut(&stcRtcConfig,true);  //set FOUT/nIRQ pin on
 **
 ** Amx8x5_CtrlOut(&stcRtcConfig,false); //setFOUT/nIRQ pin off
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_CtrlOut(stc_amx8x5_handle_t* pstcHandle, bool bOnOff)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_CtrlOut");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bOnOff)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_OUT_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_OUT_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  Reset Polarity.
 **
 ** see also #AMX8X5_REG_CONTROL_1 definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bAssertHigh    Reset Polarity. When true, the nRST pin is asserted high. When false, the nRST pin is asserted low.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_SetResetPolarity(&stcRtcConfig,true);  //nRST pin is asserted high
 **
 ** Amx8x5_SetResetPolarity(&stcRtcConfig,false);  //the nRST pin is asserted low
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_SetResetPolarity(stc_amx8x5_handle_t* pstcHandle, bool bAssertHigh)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetResetPolarity");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bAssertHigh)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_RSP_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_RSP_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  Auto reset enable.
 **
 ** see also #AMX8X5_REG_CONTROL_1 definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnabled       When true, a read of the Status register will cause any interrupt bits (TIM, BL,
 **                        ALM, WDT, XT1, XT2) to be cleared. When false, the bits must be explicitly cleared by writing the
 **                        Status register.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_AutoResetStatus(&stcRtcConfig,true);  //Enable auto reset
 **
 ** Amx8x5_AutoResetStatus(&stcRtcConfig,false);  //Disable auto reset
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_AutoResetStatus(stc_amx8x5_handle_t* pstcHandle, bool bEnabled)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_AutoResetStatus");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bEnabled)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_ARST_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_ARST_MSK));
    }
}
      
/**
 ******************************************************************************
 ** \brief  PSW/nIRQ2 Feature (only available at AM18X5)
 **
 ** see also #AMX8X5_REG_CONTROL_1 definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnabled       When true, the PSW/nIRQ2 pin is driven by an approximately 1 Ohm pull-down which allows the
 **                        AM18X5 to switch power to other system devices through this pin. When false, the PSW/nIRQ2pin
 **                        is a normal open drain output.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_SetPswHighCurrent(&stcRtcConfig,true);  //Enable high current PSW
 **
 ** Amx8x5_SetPswHighCurrent(&stcRtcConfig,false);  //Disable the high current PSW
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_SetPswHighCurrent(stc_amx8x5_handle_t* pstcHandle, bool bEnabled)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetPswHighCurrent");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if ((pstcHandle->enRtcType != AMx8x5Type1805) && (pstcHandle->enRtcType != AMx8x5Type1815)) 
    {
        return AMX8X5_FUNC_END(ErrorInvalidMode);
    }
    
    if (true == bEnabled)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_PWR2_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_1,AMX8X5_REG_CONTROL_1_PWR2_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  nEXTR pin to generate nRST.
 **
 ** see also #AMX8X5_REG_CONTROL_2 definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnabled       When true, enable the nEXTR pin to generate nRST.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_UsenExtrAsReset(&stcRtcConfig,true);  //Enable the nEXTR pin to generate nRST
 **
 ** Amx8x5_UsenExtrAsReset(&stcRtcConfig,false);  //Disable the nEXTR pin to generate nRST
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_UsenExtrAsReset(stc_amx8x5_handle_t* pstcHandle, bool bEnabled)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_UsenExtrAsReset");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bEnabled)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,0x20));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_2,0x20));
    }
}


/**
 ******************************************************************************
 ** \brief  Control and configuration signals for the flexible output FOUT/nIRQ
 **
 ** see also #AMX8X5_REG_CONTROL_2 definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  enMode         Mode, see also en_amx8x5_out1_mode_t
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_SetOut2Mode(&stcRtcConfig,AMx8x5Out1nIRQAtIrqElseOut);  //nIRQ if at least one interrupt is enabled, else OUT
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_SetOut1Mode(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_out1_mode_t enMode)
{
    en_result_t res;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetOut1Mode");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    res = Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_2,AMX8X5_REG_CONTROL_2_OUT1S_MSK);
    if (res != Ok)
    {
        return AMX8X5_FUNC_END(res);
    }
    
    switch(enMode)
    {
      case AMx8x5Out1nIRQAtIrqElseOut:
        return AMX8X5_FUNC_END(Ok);
      case AMx8x5Out1SwqIfSqweElseOut:
         res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,(0x1 << AMX8X5_REG_CONTROL_2_OUT1S_POS));
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
         return AMX8X5_FUNC_END(Ok);
      case AMx8x5Out1SwqIfSqweElsenIRQAtIrqElseOut:
        res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,(0x2 << AMX8X5_REG_CONTROL_2_OUT1S_POS));
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
         return AMX8X5_FUNC_END(Ok);
      case AMx8x5Out1nAIRQIfAieElseOut:
        res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,(0x3 << AMX8X5_REG_CONTROL_2_OUT1S_POS));
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
         return AMX8X5_FUNC_END(Ok);
      default:
        return AMX8X5_FUNC_END(ErrorInvalidParameter);
    }
}

/**
 ******************************************************************************
 ** \brief  ontrol and configuration signals for the flexible output PSW/nIRQ2
 **
 ** see also #AMX8X5_REG_CONTROL_2 definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  enMode         Mode, see also en_amx8x5_out2_mode_t
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_SetOut2Mode(&stcRtcConfig,AMx8x5Out2Sleep);  //Set OUT2 mode sleep
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_SetOut2Mode(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_out2_mode_t enMode)
{
    en_result_t res;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetOut2Mode");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    res = Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_2,AMX8X5_REG_CONTROL_2_OUT2S_MSK);
    if (res != Ok)
    {
        return AMX8X5_FUNC_END(res);
    }
    
    switch(enMode)
    {
      case AMx8x5Out2nIRQAtIrqElseOutB:
         return AMX8X5_FUNC_END(Ok);
      case AMx8x5Out2SwqIfSqweElseOutB:
         res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,(0x1 << AMX8X5_REG_CONTROL_2_OUT2S_POS));
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
         return AMX8X5_FUNC_END(Ok);
      case AMx8x5Out2nAIRQIfAieElseOutB:
         res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,(0x3 << AMX8X5_REG_CONTROL_2_OUT2S_POS));
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
         return AMX8X5_FUNC_END(Ok);
      case AMx8x5Out2TIRQIfTieElseOutB:
         res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,(0x4 << AMX8X5_REG_CONTROL_2_OUT2S_POS));
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
         return AMX8X5_FUNC_END(Ok);
      case AMx8x5Out2nTIRQIfTieElseOutB:
         res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,(0x5 << AMX8X5_REG_CONTROL_2_OUT2S_POS));
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
         return AMX8X5_FUNC_END(Ok);
      case AMx8x5Out2Sleep:
         res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,(0x6 << AMX8X5_REG_CONTROL_2_OUT2S_POS));
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
         return AMX8X5_FUNC_END(Ok);
      case AMx8x5Out2OutB:
         res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2,(0x7 << AMX8X5_REG_CONTROL_2_OUT2S_POS));
         if (res != Ok)
         {
            return AMX8X5_FUNC_END(res);
         }
         return AMX8X5_FUNC_END(Ok);
      default:
        return AMX8X5_FUNC_END(ErrorInvalidParameter);
    }
}

/**
 ******************************************************************************
 ** \brief  XT1 Interrupt Enable.
 **
 ** see also #AMX8X5_REG_INT_MASK definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnabled       true: The WDI input pin will generate the XT1 interrupt when the edge specified by EX1P occurs.
 **                        false: Disable the XT2 interrupt.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableIrqXt1OnExti(&stcRtcConfig,true);  //Enable the XT1 interrupt.
 **
 ** Amx8x5_EnableIrqXt1OnExti(&stcRtcConfig,false); //Disable the XT1 interrupt.
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_EnableIrqXt1OnExti(stc_amx8x5_handle_t* pstcHandle, bool bEnabled)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_EnableIrqXt1OnExti");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bEnabled)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_EX1E_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_EX1E_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  XT2 Interrupt Enable.
 **
 ** see also #AMX8X5_REG_INT_MASK definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnabled       true: The WDI input pin will generate the XT2 interrupt when the edge specified by EX2P occurs.
 **                        false: Disable the XT2 interrupt.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableIrqXt2OnWdi(&stcRtcConfig,true);  //Enable the XT2 interrupt.
 **
 ** Amx8x5_EnableIrqXt2OnWdi(&stcRtcConfig,false); //Disable the XT2 interrupt.
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_EnableIrqXt2OnWdi(stc_amx8x5_handle_t* pstcHandle, bool bEnabled)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_EnableIrqXt2OnWdi");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bEnabled)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_EX2E_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_EX2E_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  Alarm Interrupt Enable.
 **
 ** see also #AMX8X5_REG_INT_MASK definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnabled       true: A match of all the enabled alarm registers will generate an IRQ interrupt signal. 
 **                        false: Disable the alarm interrupt.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableIrqAlarm(&stcRtcConfig,true);  //set Alarm Interrupt
 **
 ** Amx8x5_EnableIrqAlarm(&stcRtcConfig,false); //clear Alarm Interrupt
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_EnableIrqAlarm(stc_amx8x5_handle_t* pstcHandle, bool bEnabled)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_EnableIrqAlarm");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bEnabled)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_AIE_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_AIE_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  Timer Interrupt Enable.
 **
 ** see also #AMX8X5_REG_INT_MASK definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnabled       true: The Countdown Timer will generate an IRQ interrupt signal 
 **                        and set the TIM flag when the timer reaches 0.
 **                        false: Disable the timer interrupt.
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableIrqTimer(&stcRtcConfig,true);  //set Timer Interrupt
 **
 ** Amx8x5_EnableIrqTimer(&stcRtcConfig,false); //clear Timer Interrupt
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_EnableIrqTimer(stc_amx8x5_handle_t* pstcHandle, bool bEnabled)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_EnableIrqTimer");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bEnabled)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_TIE_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_TIE_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  Battery Low Interrupt Enable.
 **
 ** see also #AMX8X5_REG_INT_MASK definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  bEnabled       true for enable, false for disable
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableIrqBatteryLow(&stcRtcConfig,true);  //set Battery Low Interrupt
 **
 ** Amx8x5_EnableIrqBatteryLow(&stcRtcConfig,false); //clear Battery Low Interrupt
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_EnableIrqBatteryLow(stc_amx8x5_handle_t* pstcHandle, bool bEnabled)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_EnableIrqBatteryLow");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bEnabled)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_BLIE_MSK));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_BLIE_MSK));
    }
}

/**
 ******************************************************************************
 ** \brief  This function is controlling the behavior of the I/O pins under 
 **         various power down conditions.
 **
 ** see also #AMX8X5_REG_OCTRL definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  u8OutputMask   Mask of outputs to enable / disable as defined in #AMX8X5_REG_OCTRL
 **
 ** \param  bEnable        1 for enable, 0 for disable
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 ** Example:
 ** @code
 ** Amx8x5_EnableOutput(&stcRtcConfig,AMX8X5_REG_OCTRL_O1EN_MSK,true);  //set FOUT/nIRQ as output
 **
 ** Amx8x5_EnableOutput(&stcRtcConfig,AMX8X5_REG_OCTRL_O1EN_MSK,false); //dont use FOUT/nIRQ as output
 ** @endcode
 **
 ******************************************************************************/
en_result_t Amx8x5_EnableOutput(stc_amx8x5_handle_t* pstcHandle, uint8_t u8OutputMask, bool bEnable)
{
    AMX8X5_DEBUG_FUNC_START("Amx8x5_EnableOutput");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    if (true == bEnable)
    {
        return AMX8X5_FUNC_END(Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_OCTRL,u8OutputMask));
    } else
    {
        return AMX8X5_FUNC_END(Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_OCTRL,u8OutputMask));
    }
}

/**
 ******************************************************************************
 ** \brief  This function controls the Trickle Charger
 **
 ** see also #AMX8X5_REG_TRICKLE definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  enDiode        Diode Select.
 **                        AMx8x5TrickleDiodeDisabled => disable the Trickle Charger.
 **                        AMx8x5TrickleDiodeSchottky => inserts a schottky diode into the trickle charge circuit, with a voltage drop of 0.3V
 **                        AMx8x5TrickleDiodeNormal => inserts a standard diode into the trickle charge circuit, with a voltage drop of 0.6V
 ** 
 ** \param  enResistor     Output Resistor.
 **
 ** \param  bEnable        true for enable, false for disable
 **                        AMx8x5TrickleResistorDisabled = 0> disable the Trickle Charger.
 **                        AMx8x5TrickleResistor3K => this selects 3K Ohm as the output resistor of the trickle charge circuit
 **                        AMx8x5TrickleResistor6K => this selects 6K Ohm as the output resistor of the trickle charge circuit
 **                        AMx8x5TrickleResistor11K => this selects 11K Ohm as the output resistor of the trickle charge circuit
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 **
 ******************************************************************************/
en_result_t Amx8x5_EnableTrickleCharger(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_trickle_diode_t enDiode, en_amx8x5_trickle_resistor_t enResistor, bool bEnable)
{
    uint8_t u8Tmp = 0;
    en_result_t res;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_EnableTrickleCharger");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    //
    // A value of 1010 enables the trickle charge function. All other values disable the Trickle Charger.
    //
    if (true == bEnable)
    {
        u8Tmp |= (AMX8X5_REG_TRICKLE_TCS_ENABLE_VALUE << AMX8X5_REG_TRICKLE_TCS_POS);
    }
    
    switch(enDiode)
    {
        case AMx8x5TrickleDiodeSchottky:
            u8Tmp |= (0x1) << AMX8X5_REG_TRICKLE_DIODE_POS;
            break;
        case AMx8x5TrickleDiodeNormal:
            u8Tmp |= (0x2) << AMX8X5_REG_TRICKLE_DIODE_POS;
            break;
        default:
           u8Tmp &= ~(AMX8X5_REG_TRICKLE_TCS_ENABLE_VALUE << AMX8X5_REG_TRICKLE_TCS_POS);
           break;
    }
    
    switch(enResistor)
    {
        case AMx8x5TrickleResistor3K:
            u8Tmp |= (0x1) << AMX8X5_REG_TRICKLE_DIODE_POS;
            break;
        case AMx8x5TrickleResistor6K:
            u8Tmp |= (0x2) << AMX8X5_REG_TRICKLE_DIODE_POS;
            break;
        case AMx8x5TrickleResistor11K:
            u8Tmp |= (0x3) << AMX8X5_REG_TRICKLE_DIODE_POS;
            break;
        default:
           u8Tmp &= ~(AMX8X5_REG_TRICKLE_TCS_ENABLE_VALUE << AMX8X5_REG_TRICKLE_TCS_POS);
           break;
    }
    
    //
    // The Key Register must be written with the value 0x9D in order to enable access to this register.
    //
    res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_CONFIG_KEY, 0x9D);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    //
    // Write the trickle register
    //
    return AMX8X5_FUNC_END(Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_TRICKLE,u8Tmp));
}
    
/**
 ******************************************************************************
 ** \brief  This function controls the VBAT Reference Voltage
 **
 ** This selects the voltage reference which is compared to the battery voltage VBAT 
 ** to produce the BBOD signal.
 **
 ** see also #AMX8X5_REG_BREF_CTRL definition
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  enBref         This selects the voltage reference
 **                        AMx8x5BatReferenceFalling25V_Rising30V => VBAT Falling Voltage 2.5V, VBAT Rising Voltage 3.0V
 **                        AMx8x5BatReferenceFalling21V_Rising25V => VBAT Falling Voltage 2.1V, VBAT Rising Voltage 2.5V
 **                        AMx8x5BatReferenceFalling18V_Rising22V => VBAT Falling Voltage 1.8V, VBAT Rising Voltage 2.2V
 **                        AMx8x5BatReferenceFalling14V_Rising16V => VBAT Falling Voltage 1.4V, VBAT Rising Voltage 1.6V
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 **
 ******************************************************************************/
en_result_t Amx8x5_SetBatteryReferenceVoltage(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_bat_reference_t enBref)
{
    en_result_t res;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetBatteryReferenceVoltage");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    
    //
    // The Key Register must be written with the value 0x9D in order to enable access to this register.
    //
    res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_CONFIG_KEY, 0x9D);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    switch(enBref)
    {
        case AMx8x5BatReferenceFalling25V_Rising30V:
            //
            // Write the BREF_CTRL register
            //
            return AMX8X5_FUNC_END(Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_BREF_CTRL,0x70));
        case AMx8x5BatReferenceFalling21V_Rising25V:
            //
            // Write the BREF_CTRL register
            //
            return AMX8X5_FUNC_END(Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_BREF_CTRL,0xB0));
        case AMx8x5BatReferenceFalling18V_Rising22V:
            //
            // Write the BREF_CTRL register
            //
            return AMX8X5_FUNC_END(Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_BREF_CTRL,0xD0));
        case AMx8x5BatReferenceFalling14V_Rising16V:
            //
            // Write the BREF_CTRL register
            //
            return AMX8X5_FUNC_END(Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_BREF_CTRL,0xF0));
        default:
            return AMX8X5_FUNC_END(Error);
    }
}

/**
 ******************************************************************************
 ** \brief  Configure and enable the square wave output.
 **
 ** see also #AMX8X5_REG_SQW definition.
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  u8SQFS         square wave output select (0 to 31) as defined in SQFS in #AMX8X5_REG_SQW
 **
 ** \param  u8PinMsk       output pin for SQW (may be ORed) in addition to CLKOUT
 **                        0 => disable SQW
 **                        1 => FOUT
 **                        2 => PSW/nIRQ2
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 **
 ******************************************************************************/
en_result_t Amx8x5_SetSquareWaveOutput(stc_amx8x5_handle_t* pstcHandle, uint8_t u8SQFS, uint8_t u8PinMsk)
{
    uint8_t u8Temp;
    en_result_t res;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetSquareWaveOutput");
    
    //
    // Set up SQW multiplexor:
    // Read the SQW register.
    // Load u8SQFS, set SQWE.
    //
    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_SQW,&u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    u8Temp = (u8Temp & 0x70) | u8SQFS | 0x80;

    if (u8PinMsk == 0)
    {
        //
        // Clear SQWE.
        //
        u8Temp &= 0x7F;
    }

    if (u8PinMsk & 0x1)
    {
        //
        // Enable FOUT:
        // Clear OUT1S.
        // Load OUT1S with 1.
        //
        res = Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_2,0x03);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2, 0x01);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }
    if (u8PinMsk & 0x2)
    {
        //
        // Enable PSW/nIRQ2:
        // Clear OUT2S.
        // Load OUT2S with 1.
        //
        res = Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_CONTROL_2,0x1C);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_CONTROL_2, 0x04);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }

    //
    // Write the SQW register.
    //
    res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_SQW,u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    return AMX8X5_FUNC_END(Ok);
}



/**
 ******************************************************************************
 ** \brief  Set up sleep mode (AM18x5 only)
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  u8Timeout  minimum timeout period in 7.8 ms periods (0 to 7)
 **
 ** \param  enMode     sleep mode (nRST modes not available in AM08xx)
 **                    AMx8x5nRstLowInSleep => nRST is pulled low in sleep mode
 **                    AMx8x5PswIrq2HighInSleep =>  PSW/nIRQ2 is pulled high on a sleep
 **                    AMx8x5nRstLoPswIrq2HighInSleep =>  nRST pulled low and PSW/nIRQ2 pulled high on sleep
 **
 ** \return Ok on sleep request accepted, sleep mode will be initiated in u8Timeout seconds, 
 **         OperationInProgress on sleep request declined, interrupt is currently pending,
 **         ErrorInvalidMode on sleep request declined, no sleep trigger interrupt enabled
 ** 
 **
 ******************************************************************************/
en_result_t Amx8x5_SetSleepMode(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Timeout, en_amx8x5_sleep_mode_t enMode)
{
    uint8_t u8SLRES;
    uint8_t u8Temp0, u8Temp1;
    en_result_t res;
      
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetSleepMode");
    
    if (pstcHandle == NULL) 
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }  
    
    if ((pstcHandle->enRtcType != AMx8x5Type1805) && (pstcHandle->enRtcType != AMx8x5Type1815)) 
    {
        return AMX8X5_FUNC_END(Error);
    }
    
    if (enMode != AMx8x5nRstLowInSleep)
    {
        //
        // Sleep to PSW/nIRQ2.
        // Read OUT2S.
        // MUST NOT WRITE OUT2S WITH 000.
        // Write value to OUT2S.
        //
        res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_CONTROL_2,&u8Temp0);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        u8Temp0 = (u8Temp0 & 0xE3) | 0x18;
        res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_CONTROL_2,u8Temp0);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        u8SLRES = 0;
    }

    if (enMode != AMx8x5PswIrq2HighInSleep)
    {
        //
        // Sleep to nRST.
        //
        u8SLRES = 1;
    }

    //
    // Assemble SLEEP register value.
    // Write to the register.
    //
    u8Temp0 = u8Timeout | (u8SLRES << 6) | 0x80;
    res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_SLEEP_CTRL,u8Temp0);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }

    //
    // Determine if SLEEP was accepted:
    // Get SLP bit.
    //
    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_SLEEP_CTRL,&u8Temp0);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    u8Temp0 = u8Temp0 & 0x80;

    if (u8Temp0 == 0)
    {
        //
        // SLEEP did not happen. Determine why and return reason:
        // Get status register interrupt enables.
        // Get WDT register.
        //
        res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_INT_MASK,&u8Temp0);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        u8Temp0 = u8Temp0 & 0x0F;
        res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_WDT,&u8Temp1);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        if ((u8Temp0 == 0) & (((u8Temp1 & 0x7C) == 0) ||
                               ((u8Temp1 & 0x80) == 0x80)))
        {
            //
            // No trigger interrupts enabled.
            //
            return AMX8X5_FUNC_END(ErrorInvalidMode);
        }
        else
        {
            //
            // Interrupt pending.
            //
            return AMX8X5_FUNC_END(OperationInProgress);
        }
    }
    else
    {
        //
        // SLEEP request successful.
        //
        return AMX8X5_FUNC_END(Ok);
    }
}

/**
 ******************************************************************************
 ** \brief  Select an oscillator mode.
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  enSelect     the oscillator to select
 **                      AMx8x5Xt32KHzNoSwitch => 32 KHz XT oscillator, no automatic oscillator switching
 **                      AMx8x5Xt32KHzSwitchRcOnBat => 32 KHz XT oscillator, automatic oscillator switching to RC on
 **                           switch to battery power
 **                      AMx8x5nRc128Hz => 128 Hz RC oscillator
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 **
 ******************************************************************************/
en_result_t Amx8x5_SelectOscillatorMode(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_osc_select_t enSelect)
{
    uint8_t i;
    uint8_t u8Temp;
    volatile uint32_t u32Timeout;
    en_result_t res;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SelectOscillatorMode");
    
    //
    // Read Oscillator Control register.
    //
    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_OSC_CONTROL,&u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    u8Temp = u8Temp & 0x67;

    //
    // Enable Oscillator Register writes.
    // Write the Key register.
    //
    res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_CONFIG_KEY, AMX8X5_CONFIG_KEY_VAL);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    switch (enSelect)
    {
        //
        // Do nothing, clear Key register.
        //
        case AMx8x5Xt32KHzNoSwitch:
          res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_OSC_CONTROL, u8Temp);
            if (res != Ok) 
            {
                return AMX8X5_FUNC_END(res);
            }
        break;

        //
        // Set AOS.
        //
        case AMx8x5Xt32KHzSwitchRcOnBat:
            u8Temp = u8Temp | 0x10;
            res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_OSC_CONTROL, u8Temp);
            if (res != Ok) 
            {
                return AMX8X5_FUNC_END(res);
            }
        break;

        //
        // Set OSEL
        //
        case AMx8x5nRc128Hz:
            u8Temp = u8Temp | 0x80;
            res = Amx8x5_WriteByte(pstcHandle, AMX8X5_REG_OSC_CONTROL, u8Temp);
            if (res != Ok) 
            {
                return AMX8X5_FUNC_END(res);
            }
        break;
        
        default:
            return AMX8X5_FUNC_END(ErrorInvalidParameter);
    }

    //
    // Wait to make sure switch occurred by testing OMODE.
    //
    for (i = 0; i < 100; i++)
    {
        //
        // Wait 100 ms.
        // Read OMODE.
        //
        //SystemCoreClockUpdate();
        //u32Timeout = SystemCoreClock/25;
        u32Timeout = 100000;
        while(u32Timeout > 0) 
        {
            u32Timeout--;
        }
        
        res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_OSC_STATUS,&u8Temp);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        u8Temp = (u8Temp & 0x10) >> 4;

        if (u8Temp == (((uint8_t)enSelect) >> 1))
        {
            //
            // Successful switch.
            //
            return AMX8X5_FUNC_END(Ok);
        }
    }

    //
    // Return Error.
    //
    return Error;
}


/**
 ******************************************************************************
 ** \brief  Select an oscillator mode.
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param u32Period     timeout period in ms (65 to 124,000)
 **
 ** \param  enPin        pin to generate the watchdog signal
 **                      AMx8x5WatchdogInterruptPinDisable => disable WDT
 **                      AMx8x5WatchdogInterruptPinFOUTnIRQ => generate an interrupt on FOUT/nIRQ
 **                      AMx8x5WatchdogInterruptPinPSWnIRQ2 => generate an interrupt on PSW/nIRQ2
 **                      AMx8x5WatchdogInterruptPinnRST => generate a reset on nRST (AM18xx only)
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 **
 ******************************************************************************/
en_result_t Amx8x5_SetWatchdog(stc_amx8x5_handle_t* pstcHandle, uint32_t u32Period, en_amx8x5_watchdog_interrupt_pin_t enPin)
{
    uint8_t u8WDTreg;
    uint8_t u8WDS;
    uint8_t u8BMB;
    uint8_t u8WRB;
    en_result_t res;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetWatchdog");
    
    //
    // Disable the WDT with BMB = 0.
    // Clear the WDT flag.
    //
    res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_WDT,0);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    res = Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_STATUS,AMX8X5_REG_STATUS_WDT_MSK);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }

    //
    // Use the shortest clock interval which will allow the selected period.
    //
    if (u32Period < (31000 / 16))
    {
        //
        // Use 16 Hz.
        //
        u8WRB = 0;
        u8BMB = (u32Period * 16) / 1000;
    }
    else if (u32Period < (31000 / 4))
    {
        //
        // Use 4 Hz.
        //
        u8WRB = 1;
        u8BMB = (u32Period * 4) / 1000;
    }
    else if (u32Period < 31000)
    {
        //
        // Use 1 Hz.
        //
        u8WRB = 2;
        u8BMB = u32Period / 1000;
    }
    else
    {
        //
        // Use 1/4 Hz.
        //
        u8WRB = 3;
        u8BMB = u32Period / 4000;
    }

    switch (enPin)
    {
        //
        // Disable WDT.
        //
        case AMx8x5WatchdogInterruptPinDisable:
        u8WDS = 0;
        u8BMB = 0;
        break;

        //
        // Interrupt on FOUT/nIRQ.
        //
        case AMx8x5WatchdogInterruptPinFOUTnIRQ:
        //
        // Select interrupt.
        // Clear the OUT1S field
        //
        u8WDS = 0;
        res = Amx8x5_SetOut1Mode(pstcHandle,AMx8x5Out1nIRQAtIrqElseOut);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        break;

        //
        // Interrupt on PSW/nIRQ2.
        //
        case AMx8x5WatchdogInterruptPinPSWnIRQ2:
        //
        // Select interrupt.
        // Clear the OUT2S field.
        //
        u8WDS = 0;
        res = Amx8x5_SetOut2Mode(pstcHandle,AMx8x5Out2nIRQAtIrqElseOutB);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        break;

        //
        // Interrupt on nRST.
        //
        case AMx8x5WatchdogInterruptPinnRST:
        default:
        //
        // Select reset out.
        //
        u8WDS = 1;
        break;
    }

    //
    // Create the correct value.
    // Write the register.
    //
    u8WDTreg = (u8WDS * 0x80) + (u8BMB * 0x4) + u8WRB;
 
    res = Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_WDT,u8WDTreg);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    return Ok;
}

/**
 ******************************************************************************
 ** \brief  Set up autocalibration.
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  enPeriod       the repeat period for autocalibration.
 **                        AMx8x5AutoCalibrationPeriodDisable => disable autocalibration
 **                        AMx8x5AutoCalibrationPeriodSingleCycle => execute a single autocalibration cycle
 **                        AMx8x5AutoCalibrationPeriodCycleSecods1024 => execute a cycle every 1024 seconds (~17 minutes)
 **                        AMx8x5AutoCalibrationPeriodCycleSecods512 => execute a cycle every 512 seconds (~8.5 minutes)
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 **
 ******************************************************************************/
en_result_t  Amx8x5_SetAutocalibration(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_autocalibration_period_t enPeriod)
{
    uint8_t u8Temp;
    en_result_t res;
    volatile uint32_t u32Timeout;
    
    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetAutocalibration");
    
    //
    // Read Oscillator Control, mask ACAL.
    //
    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_OSC_CONTROL,&u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }

    u8Temp &= 0x9F;

    //
    // Write the Key register.
    //
    res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_CONFIG_KEY,AMX8X5_CONFIG_KEY_VAL);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }   
    

    switch (enPeriod)
    {
        case AMx8x5AutoCalibrationPeriodDisable:
            //
            // Set ACAL to 0.
            //
            res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_OSC_CONTROL,u8Temp);
            if (res != Ok) 
            {
                return AMX8X5_FUNC_END(res);
            }   
            break;

        case AMx8x5AutoCalibrationPeriodSingleCycle:
            //
            // Set ACAL to 2
            //
            u8Temp |= 0x40;
            res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_OSC_CONTROL,u8Temp);
            if (res != Ok) 
            {
                return AMX8X5_FUNC_END(res);
            }  

            //
            // Wait for initiation of autocal (10 ms).
            //
            //SystemCoreClockUpdate();
            //u32Timeout = SystemCoreClock/250;
            u32Timeout = 100000;
            while(u32Timeout > 0) 
            {
                u32Timeout--;
            }

            //
            // Write the Key register.
            //
            res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_CONFIG_KEY,AMX8X5_CONFIG_KEY_VAL);
            if (res != Ok) 
            {
                return AMX8X5_FUNC_END(res);
            }   

            //
            // Mask ACAL.
            // Set ACAL to 0
            //
            u8Temp = u8Temp & 0x9F;
            res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_OSC_CONTROL,u8Temp);
            if (res != Ok) 
            {
                return AMX8X5_FUNC_END(res);
            }  
            break;

        case AMx8x5AutoCalibrationPeriodCycleSecods1024:
            //
            // Set ACAL to 2.
            //
            u8Temp = u8Temp | 0x40;
            res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_OSC_CONTROL,u8Temp);
            if (res != Ok) 
            {
                return AMX8X5_FUNC_END(res);
            }  
            break;

        case AMx8x5AutoCalibrationPeriodCycleSecods512:
            //
            // Set ACAL to 3.
            //
            u8Temp = u8Temp | 0x60;
            res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_OSC_CONTROL,u8Temp);
            if (res != Ok) 
            {
                return AMX8X5_FUNC_END(res);
            }  
            break;
    }
    return AMX8X5_FUNC_END(Ok);
}

/**
 ******************************************************************************
 ** \brief  Configure and set the countdown.
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  enRange        AMx8x5PeriodeUs => iPeriod in us, AMx8x5PeriodeSeconds => iPeriod in seconds
 **
 ** \param  iPeriod        the iPeriod of the countdown timer.
 **   
 ** \param  enRepeat       Configure the interrupt output type:
 **                        AMx8x5RepeatModeSingleLevel => generate a single level interrupt
 **                        AMx8x5RepeatModeRepeatedPulseShort => generate a repeated pulsed interrupt, 1/4096 s (XT mode), 1/128 s (RC mode), en8Range must be AMx8x5PeriodeUs
 **                        AMx8x5RepeatModeSinglePulseShort => generate a single pulsed interrupt, 1/4096 s (XT mode), 1/128 s (RC mode), en8Range must be AMx8x5PeriodeUs
 **                        AMx8x5RepeatModeRepeatedPulseMedium => generate a repeated pulsed interrupt, 1/128 s, en8Range must be AMx8x5PeriodeUs
 **                        AMx8x5RepeatModeSinglePulseMedium => generate a single pulsed interrupt, 1/128 s, en8Range must be AMx8x5PeriodeUs
 **                        AMx8x5RepeatModeRepeatedPulseLong => generate a repeated pulsed interrupt, 1/64 s, en8Range must be AMx8x5PeriodeSeconds
 **                        AMx8x5RepeatModeSinglePulseLong => generate a single pulsed interrupt, 1/64 s, en8Range must be AMx8x5PeriodeSeconds
 **
 ** \param enPin           Select the pin to generate a countdown interrupt:
 **                        AMx8x5CountdownInterruptPinDisable => disable the countdown timer
 **                        AMx8x5CountdownInterruptPinnTIRQLow =>generate an interrupt on nTIRQ only, asserted low
 **                        AMx8x5CountdownInterruptPinFOUTnIRQLownTIRQLow => generate an interrupt on FOUT/nIRQ and nTIRQ, both asserted low
 **                        AMx8x5CountdownInterruptPinPSWnIRQ2LownTIRQLow => generate an interrupt on PSW/nIRQ2 and nTIRQ, both asserted low
 **                        AMx8x5CountdownInterruptPinCLKOUTnIRQ3LownTIRQLow => generate an interrupt on CLKOUT/nIRQ3 and nTIRQ, both asserted low
 **                        AMx8x5CountdownInterruptPinCLKOUTnIRQ3HighnTIRQLow => generate an interrupt on CLKOUT/nIRQ3 (asserted high) and nTIRQ (asserted low)    
 **
 ** \return Ok on success, else the Error as en_result_t
 ** 
 **
 ******************************************************************************/
en_result_t Amx8x5_SetCountdown(stc_amx8x5_handle_t* pstcHandle, en_amx8x5_periode_range_t enRange, int32_t iPeriod, en_amx8x5_countdown_interrupt_output_t enRepeat, en_amx8x5_countdown_interrupt_pin_t enPin)
{
    uint8_t u8TM = 0;
    uint8_t u8TRPT = 0;
    uint8_t u8TFS = 0;
    uint8_t u8TE;
    uint8_t u8Temp;
    uint8_t u8TCTRL;
    int32_t u8Timer = 0;
    en_amx8x5_calibration_mode_t enOMODE;
    en_result_t res;

    AMX8X5_DEBUG_FUNC_START("Amx8x5_SetCountdown");
    
    //
    // 0 = XT, 1 = RC
    //
    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_OSC_STATUS,&u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    if (u8Temp & AMX8X5_REG_OSC_STATUS_OMODE_MSK)
    {
        enOMODE = AMx8x5ModeCalibrateRC;
    } else
    {
        enOMODE = AMx8x5ModeCalibrateXT;
    }

    if (enPin == AMx8x5CountdownInterruptPinDisable)
    {
        u8TE = 0;
    }
    else
    {
        u8TE = 1;
        if (enRepeat == AMx8x5RepeatModeSingleLevel)
        {
            //
            // Level interrupt
            //
            u8TM = 1;
            u8TRPT = 0;
            if (enRange == AMx8x5PeriodeUs)
            {
                // Microseconds
                if (enOMODE == AMx8x5ModeCalibrateXT)
                {
                    //
                    // XT Mode.
                    // Use 4K Hz.
                    //
                    if (iPeriod <= 62500)
                    {
                        u8TFS = 0;
                        u8Timer = (iPeriod * 4096);
                        u8Timer = u8Timer / 1000000;
                        u8Timer = u8Timer - 1;
                    }

                    //
                    // Use 64 Hz
                    //
                    else if (iPeriod <= 16384000)
                    {
                        u8TFS = 1;
                        u8Timer = (iPeriod * 64);
                        u8Timer /= 1000000;
                        u8Timer = u8Timer - 1;
                    }

                    //
                    // Else, use 1 Hz.
                    //
                    else
                    {
                        u8TFS = 2;
                        u8Timer = iPeriod / 1000000;
                        u8Timer = u8Timer - 1;
                    }
                }
                else
                {
                    //
                    // RC Mode.
                    // Use 128 Hz.
                    //
                    if (iPeriod <= 2000000)
                    {
                        u8TFS = 0;
                        u8Timer = (iPeriod * 128);
                        u8Timer /= 1000000;
                        u8Timer = u8Timer - 1;
                    }

                    //
                    // Use 64 Hz.
                    //
                    else if (iPeriod <= 4000000)
                    {
                        u8TFS = 1;
                        u8Timer = (iPeriod * 64);
                        u8Timer /= 1000000;
                        u8Timer = u8Timer - 1;
                    }

                    //
                    // Else, use 1 Hz.
                    //
                    else
                    {
                        u8TFS = 2;
                        u8Timer = iPeriod / 1000000;
                        u8Timer = u8Timer - 1;
                    }
                }
            }
            else
            {
                //
                // Seconds
                //
                if (iPeriod <= 256)
                {
                    //
                    // Use 1 Hz.
                    //
                    u8TFS = 2;
                    u8Timer = iPeriod - 1;
                }
                else
                {
                    //
                    // Use 1/60 Hz.
                    //
                    u8TFS = 3;
                    u8Timer = iPeriod / 60;
                    u8Timer = u8Timer - 1;
                }
            }
        }
        else
        {
            //
            // Pulse interrupts.
            // Set up u8Repeat.
            //
            u8TM = 0;
            u8TRPT = ENUM_TO_UINT8(enRepeat) & 0x01;
            if (ENUM_TO_UINT8(enRepeat) < 3)
            {
                u8TFS = 0;
                if (enOMODE == AMx8x5ModeCalibrateXT)
                {
                    u8Timer = (iPeriod * 4096);
                    u8Timer /= 1000000;
                    u8Timer = u8Timer - 1;
                }
                else
                {
                    u8Timer = (iPeriod * 128);
                    u8Timer /= 1000000;
                    u8Timer = u8Timer - 1;
                }
            }
            else if (ENUM_TO_UINT8(enRepeat) < 5)
            {
                u8TFS = 1;
                u8Timer = (iPeriod * 128);
                u8Timer /= 1000000;
                u8Timer = u8Timer - 1;
            }
            else if (iPeriod <= 256)
            {
                //
                // Use 1 Hz.
                //
                u8TFS = 2;
                u8Timer = iPeriod - 1;
            }
            else
            {
                //
                // Use 1/60 Hz.
                //
                u8TFS = 3;
                u8Timer = iPeriod / 60;
                u8Timer = u8Timer - 1;
            }
        }
    }

    //
    // Get TCTRL, keep RPT, clear TE.
    //
    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_TIMER_CTRL,&u8TCTRL);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    u8TCTRL = u8TCTRL & 0x1C;
    
    res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_TIMER_CTRL,u8TCTRL);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    //
    // Merge the fields.
    //
    u8TCTRL = u8TCTRL | (u8TE * 0x80) | (u8TM * 0x40) | (u8TRPT * 0x20) | u8TFS;

    //
    // Generate nTIRQ interrupt on FOUT/nIRQ (asserted low).
    //
    if (enPin == AMx8x5CountdownInterruptPinFOUTnIRQLownTIRQLow)
    {
        //
        // Clear OUT1S.
        //
        res = Amx8x5_SetOut1Mode(pstcHandle,AMx8x5Out1nIRQAtIrqElseOut);
        if (res != Ok)
        {
            return AMX8X5_FUNC_END(res);
        }
    }

    //
    // Generate nTIRQ interrupt on PSW/nIRQ2 (asserted low).
    //
    if (enPin == AMx8x5CountdownInterruptPinPSWnIRQ2LownTIRQLow)
    {
        //
        // Get OUT2S.
        //
        res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_CONTROL_2,&u8Temp);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }

        //
        // If OUT2S != 0, set OUT2S to 5.
        //
        if ((u8Temp & 0x1C) != 0)
        {
            u8Temp = (u8Temp & 0xE3) | 0x14;
        }

        //
        // Write back.
        //
        res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_CONTROL_2,u8Temp);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }

    //
    // Generate TIRQ interrupt on CLKOUT/nIRQ3 (asserted low).
    //
    if (enPin == AMx8x5CountdownInterruptPinCLKOUTnIRQ3LownTIRQLow)
    {
        //
        // Setup SQFS field and enable SQWE.
        //
        res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_SQW,0x9B);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }

    //
    // Generate TIRQ interrupt on CLKOUT/nIRQ3 (asserted high).
    //
    if (enPin == AMx8x5CountdownInterruptPinCLKOUTnIRQ3HighnTIRQLow)
    {
        //
        // Setup SQFS field and enable SQWE.
        //
        res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_SQW,0x9A);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }

    if (enPin != AMx8x5CountdownInterruptPinDisable)
    {
        //
        // Clear TIM.
        // Set TIE.
        // Initialize the timer.
        // Initialize the timer repeat.
        // Start the timer.
        //
        res = Amx8x5_ClearRegister(pstcHandle,AMX8X5_REG_STATUS,AMX8X5_REG_STATUS_TIM_MSK);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        res = Amx8x5_SetRegister(pstcHandle,AMX8X5_REG_INT_MASK,AMX8X5_REG_INT_MASK_TIE_MSK);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_TIMER,u8Timer);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_TIMER_INITIAL,u8Timer);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
        res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_TIMER_CTRL,u8TCTRL);
        if (res != Ok) 
        {
            return AMX8X5_FUNC_END(res);
        }
    }
    return AMX8X5_FUNC_END(Ok);
}

/**
 ******************************************************************************
 ** \brief  Gets the extension address for the AMx8x5.
 **
 ** see also #AMX8X5_REG_EXTENDED_ADDR definition
 **
 ** \param  pstcHandle          RTC Handle
 **
 ** \param  u8Address           Mask of outputs to enable / disable as defined in #AMX8X5_REG_OCTRL
 **
 ** \param  pu8ExtensionAddress returned pointer to externsion address
 **
 ** \return                     Ok on success, else the Error as en_result_t
 **
 ******************************************************************************/
en_result_t Amx8x5_GetExtensionAddress(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Address, uint8_t* pu8ExtensionAddress)
{
    uint8_t u8Xadd, u8Temp;
    en_result_t res;

    AMX8X5_DEBUG_FUNC_START("Amx8x5_GetExtensionAddress");
    
    if (pstcHandle == NULL)
    {
        return AMX8X5_FUNC_END(ErrorUninitialized);
    }
    if (pu8ExtensionAddress == NULL)
    {
        return AMX8X5_FUNC_END(ErrorInvalidParameter);
    }
    
    
    res = Amx8x5_ReadByte(pstcHandle,AMX8X5_REG_EXTENDED_ADDR,&u8Temp);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    u8Temp = u8Temp & 0xC0;

    if (u8Address < 64)
    {
        u8Xadd = 0x8;
    }
    else if (u8Address < 128)
    {
        u8Xadd = 0x9;
    }
    else if (u8Address < 192)
    {
        u8Xadd = 0xA;
    }
    else
    {
        u8Xadd = 0xB;
    }

    //
    // Return the address.
    //
    *pu8ExtensionAddress = (u8Xadd | u8Temp);
    return AMX8X5_FUNC_END(Ok);
}


/**
 ******************************************************************************
 ** \brief  Read a byte from the local AMX8X5 RAM.
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  u8Address      RTC RAM address.
 **
 ** \param  pu8Data        Data Byte returned
 **
 ** \return Ok on success, else the Error as en_result_t
 **
 ******************************************************************************/
en_result_t Amx8x5_RamRead(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Address, uint8_t* pu8Data)
{
    uint8_t u8Xadd;
    en_result_t res;

    AMX8X5_DEBUG_FUNC_START("Amx8x5_RamRead");
    
    //
    // Calc XADDR value from address.
    //
    res = Amx8x5_GetExtensionAddress(pstcHandle,u8Address,&u8Xadd);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    //
    // Load the XADDR register.
    //
    res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_EXTENDED_ADDR,u8Xadd);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }

    //
    // Read and return the data.
    //
    res = Amx8x5_ReadByte(pstcHandle,(u8Address & 0x3F) | 0x40, pu8Data);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    return AMX8X5_FUNC_END(Ok);
}

/**
 ******************************************************************************
 ** \brief  Write a byte to the local AMX8X5 RAM.
 **
 ** \param  pstcHandle     RTC Handle
 **
 ** \param  u8Address      RTC RAM address.
 **
 ** \param  u8Data         Data Byte
 **
 ** \return Ok on success, else the Error as en_result_t
 **
 ******************************************************************************/
en_result_t Amx8x5_RamWrite(stc_amx8x5_handle_t* pstcHandle, uint8_t u8Address, uint8_t u8Data)
{
    uint8_t u8Xadd;
    en_result_t res;

    AMX8X5_DEBUG_FUNC_START("Amx8x5_RamWrite");
    
    //
    // Calc XADDR value from address.
    //
    res = Amx8x5_GetExtensionAddress(pstcHandle,u8Address,&u8Xadd);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    //
    // Load the XADDR register.
    //
    res = Amx8x5_WriteByte(pstcHandle,AMX8X5_REG_EXTENDED_ADDR,u8Xadd);
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }

    //
    // Read and return the data.
    //
    res = Amx8x5_WriteByte(pstcHandle,(u8Address & 0x3F) | 0x40, u8Data); 
    if (res != Ok) 
    {
        return AMX8X5_FUNC_END(res);
    }
    
    return AMX8X5_FUNC_END(Ok);
}


#ifdef __cplusplus
    #if defined(ARDUINO)
    #include <Arduino.h>
    #include <Wire.h>
    static int i2cWrite(void* pHandle, uint32_t u32Address, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
    {
        for(int i = 0; i < u32Len;i++)
        {
            Wire.beginTransmission(u32Address);  	// Initialize the Tx buffer
            Wire.write(u8Register+i);    		// Put slave register address in Tx buffer
            Wire.write(*pu8Data);					// Put data in Tx buffer
            pu8Data++;
            Wire.endTransmission();				// Send the Tx buffer
        }
        
        return Ok;
    }

    static int i2cRead(void* pHandle, uint32_t u32Address, uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
    {
        for(int i = 0; i < u32Len;i++)
        {
            Wire.beginTransmission(u32Address);  	// Initialize the Tx buffer
            Wire.write(u8Register+i);    		// Put slave register address in Tx buffer
            Wire.endTransmission();				// Send the Tx buffer
            Wire.requestFrom(u32Address,1);
            *pu8Data = Wire.read();					// Get data in Rx buffer
            pu8Data++;
        }
       
        return Ok;
    }
    #endif

    bool AMx8x5::begin(void)
    {
        #if defined(ARDUINO)
        stcRtcConfig.pfnReadI2C = i2cRead;
        stcRtcConfig.pfnWriteI2C = i2cWrite;
        #endif
        if (Amx8x5_Init(&stcRtcConfig) == Ok)
        {
            return true;
        }
        return false;
    }

    en_result_t AMx8x5::init(AMx8x5::stcHandle* pstcHandle)
    {
        memcpy(&stcRtcConfig,pstcHandle,sizeof(stcRtcConfig));
        return Ok;
    }

    void AMx8x5::update(void)
    {

    }

    bool AMx8x5::end(void)
    {
        return true;
    }

    AMx8x5::enResult  AMx8x5::reset(void){
        return Amx8x5_Reset(&stcRtcConfig);
    }

    AMx8x5::enResult  AMx8x5::getTime(AMx8x5::stcTime** ppstcTime)
    {
        return Amx8x5_GetTime(&stcRtcConfig,ppstcTime);
    }

    int16_t AMx8x5::getHundredth(void)
    {
        return Amx8x5_GetHundredth(&stcRtcConfig);
    }

    int16_t AMx8x5::getSecond(void)
    {
        return Amx8x5_GetSecond(&stcRtcConfig);
    }

    int16_t AMx8x5::getMinute(void)
    {
        return Amx8x5_GetMinute(&stcRtcConfig);
    }

    int16_t AMx8x5::getHour(void)
    {
        return Amx8x5_GetHour(&stcRtcConfig);
    }

    int16_t AMx8x5::getDay(void)
    {
        return Amx8x5_GetDay(&stcRtcConfig);
    }

    int16_t AMx8x5::getWeekday(void)
    {
        return Amx8x5_GetWeekday(&stcRtcConfig);
    }

    int16_t AMx8x5::getMonth(void)
    {
        return Amx8x5_GetMonth(&stcRtcConfig);
    }

    int16_t AMx8x5::getYear(void)
    {
        return Amx8x5_GetYear(&stcRtcConfig);
    }



    AMx8x5::enResult  AMx8x5::setTime(AMx8x5::stcTime* pstcTime, bool bProtect)
    {
        return Amx8x5_SetTime(&stcRtcConfig,pstcTime,bProtect);
    }

    AMx8x5::enResult  AMx8x5::setCalibrationValue(AMx8x5::enCalibrationMode enMode, int32_t iAdjust)
    {
        return Amx8x5_SetCalibrationValue(&stcRtcConfig,enMode,iAdjust);
    }

    AMx8x5::enResult  AMx8x5::setAlarm(AMx8x5::stcTime* pstcTime, AMx8x5::enAlarmRepeat enModeRepeat, AMx8x5::enInterruptMode enModeIrq, AMx8x5::enInterruptPin enModePin)
    {
        return Amx8x5_SetAlarm(&stcRtcConfig,pstcTime,enModeRepeat,enModeIrq,enModePin);
    }

    AMx8x5::enResult  AMx8x5::stop(bool bStop)
    {
        return Amx8x5_Stop(&stcRtcConfig,bStop);
    }

    AMx8x5::enResult  AMx8x5::ctrlOutB(bool bOnOff)
    {
        return Amx8x5_CtrlOutB(&stcRtcConfig,bOnOff);
    }
    AMx8x5::enResult  AMx8x5::ctrlOut(bool bOnOff)
    {
        return Amx8x5_CtrlOut(&stcRtcConfig,bOnOff);
    }
    AMx8x5::enResult  AMx8x5::setResetPolarity(bool bAssertHigh)
    {
        return Amx8x5_SetResetPolarity(&stcRtcConfig,bAssertHigh);
    }
    AMx8x5::enResult  AMx8x5::autoResetStatus(bool bEnabled)
    {
        return Amx8x5_AutoResetStatus(&stcRtcConfig,bEnabled);
    }
    AMx8x5::enResult  AMx8x5::setPswHighCurrent(bool bEnabled)
    {
        return Amx8x5_SetPswHighCurrent(&stcRtcConfig,bEnabled);
    }
    AMx8x5::enResult  AMx8x5::usenExtrAsReset(bool bEnabled)
    {
        return Amx8x5_UsenExtrAsReset(&stcRtcConfig,bEnabled);
    }
    AMx8x5::enResult  AMx8x5::setOut1Mode(AMx8x5::enOut1Mode enMode)
    {
        return Amx8x5_SetOut1Mode(&stcRtcConfig,enMode);
    }
    AMx8x5::enResult  AMx8x5::setOut2Mode(AMx8x5::enOut2Mode enMode)
    {
        return Amx8x5_SetOut2Mode(&stcRtcConfig,enMode);
    }
    AMx8x5::enResult  AMx8x5::enableIrqXt1OnExti(bool bEnabled)
    {
        return Amx8x5_EnableIrqXt1OnExti(&stcRtcConfig,bEnabled);
    }
    AMx8x5::enResult  AMx8x5::enableIrqXt2OnWdi(bool bEnabled)
    {
        return Amx8x5_EnableIrqXt2OnWdi(&stcRtcConfig,bEnabled);
    }
    AMx8x5::enResult  AMx8x5::enableIrqAlarm(bool bEnabled)
    {
        return Amx8x5_EnableIrqAlarm(&stcRtcConfig,bEnabled);
    }
    AMx8x5::enResult  AMx8x5::enableIrqTimer(bool bEnabled)
    {
        return Amx8x5_EnableIrqTimer(&stcRtcConfig,bEnabled);
    }
    AMx8x5::enResult  AMx8x5::enableIrqBatteryLow(bool bEnabled)
    {
        return Amx8x5_EnableIrqBatteryLow(&stcRtcConfig,bEnabled);
    }
    AMx8x5::enResult  AMx8x5::enableTrickleCharger(AMx8x5::enTrickleDiode enDiode, AMx8x5::enTrickleResistor enResistor, bool bEnabled)
    {
        return Amx8x5_EnableTrickleCharger(&stcRtcConfig,enDiode,enResistor,bEnabled);
    }
    AMx8x5::enResult  AMx8x5::setBatteryReferenceVoltage(AMx8x5::enBatReference enBref)
    {
        return Amx8x5_SetBatteryReferenceVoltage(&stcRtcConfig,enBref);
    }

    AMx8x5::enResult  AMx8x5::setWatchdog(uint32_t u32Period, AMx8x5::enWatchdogInterruptPin enPin)
    {
        return Amx8x5_SetWatchdog(&stcRtcConfig,u32Period,enPin);
    }
    AMx8x5::enResult  AMx8x5::setSleepMode(uint8_t ui8Timeout, AMx8x5::enSleepMode enMode)
    {
        return Amx8x5_SetSleepMode(&stcRtcConfig,ui8Timeout,enMode);
    }
    AMx8x5::enResult  AMx8x5::getExtensionAddress(uint8_t u8Address, uint8_t* pu8ExtensionAddress)
    {
        return Amx8x5_GetExtensionAddress(&stcRtcConfig,u8Address,pu8ExtensionAddress);
    }
    AMx8x5::enResult  AMx8x5::setSquareWaveOutput(uint8_t u8SQFS, uint8_t u8PinMsk)
    {
        return Amx8x5_SetSquareWaveOutput(&stcRtcConfig,u8SQFS,u8PinMsk);
    }

    AMx8x5::enResult  AMx8x5::selectOscillatorMode(AMx8x5::enOscSelect enSelect)
    {
        return Amx8x5_SelectOscillatorMode(&stcRtcConfig,enSelect);
    }

    AMx8x5::enResult  AMx8x5::setCountdown(AMx8x5::enPeriodeRange enRange, int32_t iPeriod, AMx8x5::enCountdownInterruptOutput enRepeat, AMx8x5::enCountdownInterruptPin enPin)
    {
        return Amx8x5_SetCountdown(&stcRtcConfig,enRange,iPeriod,enRepeat,enPin);
    }

    /// @brief Set autocalibration
    /// @param enPeriod period of autocalibration
    /// @return Result
    AMx8x5::enResult  AMx8x5::setAutocalibration(AMx8x5::enAutocalibrationPeriod enPeriod)
    {
        return Amx8x5_SetAutocalibration(&stcRtcConfig,enPeriod);
    }

    /// @brief Read data from RTC-RAM
    /// @param u8Address Address in RAM
    /// @param u8Data Pointert to data byte
    /// @return Result
    AMx8x5::enResult  AMx8x5::ramRead(uint8_t u8Address, uint8_t* pu8Data)
    {
        return Amx8x5_RamRead(&stcRtcConfig,u8Address, pu8Data);
    }

    /// @brief Write data to RTC-RAM
    /// @param u8Address Address in RAM
    /// @param u8Data Data byte
    /// @return Result
    AMx8x5::enResult  AMx8x5::ramWrite(uint8_t u8Address, uint8_t u8Data)
    {
        return Amx8x5_RamWrite(&stcRtcConfig,u8Address,u8Data);
    }

    /// @brief Clear Register
    /// @param u8Address Register address
    /// @param u8Mask Bitmask
    /// @return Result
    AMx8x5::enResult  AMx8x5::clearRegister(uint8_t u8Address, uint8_t u8Mask)
    {
        return Amx8x5_ClearRegister(&stcRtcConfig,u8Address,u8Mask);
    }

    /// @brief Set Register
    /// @param u8Address Register address
    /// @param u8Mask Bitmask
    /// @return Result
    AMx8x5::enResult  AMx8x5::setRegister(uint8_t u8Address, uint8_t u8Mask)
    {
        return Amx8x5_SetRegister(&stcRtcConfig,u8Address,u8Mask);
    }

    /// @brief Read byte from RTC
    /// @param u8Register Register
    /// @param pu8Value Pointer to data
    /// @return Result
    AMx8x5::enResult  AMx8x5::readByte(uint8_t u8Register, uint8_t* pu8Value)
    {
       return Amx8x5_ReadByte(&stcRtcConfig,u8Register,pu8Value);
    }

    /// @brief Read bytes from RTC
    /// @param u8Register Register
    /// @param pu8Data Pointer to data
    /// @param u32Length  Length of data
    /// @return Result
    AMx8x5::enResult  AMx8x5::readBytes(uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length)
    {
        return Amx8x5_ReadBytes(&stcRtcConfig,u8Register,pu8Data,u32Length);
    }

    /// @brief Write byte to RTC
    /// @param u8Register Register
    /// @param u8Value Data byte
    /// @return Result
    AMx8x5::enResult  AMx8x5::writeByte(uint8_t u8Register, uint8_t u8Value)
    {
        return Amx8x5_WriteByte(&stcRtcConfig,u8Register, u8Value);
    }

    /// @brief Write bytes to RTC
    /// @param u8Register Register
    /// @param pu8Data Pointer to data
    /// @param u32Length  Length of data
    /// @return Result
    AMx8x5::enResult  AMx8x5::writeBytes(uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Length)
    {
        return Amx8x5_WriteBytes(&stcRtcConfig,u8Register, pu8Data,u32Length);
    }

#endif
/******************************************************************************/
/* EOF (not truncated)                                                        */
/******************************************************************************/

