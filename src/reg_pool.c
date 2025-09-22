#include "reg_pool.h"

#include "common/types.h"
#include "common/util.h"

int alloc_temp_reg(RegPool *reg_pool)
{
    for (int i = 0; i < MAX_TEMP_REGS; i++)
    {
        if (!reg_pool->used[i])
        {
            reg_pool->used[i] = true;
            return FIRST_TEMP_REG + i;
        }
    }
    fatal("Ran out of temp registers");
}

void free_temp_reg(RegPool *reg_pool, int reg)
{
    int idx = reg - FIRST_TEMP_REG;
    if (idx < 0 || idx >= MAX_TEMP_REGS) return;
    reg_pool->used[reg] = false;
}
