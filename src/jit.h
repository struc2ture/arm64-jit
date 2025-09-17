#pragma once

#include <stdlib.h>

#include "common/types.h"

typedef struct
{
    u8 *base;
    size_t capacity;
    size_t offset;

} CodeBuffer;

CodeBuffer cb_create(size_t size);
void cb_destroy(CodeBuffer *code_buffer);
void cb_copy_code(CodeBuffer *code_buffer, u32 *code, size_t code_size);
void cb_write_4bytes(CodeBuffer *cb, u32 data);
void *cb_get_proc(CodeBuffer *cb);
