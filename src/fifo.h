#ifndef __MEFIFO_H
#define __MEFIFO_H

#include <stdint.h>
#include <stdbool.h>
#define DEFINE_FIFO(_name, _size) \
                                  \
    uint8_t _name##_buff[_size];  \
    ME_Fifo _name = {             \
        .rd = 0,                  \
        .wr = 0,                  \
        .buf = _name##_buff,      \
        .mask = _size - 1};

typedef struct
{
    uint32_t rd;
    uint32_t wr;
    uint8_t *buf;
    uint16_t mask;
} ME_Fifo;

bool ME_FifoWait(ME_Fifo *fifo);
bool ME_FifoIsEmpty(ME_Fifo *fifo);
bool ME_FifoIsFull(ME_Fifo *fifo);
bool ME_FifoCanRead(ME_Fifo *fifo, int n);
bool ME_FifoCanWrite(ME_Fifo *fifo, int n);
int ME_FifoCount(ME_Fifo *fifo);
int ME_FifoFree(ME_Fifo *fifo);
uint8_t ME_FifoRead(ME_Fifo *fifo);
void ME_FifoWrite(ME_Fifo *fifo, uint8_t val);

#endif // __MYFIFO_H
