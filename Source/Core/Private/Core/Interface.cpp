#include <Core/Interface.hpp>
#include <potto/module/pottodef.hpp>

#include <boost/dll.hpp>
#include <Log/Wrapper.hpp>

namespace Potto
{
    static constexpr const char c_ModuleFile[] = "module.aml";

    void Initialize()
    {
        auto libDir     = boost::dll::program_location().parent_path();
        auto modulePath = libDir / c_ModuleFile;

        Ame::Log::Engine().Validate(Initialize(modulePath.string(), libDir.string()) == POTTO_E_OK, "Failed to initialize Engine");
    }
} // namespace Potto