#ifndef MCP4728_H
#define MCP4728_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>


// Enum declarations
typedef enum DAC_CH 
{ 
	DAC_CH_A, 
	DAC_CH_B, 
	DAC_CH_C, 
	DAC_CH_D 
} DAC_CH;

typedef enum VREF 
{ 
	VREF_VDD, 
	VREF_INTERNAL_2_8V 
} VREF;

typedef enum PWR_DOWN 
{ 
	PWR_DOWN_NORMAL, 
	PWR_DOWN_1KOHM, 
	PWR_DOWN_100KOHM, 
	PWR_DOWN_500KOHM 
} PWR_DOWN;

typedef enum GAIN 
{ 
	GAIN_X1, 
	GAIN_X2 
} GAIN;

// Driver function prototypes
typedef void      (*gpioLdac_fn_t)(uint8_t pinState);
typedef uint8_t   (*i2cSend_fn_t)(const uint16_t i2cAddress, const uint8_t *pBuffer, const uint16_t nSize); 
typedef uint8_t   (*i2cReceive_fn_t)(const uint8_t i2cAddress, uint8_t* pRxBuffer, const uint32_t nTxCount); 

// Function prototypes
void mcp4728_initLib(gpioLdac_fn_t fnLdac, i2cSend_fn_t fnSend, i2cReceive_fn_t fnReceive);
void mcp4728_begin(void);
void mcp4728_enable(bool b);
void mcp4728_SetPercent(uint8_t nChannel, uint8_t nPercent);
uint8_t mcp4728_analogWriteCh(DAC_CH ch, uint16_t data, bool b_eep);
uint8_t mcp4728_analogWrite(uint8_t ch, uint16_t data, bool b_eep);
uint8_t mcp4728_analogWriteAll(uint16_t a, uint16_t b, uint16_t c, uint16_t d, bool b_eep);
uint8_t mcp4728_selectVref(VREF a, VREF b, VREF c, VREF d);
uint8_t mcp4728_selectPowerDown(PWR_DOWN a, PWR_DOWN b, PWR_DOWN c, PWR_DOWN d);
uint8_t mcp4728_selectGain(GAIN a, GAIN b, GAIN c, GAIN d);
void mcp4728_setID(uint8_t id);
void mcp4728_readRegisters();
uint8_t mcp4728_getVref(uint8_t ch, bool b_eep);
uint8_t mcp4728_getGain(uint8_t ch, bool b_eep);
uint8_t mcp4728_getPowerDown(uint8_t ch, bool);
uint16_t mcp4728_getDACData(uint8_t ch, bool b_eep);

#endif // MCP4728_H