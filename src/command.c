#include "console.h"
#include "system.h"
#include "me_timer.h"
#include "command.h"
#include <stdlib.h>
#include "tiny_printf.h"
#include <string.h>
#include "vt100.h"
#define UPTIME_DELAY 1000
#define CMD_CTX_BUF(_type, _varname)               \
    if (sizeof(_type) > CON_CMD_CTX_SIZE)          \
    {                                              \
        C_PRINTF("CON_CMD_CTX_SIZE is too small"); \
        return CON_RC_BAD_ARG;                     \
    }                                              \
    _type *_varname = (_type *)(ctx->cmd_ctx);
#define ADD_CONS_CMD(id, name, desc, usage, fn)                     \
    __attribute__((used, section(".console_cmd_list"), aligned(4))) \
    const con_cmd_dsc_t __console_cmd_list_##id = {name, desc, usage, fn};

con_cmd_rc_t ME_CMD_help(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_echo(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_uptime(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_reboot(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_clear(console_ctx_t *ctx);
con_cmd_rc_t ME_CMD_ping(console_ctx_t *ctx);
ADD_CONS_CMD(cmd_help, "help", "Display help", "help [command]", ME_CMD_help);
ADD_CONS_CMD(cmd_echo, "echo", "Print arguments", "echo [args]", ME_CMD_echo);
ADD_CONS_CMD(cmd_uptime, "uptime", "Display uptime", "uptime [-f]", ME_CMD_uptime);
ADD_CONS_CMD(cmd_reboot, "reboot", "Reboot the system", "reboot", ME_CMD_reboot);
ADD_CONS_CMD(cmd_clear, "clear", "Clear the screen", "clear", ME_CMD_clear);
ADD_CONS_CMD(cmd_ping, "ping", "Ping the system", "ping", ME_CMD_ping);

GET_CONSOLE_SECTION()
static const con_cmd_dsc_t *cmds = &__start_console_cmd_list;
static const con_cmd_dsc_t *end_cmd = &__stop_console_cmd_list;

#define ERROR_ARG(_arg) \
    GET_CURRENT_CMD()   \
    C_PRINTF("Error: Unknown argument '%s'. Usage: %s\n", _arg, cmd->usage);

#define ERROR_MISSING_ARG() \
    GET_CURRENT_CMD()       \
    C_PRINTF("Error: Missing argument. Usage: %s\n", cmd->usage);

con_cmd_rc_t ME_CMD_help(console_ctx_t *ctx)
{

    if (ctx->argc == 1)
    {

        C_PRINTF("Available commands:\n");
        for (const con_cmd_dsc_t *cmd = cmds; cmd < end_cmd; cmd++)
        {
            C_PRINTF("%-10s - %s\n", cmd->name, cmd->desc);
        }
        GET_CURRENT_CMD();
        C_PRINTF("\nUse '%s' for more details.\n", cmd->usage);
        return CON_RC_DONE;
    }

    for (const con_cmd_dsc_t *cmd = cmds; cmd < end_cmd; cmd++)
    {
        if (strcmp(cmd->name, ctx->argv[1]) == 0)
        {
            C_PRINTF("%s - %s\n", cmd->name, cmd->desc);
            C_PRINTF("- Usage: %s\n", cmd->usage);
            return CON_RC_DONE;
        }
    }
    return CON_RC_BAD_ARG;
}
con_cmd_rc_t ME_CMD_echo(console_ctx_t *ctx)
{
    if (ctx->argc <= 1)
    {
        ERROR_MISSING_ARG();
        return CON_RC_BAD_ARG;
    }
    for (int i = 1; i < ctx->argc; i++)
    {
        C_PRINTF("%s ", ctx->argv[i]);
    }
    C_PRINTF("\n");

    return CON_RC_DONE;
}
void secToUptime(uint8_t *buf, uint8_t size)
{

    uint32_t sec = GiveTimeSec();
    uint32_t days = sec / 86400;
    sec %= 86400;
    uint32_t hours = sec / 3600;
    sec %= 3600;
    uint32_t minutes = sec / 60;
    uint32_t seconds = sec % 60;
    tiny_sprintf(buf, size, "%d days %-2d hours %-2d minutes %-2d secondes", days, hours, minutes, seconds);
}
con_cmd_rc_t ME_CMD_uptime(console_ctx_t *ctx)
{
    typedef struct
    {
        ME_Timer t;
    } ctx_t;

    CMD_CTX_BUF(ctx_t, ctx_cmd);

    uint8_t buf[64];
    if (ctx->is_interactive)
    {
        if (ME_isTimedOut(&ctx_cmd->t))
        {
            VT100_Clear_Line(ctx);
            VT100_Move_CursorToCol(ctx, 0);
            secToUptime(buf, 64);
            goto out_interactive;
        }
        return CON_RC_INTERACTIVE;
    }
    if (ctx->argc > 1 && strcmp(ctx->argv[1], "-f") == 0)
    {
        secToUptime(buf, 64);
        ME_timerInit(&ctx_cmd->t, UPTIME_DELAY);
        goto out_interactive;
    }
    if (ctx->argc > 1)
    {
        ERROR_ARG(ctx->argv[1])
        return CON_RC_BAD_ARG;
    }
    secToUptime(buf, 64);
    C_PRINTF("Uptime: %s\n", buf);
    return CON_RC_DONE;
out_interactive:
    ME_timerInit(&ctx_cmd->t, UPTIME_DELAY);
    C_PRINTF("Uptime: %s", buf);
    return CON_RC_INTERACTIVE;
}
con_cmd_rc_t ME_CMD_reboot(console_ctx_t *ctx)
{
    NVIC_SystemReset();
    return CON_RC_DONE;
}
con_cmd_rc_t ME_CMD_clear(console_ctx_t *ctx)
{
    if (ctx->argc > 1)
    {
        ERROR_ARG(ctx->argv[1])
        return CON_RC_BAD_ARG;
    }
    VT100_Clear_Screen(ctx);
    VT100_Home(ctx);
    return CON_RC_DONE;
}
con_cmd_rc_t ME_CMD_ping(console_ctx_t *ctx)
{
    if (ctx->argc > 1)
    {
        ERROR_ARG(ctx->argv[1])
        return CON_RC_BAD_ARG;
    }
    C_PRINTF("pong\n");
    return CON_RC_DONE;
}
