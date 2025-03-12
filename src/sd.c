#include "sd.h"
#include "tiny_printf.h"
int sd_Write(me_sd_t *sd, const uint8_t *pB, int n)
{
    int res = 0;
    if (sd->ops->canWrite(sd->ctx, n))
    {
        while (*pB && res < n)
        {
            sd->ops->putc(sd->ctx, *pB);
            res++;
            pB++;
        }
    }
    return res;
}

int sd_Read(me_sd_t *sd, uint8_t *pB, int n)
{

    int res = 0;
    if (sd->ops->canRead(sd->ctx, n))
    {
        while (res < n)
        {
            *pB = sd->ops->getc(sd->ctx);
            res++;
            pB++;
        }
    }
    return res;
}

void sd_Print(me_sd_t *sd, const char *buf)
{

    while (*buf)
    {
        sd->ops->putc(sd->ctx, *buf);
        buf++;
    }
    sd->ops->putc(sd, '\0');
}

void sd_Printf(me_sd_t *sd, const char *format, ...)
{
    va_list vlist;
    va_start(vlist, format);
    tiny_format(format, sd->ctx, sd->ops->putc, vlist);
    va_end(vlist);
}
