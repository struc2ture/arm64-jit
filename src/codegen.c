#include "codegen.h"

#include "common/util.h"
#include "jit.h"
#include "parser.h"
#include "reg_pool.h"

void codegen_expr(CodeBuffer *cb, Ast *node, SymbolList *locals, RegPool *reg_pool, int target_reg)
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
            int right_reg = alloc_temp_reg(reg_pool);

            codegen_expr(cb, node->left, locals, reg_pool, left_reg);
            codegen_expr(cb, node->right, locals, reg_pool, right_reg);

            switch (node->kind)
            {
                case AST_ADD: emit_add(cb, target_reg, left_reg, right_reg); break;
                case AST_SUB: emit_sub(cb, target_reg, left_reg, right_reg); break;
                case AST_MUL: emit_mul(cb, target_reg, left_reg, right_reg); break;
                case AST_DIV: emit_div(cb, target_reg, left_reg, right_reg); break;
                default: break;
            }

            free_temp_reg(reg_pool, right_reg);
        } break;

        case AST_VAR:
        {
            int reg = get_reg_for_var(node->name, locals);
            if (reg != target_reg)
            {
                emit_mov_reg(cb, target_reg, reg);
            }
        } break;

        case AST_ASSIGN:
        {
            int reg = get_reg_for_var(node->name, locals);
            codegen_expr(cb, node->left, locals, reg_pool, target_reg);
            if (target_reg != reg)
            {
                emit_mov_reg(cb, reg, target_reg);
            }
        } break;

        case AST_NONE: case AST_COUNT: fatal("Codegen: invalid AST node");

        case AST_STMT_LIST:
        case AST_RETURN:
        case AST_EXPR_STMT: fatal("Not implemented");
    }
}

// ========================================

void emit_mov_imm(CodeBuffer *cb, int reg, u16 imm)
{
    uint32_t instr = 0xd2800000 | (imm << 5) | reg;
    cb_write_4bytes(cb, instr);
}

void emit_mov_reg(CodeBuffer *cb, int dst, int src)
{
    uint32_t instr = 0xaa0003e0 | (src << 16) | dst;
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

// ========================================

int get_reg_for_var(const char *name, const SymbolList *locals)
{
    int reg = find_local(locals, name);
    if (reg < 0) fatal("Codegen: unknown variable %s\n", name);
    return reg;
}
