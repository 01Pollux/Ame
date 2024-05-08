#include <Asset/Asset.hpp>

namespace Ame::Asset
{
    IAsset::IAsset(
        const Handle& AssetGuid,
        String        Path) :
        m_AssetGuid(AssetGuid),
        m_AssetPath(std::move(Path))
    {
    }

    const Handle& IAsset::GetGuid() const noexcept
    {
        return m_AssetGuid;
    }

    const String& IAsset::GetPath() const noexcept
    {
        return m_AssetPath;
    }

    void IAsset::SetPath(
        String Path) noexcept
    {
        m_AssetPath = std::move(Path);
        MarkDirty();
    }

    void IAsset::MarkDirty(
        bool IsDirty) noexcept
    {
        m_IsDirty = IsDirty;
    }

    bool IAsset::IsDirty() const noexcept
    {
        return m_IsDirty;
    }
} // namespace Ame::Asset