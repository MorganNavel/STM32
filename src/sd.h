#ifndef __MESD_H
#define __MESD_H
#include "stdbool.h"
#include <stm32f0xx_usart.h>

typedef struct
{
    void (*putc)(void *pCtx, uint8_t c);
    uint8_t (*getc)(void *pCtx);
    bool (*canRead)(void *pCtx, int n);
    bool (*canWrite)(void *pCtx, int n);

} me_sd_ops_t;

typedef struct
{
    me_sd_ops_t *ops;
    void *ctx;
} me_sd_t;

int sd_Write(me_sd_t *sd, const uint8_t *pB, int n);
int sd_Read(me_sd_t *sd, uint8_t *pB, int n);
void sd_Print(me_sd_t *sd, const char *buf);
void sd_Printf(me_sd_t *sd, const char *format, ...);

#endif // __MESD_H
