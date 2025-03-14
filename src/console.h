#ifndef CONSOLE_H
#define CONSOLE_H
#include "sd.h"
#include "sd_uart.h"
#include "me_timer.h"
#define CON_MAX_BUFF_SIZE 64
#define CON_ARGV_MAX 8
#ifndef CON_CMD_CTX_SIZE
#define CON_CMD_CTX_SIZE 8
#endif
#define DEFINE_BUF_CONSOLE(_name) uint8_t _name##_console_buff[CON_MAX_BUFF_SIZE];
#define DEFINE_ESC_SEQ(_name) uint8_t _name##_seq_buff[10];
#define C_PRINTF(_format, ...) sd_Printf(ctx->sd, _format, ##__VA_ARGS__)

enum Escape_Char
{
    START = 2,
    CTRL_C = 3,
    INSERT = 4,
    END = 6,
    DELETE = 7,
    CTRL_LEFT = 16,
    CTRL_RIGHT,
    RIGHT,
    LEFT,
    UP,
    DOWN,
    ESCAPE = 27,
    BACKSPACE = 127,

};
typedef enum
{
    CON_RC_DONE = 0,
    CON_RC_BAD_ARG,
    CON_RC_INTERACTIVE,
} con_cmd_rc_t;

typedef struct
{
    me_sd_t *sd;
    uint8_t *buf;
    uint8_t index;
    bool is_escape;
    uint8_t current_size;
    uint8_t *pSeq;
    uint8_t index_seq;
    bool is_insert;
    ME_Timer *esc_timer;
    uint8_t argc;
    char *argv[CON_ARGV_MAX];
    uint8_t cmd_ctx[CON_CMD_CTX_SIZE];
    bool is_interactive;
    void *current_cmd;
} console_ctx_t;
typedef struct
{
    const char *name;
    const char *desc;
    const char *usage;
    con_cmd_rc_t (*fnCmd)(console_ctx_t *ctx);

} con_cmd_dsc_t;
typedef struct
{
    const char *pSeq;
    uint8_t len;
    uint8_t c;

} con_sp_char_t;
uint16_t Console_ParseInput(console_ctx_t *ctx, uint8_t c);
void ME_Console_Init(me_sd_t *sd, console_ctx_t *ctx, uint8_t *pBuf, uint8_t *pSeq);
void ME_Console_Poll(console_ctx_t *ctx);
#endif // CONSOLE_H