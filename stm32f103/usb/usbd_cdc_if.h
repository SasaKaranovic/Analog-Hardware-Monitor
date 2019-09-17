#ifndef __USBD_CDC_IF_H__
#define __USBD_CDC_IF_H__

/* Includes ------------------------------------------------------------------*/
#include "usbd_cdc.h"
#include <stdbool.h>

/** CDC Interface callback. */
extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;


/** @defgroup USBD_CDC_IF_Exported_FunctionsPrototype USBD_CDC_IF_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
  */

uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);

void CDC_getRXBuffer(uint8_t **pRxBuffer, uint32_t *pRxCnt);
void CDC_resetBuffer(void);
bool CDC_ReadyForParse(void);

#endif