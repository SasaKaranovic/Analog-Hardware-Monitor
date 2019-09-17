// App
#include "board.h"
#include "sys_init.h"
#include "debug.h"
#include "mcp4728.h"
#include "stm32f103_mcp4728_Driver.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "uart_interface.h"

#define COMM_TIMEOUT    5000    // after 5000ms (5s) of inactivity move dials to zero
volatile uint32_t nCommTimeOutTarget   = 0;
volatile uint32_t miliseconds   = 0;

int main(void)
{
    Sys_Init();
    mcp4728_Start();

    // Loop forever
    while (1)
    {
        // Check if we received new dial values
        if(CDC_ReadyForParse())
        {
            nCommTimeOutTarget = miliseconds + COMM_TIMEOUT;
            parseRxBuffer();
        }
        // If not, check if timeout expired (and reset dials)
        else if(miliseconds > nCommTimeOutTarget)
        {
            mcp4728_analogWriteAll(0, 0, 0, 0, false);
            nCommTimeOutTarget = miliseconds + COMM_TIMEOUT;
        }
    }
}


void HAL_SYSTICK_Callback(void)
{
    miliseconds++;
}

