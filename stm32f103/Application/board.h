#ifndef __BOARD_H__
#define __BOARD_H__

// C99
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

// HAL
#include <stm32f1xx.h>
#include "stm32f1xx_hal_conf.h"
#include <stm32f1xx_hal_gpio.h>
#include "stm32f1xx_hal_uart.h"
#include "stm32f1xx_hal_i2c.h"
#include "stm32f1xx_hal_rcc.h"

// Application
#include "status_codes.h"
#include "sys_init.h"
#include "debug.h"
#include "uart.h"
#include "i2c.h"

#ifndef _UNUSED
#define _UNUSED(x) ( (void) x )
#endif
#ifndef UNUSED
#define UNUSED(x) ( (void) x )
#endif

// Global definitions
#define FW_VERSION	"1.0"
#define HW_VERSION	"1.0"

// -- GPIO definitions 
    
#define GPIO_DBG_LED_PIN        GPIO_PIN_15
#define GPIO_DBG_LED_PORT       GPIOA
    
#define GPIO_USR_LED_PIN        GPIO_PIN_3
#define GPIO_USR_LED_PORT       GPIOB


/**I2C1 Configuration    
PB6      ------> I2C1_SCL
PB7      ------> I2C1_SDA
*/

#define I2C1_SCL_PIN			GPIO_PIN_6
#define I2C1_SCL_PORT			GPIOB

#define I2C1_SDA_PIN			GPIO_PIN_7
#define I2C1_SDA_PORT			GPIOB

// MCP4728 LDAC
#define MCP4728_LDAC_PIN		GPIO_PIN_8
#define MCP4728_LDAC_PORT		GPIOB
#define MCP4728_LDAC_SET()       (HAL_GPIO_WritePin(MCP4728_LDAC_PORT, MCP4728_LDAC_PIN, GPIO_PIN_SET))
#define MCP4728_LDAC_RESET()     (HAL_GPIO_WritePin(MCP4728_LDAC_PORT, MCP4728_LDAC_PIN, GPIO_PIN_RESET))

/**USART1 GPIO Configuration    
PA9      ------> USART1_TX
PA10     ------> USART1_RX 
*/

#define USART1_TX_PIN			GPIO_PIN_9
#define USART1_TX_PORT			GPIOA

#define USART1_RX_PIN			GPIO_PIN_10
#define USART1_RX_PORT			GPIOA

// Convenience macros and definitions
#define DebugLED_On()       (HAL_GPIO_WritePin(GPIO_DBG_LED_PORT, GPIO_DBG_LED_PIN, GPIO_PIN_SET))
#define DebugLED_Off()      (HAL_GPIO_WritePin(GPIO_DBG_LED_PORT, GPIO_DBG_LED_PIN, GPIO_PIN_RESET))
#define DebugLED_Toggle()   (HAL_GPIO_TogglePin(GPIO_DBG_LED_PORT, GPIO_DBG_LED_PIN))

#define UserLED_On()        (HAL_GPIO_WritePin(GPIO_USR_LED_PORT, GPIO_USR_LED_PIN, GPIO_PIN_SET))
#define UserLED_Off()       (HAL_GPIO_WritePin(GPIO_USR_LED_PORT, GPIO_USR_LED_PIN, GPIO_PIN_RESET))
#define UserLED_Toggle()    (HAL_GPIO_TogglePin(GPIO_USR_LED_PORT, GPIO_USR_LED_PIN))

#endif