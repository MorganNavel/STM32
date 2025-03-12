#include "system.h"
#include "me_timer.h"
#include <stdbool.h>
bool ME_isTimedOut(ME_Timer *me_Timer)
{
    return meTime >= me_Timer->end;
}
void ME_timerDisable(ME_Timer *me_Timer)
{
    me_Timer->end = 0;
}

void ME_timerInit(ME_Timer *me_Timer, uint32_t delay)
{
    me_Timer->end = meTime + delay;
}
