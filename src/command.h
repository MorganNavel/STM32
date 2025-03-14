#ifndef __COMMAND_H
#define __COMMAND_H
#include "console.h"

#define DEFINE_CMDS(_name) const con_cmd_dsc_t _name##_cmds[] = {
#define CMD(_name, _desc, _usage, _fn) \
    {                                  \
        .name = _name,                 \
        .desc = _desc,                 \
        .usage = _usage,               \
        .fnCmd = _fn,                  \
    },
#define END_CMDS() \
    {              \
    }              \
    }              \
    ;

#endif // __COMMAND_H