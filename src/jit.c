#include "jit.h"

#include <libkern/OSCacheControl.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "common/types.h"
#include "common/util.h"

CodeBuffer cb_create(size_t size)
{
    void *mem = mmap(NULL, size,
                     PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANON | MAP_JIT,
                     -1, 0);
    if (mem == MAP_FAILED)
    {
        fatal("Failed mmap");
    }

    return (CodeBuffer){
        .base = mem,
        .capacity = size,
        .offset = 0
    };
}

void cb_destroy(CodeBuffer *code_buffer)
{
    munmap(code_buffer->base, code_buffer->capacity);
    *code_buffer = (CodeBuffer) {};
}

void cb_copy_code(CodeBuffer *code_buffer, u32 *code, size_t code_size)
{
    pthread_jit_write_protect_np(0);
    memcpy(code_buffer->base + code_buffer->offset, code, code_size);
    code_buffer->offset += code_size;
    pthread_jit_write_protect_np(1);
}

void cb_write_4bytes(CodeBuffer *cb, u32 data)
{
    pthread_jit_write_protect_np(0);
    *(u32 *)(cb->base + cb->offset) = data;
    pthread_jit_write_protect_np(1);
    cb->offset += sizeof(data);
}

void *cb_get_proc(CodeBuffer *cb)
{
    sys_icache_invalidate((void *)cb->base, cb->offset);
    return cb->base;
}
