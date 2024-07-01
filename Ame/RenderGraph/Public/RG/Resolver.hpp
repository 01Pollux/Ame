#pragma once

#include <RG/Resource.hpp>

namespace Ame::RG
{
    class Resolver
    {
        friend class PassStorage;

    public:
        Resolver(
            ResourceStorage& resourceStorage);

    public:
        /// <summary>
        /// Helper function to get device of the engine
        /// </summary>
        [[nodiscard]] Rhi::Device& GetDevice() const;

        /// <summary>
        /// Get backbuffer texture format
        /// </summary>
        [[nodiscard]] Rhi::ResourceFormat GetBackbufferFormat() const;

        /// <summary>
        /// Get backbuffer texture desc
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
            const ResourceId&      id,
            const Rhi::BufferDesc& desc);

        /// <summary>
        /// Create texture
        /// </summary>
        void CreateTexture(
            const ResourceId&       id,
            const Rhi::TextureDesc& desc);

    public:
        /// <summary>
        /// import buffer to be used later when dispatching passes
        /// </summary>
        void ImportBuffer(
            const ResourceId&   id,
            Rhi::MemoryLocation location,
            Rhi::Buffer         buffer,
            Rhi::AccessStage    initialState);

        /// <summary>
        /// import texture to be used later when dispatching passes
        /// </summary>
        void ImportTexture(
            const ResourceId&      id,
            Rhi::Texture           texture,
            Rhi::AccessLayoutStage initialState);

    public:
        /// <summary>
        /// Write resource view to add dependency without actually writing anything
        /// </summary>
        void WriteResourceEmpty(
            const ResourceId& Id);

    public:
        /// <summary>
        /// Write to buffer resource
        /// </summary>
        void WriteBuffer(
            const ResourceViewId&   viewId,
            Rhi::StageBits          stages,
            Rhi::ResourceFormat     format,
            const Rhi::BufferRange& range = Rhi::c_EntireBuffer);

    private:
        /// <summary>
        /// Write to resource
        /// </summary>
        void WriteTexture(
            const ResourceViewId&          viewId,
            const TextureResourceViewDesc& viewDesc,
            const Rhi::AccessStage&        accessStage,
            Rhi::TextureUsageBits          usageBits);

    public:
        /// <summary>
        /// Write to texture resource
        /// </summary>
        void WriteTexture(
            const ResourceViewId&       viewId,
            const Rhi::TextureViewDesc& viewDesc,
            Rhi::StageBits              stages);

        /// <summary>
        /// Write to texture resource as copy destination
        /// </summary>
        void WriteCopyDstResource(
            const ResourceViewId& viewId);

        /// <summary>
        /// Present resource to the backbuffer
        /// Resource must be an imported backbuffer, otherwise will thrown an assertion
        /// </summary>
        void WritePresentResource(
            const ResourceViewId& viewId);

        /// <summary>
        /// Write resource as renter target with clear operations
        /// </summary>
        void WriteRenderTarget(
            const ResourceViewId&          viewId,
            Rhi::StageBits                 stages,
            const RtvCustomDesc&           rtvDesc,
            Rhi::ResourceFormat            format      = Rhi::ResourceFormat::UNKNOWN,
            const Rhi::TextureSubresource& subresource = Rhi::c_AllSubresources);

        /// <summary>
        /// Write resource as renter target with no clear operations
        /// </summary>
        void WriteRenderTarget(
            const ResourceViewId&          viewId,
            Rhi::StageBits                 stages,
            Rhi::ResourceFormat            format      = Rhi::ResourceFormat::UNKNOWN,
            const Rhi::TextureSubresource& subresource = Rhi::c_AllSubresources);

        /// <summary>
        /// Write resource as depth stencil
        /// </summary>
        void WriteDepthStencil(
            const ResourceViewId&          viewId,
            Rhi::StageBits                 stages,
            const DsvCustomDesc&           dsvDesc,
            Rhi::ResourceFormat            format      = Rhi::ResourceFormat::UNKNOWN,
            const Rhi::TextureSubresource& subresource = Rhi::c_AllSubresources);

        /// <summary>
        /// Write resource as depth stencil
        /// </summary>
        void WriteDepthStencil(
            const ResourceViewId&          viewId,
            Rhi::StageBits                 stages,
            Rhi::ResourceFormat            format      = Rhi::ResourceFormat::UNKNOWN,
            const Rhi::TextureSubresource& subresource = Rhi::c_AllSubresources);

    public:
        /// <summary>
        /// Read dummy resource
        /// </summary>
        void ReadResourceEmpty(
            const ResourceId& id);

    private:
        /// <summary>
        /// Read from buffer resource
        /// </summary>
        void ReadBuffer(
            const ResourceViewId&      viewId,
            const Rhi::BufferViewDesc& viewDesc,
            const Rhi::AccessStage&    accessStage,
            Rhi::BufferUsageBits       usageBits);

    public:
        /// <summary>
        /// Read from buffer resource
        /// </summary>
        void ReadBuffer(
            const ResourceViewId&   viewId,
            Rhi::StageBits          stages,
            Rhi::ResourceFormat     format = Rhi::ResourceFormat::UNKNOWN,
            const Rhi::BufferRange& range  = Rhi::c_EntireBuffer)
        {
            ReadBuffer(
                viewId,
                Rhi::BufferViewDesc{
                    .Range  = range,
                    .Format = format,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { Rhi::AccessBits::SHADER_RESOURCE, stages },
                Rhi::BufferUsageBits::SHADER_RESOURCE);
        }

        /// <summary>
        /// Read from vertex buffer resource
        /// </summary>
        void ReadVertexBuffer(
            const ResourceViewId&   viewId,
            const Rhi::BufferRange& range = Rhi::c_EntireBuffer)
        {
            ReadBuffer(
                viewId,
                Rhi::BufferViewDesc{
                    .Range  = range,
                    .Format = Rhi::ResourceFormat::UNKNOWN,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { Rhi::AccessBits::VERTEX_BUFFER, Rhi::StageBits::VERTEX_SHADER },
                Rhi::BufferUsageBits::VERTEX_BUFFER);
        }

        /// <summary>
        /// Read from vertex buffer resource
        /// </summary>
        void ReadIndexBuffer(
            const ResourceViewId&   viewId,
            const Rhi::BufferRange& range = Rhi::c_EntireBuffer)
        {
            ReadBuffer(
                viewId,
                Rhi::BufferViewDesc{
                    .Range  = range,
                    .Format = Rhi::ResourceFormat::UNKNOWN,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { Rhi::AccessBits::INDEX_BUFFER, Rhi::StageBits::INDEX_INPUT },
                Rhi::BufferUsageBits::INDEX_BUFFER);
        }

        /// <summary>
        /// Read from constant buffer resource
        /// </summary>
        void ReadConstantBuffer(
            const ResourceViewId&   viewId,
            Rhi::StageBits          stages,
            const Rhi::BufferRange& range = Rhi::c_EntireBuffer)
        {
            ReadBuffer(
                viewId,
                Rhi::BufferViewDesc{
                    .Range  = range,
                    .Format = Rhi::ResourceFormat::UNKNOWN,
                    .Type   = Rhi::BufferViewType::ConstantBuffer },
                { Rhi::AccessBits::CONSTANT_BUFFER, stages },
                Rhi::BufferUsageBits::CONSTANT_BUFFER);
        }

        /// <summary>
        /// Read from structured buffer resource
        /// </summary>
        void ReadStructuredBuffer(
            const ResourceViewId&   viewId,
            Rhi::StageBits          stages,
            const Rhi::BufferRange& range = Rhi::c_EntireBuffer)
        {
            ReadBuffer(
                viewId,
                Rhi::BufferViewDesc{
                    .Range  = range,
                    .Format = Rhi::ResourceFormat::UNKNOWN,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { Rhi::AccessBits::SHADER_RESOURCE, stages },
                Rhi::BufferUsageBits::SHADER_RESOURCE);
        }

        /// <summary>
        /// Read from structured buffer resource
        /// </summary>
        void ReadTypedBuffer(
            const ResourceViewId&   viewId,
            Rhi::StageBits          stages,
            Rhi::ResourceFormat     format,
            const Rhi::BufferRange& range = Rhi::c_EntireBuffer)
        {
            ReadBuffer(
                viewId,
                Rhi::BufferViewDesc{
                    .Range  = range,
                    .Format = format,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { Rhi::AccessBits::SHADER_RESOURCE, stages },
                Rhi::BufferUsageBits::SHADER_RESOURCE);
        }

        /// <summary>
        /// Read from vertex buffer resource
        /// </summary>
        void ReadIndirectBuffer(
            const ResourceViewId&   viewId,
            const Rhi::BufferRange& range = Rhi::c_EntireBuffer)
        {
            ReadBuffer(
                viewId,
                Rhi::BufferViewDesc{
                    .Range  = range,
                    .Format = Rhi::ResourceFormat::R32_UINT,
                    .Type   = Rhi::BufferViewType::ShaderResource },
                { Rhi::AccessBits::ARGUMENT_BUFFER, Rhi::StageBits::INDIRECT },
                Rhi::BufferUsageBits::ARGUMENT_BUFFER);
        }

    public:
        /// <summary>
        /// Read from texture resource
        /// </summary>
        void ReadTexture(
            const ResourceViewId&       viewId,
            const Rhi::TextureViewDesc& viewDesc,
            Rhi::StageBits              stages);

        /// <summary>
        /// Read from texture resource as copy destination
        /// </summary>
        void ReadCopyDstResource(
            const ResourceViewId& viewId);

        /// <summary>
        /// Read from texture as depth stencil
        /// </summary>
        void ReadDepthStencil(
            const ResourceViewId&       viewId,
            const Rhi::TextureViewDesc& viewDesc,
            Rhi::StageBits              stages);

    private:
        /// <summary>
        /// Append resource state
        /// </summary>
        void AppendResourceState(
            const ResourceViewId&   viewId,
            const Rhi::AccessStage& accessStage);

        /// <summary>
        /// Initialize texture layout
        /// </summary>
        void SetTextureLayout(
            const ResourceId& viewId,
            Rhi::LayoutType   layout);

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
} // namespace Ame::RG