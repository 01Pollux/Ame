#pragma once

#include <Module/Submodule.hpp>
#include <Core/Signal.hpp>

namespace Ame::Signals
{
    AME_SIGNAL_DECL(OnRenderBegin, void());
    AME_SIGNAL_DECL(OnRenderUpdate, void());
    AME_SIGNAL_DECL(OnRenderPostUpdate, void());
    AME_SIGNAL_DECL(OnRenderEnd, void());
} // namespace Ame::Signals

namespace Ame::Rhi
{
    class IRhiDevice;
} // namespace Ame::Rhi

namespace Ame
{
    class RhiGraphicsSubmodule;

    // {1EE289FB-1A56-4AC1-9D2B-7CF354B9A9BE}
    static const UId IID_RendererSubmodule = { 0x1ee289fb, 0x1a56, 0x4ac1, { 0x9d, 0x2b, 0x7c, 0xf3, 0x54, 0xb9, 0xa9, 0xbe } };

    class RendererSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(
            IID_RendererSubmodule, IID_BaseSubmodule, Base);

    public:
        RendererSubmodule(
            IReferenceCounters*   counters,
            RhiGraphicsSubmodule* rhiGraphicsSubmodule,
            uint32_t              syncInterval);

        /// <summary>
        /// Returns true if the application should continue running.
        /// </summary>
        [[nodiscard]] bool Tick();

    public:
        [[nodiscard]] uint32_t GetSyncInterval() const noexcept
        {
            return m_SyncInterval;
        }

        void SetSyncInterval(
            uint32_t syncInterval) noexcept
        {
            m_SyncInterval = syncInterval;
        }

    public:
        AME_SIGNAL_INST(OnRenderBegin);
        AME_SIGNAL_INST(OnRenderUpdate);
        AME_SIGNAL_INST(OnRenderPostUpdate);
        AME_SIGNAL_INST(OnRenderEnd);

    private:
        Ptr<Rhi::IRhiDevice> m_RhiDevice;

        uint32_t m_SyncInterval = 0;
    };
} // namespace Ame