#include <boost/property_tree/json_parser.hpp>
#include <Asset/Metadata.hpp>
#include <FileSystem/Path.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    AssetMetaDataDef::AssetMetaDataDef(
        std::istream& stream)
    {
        boost::property_tree::read_json(stream, m_MetaData);
        if (m_MetaData.find("LoaderData") == m_MetaData.not_found())
        {
            m_MetaData.put_child("LoaderData", boost::property_tree::ptree());
        }
        if (m_MetaData.find("Dependencies") == m_MetaData.not_found())
        {
            m_MetaData.put_child("Dependencies", boost::property_tree::ptree());
        }
    }

    AssetMetaDataDef::AssetMetaDataDef(
        const UId& uid,
        String     path) :
        m_IsDirty(true)
    {
        SetGuid(uid);
        SetMetaPath(std::move(path));
        m_MetaData.add_child("LoaderData", boost::property_tree::ptree());
        m_MetaData.add_child("Dependencies", boost::property_tree::ptree());
    }

    void AssetMetaDataDef::Export(
        std::ostream& stream) const
    {
        boost::property_tree::write_json(stream, m_MetaData);
    }

    //

    UId AssetMetaDataDef::GetUId() const noexcept
    {
        auto Iter = m_MetaData.find("Guid");
        return Iter != m_MetaData.not_found() ? UIdUtils::FromString(Iter->second.get_value<String>()) : UIdUtils::Null();
    }

    void AssetMetaDataDef::SetGuid(
        const UId& uid) noexcept
    {
        m_MetaData.put("Guid", UIdUtils::ToString(uid));
    }

    String AssetMetaDataDef::GetHash() const noexcept
    {
        auto iter = m_MetaData.find("Hash");
        return iter != m_MetaData.not_found() ? iter->second.get_value<String>() : String();
    }

    void AssetMetaDataDef::SetHash(
        String hash) noexcept
    {
        m_MetaData.put("Hash", std::move(hash));
    }

    UId AssetMetaDataDef::GetLoaderId() const noexcept
    {
        auto uid = m_MetaData.get_optional<String>("LoaderId");
        return uid ? UIdUtils::FromString(uid.get()) : UIdUtils::Null();
    }

    void AssetMetaDataDef::SetLoaderId(
        const UId& uid) noexcept
    {
        m_MetaData.put("LoaderId", UIdUtils::ToString(uid));
    }

    AssetMetaData& AssetMetaDataDef::GetLoaderData() noexcept
    {
        return m_MetaData.get_child("LoaderData");
    }

    const AssetMetaData& AssetMetaDataDef::GetLoaderData() const noexcept
    {
        return m_MetaData.get_child("LoaderData");
    }

    std::filesystem::path AssetMetaDataDef::GetAssetPath() const
    {
        return FileSystem::ConvertToUnixPath(std::filesystem::path(GetMetaPath()).replace_extension(""));
    }

    std::filesystem::path AssetMetaDataDef::GetMetaPath() const
    {
        auto path = m_MetaData.get<String>("Path");
        Log::Asset().Validate(!(path.empty() || path.starts_with("..")), "Path '{}' cannot be empty or start with '..'", path);
        return path;
    }

    void AssetMetaDataDef::SetMetaPath(
        String path)
    {
        Log::Asset().Validate(!(path.empty() || path.starts_with("..")), "Path '{}' cannot be empty or start with '..'", path);
        m_MetaData.put("Path", std::move(path));
    }

    bool AssetMetaDataDef::IsDirty() const noexcept
    {
        return m_IsDirty;
    }

    void AssetMetaDataDef::SetDirty(
        bool IsDirty) noexcept
    {
        m_IsDirty = IsDirty;
    }

    Co::generator<UId> AssetMetaDataDef::GetDependencies() const
    {
        for (auto& dependency : m_MetaData.get_child("Dependencies"))
        {
            co_yield UIdUtils::FromString(dependency.second.get_value<String>());
        }
    }

    void AssetMetaDataDef::SetDependencies(
        std::span<String> Dependencies)
    {
        auto& depsnode = m_MetaData.get_child("Dependencies");
        depsnode.clear();
        for (auto& dependency : Dependencies)
        {
            depsnode.push_back({ "", boost::property_tree::ptree(std::move(dependency)) });
        }
    }
} // namespace Ame::Asset