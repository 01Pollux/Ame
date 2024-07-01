#include <RG/Resolver.hpp>
#include <RG/ResourceStorage.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
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

    Rhi::ResourceFormat Resolver::GetBackbufferFormat() const
    {
        return m_Storage.GetBackbufferFormat();
    }

    const Rhi::TextureDesc& Resolver::GetBackbufferDesc() const
    {
        return m_Storage.GetBackbufferDesc();
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
        m_Storage.DeclareResource(id, BufferResource{ .Desc = desc });
        m_ResourcesCreated.emplace(id);
    }

    void Resolver::CreateTexture(
        const ResourceId&       id,
        const Rhi::TextureDesc& desc)
    {
        m_Storage.DeclareResource(id, TextureResource{ .Desc = desc });
        m_ResourcesCreated.emplace(id);
    }

    //

    void Resolver::ImportBuffer(
        const ResourceId&   id,
        Rhi::MemoryLocation location,
        Rhi::Buffer         buffer,
        Rhi::AccessStage    initialState)
    {
        m_ResourcesCreated.emplace(id);
        m_Storage.ImportBuffer(id, location, std::move(buffer), initialState);
    }

    void Resolver::ImportTexture(
        const ResourceId&      id,
        Rhi::Texture           texture,
        Rhi::AccessLayoutStage initialState)
    {
        m_ResourcesCreated.emplace(id);
        m_Storage.ImportTexture(id, std::move(texture), initialState);
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

        auto& buffer = m_Storage.DeclareBufferView(
            viewId,
            Rhi::BufferViewDesc{
                .Range  = range,
                .Format = format,
                .Type   = Rhi::BufferViewType::UnorderedAccess });

        buffer.Desc.usageMask |= Rhi::BufferUsageBits::SHADER_RESOURCE_STORAGE;
    }

    //

    void Resolver::WriteTexture(
        const ResourceViewId&          viewId,
        const TextureResourceViewDesc& viewDesc,
        const Rhi::AccessStage&        accessStage,
        Rhi::TextureUsageBits          usageBits)
    {
        WriteResourceEmpty(viewId.GetResource());
        AppendResourceState(viewId, accessStage);

        auto& texture = m_Storage.DeclareTextureView(viewId, viewDesc);
        texture.Desc.usageMask |= usageBits;
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

    //

    void Resolver::WriteRenderTargetImpl(
        const ResourceViewId&          viewId,
        Rhi::AccessLayoutStage         accessType,
        const RtvCustomDesc&           rtvDesc,
        Rhi::ResourceFormat            format,
        const Rhi::TextureSubresource& subresource)
    {
        auto resource = m_Storage.GetResource(viewId.GetResource());
        auto texture  = resource->AsTexture();
        AME_LOG_ASSERT(Log::Gfx(), texture != nullptr, "Resource is not a texture");

        RenderTargetViewDesc viewDesc{
            { .Subresource = subresource,
              .Format      = format },
            { .ClearColor = rtvDesc.ClearColor,
              .ClearType  = rtvDesc.ClearType,
              .ForceColor = rtvDesc.ForceColor }
        };

        switch (texture->Desc.type)
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

        WriteTexture(viewId, viewDesc, { accessType.access, accessType.stages }, Rhi::TextureUsageBits::COLOR_ATTACHMENT);
        SetTextureLayout(viewId.GetResource(), accessType.layout);
        m_RenderTargets.emplace_back(viewId);
    }

    void Resolver::WriteRenderTargetImpl(
        const ResourceViewId&          viewId,
        Rhi::AccessLayoutStage         accessType,
        Rhi::ResourceFormat            format,
        const Rhi::TextureSubresource& subresource)
    {
        auto resource = m_Storage.GetResource(viewId.GetResource());
        auto texture  = resource->AsTexture();
        AME_LOG_ASSERT(Log::Gfx(), texture != nullptr, "Resource is not a texture");

        RenderTargetViewDesc viewDesc{
            { .Subresource = subresource,
              .Format      = format }
        };

        switch (texture->Desc.type)
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

        WriteTexture(viewId, viewDesc, { accessType.access, accessType.stages }, Rhi::TextureUsageBits::COLOR_ATTACHMENT);
        SetTextureLayout(viewId.GetResource(), accessType.layout);
        m_RenderTargets.emplace_back(viewId);
    }

    //

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
        const RtvCustomDesc&           rtvDesc,
        Rhi::ResourceFormat            format,
        const Rhi::TextureSubresource& subresource)
    {
        WriteRenderTargetImpl(viewId, { Rhi::AccessBits::COLOR_ATTACHMENT, Rhi::LayoutType::COLOR_ATTACHMENT, stages }, rtvDesc, format, subresource);
    }

    void Resolver::WriteRenderTarget(
        const ResourceViewId&          viewId,
        Rhi::StageBits                 stages,
        Rhi::ResourceFormat            format,
        const Rhi::TextureSubresource& subresource)
    {
        WriteRenderTargetImpl(viewId, { Rhi::AccessBits::COLOR_ATTACHMENT, Rhi::LayoutType::COLOR_ATTACHMENT, stages }, format, subresource);
    }

    //

    void Resolver::WritePresentResource(
        const ResourceViewId& viewId,
        const RtvCustomDesc&  rtvDesc)
    {
        WriteRenderTargetImpl(viewId, { Rhi::AccessBits::UNKNOWN, Rhi::LayoutType::PRESENT, Rhi::StageBits::ALL }, rtvDesc);
    }

    void Resolver::WritePresentResource(
        const ResourceViewId& viewId)
    {
        WriteRenderTargetImpl(viewId, { Rhi::AccessBits::UNKNOWN, Rhi::LayoutType::PRESENT, Rhi::StageBits::ALL });
    }

    //

    void Resolver::WriteDepthStencil(
        const ResourceViewId&          viewId,
        Rhi::StageBits                 stages,
        const DsvCustomDesc&           DsvDesc,
        Rhi::ResourceFormat            format,
        const Rhi::TextureSubresource& subresource)
    {
        auto resource = m_Storage.GetResource(viewId.GetResource());
        auto texture  = resource->AsTexture();
        AME_LOG_ASSERT(Log::Gfx(), texture != nullptr, "Resource is not a texture");

        DepthStencilViewDesc viewDesc{
            { .Subresource = subresource,
              .Format      = format },
            { .Depth     = DsvDesc.Depth,
              .Stencil   = DsvDesc.Stencil,
              .ClearType = DsvDesc.ClearType }
        };

        switch (texture->Desc.type)
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
        const Rhi::TextureSubresource& subresource)
    {
        auto resource = m_Storage.GetResource(viewId.GetResource());
        auto texture  = resource->AsTexture();
        AME_LOG_ASSERT(Log::Gfx(), texture != nullptr, "Resource is not a texture");

        DepthStencilViewDesc viewDesc{
            { .Subresource = subresource,
              .Format      = format }
        };

        switch (texture->Desc.type)
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

        auto& buffer = m_Storage.DeclareBufferView(viewId, ViewDesc);
        buffer.Desc.usageMask |= Usage;
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

        auto& texture = m_Storage.DeclareTextureView(viewId, ViewDesc);
        texture.Desc.usageMask |= Rhi::TextureUsageBits::SHADER_RESOURCE;
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

        auto& texture = m_Storage.DeclareTextureView(viewId, viewDesc);
        texture.Desc.usageMask |= Rhi::TextureUsageBits::SHADER_RESOURCE | Rhi::TextureUsageBits::DEPTH_STENCIL_ATTACHMENT;
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
} // namespace Ame::RG