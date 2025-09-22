#pragma once

#include "jit.h"
#include "parser.h"
#include "reg_pool.h"

typedef u64 (*expr_fn_t)(void);

void codegen_expr(CodeBuffer *cb, Ast *node, SymbolList *locals, RegPool *reg_pool, int target_reg);

// ========================================

void emit_mov_imm(CodeBuffer *cb, int reg, u16 imm);
void emit_mov_reg(CodeBuffer *cb, int dst, int src);
void emit_add(CodeBuffer *cb, int dst, int src1, int src2);
void emit_sub(CodeBuffer *cb, int dst, int src1, int src2);
void emit_mul(CodeBuffer *cb, int dst, int src1, int src2);
void emit_div(CodeBuffer *cb, int dst, int src1, int src2);
void emit_ret(CodeBuffer *cb);

// ========================================

int get_reg_for_var(const char *name, const SymbolList *locals);
