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
#define END_CMDS()             \
    {                          \
        NULL, NULL, NULL, NULL \
    }                          \
    }                          \
    ;

con_cmd_rc_t ME_CMD_help(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_echo(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_uptime(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_reboot(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_clear(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_ping(console_ctx_t *ctx);

#endif // __COMMAND_H