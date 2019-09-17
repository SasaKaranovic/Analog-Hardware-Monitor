#ifndef __STATUS_CODES_H__
#define __STATUS_CODES_H__

#define HAL_to_Status(x)    (0xA0000000+x)

typedef enum STATUS_CODES
{
    STATUS_OK               = 0,
    STATUS_ERROR            = 1,
    STATUS_BUSY             = 2,
    STATUS_INVALID_ARGUMENT = 3,
    STATUS_INVALID_STATE    = 4,
    STATUS_UNSUPPORTED      = 5,
} status_t;

#endif