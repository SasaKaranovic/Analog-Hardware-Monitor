#include <assert.h>
#include "debug.h"
#include "mcp4728.h"

// Macros and convenience
#if 1
    #define CHECK_FN_POINTER(x) do {                \
                            if((void*)x==NULL)     \
                            {   \
                                debug("%s:%d: Invalid FN pointer!", __FUNCTION__, __LINE__);   \
                                return;       \
                            }   \
                        } while(0);
#else
    #define CHECK_FN_POINTER(x) (assert((void*)x==NULL))
#endif

#define HIGHBYTE(x)     ((x>>8)&0xFF)
#define LOWBYTE(x)      (x&0xFF)

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


// I2C Address
// #define DEFAULT_I2C_ADDR 0x60
#define DEFAULT_I2C_ADDR 0xC0

// Enum and struct
typedef enum CMD
{
    FAST_WRITE = 0x00,
    MULTI_WRITE = 0x40,
    SINGLE_WRITE = 0x58,
    SEQ_WRITE = 0x50,
    SELECT_VREF = 0x80,
    SELECT_GAIN = 0xC0,
    SELECT_PWRDOWN = 0xA0
} CMD;

typedef struct mcp4728_Driver
{
    gpioLdac_fn_t   fnLdac;
    i2cSend_fn_t     fnSend;
    i2cReceive_fn_t fnReceive;
} mcp4728_Driver;

typedef struct DACInputData
{
    VREF vref;
    PWR_DOWN pd;
    GAIN gain;
    uint16_t data;
} DACInputData;

// Global variables
static mcp4728_Driver gCntxDriver = {0};
static uint8_t gI2C_ADDR = DEFAULT_I2C_ADDR;
DACInputData reg_[4];
DACInputData eep_[4];
DACInputData read_reg_[4];
DACInputData read_eep_[4];

// Private functions
static uint8_t mcp4728_fastWrite(void);
// static uint8_t mcp4728_multiWrite(void);
static uint8_t mcp4728_seqWrite(void);
static uint8_t mcp4728_singleWrite(uint8_t ch);


void mcp4728_begin(void)
{
    mcp4728_enable(false);
    mcp4728_readRegisters();
}

void mcp4728_initLib(gpioLdac_fn_t fnLdac, i2cSend_fn_t fnSend, i2cReceive_fn_t fnReceive)
{
    CHECK_FN_POINTER(fnLdac);
    CHECK_FN_POINTER(fnSend);
    CHECK_FN_POINTER(fnReceive);

    gCntxDriver.fnLdac      = fnLdac;
    gCntxDriver.fnSend      = fnSend;
    gCntxDriver.fnReceive   = fnReceive;

}

void mcp4728_enable(bool b)
{
    gCntxDriver.fnLdac(!b);
}


static uint8_t mcp4728_fastWrite(void)
{
    #if 0
    debug("Ch0 -> %d", reg_[0].data);
    debug("Ch1 -> %d", reg_[1].data);
    debug("Ch2 -> %d", reg_[2].data);
    debug("Ch3 -> %d", reg_[3].data);
    #endif
    
    uint8_t txBuffer[8]={0};

    for (uint8_t i = 0; i < 4; ++i)
    {
        txBuffer[0+(i*2)] = (uint8_t)FAST_WRITE | HIGHBYTE(reg_[i].data);
        txBuffer[1+(i*2)] = LOWBYTE(reg_[i].data);
    }

    return gCntxDriver.fnSend(gI2C_ADDR, txBuffer, 8);

    // wire_->beginTransmission(gI2C_ADDR);
    // for (uint8_t i = 0; i < 4; ++i)
    // {
    //     wire_->write((uint8_t)FAST_WRITE | HIGHBYTE(reg_[i].data));
    //     wire_->write(LOWBYTE(reg_[i].data));
    // }
    // return wire_->endTransmission();
}

static uint8_t mcp4728_seqWrite(void)
{
    uint8_t txBuffer[9]={0};

    txBuffer[0] = (uint8_t)SEQ_WRITE;
    for(uint8_t i=0; i<4; i++)
    {
        txBuffer[1+(i*2)] = ((uint8_t)eep_[i].vref << 7) | ((uint8_t)eep_[i].pd << 5) | ((uint8_t)eep_[i].gain << 4) | HIGHBYTE(eep_[i].data);
        txBuffer[2+(i*2)] = (LOWBYTE(eep_[i].data));
    }

    return gCntxDriver.fnSend(gI2C_ADDR, txBuffer, 9);

    // wire_->beginTransmission(gI2C_ADDR);
    // wire_->write((uint8_t)SEQ_WRITE);
    // for (uint8_t i = 0; i < 4; ++i)
    // {
    //     wire_->write(((uint8_t)eep_[i].vref << 7) | ((uint8_t)eep_[i].pd << 5) | ((uint8_t)eep_[i].gain << 4) | HIGHBYTE(eep_[i].data));
    //     wire_->write(LOWBYTE(eep_[i].data));
    // }
    // return wire_->endTransmission();
}

static uint8_t mcp4728_singleWrite(uint8_t ch)
{
    uint8_t txBuffer[3]={0};
    txBuffer[0] = (uint8_t)SINGLE_WRITE | (ch << 1);
    txBuffer[1] = ((uint8_t)eep_[ch].vref << 7) | ((uint8_t)eep_[ch].pd << 5) | ((uint8_t)eep_[ch].gain << 4) | HIGHBYTE(eep_[ch].data);
    txBuffer[2] = LOWBYTE(eep_[ch].data);

    return gCntxDriver.fnSend(gI2C_ADDR, txBuffer, 3);

    // wire_->write((uint8_t)SINGLE_WRITE | (ch << 1));
    // wire_->write(((uint8_t)eep_[ch].vref << 7) | ((uint8_t)eep_[ch].pd << 5) | ((uint8_t)eep_[ch].gain << 4) | HIGHBYTE(eep_[ch].data));
    // wire_->write(LOWBYTE(eep_[ch].data));
    // return wire_->endTransmission();
}

void mcp4728_SetPercent(uint8_t nChannel, uint8_t nPercent)
{
    nPercent = (nPercent > 100) ? 100 : nPercent;

    uint16_t value = (uint16_t)((nPercent*4095)/100);
    mcp4728_analogWrite(nChannel, value, false);
}

uint8_t mcp4728_analogWriteCh(DAC_CH ch, uint16_t data, bool b_eep)
{
    return mcp4728_analogWrite((uint8_t)ch, data, b_eep);
}

uint8_t mcp4728_analogWrite(uint8_t ch, uint16_t data, bool b_eep)
{
    if (b_eep)
    {
        eep_[ch].data = data;
        return mcp4728_singleWrite(ch);
    }
    else
    {
        reg_[ch].data = data;
        return mcp4728_fastWrite();
    }
}

uint8_t mcp4728_analogWriteAll(uint16_t a, uint16_t b, uint16_t c, uint16_t d, bool b_eep)
{
    if (b_eep)
    {
        reg_[0].data = eep_[0].data = a;
        reg_[1].data = eep_[1].data = b;
        reg_[2].data = eep_[2].data = c;
        reg_[3].data = eep_[3].data = d;
        return mcp4728_seqWrite();
    }
    else
    {
        reg_[0].data = a;
        reg_[1].data = b;
        reg_[2].data = c;
        reg_[3].data = d;
        return mcp4728_fastWrite();
    }
}

uint8_t mcp4728_selectVref(VREF a, VREF b, VREF c, VREF d)
{
    reg_[0].vref = a;
    reg_[1].vref = b;
    reg_[2].vref = c;
    reg_[3].vref = d;

    uint8_t data = (uint8_t)SELECT_VREF;
    for (uint8_t i = 0; i < 4; ++i)
    {
        bitWrite(data, 3 - i, (uint8_t)reg_[i].vref);
    }

    return gCntxDriver.fnSend(gI2C_ADDR, &data, 1);

    // wire_->beginTransmission(gI2C_ADDR);
    // wire_->write(data);
    // return wire_->endTransmission();
}

uint8_t mcp4728_selectPowerDown(PWR_DOWN a, PWR_DOWN b, PWR_DOWN c, PWR_DOWN d)
{
    reg_[0].pd = a;
    reg_[1].pd = b;
    reg_[2].pd = c;
    reg_[3].pd = d;

    uint8_t h = ((uint8_t)SELECT_PWRDOWN) | ((uint8_t)a << 2) | (uint8_t)b;
    uint8_t l = 0 | ((uint8_t)c << 6) | ((uint8_t)d << 4);

    uint8_t txBuffer[2]={0};
    txBuffer[0]=h;
    txBuffer[0]=l;

    return gCntxDriver.fnSend(gI2C_ADDR, txBuffer, 2);

    // wire_->beginTransmission(gI2C_ADDR);
    // wire_->write(h);
    // wire_->write(l);
    // return wire_->endTransmission();
}

uint8_t mcp4728_selectGain(GAIN a, GAIN b, GAIN c, GAIN d)
{
    reg_[0].gain = a;
    reg_[1].gain = b;
    reg_[2].gain = c;
    reg_[3].gain = d;

    uint8_t data = (uint8_t)SELECT_GAIN;
    for (uint8_t i = 0; i < 4; ++i)
    {
        bitWrite(data, 3 - i, (uint8_t)reg_[i].gain);
    }

    return gCntxDriver.fnSend(gI2C_ADDR, &data, 1);

    // wire_->beginTransmission(gI2C_ADDR);
    // wire_->write(data);
    // return wire_->endTransmission();
}

void mcp4728_setID(uint8_t id) 
{ 
    gI2C_ADDR = DEFAULT_I2C_ADDR + id;
}


void mcp4728_readRegisters(void)
{
    // wire_->requestFrom((int)gI2C_ADDR, 24);
    // if (wire_->available() == 24)
    // {
    //     for (uint8_t i = 0; i < 8; ++i)
    //     {
    //         uint8_t data[3];
    //         bool isEeprom = i % 2;
    //         for (uint8_t i = 0; i < 3; ++i) data[i] = wire_->read();

    //         uint8_t ch = (data[0] & 0x30) >> 4;
    //         if (isEeprom)
    //         {
    //             read_eep_[ch].vref = (VREF)    ((data[1] & 0b10000000) >> 7);
    //             read_eep_[ch].pd   = (PWR_DOWN)((data[1] & 0b01100000) >> 5);
    //             read_eep_[ch].gain = (GAIN)    ((data[1] & 0b00010000) >> 4);
    //             read_eep_[ch].data = (uint16_t)((data[1] & 0b00001111) << 8 | data[2]);
    //         }
    //         else
    //         {
    //             read_reg_[ch].vref = (VREF)    ((data[1] & 0b10000000) >> 7);
    //             read_reg_[ch].pd   = (PWR_DOWN)((data[1] & 0b01100000) >> 5);
    //             read_reg_[ch].gain = (GAIN)    ((data[1] & 0b00010000) >> 4);
    //             read_reg_[ch].data = (uint16_t)((data[1] & 0b00001111) << 8 | data[2]);
    //         }
    //     }
    // }
}

uint8_t mcp4728_getVref(uint8_t ch, bool b_eep)
{
    return b_eep ? (uint8_t)read_eep_[ch].vref : (uint8_t)read_reg_[ch].vref;
}

uint8_t mcp4728_getGain(uint8_t ch, bool b_eep)
{
    return b_eep ? (uint8_t)read_eep_[ch].gain: (uint8_t)read_reg_[ch].gain;
}

uint8_t mcp4728_getPowerDown(uint8_t ch, bool b_eep) 
{
    return b_eep ? (uint8_t)read_eep_[ch].pd : (uint8_t)read_reg_[ch].pd;
}

uint16_t mcp4728_getDACData(uint8_t ch, bool b_eep)
{
    return b_eep ? (uint16_t)read_eep_[ch].data : (uint16_t)read_reg_[ch].data;
}

