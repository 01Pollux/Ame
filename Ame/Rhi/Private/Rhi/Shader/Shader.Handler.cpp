#include <Core/Enum.hpp>
#include <Rhi/Shader/Shader.Handler.hpp>

#include <Asset/Storage.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    ShaderIncludeHandler::ShaderIncludeHandler(
        IDxcUtils*          dxcUtils,
        IDxcIncludeHandler* defaultIncludeHandler,
        Asset::Storage*     assetStorage) :
        m_Utils(dxcUtils),
        m_DefaultIncludeHandler(defaultIncludeHandler),
        m_AssetStorage(assetStorage)
    {
    }

    HRESULT STDMETHODCALLTYPE ShaderIncludeHandler::LoadSource(
        _In_ LPCWSTR                             fileName,
        _COM_Outptr_result_maybenull_ IDxcBlob** includeSourceBlob)
    {
        *includeSourceBlob = nullptr;

        auto foundFile = m_LoadedFiles.find(fileName);
        if (foundFile != m_LoadedFiles.end())
        {
            *includeSourceBlob = foundFile->second.Detach();
            return S_OK;
        }

        if (fileName[0] == '.' && fileName[1] == '/')
        {
            fileName += 2;
        }

        // Skip all "../" in the path just to reach the root of the shader directory.
        while (fileName && fileName[0] == '.' && fileName[1] == '.' && fileName[2] == '/')
        {
            fileName += 3;
        }

        if (!fileName || !fileName[0]) [[unlikely]]
        {
            return E_FAIL;
        }

        auto       fileNameStr = Strings::To<String>(fileName);
        std::regex pathToAsset(std::format(".*{}", fileNameStr));

        using namespace EnumBitOperators;

        auto assetIter   = m_AssetStorage->FindAssets(pathToAsset);
        auto firstHandle = assetIter.begin();

        if (firstHandle == assetIter.end()) [[unlikely]]
        {
            return E_FAIL;
        }

        auto handle = firstHandle->Handle;
        if (handle.is_nil()) [[unlikely]]
        {
            return E_FAIL;
        }

        if (assetIter.begin() != assetIter.end()) [[unlikely]]
        {
            Log::Rhi().Warning("Multiple assets found for path: {}, taking the first one {}", fileNameStr, handle.ToString());
        }

        auto& assetManager = m_AssetStorage->GetManager();
        auto  textFile     = m_TextFiles.emplace_back(std::dynamic_pointer_cast<FileAssetType>(assetManager.Load(handle, true)));
        if (!textFile) [[unlikely]]
        {
            m_TextFiles.pop_back();
            return E_FAIL;
        }

        ShaderUtil::CComPtr<IDxcBlobEncoding> shaderCodeBlob;

        auto& sourceCode = textFile->Get();
        auto  res        = m_Utils->CreateBlobFromPinned(
            sourceCode.data(),
            static_cast<uint32_t>(sourceCode.size()),
            DXC_CP_UTF8,
            &shaderCodeBlob);

        if (SUCCEEDED(res))
        {
            *includeSourceBlob = shaderCodeBlob.Detach();
            m_LoadedFiles.emplace(fileName, std::move(shaderCodeBlob));
            return S_OK;
        }
        else
        {
            m_TextFiles.pop_back();
        }
        return E_FAIL;
    }

    HRESULT __stdcall ShaderIncludeHandler::QueryInterface(
        REFIID riid,
        void** object)
    {
        return m_DefaultIncludeHandler->QueryInterface(riid, object);
    }

    ULONG __stdcall ShaderIncludeHandler::AddRef()
    {
        return 1;
    }

    ULONG __stdcall ShaderIncludeHandler::Release()
    {
        return 1;
    }
} // namespace Ame::Rhi