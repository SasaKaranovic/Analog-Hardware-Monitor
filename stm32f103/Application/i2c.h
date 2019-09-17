#ifndef __I2C_H__
#define __I2C_H__

void i2c1_init(void);
void i2c1_send(const uint16_t i2cAddress, const uint8_t *pBuffer, const uint16_t nSize);
void i2c1_receive(const uint16_t i2cAddress, uint8_t *pBuffer, const uint16_t nSize);

#endif