#include <Asset/Metadata.hpp>
#include <boost/property_tree/json_parser.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    AssetMetaDataDef::AssetMetaDataDef(
        std::istream& Stream)
    {
        boost::property_tree::read_json(Stream, m_MetaData);
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
        const Handle& AssetGuid,
        String        Path) :
        m_IsDirty(true)
    {
        SetGuid(AssetGuid);
        SetMetaPath(std::move(Path));
        m_MetaData.add_child("LoaderData", boost::property_tree::ptree());
        m_MetaData.add_child("Dependencies", boost::property_tree::ptree());
    }

    void AssetMetaDataDef::Export(
        std::ostream& Stream) const
    {
        boost::property_tree::write_json(Stream, m_MetaData);
    }

    //

    Handle AssetMetaDataDef::GetGuid() const noexcept
    {
        auto Iter = m_MetaData.find("Guid");
        return Iter != m_MetaData.not_found() ? Handle::FromString(Iter->second.get_value<std::string>()) : Handle::Null;
    }

    void AssetMetaDataDef::SetGuid(
        const Handle& Guid) noexcept
    {
        m_MetaData.put("Guid", Guid.ToString());
    }

    String AssetMetaDataDef::GetHash() const noexcept
    {
        auto Iter = m_MetaData.find("Hash");
        return Iter != m_MetaData.not_found() ? Iter->second.get_value<std::string>() : String();
    }

    void AssetMetaDataDef::SetHash(
        String Hash) noexcept
    {
        m_MetaData.put("Hash", std::move(Hash));
    }

    size_t AssetMetaDataDef::GetLoaderId() const noexcept
    {
        return m_MetaData.get_optional<size_t>("LoaderId").value_or(0);
    }

    void AssetMetaDataDef::SetLoaderId(
        size_t Id) noexcept
    {
        m_MetaData.put("LoaderId", Id);
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
        auto Path = m_MetaData.get<String>("Path");
        Log::Asset().Validate(!(Path.empty() || Path.starts_with("..")), "Path '{}' cannot be empty or start with '..'", Path);
        return Path;
    }

    void AssetMetaDataDef::SetMetaPath(
        String Path)
    {
        Log::Asset().Validate(!(Path.empty() || Path.starts_with("..")), "Path '{}' cannot be empty or start with '..'", Path);
        m_MetaData.put("Path", std::move(Path));
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

    Co::generator<Handle> AssetMetaDataDef::GetDependencies() const
    {
        for (auto& Dependency : m_MetaData.get_child("Dependencies"))
        {
            co_yield Handle::FromString(Dependency.second.get_value<String>());
        }
    }

    void AssetMetaDataDef::SetDependencies(
        std::span<String> Dependencies)
    {
        auto& DepsNode = m_MetaData.get_child("Dependencies");
        DepsNode.clear();
        for (auto& Dependency : Dependencies)
        {
            DepsNode.push_back({ "", boost::property_tree::ptree(std::move(Dependency)) });
        }
    }
} // namespace Ame::Asset