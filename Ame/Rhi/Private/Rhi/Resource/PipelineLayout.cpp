#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/Resource/PipelineLayout.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    PipelineLayout::PipelineLayout(
        Device&              RhiDevice,
        nri::PipelineLayout& Layout,
        size_t               Hash) :
        m_RhiDevice(RhiDevice),
        m_Layout(Layout),
        m_Hash(Hash)
    {
    }

    PipelineLayout::~PipelineLayout()
    {
        m_RhiDevice.Release(Unwrap());
    }

    void PipelineLayout::SetName(
        const char* Name) const
    {
        m_RhiDevice.SetName(Unwrap(), Name);
    }

    nri::PipelineLayout& PipelineLayout::Unwrap() const
    {
        return m_Layout;
    }

    size_t PipelineLayout::GetHash() const
    {
        return m_Hash;
    }

    //

    Co::result<Ptr<PipelineLayout>> Device::CreatePipelineLayout(
        Co::executor_tag,
        Co::executor&             Executor,
        const PipelineLayoutDesc& Desc)
    {
        co_return CreatePipelineLayout(Desc);
    }

    Ptr<PipelineLayout> Device::CreatePipelineLayout(
        const PipelineLayoutDesc& Desc)
    {
        return GetImpl().m_PipelineLayoutCache.Load(
            Desc,
            [this](size_t Hash, const PipelineLayoutDesc& Desc)
            {
                nri::PipelineLayout* NriLayout = nullptr;

                auto& Nri     = m_Impl->GetNRI();
                auto& NriCore = *Nri.GetCoreInterface();

                ThrowIfFailed(NriCore.CreatePipelineLayout(
                                  m_Impl->GetDevice(), Desc, NriLayout),
                              "Failed to create pipeline layout");

                return std::make_shared<PipelineLayout>(*this, *NriLayout, Hash);
            });
    }

    //

    void Device::SetName(
        nri::PipelineLayout& Layout,
        const char*          Name) const
    {
        auto& Nri     = m_Impl->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.SetPipelineLayoutDebugName(Layout, Name);
    }

    void Device::Release(
        nri::PipelineLayout& Layout)
    {
        auto& Nri     = m_Impl->GetNRI();
        auto& NriCore = *Nri.GetCoreInterface();

        NriCore.DestroyPipelineLayout(Layout);
    }

    void DeviceImpl::DeferRelease(
        nri::Layout& Layout)
    {
        //m_FrameManager.DeferRelease(Layout);
    }
} // namespace Ame::Rhi