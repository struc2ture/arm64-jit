#include "basic_expr.h"

#include "common/types.h"

#include "jit.h"

void emit_mov_imm(CodeBuffer *cb, int reg, u16 imm)
{
    uint32_t instr = 0xd2800000 | (imm << 5) | reg;
    cb_write_4bytes(cb, instr);
}

void emit_add(CodeBuffer *cb, int dst, int src1, int src2)
{
    uint32_t instr = 0x8b000000 | (src2 << 16) | (src1 << 5) | dst;
    cb_write_4bytes(cb, instr);
}

void emit_sub(CodeBuffer *cb, int dst, int src1, int src2)
{
    uint32_t instr = 0xcb000000 | (src2 << 16) | (src1 << 5) | dst;
    cb_write_4bytes(cb, instr);
}

void emit_mul(CodeBuffer *cb, int dst, int src1, int src2)
{
    uint32_t instr = 0x9b007c00 | (src2 << 16) | (src1 << 5) | dst;
    cb_write_4bytes(cb, instr);
}

void emit_div(CodeBuffer *cb, int dst, int src1, int src2)
{
    uint32_t instr = 0x9ac00c00 | (src2 << 16) | (src1 << 5) | dst;
    cb_write_4bytes(cb, instr);
}

void emit_ret(CodeBuffer *cb)
{
    uint32_t instr = 0xd65f03c0;
    cb_write_4bytes(cb, instr);
}

void compile_expr(CodeBuffer *cb, Expr *e, int target)
{
    if (e->kind == EXPR_CONST)
    {
        emit_mov_imm(cb, target, e->value);
        return;
    }

    compile_expr(cb, e->left, target);
    compile_expr(cb, e->right, target + 1);

    switch (e->kind)
    {
        case EXPR_ADD: emit_add(cb, target, target, target + 1); break;
        case EXPR_SUB: emit_sub(cb, target, target, target + 1); break;
        case EXPR_MUL: emit_mul(cb, target, target, target + 1); break;
        case EXPR_DIV: emit_div(cb, target, target, target + 1); break;
        default: break;
    }
}