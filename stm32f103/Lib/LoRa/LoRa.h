#ifndef __SK_LORA_H__
#define __SK_LORA_H__

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define PA_OUTPUT_RFO_PIN          0
#define PA_OUTPUT_PA_BOOST_PIN     1

#ifndef UNUSED
#define UNUSED(x) ((void)(x))
#endif
#ifndef HIGH
#define HIGH     1
#endif
#ifndef LOW
#define LOW     0
#endif

// Status Codes
typedef enum LORA_STATUS
{
    LORA_STATUS_OK                  = 0,
    LORA_STATUS_ERROR               = 1,
    LORA_STATUS_BUSY                = 2,
    LORA_STATUS_INVALID_ARGUMENT    = 3,
    LORA_STATUS_INVALID_STATE       = 4,
    LORA_STATUS_UNSUPPORTED         = 5,
} lora_status_t;

// LoRa Driver Function typedef
typedef lora_status_t   (*LoRa_Driver_Init_fn_t)(void); 
typedef void            (*LoRa_Driver_CS_fn_t)(uint8_t pinState); 
typedef void            (*LoRa_Driver_RST_fn_t)(uint8_t pinState); 
typedef void            (*LoRa_Driver_Delay_fn_t)(const uint32_t nDelayms); 
typedef lora_status_t   (*LoRa_Transmit_fn_t)(const uint8_t* pTxBuffer, uint8_t *pRxBuffer, const uint32_t nTxCount); 
typedef lora_status_t   (*LoRa_Receive_fn_t)(uint8_t* pBuffer, uint32_t nRxCount); 
typedef void            (*LoRa_IntCallback_fn_t)(const uint32_t packetLength); 

typedef struct LoRa_Driver
{
    LoRa_Driver_Init_fn_t   fnInit;
    LoRa_Driver_CS_fn_t     fnChipSelect;
    LoRa_Driver_RST_fn_t    fnReset;
    LoRa_Transmit_fn_t      fnTransmit;
    LoRa_Receive_fn_t       fnCRecieve;
    LoRa_IntCallback_fn_t   fnCallback;
    LoRa_Driver_Delay_fn_t  fnDelay;
} LoRa_Driver_t;




lora_status_t LoRa_InitLib(     LoRa_Driver_Init_fn_t   fnInit,
                                LoRa_Driver_CS_fn_t     fnChipSelect,
                                LoRa_Driver_RST_fn_t    fnReset,
                                LoRa_Transmit_fn_t      fnTransmit,
                                LoRa_Receive_fn_t       fnCRecieve,
                                LoRa_IntCallback_fn_t   fnCallback,
                                LoRa_Driver_Delay_fn_t  fnDelay
                            );

lora_status_t LoRa_begin(long frequency);
size_t LoRa_write(const uint8_t *buffer, size_t size);
void LoRa_end();

int LoRa_beginPacket(int implicitHeader);
int LoRa_endPacket(void);

int LoRa_parsePacket(int size);
int LoRa_packetRssi(void);
float LoRa_packetSnr(void);
long LoRa_packetFrequencyError(void);

int LoRa_available(void);
int LoRa_read(void);
int LoRa_peek(void);
void LoRa_flush(void);

void LoRa_setTxPower(int level, int outputPin);
void LoRa_setFrequency(long frequency);
void LoRa_setSpreadingFactor(int sf);
void LoRa_setSignalBandwidth(long sbw);
void LoRa_setCodingRate4(int denominator);
void LoRa_setPreambleLength(long length);
void LoRa_setSyncWord(int sw);
void LoRa_enableCrc();
void LoRa_disableCrc();
void LoRa_enableInvertIQ();
void LoRa_disableInvertIQ();

void LoRa_setOCP(uint8_t mA); // Over Current Protection control

// deprecated
void LoRa_enableCrc(void);
void LoRa_disableCrc(void);

uint8_t LoRa_random(void);

void LoRa_dumpRegisters(void);

void LoRa_explicitHeaderMode(void);
void LoRa_implicitHeaderMode(void);

bool LoRa_isTransmitting(void);

int LoRa_getSpreadingFactor();
long LoRa_getSignalBandwidth();

void LoRa_setLdoFlag();

uint8_t LoRa_readRegister(uint8_t address);
void LoRa_writeRegister(uint8_t address, uint8_t value);
uint8_t LoRa_singleTransfer(uint8_t address, uint8_t value);

#endif