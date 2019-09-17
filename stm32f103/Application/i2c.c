#include "board.h"
#include "i2c.h"

#define I2C_ADDRESS      0x30F
#define I2C_SPEEDCLOCK   100000
#define I2C_DUTYCYCLE    I2C_DUTYCYCLE_2


/* I2C init function */
I2C_HandleTypeDef I2cHandle;

void i2c1_init(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    __HAL_RCC_I2C1_CLK_ENABLE();


    GPIO_InitStruct.Pin         = I2C1_SCL_PIN;
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull        = GPIO_PULLUP;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C1_SCL_PORT, &GPIO_InitStruct);


    GPIO_InitStruct.Pin         = I2C1_SDA_PIN;
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull        = GPIO_PULLUP;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(I2C1_SDA_PORT, &GPIO_InitStruct);


    I2cHandle.Instance             = I2C1;
    I2cHandle.Init.ClockSpeed      = I2C_SPEEDCLOCK;
    I2cHandle.Init.DutyCycle       = I2C_DUTYCYCLE;
    I2cHandle.Init.OwnAddress1     = I2C_ADDRESS;
    I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
    I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    I2cHandle.Init.OwnAddress2     = 0xFF;
    I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE; 
    if (HAL_I2C_Init(&I2cHandle) != HAL_OK)
    {
        debug("%s failed!", __FUNCTION__);
        while(1);
    }

}


void i2c1_send(const uint16_t i2cAddress, const uint8_t *pBuffer, const uint16_t nSize)
{
    HAL_I2C_Master_Transmit(&I2cHandle, (uint16_t)i2cAddress, (uint8_t*)pBuffer, (uint16_t) nSize, 0xFFFF);
}


void i2c1_receive(const uint16_t i2cAddress, uint8_t *pBuffer, const uint16_t nSize)
{
    HAL_I2C_Master_Receive(&I2cHandle, (uint16_t)i2cAddress, (uint8_t*)pBuffer, (uint16_t) nSize, 0xFFFF);
}

