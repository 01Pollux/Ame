#include <Gfx/RG/Resolver.hpp>
#include <Gfx/RG/ResourceStorage.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::RG
{
    Resolver::Resolver(
        ResourceStorage& RgStorage) :
        m_Storage(RgStorage)
    {
    }

    //

    Rhi::Device& Resolver::GetDevice() const
    {
        return m_Storage.GetDevice();
    }

    const FrameResourceCPU& Resolver::GetFrameResourceData() const
    {
        return m_Storage.GetFrameResourceData();
    }

    //

    void Resolver::CreateBuffer(
        const ResourceId&      Id,
        const Rhi::BufferDesc& Desc)
    {
        m_Storage.DeclareBuffer(Id, Desc);
        m_ResourcesCreated.emplace(Id);
    }

    void Resolver::CreateTexture(
        const ResourceId&       Id,
        const Rhi::TextureDesc& Desc)
    {
        m_Storage.DeclareTexture(Id, Desc);
        m_ResourcesCreated.emplace(Id);
    }

    //

    void Resolver::ImportBuffer(
        const ResourceId&  Id,
        const Rhi::Buffer& Buffer)
    {
        m_ResourcesCreated.emplace(Id);
        m_Storage.ImportBuffer(Id, Buffer);
    }

    void Resolver::ImportTexture(
        const ResourceId&   Id,
        const Rhi::Texture& Texture)
    {
        m_ResourcesCreated.emplace(Id);
        m_Storage.ImportTexture(Id, Texture);
    }

    //

    void Resolver::WriteResourceEmpty(
        ResourceId Id)
    {
        AME_LOG_ASSERT(Log::Renderer(), m_Storage.ContainsResource(Id), "Resource '{}' doesn't exists", Id.GetName());
        m_ResourcesWritten.emplace(Id);
    }

    //

    void Resolver::WriteBuffer(
        const ResourceViewId&   ViewId,
        Rhi::ShaderBits         Shaders,
        Rhi::ResourceFormat     Format,
        const Rhi::BufferRange& Range)
    {
        WriteResourceEmpty(ViewId.GetResource());
        AppendResourceState(ViewId, { Rhi::AccessBits::SHADER_RESOURCE_STORAGE, Shaders });

        auto& Desc = m_Storage.DeclareBufferView(
            ViewId,
            Rhi::BufferViewDesc{
                .Range  = Range,
                .Format = Format,
                .Type   = Rhi::BufferViewType::UnorderedAccess });

        Desc.usageMask |= Rhi::BufferUsageBits::SHADER_RESOURCE_STORAGE;
    }

    //

    void Resolver::WriteTexture(
        const ResourceViewId&   ViewId,
        const ResourceViewDesc& ViewDesc,
        const Rhi::AccessStage& Stage,
        Rhi::TextureUsageBits   UsageBits)
    {
        WriteResourceEmpty(ViewId.GetResource());
        AppendResourceState(ViewId, Stage);

        auto& Desc = m_Storage.DeclareTextureView(ViewId, ViewDesc);
        Desc.usageMask |= UsageBits;
    }

    //

    void Resolver::WriteTexture(
        const ResourceViewId&       ViewId,
        const Rhi::TextureViewDesc& ViewDesc,
        Rhi::ShaderBits             Shaders)
    {
        WriteTexture(ViewId, ViewDesc, { Rhi::AccessBits::SHADER_RESOURCE_STORAGE, Shaders }, Rhi::TextureUsageBits::SHADER_RESOURCE_STORAGE);
        SetTextureLayout(ViewId.GetResource(), Rhi::LayoutType::SHADER_RESOURCE_STORAGE);
    }

    void Resolver::WriteCopyDstResource(
        const ResourceViewId& ViewId)
    {
        WriteResourceEmpty(ViewId.GetResource());
        AppendResourceState(ViewId, { Rhi::AccessBits::COPY_SOURCE, Rhi::ShaderBits::NONE });
        SetTextureLayout(ViewId.GetResource(), Rhi::LayoutType::COPY_SOURCE);
    }

    //

    void Resolver::WriteRenderTarget(
        const ResourceViewId&          ViewId,
        Rhi::ShaderBits                Shaders,
        const RtvCustomDesc&           RtvDesc,
        Rhi::ResourceFormat            Format,
        const Rhi::TextureSubresource& Subresource)
    {
        auto& Desc = std::get<Rhi::TextureDesc>(m_Storage.GetResource(ViewId.GetResource()).GetDesc());

        RenderTargetViewDesc ViewDesc{
            { .Subresource = Subresource,
              .Format      = Format },
            { .ClearColor = RtvDesc.ClearColor,
              .ClearType  = RtvDesc.ClearType,
              .ForceColor = RtvDesc.ForceColor }
        };

        switch (Desc.type)
        {
        case Rhi::TextureType::TEXTURE_1D:
            ViewDesc.Type = Rhi::TextureViewType::RenderTarget2D;
            break;
        case Rhi::TextureType::TEXTURE_2D:
            ViewDesc.Type = Rhi::TextureViewType::RenderTarget2D;
            break;
        case Rhi::TextureType::TEXTURE_3D:
            ViewDesc.Type = Rhi::TextureViewType::RenderTarget3D;
            break;
        }

        WriteTexture(ViewId, ViewDesc, { Rhi::AccessBits::COLOR_ATTACHMENT, Shaders }, Rhi::TextureUsageBits::COLOR_ATTACHMENT);
        SetTextureLayout(ViewId.GetResource(), Rhi::LayoutType::COLOR_ATTACHMENT);
        m_RenderTargets.emplace_back(ViewId);
    }

    void Resolver::WriteRenderTarget(
        const ResourceViewId&          ViewId,
        Rhi::ShaderBits                Shaders,
        Rhi::ResourceFormat            Format,
        const Rhi::TextureSubresource& Subresource)
    {
        auto& Desc = std::get<Rhi::TextureDesc>(m_Storage.GetResource(ViewId.GetResource()).GetDesc());

        RenderTargetViewDesc ViewDesc{
            { .Subresource = Subresource,
              .Format      = Format }
        };

        switch (Desc.type)
        {
        case Rhi::TextureType::TEXTURE_1D:
            ViewDesc.Type = Rhi::TextureViewType::RenderTarget2D;
            break;
        case Rhi::TextureType::TEXTURE_2D:
            ViewDesc.Type = Rhi::TextureViewType::RenderTarget2D;
            break;
        case Rhi::TextureType::TEXTURE_3D:
            ViewDesc.Type = Rhi::TextureViewType::RenderTarget3D;
            break;
        }

        WriteTexture(ViewId, ViewDesc, { Rhi::AccessBits::COLOR_ATTACHMENT, Shaders }, Rhi::TextureUsageBits::COLOR_ATTACHMENT);
        SetTextureLayout(ViewId.GetResource(), Rhi::LayoutType::COLOR_ATTACHMENT);
        m_RenderTargets.emplace_back(ViewId);
    }

    //

    void Resolver::WriteDepthStencil(
        const ResourceViewId&          ViewId,
        Rhi::ShaderBits                Shaders,
        const DsvCustomDesc&           DsvDesc,
        Rhi::ResourceFormat            Format,
        const Rhi::TextureSubresource& Subresource)
    {
        auto& Desc = std::get<Rhi::TextureDesc>(m_Storage.GetResource(ViewId.GetResource()).GetDesc());

        DepthStencilViewDesc ViewDesc{
            { .Subresource = Subresource,
              .Format      = Format },
            { .Depth     = DsvDesc.Depth,
              .Stencil   = DsvDesc.Stencil,
              .ClearType = DsvDesc.ClearType }
        };

        switch (Desc.type)
        {
        case Rhi::TextureType::TEXTURE_1D:
            ViewDesc.Type = Rhi::TextureViewType::DepthStencil1D;
            break;
        case Rhi::TextureType::TEXTURE_2D:
            ViewDesc.Type = Rhi::TextureViewType::DepthStencil2D;
            break;
        case Rhi::TextureType::TEXTURE_3D:
            Log::Renderer().Warning("DepthStencilViewDesc doesn't support 3D textures");
            return;
        }

        WriteTexture(ViewId, ViewDesc, { Rhi::AccessBits::DEPTH_STENCIL_WRITE, Shaders }, Rhi::TextureUsageBits::DEPTH_STENCIL_ATTACHMENT);
        SetTextureLayout(ViewId.GetResource(), Rhi::LayoutType::DEPTH_STENCIL);
        m_DepthStencil = ViewId;
    }

    void Resolver::WriteDepthStencil(
        const ResourceViewId&          ViewId,
        Rhi::ShaderBits                Shaders,
        Rhi::ResourceFormat            Format,
        const Rhi::TextureSubresource& Subresource)
    {
        auto& Desc = std::get<Rhi::TextureDesc>(m_Storage.GetResource(ViewId.GetResource()).GetDesc());

        DepthStencilViewDesc ViewDesc{
            { .Subresource = Subresource,
              .Format      = Format }
        };

        switch (Desc.type)
        {
        case Rhi::TextureType::TEXTURE_1D:
            ViewDesc.Type = Rhi::TextureViewType::DepthStencil1D;
            break;
        case Rhi::TextureType::TEXTURE_2D:
            ViewDesc.Type = Rhi::TextureViewType::DepthStencil2D;
            break;
        case Rhi::TextureType::TEXTURE_3D:
            Log::Renderer().Warning("DepthStencilViewDesc doesn't support 3D textures");
            return;
        }

        WriteTexture(ViewId, ViewDesc, { Rhi::AccessBits::DEPTH_STENCIL_WRITE, Shaders }, Rhi::TextureUsageBits::DEPTH_STENCIL_ATTACHMENT);
        SetTextureLayout(ViewId.GetResource(), Rhi::LayoutType::DEPTH_STENCIL);
        m_DepthStencil = ViewId;
    }

    //

    void Resolver::ReadResourceEmpty(
        const ResourceId& Id)
    {
        AME_LOG_ASSERT(Log::Renderer(), m_Storage.ContainsResource(Id), "Resource '{}' doesn't exists", Id.GetName());
        m_ResourcesRead.emplace(Id);
    }

    //

    void Resolver::ReadBuffer(
        const ResourceViewId&      ViewId,
        const Rhi::BufferViewDesc& ViewDesc,
        const Rhi::AccessStage&    State,
        Rhi::BufferUsageBits       Usage)
    {
        ReadResourceEmpty(ViewId.GetResource());
        AppendResourceState(ViewId, State);

        auto& View = m_Storage.DeclareBufferView(ViewId, ViewDesc);
        View.usageMask |= Usage;
    }

    //

    void Resolver::ReadTexture(
        const ResourceViewId&       ViewId,
        const Rhi::TextureViewDesc& ViewDesc,
        Rhi::ShaderBits             Shaders)
    {
        ReadResourceEmpty(ViewId.GetResource());
        AppendResourceState(ViewId, { Rhi::AccessBits::SHADER_RESOURCE, Shaders });
        SetTextureLayout(ViewId.GetResource(), Rhi::LayoutType::SHADER_RESOURCE);

        auto& Desc = m_Storage.DeclareTextureView(ViewId, ViewDesc);
        Desc.usageMask |= Rhi::TextureUsageBits::SHADER_RESOURCE;
    }

    void Resolver::ReadCopyDstResource(
        const ResourceViewId& ViewId)
    {
        ReadResourceEmpty(ViewId.GetResource());
        AppendResourceState(ViewId, { Rhi::AccessBits::COPY_DESTINATION, Rhi::ShaderBits::NONE });
        SetTextureLayout(ViewId.GetResource(), Rhi::LayoutType::COPY_DESTINATION);
    }

    void Resolver::ReadDepthStencil(
        const ResourceViewId&       ViewId,
        const Rhi::TextureViewDesc& ViewDesc,
        Rhi::ShaderBits             Shaders)
    {
        ReadResourceEmpty(ViewId.GetResource());
        AppendResourceState(ViewId, { Rhi::AccessBits::DEPTH_STENCIL_READ, Shaders });
        SetTextureLayout(ViewId.GetResource(), Rhi::LayoutType::DEPTH_STENCIL_READONLY);

        auto& Desc = m_Storage.DeclareTextureView(ViewId, ViewDesc);
        Desc.usageMask |= Rhi::TextureUsageBits::SHADER_RESOURCE | Rhi::TextureUsageBits::DEPTH_STENCIL_ATTACHMENT;
    }

    //

    void Resolver::AppendResourceState(
        const ResourceViewId&   ViewId,
        const Rhi::AccessStage& State)
    {
        auto  Iter   = m_ResourceStates.emplace(ViewId, Rhi::AccessStage{});
        auto& States = Iter.first->second;
        if (Iter.second)
        {
            States.stages = Rhi::ShaderBits::NONE;
        }

        States.access |= State.access;
        if (State.stages != Rhi::ShaderBits::NONE)
        {
            if (States.stages == Rhi::ShaderBits::NONE)
            {
                States.stages = State.stages;
            }
            else
            {
                States.stages |= State.stages;
            }
        }
    }

    void Resolver::SetTextureLayout(
        const ResourceId& ViewId,
        Rhi::LayoutType   Layout)
    {
        m_TextureLayouts[ViewId] = Layout;
    }
} // namespace Ame::Gfx::RG