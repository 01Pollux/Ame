#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Descs/View.hpp>

namespace Ame::Rhi
{
    class ResourceView
    {
        friend class Texture;
        friend class Buffer;

    protected:
        ResourceView(
            DeviceImpl*      RhiDevice,
            nri::Descriptor* Descriptor);

        ResourceView(
            Device&            RhiDevice,
            const SamplerDesc& Desc);

    public:
        struct Extern
        {
        };

        ResourceView() = default;
        ResourceView(
            Extern,
            DeviceImpl&      RhiDevice,
            nri::Descriptor* Descriptor);
        ResourceView(
            Extern,
            Device&          RhiDevice,
            nri::Descriptor* Descriptor);
        ResourceView(std::nullptr_t) :
            m_Owning(false)
        {
        }

        ResourceView(const ResourceView&) = delete;
        ResourceView(ResourceView&& Other) noexcept;

        ResourceView& operator=(const ResourceView&) = delete;
        ResourceView& operator=(ResourceView&& Other) noexcept;

        ~ResourceView();

        operator bool() const noexcept
        {
            return m_Descriptor != nullptr;
        }

    public:
        /// <summary>
        /// Set the resource view name.
        /// </summary>
        void SetName(
            const char* Name) const;

        /// <summary>
        /// Get the nri descriptor.
        /// </summary>
        [[nodiscard]] nri::Descriptor* Unwrap() const;

        /// <summary>
        /// Get the descriptor native handle.
        /// </summary>
        [[nodiscard]] void* GetNative() const;

    private:
        /// <summary>
        /// Releases the resource view.
        /// </summary>
        void Release();

    protected:
        DeviceImpl*      m_Device     = nullptr;
        nri::Descriptor* m_Descriptor = nullptr;
        bool             m_Owning     = true;
    };

    //

    class BufferResourceView : public ResourceView
    {
    public:
        using ResourceView::ResourceView;
    };

    class ShaderResourceView : public ResourceView
    {
    public:
        using ResourceView::ResourceView;
    };

    class UnorderedAccessResourceView : public ResourceView
    {
    public:
        using ResourceView::ResourceView;
    };

    class RenderTargetResourceView : public ResourceView
    {
    public:
        using ResourceView::ResourceView;
    };

    class DepthStencilResourceView : public ResourceView
    {
    public:
        using ResourceView::ResourceView;
    };

    class SamplerResourceView : public ResourceView
    {
    public:
        SamplerResourceView() = default;
        SamplerResourceView(
            Extern,
            DeviceImpl&      RhiDevice,
            nri::Descriptor* Descriptor);
        SamplerResourceView(
            Extern,
            Device&          RhiDevice,
            nri::Descriptor* Descriptor);
        SamplerResourceView(std::nullptr_t) :
            ResourceView(nullptr)
        {
        }

        SamplerResourceView(
            Device&            RhiDevice,
            const SamplerDesc& Desc);

        SamplerResourceView(const SamplerResourceView&)           = delete;
        SamplerResourceView(SamplerResourceView&& Other) noexcept = default;

        SamplerResourceView& operator=(const SamplerResourceView&)           = delete;
        SamplerResourceView& operator=(SamplerResourceView&& Other) noexcept = default;

        ~SamplerResourceView() = default;
    };
} // namespace Ame::Rhi