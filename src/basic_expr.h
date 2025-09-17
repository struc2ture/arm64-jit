#pragma once

#include "common/types.h"

#include "jit.h"

typedef enum
{
    EXPR_NONE,
    EXPR_CONST,
    EXPR_ADD,
    EXPR_SUB,
    EXPR_MUL,
    EXPR_DIV,
    EXPR_COUNT

} ExprKind;

typedef struct Expr
{
    ExprKind kind;
    int value;
    struct Expr *left;
    struct Expr *right;

} Expr;

void emit_mov_imm(CodeBuffer *cb, int reg, u16 imm);
void emit_add(CodeBuffer *cb, int dst, int src1, int src2);
void emit_sub(CodeBuffer *cb, int dst, int src1, int src2);
void emit_mul(CodeBuffer *cb, int dst, int src1, int src2);
void emit_div(CodeBuffer *cb, int dst, int src1, int src2);
void emit_ret(CodeBuffer *cb);
void compile_expr(CodeBuffer *cb, Expr *e, int target);
