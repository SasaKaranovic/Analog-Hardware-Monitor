#include "board.h"
#include "mcp4728.h"
#include "stm32f103_mcp4728_Driver.h"

void mcp4728_Start(void)
{
    mcp4728_initLib(mcp4728Driver_gpioLdac, mcp4728Driver_Send, mcp4728Driver_Recieve);
    mcp4728_begin();

    mcp4728_selectVref(VREF_INTERNAL_2_8V, VREF_INTERNAL_2_8V, VREF_INTERNAL_2_8V, VREF_INTERNAL_2_8V);
    mcp4728_selectPowerDown(PWR_DOWN_NORMAL, PWR_DOWN_NORMAL, PWR_DOWN_NORMAL, PWR_DOWN_NORMAL);
    mcp4728_selectGain(GAIN_X1, GAIN_X1, GAIN_X1, GAIN_X1);

    mcp4728_enable(true);
}

void mcp4728Driver_gpioLdac(uint8_t pinState)
{
    if(pinState)
    {
        MCP4728_LDAC_SET();
    }
    else
    {
        MCP4728_LDAC_RESET();
    }
}

uint8_t mcp4728Driver_Send(const uint16_t i2cAddress, const uint8_t *pTxBuffer, const uint16_t nTxCount)
{
    if(pTxBuffer == NULL)
    {
        return 0;
    }
    UNUSED(i2cAddress);
    UNUSED(pTxBuffer);
    UNUSED(nTxCount);

    i2c1_send(i2cAddress, pTxBuffer, nTxCount);
    return 0;
}

uint8_t mcp4728Driver_Recieve(const uint8_t i2cAddress, uint8_t *pRxBuffer, const uint32_t nTxCount)
{
    if(pRxBuffer == NULL)
    {
        return 0;
    }
    UNUSED(i2cAddress);
    UNUSED(pRxBuffer);
    UNUSED(nTxCount);
    return 0;
}


