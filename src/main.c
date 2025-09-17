#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>
#include <string.h>
#include <pthread.h>

#include "util.h"

// ===========================================================

typedef struct
{
    uint8_t *base;
    size_t capacity;
    size_t offset;

} CodeBuffer;

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

typedef struct Expr {
    ExprKind kind;
    int value;
    struct Expr *left;
    struct Expr *right;
} Expr;

// ===========================================================

CodeBuffer alloc_jit_mem(size_t size)
{
    void *mem = mmap(NULL, size,
                     PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANON | MAP_JIT,
                     -1, 0);
    if (mem == MAP_FAILED)
    {
        fatal("Failed mmap");
    }

    return (CodeBuffer) {
        .base = mem,
        .capacity = size,
        .offset = 0
    };
}

void free_jit_mem(CodeBuffer *code_buffer)
{
    munmap(code_buffer->base, code_buffer->capacity);
    *code_buffer = (CodeBuffer) {};
}

void copy_code(CodeBuffer *code_buffer, uint32_t *code, size_t code_size)
{
    pthread_jit_write_protect_np(0);
    memcpy(code_buffer->base + code_buffer->offset, code, code_size);
    code_buffer->offset += code_size;
    pthread_jit_write_protect_np(1);
}

// =============================================================

void cb_write_4bytes(CodeBuffer *cb, uint32_t data)
{
    pthread_jit_write_protect_np(0);
    *(uint32_t *)(cb->base + cb->offset) = data;
    pthread_jit_write_protect_np(1);
    cb->offset += sizeof(data);
}

void emit_mov_imm(CodeBuffer *cb, int reg, uint16_t imm)
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

// =============================================================

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

// =============================================================

typedef uint64_t (*foo42_t)(void);

void foo42()
{
    CodeBuffer code_buffer = alloc_jit_mem(4096);

    emit_mov_imm(&code_buffer, 0, 42); // mov x0, #42
    emit_ret(&code_buffer);

    foo42_t func = (foo42_t)code_buffer.base;

    uint64_t result = func();
    printf("Result: %llu\n", result);

    free_jit_mem(&code_buffer);
}

typedef uint64_t (*twice_t)(uint64_t);

void twice()
{
    CodeBuffer code_buffer = alloc_jit_mem(4096);

    emit_add(&code_buffer, 0, 0, 0); // add x0, x0, x0
    emit_ret(&code_buffer);

    twice_t func = (twice_t)code_buffer.base;

    uint64_t result = func(192);
    printf("Result: %llu\n", result);

    free_jit_mem(&code_buffer);
}

typedef uint64_t (*expr_fn_t)(void);

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

    expr_fn_t func = (expr_fn_t)code_buffer.base;

    uint64_t result = func();
    printf("Result: %llu\n", result);

    free_jit_mem(&code_buffer);
}

// =============================================================

int main(void)
{
    printf("42:\n");
    foo42();
    printf("\n");

    printf("TWICE:\n");
    twice();
    printf("\n");

    printf("SIMPLE EXPR:\n");
    simple_expr();
    printf("\n");

    return 0;
}
