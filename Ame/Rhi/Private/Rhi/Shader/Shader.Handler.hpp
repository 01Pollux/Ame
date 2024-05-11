#pragma once

#include <vector>
#include <map>

#include <Rhi/Shader/Shader.Common.hpp>
#include <Asset/Types/Common/TextFile.hpp>

namespace Ame::Asset
{
    class Storage;
} // namespace Ame::Asset

namespace Ame::Rhi
{
    class ShaderIncludeHandler : public IDxcIncludeHandler
    {
    private:
        using FileAssetType = Asset::Common::TextFileAsset;
        using FileAssetList = std::vector<Ptr<FileAssetType>>;
        using LoadedFileMap = std::map<WideString, ShaderUtil::CComPtr<IDxcBlobEncoding>>;

    public:
        ShaderIncludeHandler(
            IDxcUtils*          Utils,
            IDxcIncludeHandler* DefaultIncludeHandler,
            Asset::Storage*     AssetStorage);

        HRESULT STDMETHODCALLTYPE LoadSource(
            _In_ LPCWSTR                             FileName,
            _COM_Outptr_result_maybenull_ IDxcBlob** IncludeSourceBlob) override;

        HRESULT STDMETHODCALLTYPE QueryInterface(
            REFIID Riid,
            void** Object) override;

        ULONG STDMETHODCALLTYPE AddRef() override;

        ULONG STDMETHODCALLTYPE Release() override;

    private:
        IDxcUtils*          m_Utils;
        IDxcIncludeHandler* m_DefaultIncludeHandler;
        Asset::Storage*     m_AssetStorage;

        FileAssetList m_TextFiles;
        LoadedFileMap m_LoadedFiles;
    };
} // namespace Ame::Rhi