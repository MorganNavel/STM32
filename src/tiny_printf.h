#ifndef __TINY_PRINTF_H
#define __TINY_PRINTF_H

#define ALIGN_NONE (0b00)
#define ALIGN_RIGHT (0b01)
#define ALIGN_LEFT (0b10)
#define ALIGN_MASK (0b11)
#include <stdarg.h>

typedef struct
{
    uint8_t alignement_flags;
    int minWidth;
} format_t;

typedef void (*fnPutc_t)(void *, uint8_t c);

void tiny_printf(const char *format, ...);
void tiny_sprintf(uint8_t *buf, int size, const char *format, ...);
void tiny_format(const char *format, void *context, fnPutc_t fnPutc, va_list vlist);

#endif //__TINY_PRINTF_H