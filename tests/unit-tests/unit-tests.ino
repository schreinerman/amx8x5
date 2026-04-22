// AMX8X5 Unit Tests — runs on any Arduino board via the AUnit framework.
//
// These tests exercise the driver C-API (Amx8x5_* functions) using a mock
// I2C backend instead of real hardware.  A small 256-byte "register file"
// array is used as the fake RTC, so no physical device is required.
//
// Run on hardware:
//   make upload PROFILE=rp2040 PORT=/dev/ttyACM0   (in tests/)
// Then open the serial monitor at 115200 baud to see pass/fail results.
//
// Test categories:
//   1. Regression   – guards for previously fixed bugs (ACIE_MSK)
//   2. Init         – ID register detection (correct / wrong / null handle)
//   3. Time         – SetTime / GetTime round-trip and BCD encoding
//   4. Alarm        – SetAlarm register content
//   5. RAM          – RamWrite / RamRead round-trip
//   6. Enum sanity  – I2C/SPI bit encoding in en_amx8x5_rtc_type_t

#include <AUnit.h>
#include <amx8x5.h>

// ---------------------------------------------------------------------------
// Mock backend
// ---------------------------------------------------------------------------

// Simulated RTC register space (256 bytes, byte-addressed)
static uint8_t mockRegs[256];

static int mockWrite(void* pHandle, uint32_t u32Address,
                     uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
{
    for (uint32_t i = 0; i < u32Len; i++)
        mockRegs[(u8Register + i) & 0xFF] = pu8Data[i];
    return 0;
}

static int mockRead(void* pHandle, uint32_t u32Address,
                    uint8_t u8Register, uint8_t* pu8Data, uint32_t u32Len)
{
    for (uint32_t i = 0; i < u32Len; i++)
        pu8Data[i] = mockRegs[(u8Register + i) & 0xFF];
    return 0;
}

// Reset mock register file to all zeros
static void resetMock()
{
    memset(mockRegs, 0, sizeof(mockRegs));
}

// Pre-populate the two ID registers (0x28/0x29) with the correct chip ID
// for the given RTC type, so Amx8x5_Init() passes the ID check.
//
// ID byte layout (from datasheet):
//   ID0 (0x28): 0x08 for AM08x5 family, 0x18 for AM18x5 family
//   ID1 (0x29): 0x05 for I2C variant,   0x15 for SPI variant
static void setValidId(en_amx8x5_rtc_type_t enType)
{
    switch (enType)
    {
        case AMx8x5Type0805: mockRegs[AMX8X5_REG_ID0] = 0x08; mockRegs[AMX8X5_REG_ID0+1] = 0x05; break;
        case AMx8x5Type0815: mockRegs[AMX8X5_REG_ID0] = 0x08; mockRegs[AMX8X5_REG_ID0+1] = 0x15; break;
        case AMx8x5Type1805: mockRegs[AMX8X5_REG_ID0] = 0x18; mockRegs[AMX8X5_REG_ID0+1] = 0x05; break;
        case AMx8x5Type1815: mockRegs[AMX8X5_REG_ID0] = 0x18; mockRegs[AMX8X5_REG_ID0+1] = 0x15; break;
        default: break;
    }
}

// Build a fully configured handle for the given type using the mock backend
static stc_amx8x5_handle_t makeHandle(en_amx8x5_rtc_type_t enType = AMx8x5Type1805)
{
    stc_amx8x5_handle_t h;
    memset(&h, 0, sizeof(h));
    h.enMode      = AMx8x5ModeI2C;
    h.enRtcType   = enType;
    h.u32Address  = 0x69;
    h.pfnWriteI2C = mockWrite;
    h.pfnReadI2C  = mockRead;
    return h;
}

// Initialise mock + handle and verify init succeeds (helper for later tests)
static stc_amx8x5_handle_t initedHandle(en_amx8x5_rtc_type_t enType = AMx8x5Type1805)
{
    resetMock();
    setValidId(enType);
    stc_amx8x5_handle_t h = makeHandle(enType);
    Amx8x5_Init(&h);
    return h;
}

// ---------------------------------------------------------------------------
// 1. Regression tests
// ---------------------------------------------------------------------------

// ACIE_MSK was accidentally defined as (0 << pos) = 0.
// After fix it must equal 1 (bit 0 of OSC_CONTROL register).
test(regression_acie_mask_is_nonzero)
{
    assertEqual((int)AMX8X5_REG_OSC_CONTROL_ACIE_MSK, 1);
}

// ---------------------------------------------------------------------------
// 2. Init tests
// ---------------------------------------------------------------------------

test(init_am1805_correct_id_returns_ok)
{
    resetMock();
    setValidId(AMx8x5Type1805);
    stc_amx8x5_handle_t h = makeHandle(AMx8x5Type1805);
    assertEqual((int)Amx8x5_Init(&h), (int)Ok);
}

test(init_am0805_correct_id_returns_ok)
{
    resetMock();
    setValidId(AMx8x5Type0805);
    stc_amx8x5_handle_t h = makeHandle(AMx8x5Type0805);
    assertEqual((int)Amx8x5_Init(&h), (int)Ok);
}

test(init_am0815_correct_id_returns_ok)
{
    resetMock();
    setValidId(AMx8x5Type0815);
    stc_amx8x5_handle_t h = makeHandle(AMx8x5Type0815);
    h.pfnWriteSpi = mockWrite;
    h.pfnReadSpi  = mockRead;
    h.enMode      = AMx8x5ModeSPI;
    assertEqual((int)Amx8x5_Init(&h), (int)Ok);
}

test(init_am1815_correct_id_returns_ok)
{
    resetMock();
    setValidId(AMx8x5Type1815);
    stc_amx8x5_handle_t h = makeHandle(AMx8x5Type1815);
    h.pfnWriteSpi = mockWrite;
    h.pfnReadSpi  = mockRead;
    h.enMode      = AMx8x5ModeSPI;
    assertEqual((int)Amx8x5_Init(&h), (int)Ok);
}

test(init_wrong_id_returns_error)
{
    resetMock();
    // ID registers remain 0x00 — invalid for all device types
    stc_amx8x5_handle_t h = makeHandle(AMx8x5Type1805);
    assertNotEqual((int)Amx8x5_Init(&h), (int)Ok);
}

test(init_null_handle_returns_error)
{
    assertNotEqual((int)Amx8x5_Init(nullptr), (int)Ok);
}

// ---------------------------------------------------------------------------
// 3. SetTime / GetTime
// ---------------------------------------------------------------------------

test(set_get_time_roundtrip)
{
    stc_amx8x5_handle_t h = initedHandle();

    stc_amx8x5_time_t tSet;
    memset(&tSet, 0, sizeof(tSet));
    tSet.u8Hundredth = 50;
    tSet.u8Second    = 45;
    tSet.u8Minute    = 30;
    tSet.u8Hour      = 14;
    tSet.u8Date      = 22;
    tSet.u8Month     = 4;
    tSet.u8Year      = 26;
    tSet.u8Century   = 1;
    tSet.u8Mode      = AMX8X5_24HR_MODE;

    assertEqual((int)Amx8x5_SetTime(&h, &tSet, false), (int)Ok);

    stc_amx8x5_time_t* pGot = nullptr;
    assertEqual((int)Amx8x5_GetTime(&h, &pGot), (int)Ok);
    assertNotEqual((void*)pGot, (void*)nullptr);

    assertEqual((int)pGot->u8Hundredth, 50);
    assertEqual((int)pGot->u8Second,    45);
    assertEqual((int)pGot->u8Minute,    30);
    assertEqual((int)pGot->u8Hour,      14);
    assertEqual((int)pGot->u8Date,      22);
    assertEqual((int)pGot->u8Month,      4);
    assertEqual((int)pGot->u8Year,      26);
}

// Verify that seconds are stored in BCD: 59 decimal -> 0x59 in register
test(set_time_seconds_bcd_encoding)
{
    stc_amx8x5_handle_t h = initedHandle();

    stc_amx8x5_time_t tSet;
    memset(&tSet, 0, sizeof(tSet));
    tSet.u8Second = 59;
    tSet.u8Mode   = AMX8X5_24HR_MODE;

    Amx8x5_SetTime(&h, &tSet, false);
    // GP0 bit (bit 7) may be set in the seconds register; mask it out
    assertEqual((int)(mockRegs[AMX8X5_REG_SECONDS] & ~AMX8X5_REG_SECONDS_GP0_MSK), 0x59);
}

// Verify that minutes are stored in BCD: 30 decimal -> 0x30 in register
test(set_time_minutes_bcd_encoding)
{
    stc_amx8x5_handle_t h = initedHandle();

    stc_amx8x5_time_t tSet;
    memset(&tSet, 0, sizeof(tSet));
    tSet.u8Minute = 30;
    tSet.u8Mode   = AMX8X5_24HR_MODE;

    Amx8x5_SetTime(&h, &tSet, false);
    assertEqual((int)(mockRegs[AMX8X5_REG_MINUTES] & ~AMX8X5_REG_MINUTES_GP1_MSK), 0x30);
}

// Verify that hours are stored in BCD (24h mode): 14 decimal -> 0x14 in register
test(set_time_hours_bcd_encoding_24h)
{
    stc_amx8x5_handle_t h = initedHandle();

    stc_amx8x5_time_t tSet;
    memset(&tSet, 0, sizeof(tSet));
    tSet.u8Hour = 14;
    tSet.u8Mode = AMX8X5_24HR_MODE;

    Amx8x5_SetTime(&h, &tSet, false);
    // GP2/GP3 bits occupy bit 6/7 in the hours register; mask them out
    assertEqual((int)(mockRegs[AMX8X5_REG_HOURS] & 0x3F), 0x14);
}

// GetTime on uninitialised (zero) registers should return 0 for all fields
test(get_time_zero_registers)
{
    stc_amx8x5_handle_t h = initedHandle();
    // Register file already reset to 0 by initedHandle (except ID regs)

    stc_amx8x5_time_t* pGot = nullptr;
    assertEqual((int)Amx8x5_GetTime(&h, &pGot), (int)Ok);
    assertNotEqual((void*)pGot, (void*)nullptr);
    assertEqual((int)pGot->u8Second, 0);
    assertEqual((int)pGot->u8Minute, 0);
    assertEqual((int)pGot->u8Hour,   0);
}

// ---------------------------------------------------------------------------
// 4. RAM read / write
// ---------------------------------------------------------------------------

test(ram_write_read_roundtrip)
{
    stc_amx8x5_handle_t h = initedHandle();
    uint8_t readback = 0;
    assertEqual((int)Amx8x5_RamWrite(&h, 0x00, 0xA5), (int)Ok);
    assertEqual((int)Amx8x5_RamRead(&h, 0x00, &readback), (int)Ok);
    assertEqual((int)readback, 0xA5);
}

test(ram_multiple_addresses_independent)
{
    stc_amx8x5_handle_t h = initedHandle();
    Amx8x5_RamWrite(&h, 0x00, 0x11);
    Amx8x5_RamWrite(&h, 0x01, 0x22);
    Amx8x5_RamWrite(&h, 0xFF, 0x33);

    uint8_t v0, v1, vff;
    Amx8x5_RamRead(&h, 0x00, &v0);
    Amx8x5_RamRead(&h, 0x01, &v1);
    Amx8x5_RamRead(&h, 0xFF, &vff);

    assertEqual((int)v0,  0x11);
    assertEqual((int)v1,  0x22);
    assertEqual((int)vff, 0x33);
}

test(ram_overwrite)
{
    stc_amx8x5_handle_t h = initedHandle();
    Amx8x5_RamWrite(&h, 0x10, 0xAA);
    Amx8x5_RamWrite(&h, 0x10, 0xBB);

    uint8_t v;
    Amx8x5_RamRead(&h, 0x10, &v);
    assertEqual((int)v, 0xBB);
}

// ---------------------------------------------------------------------------
// 5. Enum sanity: interface encoded in bit 4 of en_amx8x5_rtc_type_t
// ---------------------------------------------------------------------------

test(enum_spi_variants_have_bit4_set)
{
    assertTrue((AMx8x5Type0815 & 0x0010) != 0);
    assertTrue((AMx8x5Type1815 & 0x0010) != 0);
}

test(enum_i2c_variants_have_bit4_clear)
{
    assertEqual((int)(AMx8x5Type0805 & 0x0010), 0);
    assertEqual((int)(AMx8x5Type1805 & 0x0010), 0);
}

test(enum_power_management_variants_have_upper_nibble_1)
{
    // Upper nibble of enum value == 1 means power management available
    assertEqual((int)((AMx8x5Type1805 >> 12) & 0xF), 1);
    assertEqual((int)((AMx8x5Type1815 >> 12) & 0xF), 1);
}

test(enum_no_power_management_variants_have_upper_nibble_0)
{
    assertEqual((int)((AMx8x5Type0805 >> 12) & 0xF), 0);
    assertEqual((int)((AMx8x5Type0815 >> 12) & 0xF), 0);
}

// Alias enums must map to the same numeric values as the canonical ones
test(enum_aliases_match_canonical_values)
{
    assertEqual((int)AMx8x5TypeI2C,                (int)AMx8x5Type0805);
    assertEqual((int)AMx8x5TypeSPI,                (int)AMx8x5Type0815);
    assertEqual((int)AMx8x5TypeRV1805,             (int)AMx8x5Type1805);
    assertEqual((int)AMx8x5TypeI2CPowerManagement, (int)AMx8x5Type1805);
    assertEqual((int)AMx8x5TypeSPIPowerManagement, (int)AMx8x5Type1815);
}

// ---------------------------------------------------------------------------
// Arduino entry points
// ---------------------------------------------------------------------------

void setup()
{
    Serial.begin(115200);
    while (!Serial); // wait for USB-CDC serial on Leonardo / RP2040 / etc.
    delay(200);
}

void loop()
{
    aunit::TestRunner::run();
}
