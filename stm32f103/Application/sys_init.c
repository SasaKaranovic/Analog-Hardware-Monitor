// App
#include "board.h"
#include "uart.h"
#include "i2c.h"
#include "debug.h"
#include "usb_device.h"

// Local functions
static void SystemClock_Config(void);


void Sys_Init(void)
{

    HAL_Init();
    SystemClock_Config();

    HAL_Delay(100);

    GPIO_InitTypeDef GPIO_InitStruct;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    __HAL_RCC_AFIO_CLK_ENABLE();

    USART_Init();


    // Output our current MCU config
    debug("----- SteamPunk Dashboard [%s %s] -----", __DATE__, __TIME__);
    debug("System Clock: %ldMHz", (HAL_RCC_GetSysClockFreq()/1000000));


    // -- 1 -- GPIO setup

    // Debug/activity LED
    GPIO_InitStruct.Pin     = GPIO_DBG_LED_PIN;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_DBG_LED_PORT, &GPIO_InitStruct);

    // MCP4728 LDAC
    GPIO_InitStruct.Pin     = MCP4728_LDAC_PIN;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(MCP4728_LDAC_PORT, &GPIO_InitStruct);
    HAL_GPIO_WritePin(MCP4728_LDAC_PORT, MCP4728_LDAC_PIN, GPIO_PIN_RESET);

    // User LED
    GPIO_InitStruct.Pin     = GPIO_USR_LED_PIN;
    GPIO_InitStruct.Mode    = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed   = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_USR_LED_PORT, &GPIO_InitStruct);


    HAL_GPIO_WritePin(GPIO_DBG_LED_PORT, GPIO_DBG_LED_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIO_USR_LED_PORT, GPIO_USR_LED_PIN, GPIO_PIN_RESET);

    i2c1_init();
    HAL_Init();
    MX_USB_DEVICE_Init();

    DebugLED_On();
    UserLED_On();

    HAL_Delay(500);
    DebugLED_Off();
    UserLED_Off();
}



/** System Clock Configuration
*/
static void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  HAL_RCC_OscConfig(&RCC_OscInitStruct);

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);
  
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

}


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
    UNUSED(file);
    UNUSED(line);
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */

}

#endif

