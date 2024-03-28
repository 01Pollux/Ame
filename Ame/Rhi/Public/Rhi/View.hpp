#pragma once

#include <Rhi/Resource.hpp>

namespace Ame::Rhi
{
    class ResourceView
    {
    public:
        ResourceView() = default;

        explicit ResourceView(
            nri::Descriptor* View) :
            m_Descriptor(View)
        {
        }

        operator bool() const noexcept
        {
            return m_Descriptor != nullptr;
        }

    public:
        /// <summary>
        /// Immediately releases the buffer.
        /// </summary>
        void Release();

        /// <summary>
        /// Defers the release of the buffer until the gpu is done with it.
        /// </summary>
        void DeferRelease();

    public:
        /// <summary>
        /// Set the buffer name.
        /// </summary>
        void SetName(
            const char* Name);

        /// <summary>
        /// Get the nri descriptor.
        /// </summary>
        [[nodiscard]] nri::Descriptor* Unwrap() const;

        /// <summary>
        /// Get the descriptor native handle.
        /// </summary>
        [[nodiscard]] void* GetNative() const;

    private:
        nri::Descriptor* m_Descriptor;
    };

    //

    class BufferResourceView : public ResourceView
    {
    public:
        using ResourceView::ResourceView;
    };

    class SamplerResourceView : public ResourceView
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
} // namespace Ame::Rhi