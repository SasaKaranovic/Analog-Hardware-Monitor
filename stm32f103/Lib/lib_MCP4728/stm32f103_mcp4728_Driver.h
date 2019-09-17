#ifndef __SK_MCP4728_DRIVER_H__
#define __SK_MCP4728_DRIVER_H__

void mcp4728_Start(void);
void mcp4728Driver_gpioLdac(uint8_t pinState);
uint8_t mcp4728Driver_Send(const uint16_t i2cAddress, const uint8_t *pTxBuffer, const uint16_t nTxCount);
uint8_t mcp4728Driver_Recieve(const uint8_t i2cAddress, uint8_t* pRxBuffer, const uint32_t nTxCount);

#endif