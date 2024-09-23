#pragma once

typedef struct Ame_AbstractStorage_t       Ame_AbstractStorage_t;
typedef struct Ame_AbstractStorage_Key_t   Ame_AbstractStorage_Key_t;
typedef struct Ame_AbstractStorage_Value_t Ame_AbstractStorage_Value_t;

// Storage interface for key-value pairs
// Designed to be used mostly for scripting languages where we must persist native pointer aswell as managed object
// This is a simple key-value storage that can be used to store any type of data

// clang-format off
extern "C"
{
	AME_ENGINE_C(Ame_AbstractStorage_t*, Ame_AbstractStorage_Create());
	AME_ENGINE_C(void, Ame_AbstractStorage_Release(Ame_AbstractStorage_t* storage));

	AME_ENGINE_C(void, Ame_AbstractStorage_Set(Ame_AbstractStorage_t* storage, const Ame_AbstractStorage_Key_t* key, const Ame_AbstractStorage_Value_t* value));
	AME_ENGINE_C(const Ame_AbstractStorage_Value_t*, Ame_AbstractStorage_Get(Ame_AbstractStorage_t* storage, const Ame_AbstractStorage_Key_t* key));
	AME_ENGINE_C(const Ame_AbstractStorage_Value_t*, Ame_AbstractStorage_Remove(Ame_AbstractStorage_t* storage, const Ame_AbstractStorage_Key_t* key));
}
// clang-format on