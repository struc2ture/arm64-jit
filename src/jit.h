#pragma once

#include <stdlib.h>

#include "common/types.h"

typedef struct
{
    u8 *base;
    size_t capacity;
    size_t offset;

} CodeBuffer;

CodeBuffer alloc_jit_mem(size_t size);
void free_jit_mem(CodeBuffer *code_buffer);
void copy_code(CodeBuffer *code_buffer, u32 *code, size_t code_size);
void cb_write_4bytes(CodeBuffer *cb, u32 data);
void *cb_get_proc(CodeBuffer *cb);
