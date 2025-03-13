#include "sd.h"
#include "console.h"
#include <string.h>
#include <stdlib.h>
#include "me_timer.h"
#include "command.h"
#include "vt100.h"
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
extern const con_cmd_dsc_t con_cmds[];
void Console_CmdExec(console_ctx_t *ctx)
{
    char *cmd = ctx->argv[0];
    for (int i = 0; con_cmds[i].name != NULL; i++)
    {
        if (strcmp(con_cmds[i].name, cmd) == 0)
        {
            con_cmd_rc_t rc = con_cmds[i].fnCmd(ctx);
            switch (rc)
            {
            case CON_RC_INTERACTIVE:
                ctx->is_interactive = true;
                break;
            default:
                break;
            }
            return;
        }
    }
}

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
    ctx->is_interactive = false;
    ctx->fnInter = NULL;
    ME_timerDisable(ctx->esc_timer);
    ME_timerDisable(ctx->interaction_timer);
    VT100_Clear_Screen(ctx);
    C_PRINTF("> ");
}
void ME_Console_Poll(console_ctx_t *ctx)
{
    if (ctx->is_interactive)
    {
        if (ctx->fnInter != NULL)
        {
            ctx->fnInter(ctx);
        }
    }
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
        ctx->is_interactive = false;
        ctx->fnInter = NULL;
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
        if (!ctx->is_interactive)
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

            if (ctx->index < ctx->current_size)
                UpdateConsole(ctx);
            else
            {
                VT100_ShiftCursorLeft(ctx, 1);
                VT100_Clear_EndLine(ctx);
            }
        }
        break;
    }
}
