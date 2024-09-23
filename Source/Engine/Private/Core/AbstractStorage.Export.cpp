#include <map>
#include <bit>
#include <Core/AbstractStorage.Export.h>
#include <Log/Logger.hpp>

using MapType = std::map<const Ame_AbstractStorage_Key_t*, const Ame_AbstractStorage_Value_t*>;

Ame_AbstractStorage_t* Ame_AbstractStorage_Create()
{
    return std::bit_cast<Ame_AbstractStorage_t*>(new MapType());
}

void Ame_AbstractStorage_Release(Ame_AbstractStorage_t* storage)
{
    delete std::bit_cast<MapType*>(storage);
}

void Ame_AbstractStorage_Set(Ame_AbstractStorage_t* storage, const Ame_AbstractStorage_Key_t* key,
                             const Ame_AbstractStorage_Value_t* value)
{
    auto* map = std::bit_cast<MapType*>(storage);
    AME_LOG_ASSERT(map->emplace(key, value).second, "Key already exists in storage");
}

const Ame_AbstractStorage_Value_t* Ame_AbstractStorage_Get(Ame_AbstractStorage_t*           storage,
                                                           const Ame_AbstractStorage_Key_t* key)
{
    auto* map = std::bit_cast<MapType*>(storage);
    auto  it  = map->find(key);
    if (it != map->end())
    {
        return it->second;
    }
    return nullptr;
}

const Ame_AbstractStorage_Value_t* Ame_AbstractStorage_Remove(Ame_AbstractStorage_t*           storage,
                                                              const Ame_AbstractStorage_Key_t* key)
{
    auto* map  = std::bit_cast<MapType*>(storage);
    auto  iter = map->find(key);

    const Ame_AbstractStorage_Value_t* value = nullptr;
    if (iter != map->end())
    {
        value = iter->second;
        map->erase(iter);
    }
    return value;
}