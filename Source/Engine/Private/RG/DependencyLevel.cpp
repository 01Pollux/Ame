#include <Rg/DependencyLevel.hpp>
#include <Rg/Context.hpp>
#include <Rg/Pass.hpp>

#include <DiligentCore/Graphics/GraphicsTools/interface/ScopedDebugGroup.hpp>

namespace Ame::Rg
{
    void DependencyLevel::AddPass(Pass* pass)
    {
        m_Passes.push_back(pass);
    }

    //

    void DependencyLevel::Execute(Context& context, Dg::IDeviceContext* deviceContext) const
    {
        LockStorage(context);
        ExecutePasses(context, deviceContext);
        UnlockStorage(context);
    }

    //

    void DependencyLevel::LockStorage(Context& context) const
    {
        auto& resourceStorage = context.GetStorage();
        resourceStorage.Lock();
    }

    //

    void DependencyLevel::ExecutePasses(Context& context, Dg::IDeviceContext* deviceContext) const
    {
        using namespace EnumBitOperators;

        auto& resourceStorage = context.GetStorage();
        for (auto& pass : m_Passes)
        {
#ifndef AME_DIST
            if ((pass->GetFlags() & PassFlags::NoMarker) == PassFlags::NoMarker)
            {
                pass->DoExecute(resourceStorage, deviceContext);
                continue;
            }
            Dg::ScopedDebugGroup marker(deviceContext, pass->GetName().data(), pass->GetColorPtr());
            pass->DoExecute(resourceStorage, deviceContext);
#else
            pass->DoExecute(resourceStorage, deviceContext);
#endif
        }
    }

    //

    void DependencyLevel::UnlockStorage(Context& context) const
    {
        auto& resourceStorage = context.GetStorage();
        resourceStorage.Unlock();
    }
} // namespace Ame::Rg