#pragma once

#include <stdlib.h>

#define fatal(FMT, ...) do { \
    fprintf(stderr, "[FATAL: %s:%d:%s]: " FMT "\n", __FILE__, __LINE__, __func__, ##__VA_ARGS__); \
    abort(); \
} while (0)
