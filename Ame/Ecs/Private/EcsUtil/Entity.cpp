#include <EcsUtil/Entity.hpp>

namespace Ame::EcsUtil
{
    String GetUniqueEntityName(
        const flecs::world&  FlecsWorld,
        const char*          Name,
        const flecs::entity& FlecsParent)
    {
        bool IsValidName = Name && *Name && Name[0] != '\0';

        String NewName{ IsValidName ? Name : "Entity" };
        String NewNameTmp = NewName;

        size_t Idx = 0;
        if (FlecsParent)
        {
            while (FlecsParent.lookup(NewName.c_str()))
            {
                NewName = std::format("{}_{}", NewNameTmp, ++Idx);
            }
        }
        else
        {
            while (FlecsWorld.lookup(NewName.c_str()))
            {
                NewName = std::format("{}_{}", NewNameTmp, ++Idx);
            }
        }

        return NewName;
    }
} // namespace Ame::EcsUtil