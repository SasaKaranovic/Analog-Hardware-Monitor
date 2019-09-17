// Copyright (c) Sandeep Mistry. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.


#include "debug.h"
#include "LoRa.h"

// Convenience
#define CHECK_FN_POINTER(x)     do {                \
                                    if((void*)x==NULL)     \
                                    {   \
                                        debug("%s:%d: Invalid FN pointer!", __FUNCTION__, __LINE__);   \
                                        return LORA_STATUS_INVALID_ARGUMENT;       \
                                    }   \
                                } while(0);

// registers
#define REG_FIFO                    0x00
#define REG_OP_MODE                 0x01
#define REG_FRF_MSB                 0x06
#define REG_FRF_MID                 0x07
#define REG_FRF_LSB                 0x08
#define REG_PA_CONFIG               0x09
#define REG_OCP                     0x0b
#define REG_LNA                     0x0c
#define REG_FIFO_ADDR_PTR           0x0d
#define REG_FIFO_TX_BASE_ADDR       0x0e
#define REG_FIFO_RX_BASE_ADDR       0x0f
#define REG_FIFO_RX_CURRENT_ADDR    0x10
#define REG_IRQ_FLAGS               0x12
#define REG_RX_NB_BYTES             0x13
#define REG_PKT_SNR_VALUE           0x19
#define REG_PKT_RSSI_VALUE          0x1a
#define REG_MODEM_CONFIG_1          0x1d
#define REG_MODEM_CONFIG_2          0x1e
#define REG_PREAMBLE_MSB            0x20
#define REG_PREAMBLE_LSB            0x21
#define REG_PAYLOAD_LENGTH          0x22
#define REG_MODEM_CONFIG_3          0x26
#define REG_FREQ_ERROR_MSB          0x28
#define REG_FREQ_ERROR_MID          0x29
#define REG_FREQ_ERROR_LSB          0x2a
#define REG_RSSI_WIDEBAND           0x2c
#define REG_DETECTION_OPTIMIZE      0x31
#define REG_INVERTIQ                0x33
#define REG_DETECTION_THRESHOLD     0x37
#define REG_SYNC_WORD               0x39
#define REG_INVERTIQ2               0x3b
#define REG_DIO_MAPPING_1           0x40
#define REG_VERSION                 0x42
#define REG_PA_DAC                  0x4d

// modes
#define MODE_LONG_RANGE_MODE        0x80
#define MODE_SLEEP                  0x00
#define MODE_STDBY                  0x01
#define MODE_TX                     0x03
#define MODE_RX_CONTINUOUS          0x05
#define MODE_RX_SINGLE              0x06

// PA config
#define PA_BOOST                    0x80

// IRQ masks
#define IRQ_TX_DONE_MASK            0x08
#define IRQ_PAYLOAD_CRC_ERROR_MASK  0x20
#define IRQ_RX_DONE_MASK            0x40

#define MAX_PKT_LENGTH              255


// Global
static LoRa_Driver_t gCntxDriver = {0};
static uint32_t gFrequency = 0;
static uint32_t gPacketIndex = 0;
static uint32_t gImplicitHeaderMode = 0;

// Function prototypes
static uint8_t readRegister(uint8_t address);
static void writeRegister(uint8_t address, uint8_t value);
static uint8_t singleTransfer(uint8_t address, uint8_t value);
static void idle(void);
static void sleep(void);
static bool isTransmitting(void);
static void LoRa_handleDio0Rise(void);



lora_status_t LoRa_InitLib(     LoRa_Driver_Init_fn_t   fnInit,
                                LoRa_Driver_CS_fn_t     fnChipSelect,
                                LoRa_Driver_RST_fn_t    fnReset,
                                LoRa_Transmit_fn_t      fnTransmit,
                                LoRa_Receive_fn_t       fnCRecieve,
                                LoRa_IntCallback_fn_t   fnCallback,
                                LoRa_Driver_Delay_fn_t  fnDelay
                            )
{
    CHECK_FN_POINTER(fnInit);
    CHECK_FN_POINTER(fnChipSelect);
    CHECK_FN_POINTER(fnReset);
    CHECK_FN_POINTER(fnTransmit);
    CHECK_FN_POINTER(fnCRecieve);
    // CHECK_FN_POINTER(fnCallback);    // Optional (if you want callback)
    CHECK_FN_POINTER(fnDelay);

    gCntxDriver.fnInit          = fnInit;
    gCntxDriver.fnChipSelect    = fnChipSelect;
    gCntxDriver.fnReset         = fnReset;
    gCntxDriver.fnTransmit      = fnTransmit;
    gCntxDriver.fnCRecieve      = fnCRecieve;
    gCntxDriver.fnCallback      = fnCallback;
    gCntxDriver.fnDelay         = fnDelay;

    return LORA_STATUS_OK;
}

lora_status_t LoRa_begin(long frequency)
{
    CHECK_FN_POINTER(gCntxDriver.fnInit);
    CHECK_FN_POINTER(gCntxDriver.fnChipSelect);
    CHECK_FN_POINTER(gCntxDriver.fnReset);
    CHECK_FN_POINTER(gCntxDriver.fnTransmit);
    CHECK_FN_POINTER(gCntxDriver.fnCRecieve);
    CHECK_FN_POINTER(gCntxDriver.fnCallback);
    CHECK_FN_POINTER(gCntxDriver.fnDelay);
    
    // Call low-level init function
    gCntxDriver.fnInit();

    // setup pins
    gCntxDriver.fnChipSelect(HIGH);

    // perform reset
    gCntxDriver.fnReset(LOW);
    gCntxDriver.fnDelay(10);
    gCntxDriver.fnReset(HIGH);
    gCntxDriver.fnDelay(10);

    // check version
    uint8_t version = readRegister(REG_VERSION);
    if (version != 0x12)
    {
        return LORA_STATUS_UNSUPPORTED;
    }

    // put in sleep mode
    sleep();

    // set frequency
    LoRa_setFrequency(frequency);

    // set base addresses
    writeRegister(REG_FIFO_TX_BASE_ADDR, 0);
    writeRegister(REG_FIFO_RX_BASE_ADDR, 0);

    // set LNA boost
    writeRegister(REG_LNA, readRegister(REG_LNA) | 0x03);

    // set auto AGC
    writeRegister(REG_MODEM_CONFIG_3, 0x04);

    // set output power to 17 dBm
    LoRa_setTxPower(17, PA_OUTPUT_RFO_PIN);

    // put in standby mode
    idle();

    return LORA_STATUS_OK;
}

void LoRa_end(void)
{
    // put in sleep mode
    sleep();
}

int LoRa_beginPacket(int implicitHeader)
{
    if (isTransmitting()) 
    {
        return LORA_STATUS_BUSY;
    }

    // put in standby mode
    idle();

    if (implicitHeader)
    {
        LoRa_implicitHeaderMode();
    } 
    else
    {
        LoRa_explicitHeaderMode();
    }

    // reset FIFO address and paload length
    writeRegister(REG_FIFO_ADDR_PTR, 0);
    writeRegister(REG_PAYLOAD_LENGTH, 0);

    return LORA_STATUS_OK;
}

int LoRa_endPacket(void)
{
    // put in TX mode
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);

    // wait for TX done
    // TODO: Make this safe by adding timeout!
    while ((readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0);

    // clear IRQ's
    writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);

    return LORA_STATUS_OK;
}

static bool isTransmitting(void)
{
    if ((readRegister(REG_OP_MODE) & MODE_TX) == MODE_TX)
    {
        return true;
    }

    if (readRegister(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK)
    {
        // clear IRQ's
        writeRegister(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
    }

    return false;
}

int LoRa_parsePacket(int size)
{
    int packetLength = 0;
    int irqFlags = readRegister(REG_IRQ_FLAGS);

    if (size > 0)
    {
        LoRa_implicitHeaderMode();

        writeRegister(REG_PAYLOAD_LENGTH, size & 0xff);
    }
    else 
    {
        LoRa_explicitHeaderMode();
    }

    // clear IRQ's
    writeRegister(REG_IRQ_FLAGS, irqFlags);

    if ((irqFlags & IRQ_RX_DONE_MASK) && (irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0)
    {
        // received a packet
        gPacketIndex = 0;

        // read packet length
        if (gImplicitHeaderMode)
        {
            packetLength = readRegister(REG_PAYLOAD_LENGTH);
        } 
        else 
        {
            packetLength = readRegister(REG_RX_NB_BYTES);
        }

        // set FIFO address to current RX address
        writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_CURRENT_ADDR));

        // put in standby mode
        idle();
    }
    else if (readRegister(REG_OP_MODE) != (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE))
    {
        // not currently in RX mode

        // reset FIFO address
        writeRegister(REG_FIFO_ADDR_PTR, 0);

        // put in single RX mode
        writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_RX_SINGLE);
    }

    return packetLength;
}

int LoRa_packetRssi(void)
{
    return (readRegister(REG_PKT_RSSI_VALUE) - (gFrequency < 868E6 ? 164 : 157));
}

float LoRa_packetSnr(void)
{
    return ((int8_t)readRegister(REG_PKT_SNR_VALUE)) * 0.25;
}

long LoRa_packetFrequencyError(void)
{
    int32_t freqError = 0;
    freqError = (int32_t)(readRegister(REG_FREQ_ERROR_MSB) & 0b00000111);
    freqError <<= 8L;
    freqError += (int32_t)(readRegister(REG_FREQ_ERROR_MID));
    freqError <<= 8L;
    freqError += (int32_t)(readRegister(REG_FREQ_ERROR_LSB));

    if (readRegister(REG_FREQ_ERROR_MSB) & 0b1000) { // Sign bit is on
        freqError -= 524288; // B1000'0000'0000'0000'0000
    }

    const float fXtal = 32E6; // FXOSC: crystal oscillator (XTAL) frequency (2.5. Chip Specification, p. 14)
    const float fError = (((float)freqError * (1L << 24)) / fXtal) * (LoRa_getSignalBandwidth() / 500000.0f); // p. 37

    return (long)(fError);
}

size_t LoRa_writeByte(uint8_t byte)
{
    return LoRa_write(&byte, sizeof(byte));
}

size_t LoRa_write(const uint8_t *buffer, size_t size)
{
    int currentLength = readRegister(REG_PAYLOAD_LENGTH);

    // check size
    if ((currentLength + size) > MAX_PKT_LENGTH)
    {
        size = MAX_PKT_LENGTH - currentLength;
    }

    // write data
    for (size_t i = 0; i < size; i++)
    {
        writeRegister(REG_FIFO, buffer[i]);
    }

    // update length
    writeRegister(REG_PAYLOAD_LENGTH, currentLength + size);

    return size;
}

int LoRa_available(void)
{
    return (readRegister(REG_RX_NB_BYTES) - gPacketIndex);
}

int LoRa_read(void)
{
    if (!LoRa_available())
    {
        return -1;
    }

    gPacketIndex++;

    return readRegister(REG_FIFO);
}

int LoRa_peek(void)
{
    if (!LoRa_available())
    {
        return -1;
    }

    // store current FIFO address
    int currentAddress = readRegister(REG_FIFO_ADDR_PTR);

    // read
    uint8_t b = readRegister(REG_FIFO);

    // restore FIFO address
    writeRegister(REG_FIFO_ADDR_PTR, currentAddress);

    return b;
}

void LoRa_flush(void)
{
}

static void idle(void)
{
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_STDBY);
}

static void sleep(void)
{
    writeRegister(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_SLEEP);
}

void LoRa_setTxPower(int level, int outputPin)
{
    if (PA_OUTPUT_RFO_PIN == outputPin)
    {
        // RFO
        if (level < 0)
        {
            level = 0;
        }
        else if (level > 14)
        {
            level = 14;
        }

        writeRegister(REG_PA_CONFIG, 0x70 | level);
    }
    else
    {
        // PA BOOST
        if (level > 17)
        {
            if (level > 20)
            {
                level = 20;
            }

            // subtract 3 from level, so 18 - 20 maps to 15 - 17
            level -= 3;

            // High Power +20 dBm Operation (Semtech SX1276/77/78/79 5.4.3.)
            writeRegister(REG_PA_DAC, 0x87);
            LoRa_setOCP(140);
        }
        else
        {
            if (level < 2)
            {
                level = 2;
            }
            //Default value PA_HF/LF or +17dBm
            writeRegister(REG_PA_DAC, 0x84);
            LoRa_setOCP(100);
        }

        writeRegister(REG_PA_CONFIG, PA_BOOST | (level - 2));
    }
}

void LoRa_setFrequency(long frequency)
{
    gFrequency = frequency;

    uint64_t frf = ((uint64_t)frequency << 19) / 32000000;

    writeRegister(REG_FRF_MSB, (uint8_t)(frf >> 16));
    writeRegister(REG_FRF_MID, (uint8_t)(frf >> 8));
    writeRegister(REG_FRF_LSB, (uint8_t)(frf >> 0));
}

int LoRa_getSpreadingFactor(void)
{
    return readRegister(REG_MODEM_CONFIG_2) >> 4;
}

void LoRa_setSpreadingFactor(int sf)
{
    if (sf < 6)
    {
        sf = 6;
    }
    else if (sf > 12)
    {
        sf = 12;
    }

    if (sf == 6)
    {
        writeRegister(REG_DETECTION_OPTIMIZE, 0xc5);
        writeRegister(REG_DETECTION_THRESHOLD, 0x0c);
    }
    else
    {
        writeRegister(REG_DETECTION_OPTIMIZE, 0xc3);
        writeRegister(REG_DETECTION_THRESHOLD, 0x0a);
    }

    writeRegister(REG_MODEM_CONFIG_2, (readRegister(REG_MODEM_CONFIG_2) & 0x0f) | ((sf << 4) & 0xf0));
    LoRa_setLdoFlag();
}

long LoRa_getSignalBandwidth(void)
{
    uint8_t bw = (readRegister(REG_MODEM_CONFIG_1) >> 4);

    switch (bw)
    {
    case 0: return 7.8E3;
    case 1: return 10.4E3;
    case 2: return 15.6E3;
    case 3: return 20.8E3;
    case 4: return 31.25E3;
    case 5: return 41.7E3;
    case 6: return 62.5E3;
    case 7: return 125E3;
    case 8: return 250E3;
    case 9: return 500E3;
    }

    return -1;
}

void LoRa_setSignalBandwidth(long sbw)
{
    int bw;

    if (sbw <= 7.8E3)
    {
        bw = 0;
    }
    else if (sbw <= 10.4E3)
    {
        bw = 1;
    }
    else if (sbw <= 15.6E3)
    {
        bw = 2;
    }
    else if (sbw <= 20.8E3)
    {
        bw = 3;
    }
    else if (sbw <= 31.25E3)
    {
        bw = 4;
    }
    else if (sbw <= 41.7E3)
    {
        bw = 5;
    }
    else if (sbw <= 62.5E3)
    {
        bw = 6;
    }
    else if (sbw <= 125E3)
    {
        bw = 7;
    }
    else if (sbw <= 250E3)
    {
        bw = 8;
    }
    else {   /*if (sbw <= 250E3)*/
        bw = 9;
    }

    writeRegister(REG_MODEM_CONFIG_1, (readRegister(REG_MODEM_CONFIG_1) & 0x0f) | (bw << 4));
    LoRa_setLdoFlag();
}

void LoRa_setLdoFlag(void)
{
    // Section 4.1.1.5
    long symbolDuration = 1000 / ( LoRa_getSignalBandwidth() / (1L << LoRa_getSpreadingFactor()) ) ;

    // Section 4.1.1.6
    bool ldoOn = symbolDuration > 16;

    uint8_t config3 = readRegister(REG_MODEM_CONFIG_3);
    // bitWrite(config3, 3, ldoOn);
    config3 &= ~(0<<3);
    config3 |= (ldoOn << 3);
    writeRegister(REG_MODEM_CONFIG_3, config3);
}

void LoRa_setCodingRate4(int denominator)
{
    if (denominator < 5)
    {
        denominator = 5;
    }
    else if (denominator > 8)
    {
        denominator = 8;
    }

    int cr = denominator - 4;

    writeRegister(REG_MODEM_CONFIG_1, (readRegister(REG_MODEM_CONFIG_1) & 0xf1) | (cr << 1));
}

void LoRa_setPreambleLength(long length)
{
    writeRegister(REG_PREAMBLE_MSB, (uint8_t)(length >> 8));
    writeRegister(REG_PREAMBLE_LSB, (uint8_t)(length >> 0));
}

void LoRa_setSyncWord(int sw)
{
    writeRegister(REG_SYNC_WORD, sw);
}

void LoRa_enableCrc(void)
{
    writeRegister(REG_MODEM_CONFIG_2, readRegister(REG_MODEM_CONFIG_2) | 0x04);
}

void LoRa_disableCrc(void)
{
    writeRegister(REG_MODEM_CONFIG_2, readRegister(REG_MODEM_CONFIG_2) & 0xfb);
}

void LoRa_enableInvertIQ(void)
{
    writeRegister(REG_INVERTIQ,  0x66);
    writeRegister(REG_INVERTIQ2, 0x19);
}

void LoRa_disableInvertIQ(void)
{
    writeRegister(REG_INVERTIQ,  0x27);
    writeRegister(REG_INVERTIQ2, 0x1d);
}

void LoRa_setOCP(uint8_t mA)
{
    uint8_t ocpTrim = 27;

    if (mA <= 120)
    {
        ocpTrim = (mA - 45) / 5;
    }
    else if (mA <= 240)
    {
        ocpTrim = (mA + 30) / 10;
    }

    writeRegister(REG_OCP, 0x20 | (0x1F & ocpTrim));
}

uint8_t LoRa_random(void)
{
    return readRegister(REG_RSSI_WIDEBAND);
}

void LoRa_dumpRegisters(void)
{
    for (int i = 0; i < 128; i++)
    {
        debug("0x%02X = 0x%02X", i, readRegister(i));
    }
}

void LoRa_explicitHeaderMode(void)
{
    gImplicitHeaderMode = 0;

    writeRegister(REG_MODEM_CONFIG_1, readRegister(REG_MODEM_CONFIG_1) & 0xfe);
}

void LoRa_implicitHeaderMode(void)
{
    gImplicitHeaderMode = 1;

    writeRegister(REG_MODEM_CONFIG_1, readRegister(REG_MODEM_CONFIG_1) | 0x01);
}

static void LoRa_handleDio0Rise(void)
{
    int irqFlags = readRegister(REG_IRQ_FLAGS);

    // clear IRQ's
    writeRegister(REG_IRQ_FLAGS, irqFlags);

    if ((irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0)
    {
        // received a packet
        gPacketIndex = 0;

        // read packet length
        int packetLength = gImplicitHeaderMode ? readRegister(REG_PAYLOAD_LENGTH) : readRegister(REG_RX_NB_BYTES);

        // set FIFO address to current RX address
        writeRegister(REG_FIFO_ADDR_PTR, readRegister(REG_FIFO_RX_CURRENT_ADDR));

        if (gCntxDriver.fnCallback)
        {
            gCntxDriver.fnCallback(packetLength);
        }

        // reset FIFO address
        writeRegister(REG_FIFO_ADDR_PTR, 0);
    }
}

static uint8_t readRegister(uint8_t address)
{
    return singleTransfer(address & 0x7f, 0x00);
}

static void writeRegister(uint8_t address, uint8_t value)
{
    singleTransfer(address | 0x80, value);
}

static uint8_t singleTransfer(uint8_t address, uint8_t value)
{
    UNUSED(address);
    UNUSED(value);

    uint8_t response=0;

    gCntxDriver.fnChipSelect(LOW);



    // _spi->beginTransaction(_spiSettings);
    // _spi->transfer(address);
    gCntxDriver.fnTransmit(&address, &response, 1);
    gCntxDriver.fnTransmit(&value, &response, 1);
    // response = _spi->transfer(value);
    // _spi->endTransaction();

    gCntxDriver.fnChipSelect(HIGH);

    return response;
}

void LoRa_onDio0Rise()
{
    LoRa_handleDio0Rise();
}
