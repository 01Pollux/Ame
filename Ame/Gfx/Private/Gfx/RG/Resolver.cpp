#include <Gfx/RG/Resolver.hpp>
#include <Gfx/RG/ResourceStorage.hpp>

#include <Rhi/Device/Device.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::RG
{
    Resolver::Resolver(
        ResourceStorage& resourceStorage) :
        m_Storage(resourceStorage)
    {
    }

    //

    Rhi::Device& Resolver::GetDevice() const
    {
        return m_Storage.GetDevice();
    }

    const Rhi::TextureDesc& Resolver::GetBackbufferDesc() const
    {
        return GetDevice().GetBackBufferDesc();
    }

    const FrameResourceCPU& Resolver::GetFrameResourceData() const
    {
        return m_Storage.GetFrameResourceData();
    }

    //

    void Resolver::CreateBuffer(
        const ResourceId&      id,
        const Rhi::BufferDesc& desc)
    {
        m_Storage.DeclareBuffer(id, desc);
        m_ResourcesCreated.emplace(id);
    }

    void Resolver::CreateTexture(
        const ResourceId&       id,
        const Rhi::TextureDesc& desc)
    {
        m_Storage.DeclareTexture(id, desc);
        m_ResourcesCreated.emplace(id);
    }

    //

    void Resolver::ImportBuffer(
        const ResourceId& id,
        Rhi::Buffer       buffer)
    {
        m_ResourcesCreated.emplace(id);
        m_Storage.ImportBuffer(id, std::move(buffer));
    }

    void Resolver::ImportTexture(
        const ResourceId& id,
        Rhi::Texture      texture)
    {
        m_ResourcesCreated.emplace(id);
        m_Storage.ImportTexture(id, std::move(texture));
    }

    //

    void Resolver::WriteResourceEmpty(
        const ResourceId& id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesWritten.emplace(id);
    }

    //

    void Resolver::WriteBuffer(
        const ResourceViewId&   viewId,
        Rhi::StageBits          stages,
        Rhi::ResourceFormat     format,
        const Rhi::BufferRange& range)
    {
        WriteResourceEmpty(viewId.GetResource());
        AppendResourceState(viewId, { Rhi::AccessBits::SHADER_RESOURCE_STORAGE, stages });

        auto& desc = m_Storage.DeclareBufferView(
            viewId,
            Rhi::BufferViewDesc{
                .Range  = range,
                .Format = format,
                .Type   = Rhi::BufferViewType::UnorderedAccess });

        desc.usageMask |= Rhi::BufferUsageBits::SHADER_RESOURCE_STORAGE;
    }

    //

    void Resolver::WriteTexture(
        const ResourceViewId&   viewId,
        const ResourceViewDesc& desc,
        const Rhi::AccessStage& accessStage,
        Rhi::TextureUsageBits   usageBits)
    {
        WriteResourceEmpty(viewId.GetResource());
        AppendResourceState(viewId, accessStage);

        auto& viewDesc = m_Storage.DeclareTextureView(viewId, desc);
        viewDesc.usageMask |= usageBits;
    }

    //

    void Resolver::WriteTexture(
        const ResourceViewId&       viewId,
        const Rhi::TextureViewDesc& desc,
        Rhi::StageBits              stages)
    {
        WriteTexture(viewId, desc, { Rhi::AccessBits::SHADER_RESOURCE_STORAGE, stages }, Rhi::TextureUsageBits::SHADER_RESOURCE_STORAGE);
        SetTextureLayout(viewId.GetResource(), Rhi::LayoutType::SHADER_RESOURCE_STORAGE);
    }

    void Resolver::WriteCopyDstResource(
        const ResourceViewId& viewId)
    {
        WriteResourceEmpty(viewId.GetResource());
        AppendResourceState(viewId, { Rhi::AccessBits::COPY_SOURCE, Rhi::StageBits::NONE });
        SetTextureLayout(viewId.GetResource(), Rhi::LayoutType::COPY_SOURCE);
    }

    //

    void Resolver::WriteRenderTarget(
        const ResourceViewId&          viewId,
        Rhi::StageBits                 stages,
        const RtvCustomDesc&           RtvDesc,
        Rhi::ResourceFormat            format,
        const Rhi::TextureSubresource& Subresource)
    {
        auto& desc = std::get<Rhi::TextureDesc>(m_Storage.GetResource(viewId.GetResource()).GetDesc());

        RenderTargetViewDesc viewDesc{
            { .Subresource = Subresource,
              .Format      = format },
            { .ClearColor = RtvDesc.ClearColor,
              .ClearType  = RtvDesc.ClearType,
              .ForceColor = RtvDesc.ForceColor }
        };

        switch (desc.type)
        {
        case Rhi::TextureType::TEXTURE_1D:
            viewDesc.Type = Rhi::TextureViewType::RenderTarget2D;
            break;
        case Rhi::TextureType::TEXTURE_2D:
            viewDesc.Type = Rhi::TextureViewType::RenderTarget2D;
            break;
        case Rhi::TextureType::TEXTURE_3D:
            viewDesc.Type = Rhi::TextureViewType::RenderTarget3D;
            break;
        }

        WriteTexture(viewId, viewDesc, { Rhi::AccessBits::COLOR_ATTACHMENT, stages }, Rhi::TextureUsageBits::COLOR_ATTACHMENT);
        SetTextureLayout(viewId.GetResource(), Rhi::LayoutType::COLOR_ATTACHMENT);
        m_RenderTargets.emplace_back(viewId);
    }

    void Resolver::WriteRenderTarget(
        const ResourceViewId&          viewId,
        Rhi::StageBits                 stages,
        Rhi::ResourceFormat            format,
        const Rhi::TextureSubresource& Subresource)
    {
        auto& desc = std::get<Rhi::TextureDesc>(m_Storage.GetResource(viewId.GetResource()).GetDesc());

        RenderTargetViewDesc viewDesc{
            { .Subresource = Subresource,
              .Format      = format }
        };

        switch (desc.type)
        {
        case Rhi::TextureType::TEXTURE_1D:
            viewDesc.Type = Rhi::TextureViewType::RenderTarget2D;
            break;
        case Rhi::TextureType::TEXTURE_2D:
            viewDesc.Type = Rhi::TextureViewType::RenderTarget2D;
            break;
        case Rhi::TextureType::TEXTURE_3D:
            viewDesc.Type = Rhi::TextureViewType::RenderTarget3D;
            break;
        }

        WriteTexture(viewId, viewDesc, { Rhi::AccessBits::COLOR_ATTACHMENT, stages }, Rhi::TextureUsageBits::COLOR_ATTACHMENT);
        SetTextureLayout(viewId.GetResource(), Rhi::LayoutType::COLOR_ATTACHMENT);
        m_RenderTargets.emplace_back(viewId);
    }

    //

    void Resolver::WriteDepthStencil(
        const ResourceViewId&          viewId,
        Rhi::StageBits                 stages,
        const DsvCustomDesc&           DsvDesc,
        Rhi::ResourceFormat            format,
        const Rhi::TextureSubresource& Subresource)
    {
        auto& desc = std::get<Rhi::TextureDesc>(m_Storage.GetResource(viewId.GetResource()).GetDesc());

        DepthStencilViewDesc viewDesc{
            { .Subresource = Subresource,
              .Format      = format },
            { .Depth     = DsvDesc.Depth,
              .Stencil   = DsvDesc.Stencil,
              .ClearType = DsvDesc.ClearType }
        };

        switch (desc.type)
        {
        case Rhi::TextureType::TEXTURE_1D:
            viewDesc.Type = Rhi::TextureViewType::DepthStencil1D;
            break;
        case Rhi::TextureType::TEXTURE_2D:
            viewDesc.Type = Rhi::TextureViewType::DepthStencil2D;
            break;
        case Rhi::TextureType::TEXTURE_3D:
            Log::Gfx().Warning("DepthStencilViewDesc doesn't support 3D textures");
            return;
        }

        WriteTexture(viewId, viewDesc, { Rhi::AccessBits::DEPTH_STENCIL_WRITE, stages }, Rhi::TextureUsageBits::DEPTH_STENCIL_ATTACHMENT);
        SetTextureLayout(viewId.GetResource(), Rhi::LayoutType::DEPTH_STENCIL);
        m_DepthStencil = viewId;
    }

    void Resolver::WriteDepthStencil(
        const ResourceViewId&          viewId,
        Rhi::StageBits                 stages,
        Rhi::ResourceFormat            format,
        const Rhi::TextureSubresource& Subresource)
    {
        auto& desc = std::get<Rhi::TextureDesc>(m_Storage.GetResource(viewId.GetResource()).GetDesc());

        DepthStencilViewDesc viewDesc{
            { .Subresource = Subresource,
              .Format      = format }
        };

        switch (desc.type)
        {
        case Rhi::TextureType::TEXTURE_1D:
            viewDesc.Type = Rhi::TextureViewType::DepthStencil1D;
            break;
        case Rhi::TextureType::TEXTURE_2D:
            viewDesc.Type = Rhi::TextureViewType::DepthStencil2D;
            break;
        case Rhi::TextureType::TEXTURE_3D:
            Log::Gfx().Warning("DepthStencilViewDesc doesn't support 3D textures");
            return;
        }

        WriteTexture(viewId, viewDesc, { Rhi::AccessBits::DEPTH_STENCIL_WRITE, stages }, Rhi::TextureUsageBits::DEPTH_STENCIL_ATTACHMENT);
        SetTextureLayout(viewId.GetResource(), Rhi::LayoutType::DEPTH_STENCIL);
        m_DepthStencil = viewId;
    }

    //

    void Resolver::ReadResourceEmpty(
        const ResourceId& id)
    {
        AME_LOG_ASSERT(Log::Gfx(), m_Storage.ContainsResource(id), "Resource '{}' doesn't exists", id.GetName());
        m_ResourcesRead.emplace(id);
    }

    //

    void Resolver::ReadBuffer(
        const ResourceViewId&      viewId,
        const Rhi::BufferViewDesc& ViewDesc,
        const Rhi::AccessStage&    State,
        Rhi::BufferUsageBits       Usage)
    {
        ReadResourceEmpty(viewId.GetResource());
        AppendResourceState(viewId, State);

        auto& desc = m_Storage.DeclareBufferView(viewId, ViewDesc);
        desc.usageMask |= Usage;
    }

    //

    void Resolver::ReadTexture(
        const ResourceViewId&       viewId,
        const Rhi::TextureViewDesc& ViewDesc,
        Rhi::StageBits              stages)
    {
        ReadResourceEmpty(viewId.GetResource());
        AppendResourceState(viewId, { Rhi::AccessBits::SHADER_RESOURCE, stages });
        SetTextureLayout(viewId.GetResource(), Rhi::LayoutType::SHADER_RESOURCE);

        auto& desc = m_Storage.DeclareTextureView(viewId, ViewDesc);
        desc.usageMask |= Rhi::TextureUsageBits::SHADER_RESOURCE;
    }

    void Resolver::ReadCopyDstResource(
        const ResourceViewId& viewId)
    {
        ReadResourceEmpty(viewId.GetResource());
        AppendResourceState(viewId, { Rhi::AccessBits::COPY_DESTINATION, Rhi::StageBits::NONE });
        SetTextureLayout(viewId.GetResource(), Rhi::LayoutType::COPY_DESTINATION);
    }

    void Resolver::ReadDepthStencil(
        const ResourceViewId&       viewId,
        const Rhi::TextureViewDesc& viewDesc,
        Rhi::StageBits              stages)
    {
        ReadResourceEmpty(viewId.GetResource());
        AppendResourceState(viewId, { Rhi::AccessBits::DEPTH_STENCIL_READ, stages });
        SetTextureLayout(viewId.GetResource(), Rhi::LayoutType::DEPTH_STENCIL_READONLY);

        auto& desc = m_Storage.DeclareTextureView(viewId, viewDesc);
        desc.usageMask |= Rhi::TextureUsageBits::SHADER_RESOURCE | Rhi::TextureUsageBits::DEPTH_STENCIL_ATTACHMENT;
    }

    //

    void Resolver::AppendResourceState(
        const ResourceViewId&   viewId,
        const Rhi::AccessStage& state)
    {
        auto  iter   = m_ResourceStates.emplace(viewId, Rhi::AccessStage{});
        auto& states = iter.first->second;

        Rhi::ShaderFlags flags(states.stages);
        flags.Set(state.stages);

        states.access |= state.access;
        states.stages = flags.Flags;
    }

    void Resolver::SetTextureLayout(
        const ResourceId& viewId,
        Rhi::LayoutType   layout)
    {
        m_TextureLayouts[viewId] = layout;
    }
} // namespace Ame::Gfx::RG