#include "board.h"
#include "LoRa.h"
#include "spi.h"
#include "stm32f103_LoRa_Driver.h"

lora_status_t LoRaDriver_Init(void)
{
    SPI1_Init();
    return LORA_STATUS_OK;
}

void LoRaDriver_ChipSelect(uint8_t pinState)
{
    HAL_GPIO_WritePin(GPIO_CSn_PORT, GPIO_CSn_PIN, pinState);
}

void LoRaDriver_ChipReset(uint8_t pinState)
{
    HAL_GPIO_WritePin(GPIO_RST_PORT, GPIO_RST_PIN, pinState);
}

void LoRaDriver_Delay(const uint32_t nDelayms)
{
    HAL_Delay(nDelayms);
    return;
}

lora_status_t LoRaDriver_Transmit(const uint8_t *pTxBuffer, uint8_t *pRxBuffer, const uint32_t nTxCount)
{
    if(pTxBuffer == NULL)
    {
        return LORA_STATUS_INVALID_ARGUMENT;
    }
    if(pRxBuffer == NULL)
    {
        return LORA_STATUS_INVALID_ARGUMENT;
    }
    
    status_t status;
    if( (status=SPI1_Transmit((uint8_t*)pTxBuffer, pRxBuffer, nTxCount)) != STATUS_OK)
    {
        debug("%s: SPI1_Transmit Status 0x%02X", __FUNCTION__, status);
        return LORA_STATUS_ERROR;
    }

    return LORA_STATUS_OK;
}

lora_status_t LoRaDriver_Recieve(uint8_t* pBuffer, uint32_t nTxCount)
{
    if(pBuffer == NULL)
    {
        return LORA_STATUS_INVALID_ARGUMENT;
    }
    
    status_t status;
    if( (status=SPI1_Recieve((uint8_t*)pBuffer, nTxCount)) != STATUS_OK)
    {
        debug("%s: SPI1_Recieve Status 0x%02X", __FUNCTION__, status);
        return LORA_STATUS_ERROR;
    }

    return LORA_STATUS_OK;
}

void LoRaDriver_CallBack(const uint32_t nPacketLength)
{
    debug("%s", __FUNCTION__);
    UNUSED(nPacketLength);
}


