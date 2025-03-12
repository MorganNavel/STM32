#include "fifo.h"
#include <stm32f0xx_usart.h>
#include "tiny_printf.h"

/**
 * @brief Await until write or read can be done
 * @param ME_Fifo: A circular fifo
 * @retval boolean
 */
bool ME_FifoWait(ME_Fifo *fifo)
{
    return ME_FifoCanRead(fifo, 1) || ME_FifoCanWrite(fifo, 1);
}

/**
 * @brief Return a value that says if the fifo is empty
 * @param ME_Fifo: A circular fifo
 * @retval boolean
 */
bool ME_FifoIsEmpty(ME_Fifo *fifo)
{
    return fifo->wr == fifo->rd;
}

/**
 * @brief Return a value that says if the fifo is full
 * @param ME_Fifo: A circular fifo
 * @retval boolean
 */
bool ME_FifoIsFull(ME_Fifo *fifo)
{
    return ME_FifoCount(fifo) == (fifo->mask);
}

/**
 * @brief Return a value that says if the read operation can be processed
 * @param ME_Fifo: A circular fifo
 * @retval boolean
 */

bool ME_FifoCanRead(void *pCtx, int n)
{
    ME_Fifo *fifo = (ME_Fifo *)pCtx;

    return ME_FifoCount(fifo) >= n;
}

/**
 * @brief Return a value that says if the write operation can be processed
 * @param ME_Fifo: A circular fifo
 * @retval boolean
 */
bool ME_FifoCanWrite(void *pCtx, int n)
{
    ME_Fifo *fifo = (ME_Fifo *)pCtx;

    return ME_FifoFree(fifo) >= n;
}

/**
 * @brief Return the number of current buffered values
 * @param ME_Fifo: A circular fifo
 * @retval int
 */
int ME_FifoCount(ME_Fifo *fifo)
{
    return (fifo->wr - fifo->rd) & fifo->mask;
}

/**
 * @brief Return the number of empty spaces
 * @param ME_Fifo: A circular fifo
 * @retval int
 */
int ME_FifoFree(ME_Fifo *fifo)
{
    return (fifo->mask) - ME_FifoCount(fifo);
}

/**
 * @brief Read a value from the buffer
 * @param ME_Fifo: A circular fifo
 * @retval uint8_t
 */
uint8_t ME_FifoRead(void *pCtx)
{
    ME_Fifo *fifo = (ME_Fifo *)pCtx;

    fifo->rd &= fifo->mask;
    uint8_t val = fifo->buf[fifo->rd];
    fifo->rd++;
    return val;
}

/**
 * @brief Write a value into the buffer
 * @param ME_Fifo: A circular fifo
 * @param uint16_t: value to write
 * @retval boolean - true if success else false
 */
void ME_FifoWrite(void *pCtx, uint8_t val)
{
    ME_Fifo *fifo = (ME_Fifo *)pCtx;

    fifo->wr &= fifo->mask;
    fifo->buf[fifo->wr] = val;
    fifo->wr++;
}
