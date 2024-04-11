#pragma once

#include <Core/Ame.hpp>
#include <Rhi/Core.hpp>

#include <Rhi/Resource/CommandListImpl.hpp>

#include <Rhi/Nri/Nri.hpp>

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
            nri::CommandQueue&  GraphicsQueue,
            uint32_t            FrameIndex);

        /// <summary>
        /// Cleans up the frame resource.
        /// </summary>
        void Shutdown(
            nri::CoreInterface& NriCore);

    public:
        /// <summary>
        /// Get the command list.
        /// </summary>
        [[nodiscard]] CommandListImpl& GetCommandList() noexcept;

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
        CommandListImpl m_CommandList;
    };
} // namespace Ame::Rhi