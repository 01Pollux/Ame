#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct Ame_StringView_t
{
    const char* Data;
    size_t      Size;
} Ame_StringView_t;
