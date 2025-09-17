#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "common/types.h"
#include "common/util.h"

#include "jit.h"
#include "basic_expr.h"

typedef u64 (*foo42_t)(void);

void foo42()
{
    CodeBuffer code_buffer = alloc_jit_mem(4096);

    emit_mov_imm(&code_buffer, 0, 42); // mov x0, #42
    emit_ret(&code_buffer);

    foo42_t func = (foo42_t)cb_get_proc(&code_buffer);

    u64 result = func();
    log("Result: %llu", result);

    free_jit_mem(&code_buffer);
}

typedef u64 (*twice_t)(u64);

void twice()
{
    CodeBuffer code_buffer = alloc_jit_mem(4096);

    emit_add(&code_buffer, 0, 0, 0); // add x0, x0, x0
    emit_ret(&code_buffer);

    twice_t func = (twice_t)cb_get_proc(&code_buffer);

    u64 result = func(192);
    log("Result: %llu", result);

    free_jit_mem(&code_buffer);
}

typedef u64 (*expr_fn_t)(void);

void simple_expr()
{
    CodeBuffer code_buffer = alloc_jit_mem(4096);

    Expr expr =
    {
        .kind = EXPR_ADD,
        .left = &(Expr) {
            .kind = EXPR_CONST,
            .value = 1
        },
        .right = &(Expr) {
            .kind = EXPR_MUL,
            .left = &(Expr) {
                .kind = EXPR_CONST,
                .value = 2
            },
            .right = &(Expr) {
                .kind = EXPR_CONST,
                .value = 3
            }
        }
    };

    compile_expr(&code_buffer, &expr, 0);
    emit_ret(&code_buffer);

    expr_fn_t func = (expr_fn_t)cb_get_proc(&code_buffer);

    u64 result = func();
    log("Result: %llu", result);

    free_jit_mem(&code_buffer);
}

// =============================================================

int main(void)
{
    log("42:");
    foo42();
    log("");

    log("TWICE:");
    twice();
    log("");

    log("SIMPLE EXPR:");
    simple_expr();
    log("");

    return 0;
}
