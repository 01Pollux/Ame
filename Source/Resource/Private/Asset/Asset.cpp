#include <Asset/Asset.hpp>

namespace Ame::Asset
{
    const UId& IAsset::GetUId() const noexcept
    {
        return m_AssetGuid;
    }

    void IAsset::SetUId(
        const UId& uid) noexcept
    {
        m_AssetGuid = uid;
    }

    const String& IAsset::GetPath() const noexcept
    {
        return m_AssetPath;
    }

    void IAsset::SetPath(
        const String& path) noexcept
    {
        m_AssetPath = path;
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