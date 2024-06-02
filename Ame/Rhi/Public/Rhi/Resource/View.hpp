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
            DeviceImpl*      rhiDevice,
            nri::Descriptor* nriDescriptor);

        ResourceView(
            Device&            rhiDevice,
            const SamplerDesc& desc);

    public:
        struct Extern
        {
        };

        ResourceView() = default;
        ResourceView(std::nullptr_t) :
            m_Owning(false)
        {
        }

        ResourceView(
            Extern,
            DeviceImpl&      rhiDevice,
            nri::Descriptor* descriptor);
        ResourceView(
            Extern,
            Device&          rhiDevice,
            nri::Descriptor* descriptor);

    public:
        ResourceView(const ResourceView&);
        ResourceView(ResourceView&& other) noexcept;

        ResourceView& operator=(const ResourceView&);
        ResourceView& operator=(ResourceView&& other) noexcept;

        ~ResourceView();

        auto operator<=>(
            const ResourceView& other) const
        {
            return m_Descriptor == other.m_Descriptor;
        }

        explicit operator bool() const noexcept
        {
            return m_Descriptor != nullptr;
        }

    public:
        /// <summary>
        /// Set the resource view name.
        /// </summary>
        void SetName(
            const char* name) const;

        /// <summary>
        /// Get the nri descriptor.
        /// </summary>
        [[nodiscard]] nri::Descriptor* Unwrap() const;

        /// <summary>
        /// Get the descriptor native handle.
        /// </summary>
        [[nodiscard]] void* GetNative() const;

        /// <summary>
        /// Check if the resource view is owning. (If the resource view is owning, it will be released when the resource view is destroyed)
        /// </summary>
        [[nodiscard]] bool IsOwning() const;

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
            DeviceImpl&      rhiDevice,
            nri::Descriptor* descriptor);
        SamplerResourceView(
            Extern,
            Device&          rhiDevice,
            nri::Descriptor* descriptor);
        SamplerResourceView(std::nullptr_t) :
            ResourceView(nullptr)
        {
        }

        SamplerResourceView(
            Device&            rhiDevice,
            const SamplerDesc& desc);

        SamplerResourceView(const SamplerResourceView&)           = delete;
        SamplerResourceView(SamplerResourceView&& other) noexcept = default;

        SamplerResourceView& operator=(const SamplerResourceView&)           = delete;
        SamplerResourceView& operator=(SamplerResourceView&& other) noexcept = default;

        ~SamplerResourceView() = default;
    };
} // namespace Ame::Rhi