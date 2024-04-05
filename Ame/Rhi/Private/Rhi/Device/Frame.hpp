#pragma once

#include <Rhi/Core.hpp>
#include "../Nri/Nri.hpp"

namespace Ame::Rhi
{
    class Frame : public NonCopyable,
                  public NonMovable
    {
    public:
        /// <summary>
        /// Initializes the frame resource.
        /// </summary>
        void Initialize(
            nri::CoreInterface& NriCore,
            nri::CommandQueue&  GraphicsQueue);

        /// <summary>
        /// Cleans up the frame resource.
        /// </summary>
        void Shutdown(
            nri::CoreInterface& NriCore);

    public:
        /// <summary>
        /// Get the command list.
        /// </summary>
        [[nodiscard]] nri::CommandBuffer* GetCommandList() const noexcept;

    public:
        /// <summary>
        /// Resets the frame resource and cleans up pending resources.
        /// </summary>
        void NewFrame(
            nri::CoreInterface& NriCore);

        /// <summary>
        /// Ends the frame resource and submits the command buffer.
        /// </summary>
        void EndFrame(
            nri::CoreInterface& NriCore);

    private:
        nri::CommandAllocator* m_CommandAllocator = nullptr;
        nri::CommandBuffer*    m_CommandBuffer    = nullptr;
    };
} // namespace Ame::Rhi