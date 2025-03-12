#include "sd.h"
#include "console.h"
#include <string.h>
#include <stdlib.h>
#include "me_timer.h"

#define C_PRINTF(_format, ...) sd_Printf(ctx->sd, _format, ##__VA_ARGS__)

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
#define DEFINE_SEQUENCES(_name) const con_sp_char_t _name##_sequences[] = {
#define SEQUENCE(_pSeq, _c)       \
    {                             \
        .pSeq = _pSeq,            \
        .len = sizeof(_pSeq) - 1, \
        .c = _c,                  \
    },
#define END_SEQUENCES() \
    {                   \
    }                   \
    }                   \
    ;

DEFINE_SEQUENCES(con)
SEQUENCE("\x1B[C", RIGHT)
SEQUENCE("\x1B[D", LEFT)
SEQUENCE("\x1B[A", CTRL_C)
SEQUENCE("\x1B[1~", START)
SEQUENCE("\x1B[2~", INSERT)
SEQUENCE("\x1B[3~", DELETE)
SEQUENCE("\x1B[4~", END)
SEQUENCE("\x1BOD", CTRL_LEFT)
SEQUENCE("\x1BOC", CTRL_RIGHT)
END_SEQUENCES()
con_cmd_rc_t help(console_ctx_t *ctx);
con_cmd_rc_t echo(console_ctx_t *ctx);
DEFINE_CMDS(con)
CMD("help", "Display general help or help for a specific command", "help [command]", help)
CMD("echo", "Echo a message", "echo <message>", echo)
END_CMDS()

con_cmd_rc_t help(console_ctx_t *ctx)
{
    if (ctx->argc == 1)
    {
        C_PRINTF("Available commands:\n");
        for (int i = 0; con_cmds[i].name != NULL; i++)
        {
            C_PRINTF("%-10s - %s\n", con_cmds[i].name, con_cmds[i].desc);
        }
        C_PRINTF("\nUse '%s' for more details.\n", con_cmds[0].usage);
        return CON_RC_DONE;
    }

    for (int i = 0; con_cmds[i].name != NULL; i++)
    {
        if (strcmp(con_cmds[i].name, ctx->argv[1]) == 0)
        {
            C_PRINTF("%s - %s\n", con_cmds[i].name, con_cmds[i].desc);
            C_PRINTF("- Usage: %s\n", con_cmds[i].usage);
            return CON_RC_DONE;
        }
    }

    C_PRINTF("Error: Unknown command '%s'. Use 'help' to list available commands.\n", ctx->argv[1]);
    return CON_RC_BAD_ARG;
}

con_cmd_rc_t echo(console_ctx_t *ctx)
{

    if (ctx->argc <= 1)
    {
        C_PRINTF("Error: Missing argument. Usage: %s\n", con_cmds[1].usage);
        return CON_RC_BAD_ARG;
    }
    for (int i = 1; i < ctx->argc; i++)
    {
        C_PRINTF("%s ", ctx->argv[i]);
    }
    C_PRINTF("\n");

    return CON_RC_DONE;
}
void Console_CmdExec(console_ctx_t *ctx)
{
    char *cmd = ctx->argv[0];
    for (int i = 0; con_cmds[i].name != NULL; i++)
    {
        if (strcmp(con_cmds[i].name, cmd) == 0)
        {
            con_cmds[i].fnCmd(ctx);
            return;
        }
    }
}
void VT100_Home(console_ctx_t *ctx) { C_PRINTF("\x1B[H"); }

void VT100_Move_CursorToCol(console_ctx_t *ctx, uint8_t cols) { C_PRINTF("\x1B[%dG", cols); }

void VT100_Clear_Line(console_ctx_t *ctx) { C_PRINTF("\x1B[2K"); }

void VT100_Clear_Screen(console_ctx_t *ctx)
{
    C_PRINTF("\x1B[H\x1B[2J");
}

void VT100_Clear_EndLine(console_ctx_t *ctx) { C_PRINTF("\x1B[0K"); }
void VT100_ShiftCursorLeft(console_ctx_t *ctx, uint8_t nbShift) { C_PRINTF("\x1B[%dD", nbShift); }
void VT100_ShiftCursorRight(console_ctx_t *ctx, uint8_t nbShift) { C_PRINTF("\x1B[%dC", nbShift); }

void reset_escape(console_ctx_t *ctx)
{
    ctx->is_escape = false;
    ctx->index_seq = 0;
    ME_timerDisable(ctx->esc_timer);
}
uint8_t manageSequence(console_ctx_t *ctx)
{
    int len = sizeof(con_sequences) / sizeof(con_sp_char_t);
    for (int i = 0; i < len; i++)
    {
        if (con_sequences[i].len != ctx->index_seq)
            continue;
        if (memcmp(con_sequences[i].pSeq, ctx->pSeq, con_sequences[i].len) == 0)
        {
            reset_escape(ctx);
            return con_sequences[i].c;
        }
    }
    return 0;
}
void parseCmdArgs(console_ctx_t *ctx, uint8_t *str)
{
    uint8_t *p = str;
    while (*p == ' ')
        p++;
    ctx->argc = 0;
    while (*p)
    {
        if (*p == ' ')
        {
            *p = '\0';
            ctx->argv[ctx->argc] = (char *)str;
            ctx->argc++;
            str = p + 1;
        }
        while (*p == ' ')
            p++;
        p++;
    }
    ctx->argv[ctx->argc] = (char *)str;
    ctx->argc++;
}

uint16_t Console_ParseInput(console_ctx_t *ctx, uint8_t c)
{
    if (ctx->is_escape)
    {
        if (ME_isTimedOut(ctx->esc_timer) || ctx->index_seq >= sizeof(ctx->pSeq))
            goto reset;

        if (c == 0)
            return 0;

        ctx->pSeq[ctx->index_seq] = c;
        ctx->index_seq++;
        return manageSequence(ctx);
    }
    if (c == 0)
        return 0;
    if (c == ESCAPE)
        goto init;
    if (c == '\xC3')
        goto init;
    return c;
init: // Label to jump to
    ctx->is_escape = true;
    ctx->pSeq[0] = c;
    ctx->index_seq = 1;
    ME_timerInit(ctx->esc_timer, 100);
    return 0;
reset: // Label to jump to
    c = ctx->pSeq[0];
    reset_escape(ctx);
    return c;
}

void Console_ShiftBuffer(console_ctx_t *ctx, int nbShift)
{
    if (nbShift == 0)
        return;
    if (nbShift > 0 && ctx->current_size + nbShift >= CON_MAX_BUFF_SIZE - 1)
        return;
    if (nbShift < 0 && ctx->index + nbShift < 0)
        return;

    memmove(&ctx->buf[ctx->index + nbShift],
            &ctx->buf[ctx->index],
            ctx->current_size - ctx->index);
    ctx->current_size += nbShift;
    ctx->index += nbShift;
    ctx->buf[ctx->current_size] = '\0';
}
void UpdateConsole(console_ctx_t *ctx)
{
    VT100_Clear_Line(ctx);
    VT100_Move_CursorToCol(ctx, 0);
    C_PRINTF("> %s", ctx->buf);
    VT100_Move_CursorToCol(ctx, ctx->index + 3);
}

void Console_CasePrintable(console_ctx_t *ctx, uint8_t c)
{
    if (ctx->current_size >= CON_MAX_BUFF_SIZE - 1)
        return;
    if (ctx->is_insert && ctx->index < ctx->current_size)
        goto insert;

    memmove(&ctx->buf[ctx->index + 1], &ctx->buf[ctx->index], ctx->current_size - ctx->index);

    ctx->current_size++;
insert:
    ctx->buf[ctx->index] = c;
    ctx->index++;
    ctx->buf[ctx->current_size] = '\0';
}
void ME_Console_Init(me_sd_t *sd, console_ctx_t *ctx, uint8_t *pBuf, uint8_t *pSeq)
{
    ctx->buf = pBuf;
    ctx->index = 0;
    ctx->is_escape = false;
    ctx->current_size = 0;
    ctx->sd = sd;
    ctx->pSeq = pSeq;
    ctx->is_insert = false;
    ctx->argc = 0;
    ctx->argv[0] = NULL;
    VT100_Clear_Screen(ctx);
    C_PRINTF("> ");
}
void ME_Console_Poll(console_ctx_t *ctx)
{
    uint8_t c;

    int r = sd_Read(ctx->sd, &c, 1);
    c = r == 0 ? 0 : c;
    c = Console_ParseInput(ctx, c);

    if (c == 0)
        return;
    if (c >= 32 && c < 127)
    {
        Console_CasePrintable(ctx, c);
        if (ctx->index < ctx->current_size)
            UpdateConsole(ctx);
        else
            C_PRINTF("%c", c);
        return;
    }
    switch (c)
    {
    case '\n':
        break;
    case DELETE:
        if (ctx->index < ctx->current_size)
        {
            memmove(&ctx->buf[ctx->index], &ctx->buf[ctx->index + 1], ctx->current_size - ctx->index);
            ctx->current_size--;
            ctx->buf[ctx->current_size] = '\0';
            UpdateConsole(ctx);
        }
        break;
    case CTRL_RIGHT: // Ctrl + Right
        while (ctx->index < ctx->current_size && ctx->buf[ctx->index] == ' ')
            ctx->index++;
        while (ctx->index < ctx->current_size && ctx->buf[ctx->index] != ' ')
            ctx->index++;
        VT100_Move_CursorToCol(ctx, ctx->index + 3);
        break;
    case RIGHT:
        if (ctx->index >= ctx->current_size || ctx->index >= CON_MAX_BUFF_SIZE - 1)
            break;
        VT100_ShiftCursorRight(ctx, 1);
        ctx->index++;
        break;
    case CTRL_LEFT: // Ctrl + Left
        while (ctx->index > 0 && ctx->buf[ctx->index - 1] == ' ')
            ctx->index--;
        while (ctx->index > 0 && ctx->buf[ctx->index - 1] != ' ')
            ctx->index--;
        VT100_Move_CursorToCol(ctx, ctx->index + 3);
        break;
    case LEFT: // Left
        if (ctx->index == 0)
            break;
        VT100_ShiftCursorLeft(ctx, 1);
        ctx->index--;
        break;
    case START:
        ctx->index = 0;
        VT100_Move_CursorToCol(ctx, 3);
        break;
    case END:
        ctx->index = ctx->current_size;
        VT100_Move_CursorToCol(ctx, ctx->current_size + 3);
        break;
    case ESCAPE:
    case CTRL_C:
        C_PRINTF("\n>");
        ctx->index = 0;
        ctx->current_size = 0;
        ctx->buf[0] = '\0';
        break;
    case INSERT:
        ctx->is_insert = !ctx->is_insert;
        break;
    case '\r': // Enter
        ctx->buf[ctx->current_size] = '\0';
        parseCmdArgs(ctx, ctx->buf);
        C_PRINTF("\n");
        Console_CmdExec(ctx);
        C_PRINTF("> ");
        ctx->index = 0;
        ctx->current_size = 0;
        ctx->buf[0] = '\0';
        break;
    case '\b':
    case BACKSPACE:
        if (ctx->index > 0)
        {
            Console_ShiftBuffer(ctx, -1);
            VT100_ShiftCursorLeft(ctx, 1);
            if (ctx->index < ctx->current_size)
                UpdateConsole(ctx);
            else
                VT100_Clear_EndLine(ctx);
        }
        break;
    }
}
