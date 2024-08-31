#include <Rg/DependencyLevel.hpp>
#include <Rg/Context.hpp>
#include <Rg/Pass.hpp>

#include <DiligentCore/Graphics/GraphicsTools/interface/ScopedDebugGroup.hpp>

#include <Log/Wrapper.hpp>

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
        auto& resourceStorage = context.GetStorage();
        for (auto& pass : m_Passes)
        {
            switch (pass->GetQueueType())
            {
            case PassFlags::Graphics:
            case PassFlags::Compute:
            case PassFlags::Copy:
            {
#ifndef AME_DIST
                Dg::ScopedDebugGroup marker(deviceContext, pass->GetName().data(), pass->GetColorPtr());
#endif
                pass->DoExecute(resourceStorage, deviceContext);
                break;
            }

            default:
            {
                pass->DoExecute(resourceStorage, nullptr);
                break;
            }
            }
        };
    }

    //

    void DependencyLevel::UnlockStorage(Context& context) const
    {
        auto& resourceStorage = context.GetStorage();
        resourceStorage.Unlock();
    }
} // namespace Ame::Rg