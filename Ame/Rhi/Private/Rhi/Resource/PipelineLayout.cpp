#include <Rhi/Device/DeviceImpl.hpp>

#include <Rhi/Resource/PipelineLayout.hpp>

#include <Rhi/NriError.hpp>

namespace Ame::Rhi
{
    PipelineLayout::PipelineLayout(
        Device&              rhiDevice,
        nri::PipelineLayout& nriLayout,
        size_t               hash) :
        m_RhiDevice(rhiDevice),
        m_Layout(nriLayout),
        m_Hash(hash)
    {
    }

    PipelineLayout::~PipelineLayout()
    {
        m_RhiDevice.Release(Unwrap());
    }

    void PipelineLayout::SetName(
        const char* name) const
    {
        m_RhiDevice.SetName(Unwrap(), name);
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
        Co::executor&             executor,
        const PipelineLayoutDesc& desc)
    {
        co_return CreatePipelineLayout(desc);
    }

    Ptr<PipelineLayout> Device::CreatePipelineLayout(
        const PipelineLayoutDesc& desc)
    {
        return GetImpl().m_PipelineLayoutCache.Load(
            desc,
            [this](size_t hash, const PipelineLayoutDesc& desc)
            {
                auto& nriUtils = m_Impl->GetNRI();
                auto& nriCore  = *nriUtils.GetCoreInterface();

                nri::PipelineLayout* nriLayout = nullptr;
                ThrowIfFailed(nriCore.CreatePipelineLayout(
                                  m_Impl->GetDevice(), desc, nriLayout),
                              "Failed to create pipeline layout");

                return std::make_shared<PipelineLayout>(*this, *nriLayout, hash);
            });
    }

    //

    void Device::SetName(
        nri::PipelineLayout& nriLayout,
        const char*          name) const
    {
        auto& nriUtils = m_Impl->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.SetPipelineLayoutDebugName(nriLayout, name);
    }

    void Device::Release(
        nri::PipelineLayout& nriLayout)
    {
        auto& nriUtils = m_Impl->GetNRI();
        auto& nriCore  = *nriUtils.GetCoreInterface();

        nriCore.DestroyPipelineLayout(nriLayout);
    }
} // namespace Ame::Rhi