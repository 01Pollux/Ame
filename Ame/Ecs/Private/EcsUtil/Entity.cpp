#include <EcsUtil/Entity.hpp>

namespace Ame::EcsUtil
{
    String GetUniqueEntityName(
        const flecs::world&  world,
        const char*          name,
        const flecs::entity& parent)
    {
        bool isValidName = name && *name && name[0] != '\0';

        String newName{ isValidName ? name : "Entity" };
        String newNameTmp = newName;

        size_t Idx = 0;
        if (parent)
        {
            while (parent.lookup(newName.c_str()))
            {
                newName = std::format("{}_{}", newNameTmp, ++Idx);
            }
        }
        else
        {
            while (world.lookup(newName.c_str()))
            {
                newName = std::format("{}_{}", newNameTmp, ++Idx);
            }
        }

        return newName;
    }
} // namespace Ame::EcsUtil