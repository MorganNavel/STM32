#ifndef MAIN_H
#define MAIN_H

#include "stm32f0xx.h"
#include <stdarg.h>
#include <stdint.h>
#include <string.h>

extern void *mempcpy(void *dest, const void *src, size_t n);



#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })
     
#endif
