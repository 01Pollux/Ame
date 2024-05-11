#include <Core/Enum.hpp>
#include <Rhi/Shader/Shader.Handler.hpp>

#include <Asset/Storage.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    ShaderIncludeHandler::ShaderIncludeHandler(
        IDxcUtils*          Utils,
        IDxcIncludeHandler* DefaultIncludeHandler,
        Asset::Storage*     AssetStorage) :
        m_Utils(Utils),
        m_DefaultIncludeHandler(DefaultIncludeHandler),
        m_AssetStorage(AssetStorage)
    {
    }

    HRESULT __stdcall ShaderIncludeHandler::LoadSource(
        LPCWSTR    FileName,
        IDxcBlob** IncludeSourceBlob)
    {
        *IncludeSourceBlob = nullptr;

        auto Iter = m_LoadedFiles.find(FileName);
        if (Iter != m_LoadedFiles.end())
        {
            *IncludeSourceBlob = Iter->second.Detach();
            return S_OK;
        }

        if (FileName[0] == '.' && FileName[1] == '/')
        {
            FileName += 2;
        }

        // Skip all "../" in the path just to reach the root of the shader directory.
        while (FileName && FileName[0] == '.' && FileName[1] == '.' && FileName[2] == '/')
        {
            FileName += 3;
        }

        if (!FileName || !FileName[0]) [[unlikely]]
        {
            return E_FAIL;
        }

        String PathToAsset = std::format("Shaders/{}", Strings::To<String>(FileName));

        using namespace EnumBitOperators;

        auto Handle = m_AssetStorage->FindAsset(PathToAsset, Asset::PackageFlags::Disk | Asset::PackageFlags::Memory).Guid;
        if (Handle.is_nil()) [[unlikely]]
        {
            return E_FAIL;
        }

        auto& Manager  = m_AssetStorage->GetManager();
        auto  TextFile = m_TextFiles.emplace_back(std::dynamic_pointer_cast<FileAssetType>(Manager.Load(Handle, true)));
        if (!TextFile) [[unlikely]]
        {
            m_TextFiles.pop_back();
            return E_FAIL;
        }

        auto& SourceCode = TextFile->Get();

        ShaderUtil::CComPtr<IDxcBlobEncoding> ShaderCodeBlob;

        HRESULT Res = m_Utils->CreateBlobFromPinned(
            SourceCode.data(),
            static_cast<uint32_t>(SourceCode.size()),
            DXC_CP_UTF8,
            &ShaderCodeBlob);

        if (SUCCEEDED(Res))
        {
            *IncludeSourceBlob = ShaderCodeBlob.Detach();
            m_LoadedFiles.emplace(FileName, std::move(ShaderCodeBlob));
            return S_OK;
        }
        else
        {
            m_TextFiles.pop_back();
        }
        return E_FAIL;
    }

    HRESULT __stdcall ShaderIncludeHandler::QueryInterface(
        REFIID Riid,
        void** Object)
    {
        return m_DefaultIncludeHandler->QueryInterface(Riid, Object);
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