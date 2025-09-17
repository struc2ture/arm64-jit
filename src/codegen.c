#include "codegen.h"

#include "common/util.h"
#include "jit.h"
#include "parser.h"

void codegen_expr(CodeBuffer *cb, Ast *node, int target_reg)
{
    switch(node->kind)
    {
        case AST_INT:
            emit_mov_imm(cb, target_reg, node->value);
            break;

        case AST_ADD:
        case AST_SUB:
        case AST_MUL:
        case AST_DIV:
        {
            int left_reg = target_reg;
            int right_reg = target_reg + 1;

            codegen_expr(cb, node->left, left_reg);
            codegen_expr(cb, node->right, right_reg);

            switch (node->kind)
            {
                case AST_ADD: emit_add(cb, target_reg, left_reg, right_reg); break;
                case AST_SUB: emit_sub(cb, target_reg, left_reg, right_reg); break;
                case AST_MUL: emit_mul(cb, target_reg, left_reg, right_reg); break;
                case AST_DIV: emit_div(cb, target_reg, left_reg, right_reg); break;
                default: break;
            }
        } break;

        case AST_NONE: case AST_COUNT: fatal("Invalid AST node");
    }
}

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
