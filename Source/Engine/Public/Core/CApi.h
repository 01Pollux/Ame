#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

typedef struct Ame_UID_t
{
    uint32_t Data1;
    uint16_t Data2;
    uint16_t Data3;
    uint8_t  Data4[8];
} Ame_UID_t;

typedef struct Ame_StringView_t
{
    const char* Data;
    size_t      Size;
} Ame_StringView_t;
