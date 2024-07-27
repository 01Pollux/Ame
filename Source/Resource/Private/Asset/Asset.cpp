#include <Asset/Asset.hpp>

namespace Ame::Asset
{
    const Guid& IAsset::GetGuid() const noexcept
    {
        return m_AssetGuid;
    }

    const String& IAsset::GetPath() const noexcept
    {
        return m_AssetPath;
    }

    void IAsset::SetPath(
        String path) noexcept
    {
        m_AssetPath = std::move(path);
        MarkDirty();
    }

    void IAsset::MarkDirty(
        bool isDirty) noexcept
    {
        m_IsDirty = isDirty;
    }

    bool IAsset::IsDirty() const noexcept
    {
        return m_IsDirty;
    }
} // namespace Ame::Asset