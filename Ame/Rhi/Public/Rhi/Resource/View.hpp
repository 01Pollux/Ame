#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Descs/View.hpp>

namespace Ame::Rhi
{
    class ResourceView
    {
    public:
        explicit ResourceView(
            nri::Descriptor* View = nullptr) :
            m_Descriptor(View)
        {
        }

        operator bool() const noexcept
        {
            return m_Descriptor != nullptr;
        }

    public:
        /// <summary>
        /// Immediately releases the resource view.
        /// </summary>
        void Release(
            Device& RhiDevice);

        /// <summary>
        /// Defers the release of the resource view until the gpu is done with it.
        /// </summary>
        void DeferRelease(
            Device& RhiDevice);

    public:
        /// <summary>
        /// Set the resource view name.
        /// </summary>
        void SetName(
            Device&     RhiDevice,
            const char* Name) const;

        /// <summary>
        /// Get the nri descriptor.
        /// </summary>
        [[nodiscard]] nri::Descriptor* Unwrap() const;

        /// <summary>
        /// Get the descriptor native handle.
        /// </summary>
        [[nodiscard]] void* GetNative(
            Device& RhiDevice) const;

    private:
        nri::Descriptor* m_Descriptor;
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
        using ResourceView::ResourceView;
    };
} // namespace Ame::Rhi