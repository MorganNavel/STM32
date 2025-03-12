#ifndef VT100_H
#define VT100_H

void VT100_Home(console_ctx_t *ctx);
void VT100_Move_CursorToCol(console_ctx_t *ctx, uint8_t cols);
void VT100_Clear_Line(console_ctx_t *ctx);
void VT100_Clear_Screen(console_ctx_t *ctx);
void VT100_Clear_EndLine(console_ctx_t *ctx);
void VT100_ShiftCursorLeft(console_ctx_t *ctx, uint8_t nbShift);
void VT100_ShiftCursorRight(console_ctx_t *ctx, uint8_t nbShift);

#endif // VT100_H