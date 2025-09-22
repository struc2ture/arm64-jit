#pragma once

#include "common/types.h"

#define FIRST_TEMP_REG 10
#define MAX_TEMP_REGS 6

typedef struct
{
    int used[MAX_TEMP_REGS];
} RegPool;

int alloc_temp_reg(RegPool *reg_pool);
void free_temp_reg(RegPool *reg_pool, int reg);
