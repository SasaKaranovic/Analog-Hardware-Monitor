#include "board.h"
#include "mcp4728.h"
#include "stm32f103_mcp4728_Driver.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"
#include "uart_interface.h"

// # Messages sent to host and received from host will have following format
// [c:val1:val2:val3:val4]
// |||  | |  | |  | | |  |---> Packet end character ']'
// |||  | |  | |  | | |------> Value #4 (ASCII)
// |||  | |  | |  | |--------> Parameter separator ':'
// |||  | |  | |  |----------> Value #3 (ASCII)
// |||  | |  | |-------------> Parameter separator ':'
// |||  | |  |---------------> Value #2 (ASCII)
// |||  | |------------------> Parameter separator ':'
// |||  |--------------------> Value #1 (ASCII)
// |||-----------------------> Parameter separator ':'
// ||------------------------> Command byte
// |-------------------------> Packet start character '['


// All messages have to start with '[' and end with ']' character
#define PACKET_MIN_LENGTH       5       // Packet must have at least start, cmd, separator, value and end characters
#define PACKET_START_CHAR       '['
#define PACKET_END_CHAR         ']'
#define PACKET_DELIMITER_STR    ":"
#define DAC_MAX_CHANNELS        4


// # TODO: Add support for more communication with the host (if necessary)
// # currently it's just [s:abcd] where abcd are hex values of each channel
void parseRxBuffer(void)
{
    uint8_t *rxBuffer   = 0;
    uint32_t rxCnt      = 0;
    char *start         = NULL;
    char *end           = NULL;
    char *token         = NULL;
    uint8_t channel     = 0;
    uint8_t value       = 0;

    CDC_getRXBuffer(&rxBuffer, &rxCnt);
    debug("%s", rxBuffer);

    // Find start and end characters
    start   = strchr((const char *)rxBuffer, PACKET_START_CHAR);
    end     = strchr((const char *)rxBuffer, PACKET_END_CHAR);

    // If we didn't find start and end character, packet is invalid and we will skip parsing
    // Also if packet is less than PACKET_MIN_LENGTH we consider it invalid
    if((start == NULL) || (end == NULL) || (end-start) < 5)
    {
        CDC_resetBuffer();
        return;
    }

    
    // We've found start character, now look for 's'
    start++;
    if(*start == 's')
    {
        DebugLED_Toggle();
        UserLED_Toggle();
        // Skip separator character and start searching for parameters
        start +=2;
        // Walk through buffer
        token = strtok(start, PACKET_DELIMITER_STR);

        while( (token != NULL) && (channel < DAC_MAX_CHANNELS) )
        {
            value = (uint8_t)atoi(token);
            mcp4728_SetPercent(channel, value);
            channel++;

            token = strtok(NULL, PACKET_DELIMITER_STR);
        }
    }

    CDC_resetBuffer();
}
