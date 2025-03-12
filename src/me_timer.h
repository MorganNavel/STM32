#ifndef __METIMER_H
#define __METIMER_H

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
    uint64_t end;
} ME_Timer;
bool ME_isTimedOut(ME_Timer *me_Timer);
void ME_timerDisable(ME_Timer *me_Timer);
void ME_timerInit(ME_Timer *me_Timer, uint32_t delay);

#endif // __METIMER_H
