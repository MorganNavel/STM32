#include "console.h"
#include "vt100.h"

void VT100_Home(console_ctx_t *ctx) { C_PRINTF("\x1B[H"); }
void VT100_Move_CursorToCol(console_ctx_t *ctx, uint8_t cols) { C_PRINTF("\x1B[%dG", cols); }
void VT100_Clear_Line(console_ctx_t *ctx) { C_PRINTF("\x1B[2K"); }
void VT100_Clear_Screen(console_ctx_t *ctx) { C_PRINTF("\x1B[H\x1B[2J"); }
void VT100_Clear_EndLine(console_ctx_t *ctx) { C_PRINTF("\x1B[0K"); }
void VT100_ShiftCursorLeft(console_ctx_t *ctx, uint8_t nbShift) { C_PRINTF("\x1B[%dD", nbShift); }
void VT100_ShiftCursorRight(console_ctx_t *ctx, uint8_t nbShift) { C_PRINTF("\x1B[%dC", nbShift); }
