#include "board.h"
#include "uart.h"



/* USART2 init function */
UART_HandleTypeDef      huart1;

void USART_Init(void)
{
    GPIO_InitTypeDef        GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    __HAL_RCC_USART1_CLK_ENABLE();

   /**USART1 GPIO Configuration    
    PA9      ------> USART1_TX
    PA10     ------> USART1_RX 
    */

    GPIO_InitStruct.Pin         = USART1_TX_PIN;
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull        = GPIO_PULLUP;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART1_TX_PORT, &GPIO_InitStruct);


    /* UART RX GPIO pin configuration  */
    GPIO_InitStruct.Pin         = USART1_RX_PIN;
    GPIO_InitStruct.Mode        = GPIO_MODE_AF_INPUT;
    GPIO_InitStruct.Pull        = GPIO_PULLUP;
    GPIO_InitStruct.Speed       = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(USART1_RX_PORT, &GPIO_InitStruct);


    huart1.Instance             = USART1;
    huart1.Init.WordLength      = UART_WORDLENGTH_8B;
    huart1.Init.StopBits        = UART_STOPBITS_1;
    huart1.Init.Parity          = UART_PARITY_NONE;
    huart1.Init.Mode            = UART_MODE_TX_RX;
    huart1.Init.HwFlowCtl       = UART_HWCONTROL_NONE;
    huart1.Init.OverSampling    = UART_OVERSAMPLING_16;
    huart1.Init.BaudRate        = 115200;
    
    HAL_UART_Init(&huart1);

}


char u1_putchar(char c)
{   
#ifdef DEBUG_BUILD
    while (!(USART1->SR & UART_FLAG_TXE));
            USART1->DR = (c & 0xFF);
#endif
    return c;
}




void clearFlags(void)
{
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_PE);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_TXE);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_TC);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_RXNE);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_IDLE);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_LBD);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_CTS);
    __HAL_UART_CLEAR_FLAG(&huart1, UART_IT_ERR);

}


