#ifndef __SK_LORA_DRIVER_H__
#define __SK_LORA_DRIVER_H__

lora_status_t LoRaDriver_Init(void);
void LoRaDriver_ChipSelect(uint8_t pinState);
void LoRaDriver_ChipReset(uint8_t pinState);
lora_status_t LoRaDriver_Transmit(const uint8_t *pTxBuffer, uint8_t *pRxBuffer, const uint32_t nTxCount);
lora_status_t LoRaDriver_Recieve(uint8_t *pBuffer, uint32_t nTxCount);
void LoRaDriver_CallBack(const uint32_t nPacketLength);
void LoRaDriver_Delay(const uint32_t nDelayms);

#endif