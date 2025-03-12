#include <stdarg.h>
#include "usart.h"
#include "tiny_printf.h"
#include <stdio.h>
#include <string.h>
#include "sd_uart.h"
extern me_sd_t USART1_sd;
typedef struct
{
    uint8_t *buf;
    int index;
    int size;

} sprintf_context_t;
typedef struct
{
    const char *s;
    int size;
    format_t *format;
    fnPutc_t fnPutc;
    void *context;
    char specifier;
} send_params_t;
void reverseStr(char *str, int len)
{
    for (int i = 0; i < len / 2; i++)
    {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}
int atoi(const char *s, int *nbCarParsed)
{
    int result = 0;
    int sign = 1;

    while (*s == ' ')
    {
        s++;
        (*nbCarParsed)++;
    }

    if (*s == '-' || *s == '+')
    {
        if (*s == '-')
            sign = -1;
        s++;
        (*nbCarParsed)++;
    }

    while (*s >= '0' && *s <= '9')
    {
        result = result * 10 + (*s - '0');
        s++;
        (*nbCarParsed)++;
    }

    return sign * result;
}
int itoa(int d, char *buf, int maxSize)
{
    if (d == 0)
    {
        buf[0] = '0';
        return 1;
    }
    bool isNegative = (d < 0);
    if (d < 0)
    {
        d = -d;
    }

    int i = 0;

    while (d > 0 && i < maxSize)
    {
        buf[i++] = '0' + (d % 10);
        d /= 10;
    }
    if (isNegative)
        buf[i++] = '-';
    reverseStr(buf, i);

    return i;
}
int htoa(uint32_t h, char *buf, int maxSize)
{
    if (h == 0)
    {
        buf[0] = '0';
        return 1;
    }
    int i = 0;
    while (h > 0 && i < maxSize)
    {
        int value = h & 0xF;
        buf[i++] = value >= 10 ? 'A' + (value - 10) : '0' + value;
        h >>= 4;
    }
    reverseStr(buf, i);
    return i;
}
int htoaV2(uint32_t h, char *buf, int maxSize)
{
    if (h == 0)
    {
        buf[0] = '0';
        return 1;
    }

    char *hexChars = "0123456789ABCDEF";
    int i = 0;

    while (h > 0 && i < maxSize)
    {
        int value = h & 0xF;
        buf[i++] = hexChars[value];
        h >>= 4;
    }

    return i;
}
int btoa(uint32_t b, char *buf, int maxSize)
{
    if (b == 0)
    {
        buf[0] = '0';
        return 1;
    }

    if (maxSize > 32)
        maxSize = 32;

    int cpt = 0;
    bool leadingZero = true;
    uint32_t mask = 1 << (maxSize - 1);
    for (int j = 0; j < maxSize; j++)
    {
        if (leadingZero && !(b & mask))
        {
            mask >>= 1;
            continue;
        }
        leadingZero = false;
        buf[cpt++] = (b & mask) ? '1' : '0';
        mask >>= 1;
    }
    return cpt;
}
void send(send_params_t params)
{
    format_t *f = params.format;
    fnPutc_t putc = params.fnPutc;
    void *ctx = params.context;

    bool isNumber = (params.specifier == 'd' || params.specifier == 'x' ||
                     params.specifier == 'b');

    char padChar = isNumber ? '0' : ' ';
    int padding = f->minWidth > params.size ? f->minWidth - params.size : 0;

    if (f->alignement_flags & ALIGN_RIGHT)
        while (padding--)
            putc(ctx, padChar);

    for (int i = 0; i < params.size && params.s[i]; i++)
        putc(ctx, params.s[i]);

    if ((f->alignement_flags & ALIGN_LEFT) && !isNumber)
        while (padding--)
            putc(ctx, ' ');
}
int specifyCTX(const char *format, format_t *f)
{
    int nbCarParsed = 0;
    f->alignement_flags = ALIGN_LEFT;
    f->minWidth = 0;
    if (*format == '-')
    {
        f->alignement_flags = ALIGN_RIGHT;
        format++;
        nbCarParsed++;
    }
    if (*format >= '0' && *format <= '9')
    {
        f->minWidth = atoi(format, &nbCarParsed);
    }
    return nbCarParsed;
}

void tiny_format(const char *format, void *context, fnPutc_t fnPutc, va_list vlist)
{
    while (*format)
    {
        if (*format == '%')
        {
            format++;
            format_t f;
            int c = specifyCTX(format, &f);
            format += c;

            char buf[64];
            int nb = 0;
            send_params_t params = {
                .context = context,
                .fnPutc = fnPutc,
                .format = &f,
                .s = buf,
            };

            switch (*format)
            {
            case 'c':
                buf[0] = (char)va_arg(vlist, int);
                nb = 1;
                break;
            case 's':
                params.s = va_arg(vlist, char *);
                nb = strlen(params.s);
                break;
            case 'd':
                if (f.minWidth == 0)
                    f.minWidth = 13;
                nb = itoa(va_arg(vlist, int), buf, f.minWidth);
                break;
            case 'b':
                if (f.minWidth == 0)
                    f.minWidth = 16;
                nb = btoa((uint32_t)va_arg(vlist, int), buf, f.minWidth);
                break;
            case 'x':
                if (f.minWidth == 0)
                    f.minWidth = 8;

                nb = htoa((uint32_t)va_arg(vlist, int), buf, f.minWidth);
                break;
            default:
                fnPutc(context, *format);
                break;
            }
            if (nb >= sizeof(buf))
                nb = sizeof(buf) - 1;

            if (nb > 0)
            {
                params.specifier = *format;
                params.size = nb;
                send(params);
            }
        }
        else
        {
            if (*format == '\n')
                fnPutc(context, '\r');
            fnPutc(context, *format);
        }
        format++;
    }
}
void sprintf_putc(void *context, uint8_t c)
{
    sprintf_context_t *ctx = (sprintf_context_t *)context;
    if (ctx->index < ctx->size - 1)
    {
        ctx->buf[ctx->index++] = c;
    }
}
void tiny_printf(const char *format, ...)
{
    va_list vlist;
    va_start(vlist, format);
    tiny_format(format, &USART1_sd, USART1_sd.ops->putc, vlist);
    va_end(vlist);
}
void tiny_sprintf(uint8_t *buf, int size, const char *format, ...)
{
    va_list vlist;
    va_start(vlist, format);
    sprintf_context_t context = {.buf = buf, .index = 0, .size = size};
    tiny_format(format, &context, sprintf_putc, vlist);
    if (context.index >= size - 1)
        buf[size - 1] = '\0';
    else
        buf[context.index] = '\0';

    va_end(vlist);
}
