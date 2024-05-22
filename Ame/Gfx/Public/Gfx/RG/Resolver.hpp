#pragma once

#include <Gfx/RG/Resource.hpp>

namespace Ame::Gfx::RG
{
    class Resolver
    {
        friend class PassStorage;

    public:
        Resolver(
            ResourceStorage& RgStorage);

    public:
        /// <summary>
        /// Helper function to get device of the engine
        /// </summary>
        [[nodiscard]] Rhi::Device& GetDevice() const;

        /// <summary>
        /// Helper function to get backbuffer desc
        /// </summary>
        [[nodiscard]] const Rhi::TextureDesc& GetBackbufferDesc() const;

        /// <summary>
        /// Get frame resource
        /// </summary>
        [[nodiscard]] const FrameResourceCPU& GetFrameResourceData() const;

    public:
        /// <summary>
        /// Create buffer
        /// </summary>
        void CreateBuffer(
            const ResourceId&      Id,
            const Rhi::BufferDesc& Desc);

        /// <summary>
        /// Create texture
        /// </summary>
        void CreateTexture(
            const ResourceId&       Id,
            const Rhi::TextureDesc& Desc);

    public:
        /// <summary>
        /// import buffer to be used later when dispatching passes
        /// </summary>
        void ImportBuffer(
            const ResourceId&  Id,
            const Rhi::Buffer& Buffer);

        /// <summary>
        /// import texture to be used later when dispatching passes
        /// </summary>
        void ImportTexture(
            const ResourceId&   Id,
            const Rhi::Texture& Texture);

    public:
        /// <summary>
        /// Write resource view to add dependency without actually writing anything
        /// </summary>
        void WriteResourceEmpty(
            ResourceId Id);

    public:
        /// <summary>
        /// Write to buffer resource
        /// </summary>
        void WriteBuffer(
            const ResourceViewId&   ViewId,
            Rhi::ShaderType         Shaders,
            Rhi::ResourceFormat     Format = Rhi::ResourceFormat::UNKNOWN,
            const Rhi::BufferRange& Range  = Rhi::EntireBuffer);

    private:
        /// <summary>
        /// Write to resource
        /// </summary>
        void WriteTexture(
            const ResourceViewId&   ViewId,
            const ResourceViewDesc& ViewDesc,
            const Rhi::AccessStage& AccessStage,
            Rhi::TextureUsageBits   UsageBits);

    public:
        /// <summary>
        /// Write to texture resource
        /// </summary>
        void WriteTexture(
            const ResourceViewId&       ViewId,
            const Rhi::TextureViewDesc& ViewDesc,
            Rhi::ShaderType             Shaders);

        /// <summary>
        /// Write to texture resource as copy destination
        /// </summary>
        void WriteCopyDstResource(
            const ResourceViewId& ViewId);

        /// <summary>
        /// Write resource as renter target with clear operations
        /// </summary>
        void WriteRenderTarget(
            const ResourceViewId&          ViewId,
            Rhi::ShaderType                Shaders,
            const RtvCustomDesc&           RtvDesc,
            Rhi::ResourceFormat            Format,
            const Rhi::TextureSubresource& Subresource = Rhi::AllSubresources);

        /// <summary>
        /// Write resource as renter target with no clear operations
        /// </summary>
        void WriteRenderTarget(
            const ResourceViewId&          ViewId,
            Rhi::ShaderType                Shaders,
            Rhi::ResourceFormat            Format,
            const Rhi::TextureSubresource& Subresource = Rhi::AllSubresources);

        /// <summary>
        /// Write resource as depth stencil
        /// </summary>
        void WriteDepthStencil(
            const ResourceViewId&          ViewId,
            Rhi::ShaderType                Shaders,
            const DsvCustomDesc&           DsvDesc,
            Rhi::ResourceFormat            Format,
            const Rhi::TextureSubresource& Subresource = Rhi::AllSubresources);

        /// <summary>
        /// Write resource as depth stencil
        /// </summary>
        void WriteDepthStencil(
            const ResourceViewId&          ViewId,
            Rhi::ShaderType                Shaders,
            Rhi::ResourceFormat            Format,
            const Rhi::TextureSubresource& Subresource = Rhi::AllSubresources);

    public:
        /// <summary>
        /// Read dummy resource
        /// </summary>
        void ReadResourceEmpty(
            const ResourceId& Id);

    private:
        /// <summary>
        /// Read from buffer resource
        /// </summary>
        void ReadBuffer(
            const ResourceViewId&      ViewId,
            const Rhi::BufferViewDesc& ViewDesc,
            const Rhi::AccessStage&    Stage,
            Rhi::BufferUsageBits       Usage);

    public:
        /// <summary>
        /// Read from buffer resource
        /// </summary>
        void ReadBuffer(
            const ResourceViewId&   ViewId,
            Rhi::ShaderType         Shaders,
            Rhi::ResourceFormat     Format = Rhi::ResourceFormat::UNKNOWN,
            const Rhi::BufferRange& Range  = Rhi::EntireBuffer)
        {
            ReadBuffer(
                ViewId,
                Rhi::BufferViewDesc{
                    .Range  = Range,
                    .Format = Format,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { nri::AccessBits::SHADER_RESOURCE, Shaders },
                nri::BufferUsageBits::SHADER_RESOURCE);
        }

        /// <summary>
        /// Read from vertex buffer resource
        /// </summary>
        void ReadVertexBuffer(
            const ResourceViewId&   ViewId,
            Rhi::ShaderType         Shaders,
            const Rhi::BufferRange& Range = Rhi::EntireBuffer)
        {
            ReadBuffer(
                ViewId,
                Rhi::BufferViewDesc{
                    .Range  = Range,
                    .Format = nri::Format::UNKNOWN,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { nri::AccessBits::VERTEX_BUFFER, Shaders },
                nri::BufferUsageBits::VERTEX_BUFFER);
        }

        /// <summary>
        /// Read from vertex buffer resource
        /// </summary>
        void ReadIndexBuffer(
            const ResourceViewId&   ViewId,
            Rhi::ShaderType         Shaders,
            const Rhi::BufferRange& Range = Rhi::EntireBuffer)
        {
            ReadBuffer(
                ViewId,
                Rhi::BufferViewDesc{
                    .Range  = Range,
                    .Format = nri::Format::UNKNOWN,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { nri::AccessBits::INDEX_BUFFER, Shaders },
                nri::BufferUsageBits::INDEX_BUFFER);
        }

        /// <summary>
        /// Read from vertex buffer resource
        /// </summary>
        void ReadConstantBuffer(
            const ResourceViewId&   ViewId,
            Rhi::ShaderType         Shaders,
            const Rhi::BufferRange& Range = Rhi::EntireBuffer)
        {
            ReadBuffer(
                ViewId,
                Rhi::BufferViewDesc{
                    .Range  = Range,
                    .Format = nri::Format::UNKNOWN,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { nri::AccessBits::CONSTANT_BUFFER, Shaders },
                nri::BufferUsageBits::CONSTANT_BUFFER);
        }

        /// <summary>
        /// Read from vertex buffer resource
        /// </summary>
        void ReadIndirectBuffer(
            const ResourceViewId&   ViewId,
            Rhi::ShaderType         Shaders,
            const Rhi::BufferRange& Range = Rhi::EntireBuffer)
        {
            ReadBuffer(
                ViewId,
                Rhi::BufferViewDesc{
                    .Range  = Range,
                    .Format = nri::Format::UNKNOWN,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { nri::AccessBits::ARGUMENT_BUFFER, Shaders },
                nri::BufferUsageBits::ARGUMENT_BUFFER);
        }

    public:
        /// <summary>
        /// Read from texture resource
        /// </summary>
        void ReadTexture(
            const ResourceViewId&       ViewId,
            const Rhi::TextureViewDesc& ViewDesc,
            Rhi::ShaderType             Shaders);

        /// <summary>
        /// Read from texture resource as copy destination
        /// </summary>
        void ReadCopyDstResource(
            const ResourceViewId& ViewId);

        /// <summary>
        /// Read from texture as depth stencil
        /// </summary>
        void ReadDepthStencil(
            const ResourceViewId&       ViewId,
            const Rhi::TextureViewDesc& ViewDesc,
            Rhi::ShaderType             Shaders);

    private:
        /// <summary>
        /// Append resource state
        /// </summary>
        void AppendResourceState(
            const ResourceViewId&   ViewId,
            const Rhi::AccessStage& Stage);

        /// <summary>
        /// Initialize texture layout
        /// </summary>
        void SetTextureLayout(
            const ResourceId& ViewId,
            Rhi::LayoutType   Layout);

    private:
        ResourceStorage& m_Storage;

        std::vector<ResourceViewId> m_RenderTargets;
        ResourceViewId              m_DepthStencil;

        std::set<ResourceId> m_ResourcesCreated;
        std::set<ResourceId> m_ResourcesRead;
        std::set<ResourceId> m_ResourcesWritten;

        std::map<ResourceViewId, Rhi::AccessStage> m_ResourceStates;
        std::map<ResourceId, Rhi::LayoutType>      m_TextureLayouts;
    };
} // namespace Ame::Gfx::RG