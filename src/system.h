#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
void System_Init(void);

extern volatile uint64_t meTime;

uint32_t GiveTimeSec();
uint32_t GiveTime();

#endif // SYSTEM_H
