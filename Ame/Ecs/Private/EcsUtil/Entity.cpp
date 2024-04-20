#include <EcsUtil/Entity.hpp>

namespace Ame::EcsUtil
{
    StringU8 GetUniqueEntityName(
        const flecs::world&  FlecsWorld,
        const char*          Name,
        const flecs::entity& FlecsParent)
    {
        bool IsValidName = Name && *Name && Name[0] != '\0';

        StringU8 NewName{ IsValidName ? Name : "Entity" };
        StringU8 NewNameTmp = NewName;

        size_t Idx = 0;
        while (FlecsParent.lookup(NewName.c_str()))
        {
            NewName = StringU8::formatted("{}_{}", NewNameTmp, ++Idx);
        }

        return NewName;
    }
} // namespace Ame::EcsUtil