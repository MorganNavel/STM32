#ifndef __COMMAND_H
#define __COMMAND_H
#include "console.h"
#define GET_CONSOLE_SECTION()                            \
    extern const con_cmd_dsc_t __start_console_cmd_list; \
    extern const con_cmd_dsc_t __stop_console_cmd_list;

#define DEFINE_CONSOLE_CMD(name, desc, usage, fn)                   \
    __attribute__((used, section(".console_cmd_list"), aligned(4))) \
    const con_cmd_dsc_t __console_cmd_##name = {#name, desc, usage, fn};
#define GET_CURRENT_CMD() \
    con_cmd_dsc_t *cmd = (con_cmd_dsc_t *)ctx->current_cmd;
#endif // __COMMAND_H