#pragma once

#include <Asset/Handle.hpp>
#include <exception>

namespace Ame::Asset
{
    class AssetException : public std::runtime_error
    {
    public:
        AssetException(
            const Handle& AssetGuid,
            const char*   Message) noexcept :
            runtime_error(std::format("{} '{}'", Message, AssetGuid.ToString())),
            m_AssetGuid(AssetGuid)
        {
        }

        AssetException(
            const Handle& ParentGuid,
            const Handle& ChildGuid,
            const char*   Message) noexcept :
            runtime_error(std::format("{} '{}::{}'", Message, ParentGuid.ToString(), ChildGuid.ToString())),
            m_AssetGuid(ParentGuid)
        {
        }

        [[nodiscard]] const Handle& GetAssetGuid() const noexcept
        {
            return m_AssetGuid;
        }

    private:
        const Handle m_AssetGuid;
    };

    class InvalidAssetTypeException : public AssetException
    {
    public:
        InvalidAssetTypeException(
            const Handle& AssetGuid) noexcept :
            AssetException(AssetGuid, "Invalid asset type")
        {
        }
    };

    class AssetNotFoundException : public AssetException
    {
    public:
        AssetNotFoundException(
            const Handle& AssetGuid = Handle::Null) noexcept :
            AssetException(AssetGuid, "Asset not found")
        {
        }

        AssetNotFoundException(
            const Handle& ParentGuid,
            const Handle& ChildGuid) noexcept :
            AssetException(ParentGuid, ChildGuid, "Asset not found in meta file")
        {
        }
    };

    class AssetWithNoHandlerException : public AssetException
    {
    public:
        AssetWithNoHandlerException(
            const Handle& AssetGuid) noexcept :
            AssetException(AssetGuid, "Asset with no handler")
        {
        }

        AssetWithNoHandlerException(
            const Handle& ParentGuid,
            const Handle& ChildGuid) noexcept :
            AssetException(ParentGuid, ChildGuid, "Asset with no handler")
        {
        }
    };

    class AssetHandlerFailureException : public AssetException
    {
    public:
        AssetHandlerFailureException(
            const Handle& AssetGuid) noexcept :
            AssetException(AssetGuid, "Asset handler failure")
        {
        }

        AssetHandlerFailureException(
            const Handle& ParentGuid,
            const Handle& ChildGuid) noexcept :
            AssetException(ParentGuid, ChildGuid, "Asset handler failure")
        {
        }
    };

    class AssetChildMetaNotFoundException : public AssetException
    {
    public:
        AssetChildMetaNotFoundException(
            const Handle& ParentGuid,
            const Handle& ChildGuid) noexcept :
            AssetException(ParentGuid, ChildGuid, "Asset child not found in meta file")
        {
        }
    };
} // namespace Ame::Asset
