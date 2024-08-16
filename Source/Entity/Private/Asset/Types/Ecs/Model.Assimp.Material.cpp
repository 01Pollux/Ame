#include <map>
#include <fstream>
#include <filesystem>

#include <EcsComponent/Renderables/3D/Model.hpp>
#include <Asset/Types/Ecs/Model.Assimp.hpp>
#include <Image/ImageStorage.hpp>
#include <assimp/scene.h>

#include <Shading/Types/StandardMaterial3D.hpp>
#include <CommonStates/CommonRenderPasses.hpp>
#include <Rhi/Utils/SRBBinder.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Ecs
{
    [[nodiscard]] static Ptr<Dg::ITexture> LoadStandardTexture(
        Dg::IRenderDevice* renderDevice,
        const aiTexture*   aitexture)
    {
        Dg::TextureDesc desc{
            aitexture->mFilename.C_Str(),
            Dg::RESOURCE_DIM_TEX_2D,
            aitexture->mWidth,
            aitexture->mHeight,
            1,
            Dg::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM
        };

        Dg::TextureSubResData subresource{
            static_cast<const void*>(aitexture->pcData),
            aitexture->mWidth * aitexture->mHeight * sizeof(uint8_t[4])
        };
        Dg::TextureData textureData{ &subresource, 1 };

        Ptr<Dg::ITexture> texture;
        renderDevice->CreateTexture(desc, &textureData, &texture);

        return texture;
    }

    [[nodiscard]] static Ptr<Dg::ITexture> LoadTextureFromImage(
        Dg::IRenderDevice* renderDevice,
        const Rhi::Image&  image,
        const StringView   textureName)
    {
        auto size = image.GetSize();

        Dg::TextureDesc desc{
#ifndef AME_DIST
            textureName.data(),
#else
            nullptr,
#endif
            Dg::RESOURCE_DIM_TEX_2D,
            size.x(),
            size.y(),
            1,
            Dg::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM
        };
        desc.BindFlags = Dg::BIND_SHADER_RESOURCE;

        Dg::TextureSubResData subresource[]{ image.GetSubresource() };
        Dg::TextureData       textureData{ subresource, 1 };

        Ptr<Dg::ITexture> texture;
        renderDevice->CreateTexture(desc, &textureData, &texture);

        return texture;
    }

    //

    [[nodiscard]] static Ptr<Dg::ITexture> LoadTexture(
        Rhi::IRhiDevice*   rhiDevice,
        const aiScene*     scene,
        const String&      modelPath,
        const aiString&    texturePath,
        Rhi::CommonTexture placeholderType)
    {
        Ptr<Dg::ITexture> texture;
        if (texturePath.length)
        {
            const aiTexture* aitexture = scene->GetEmbeddedTexture(texturePath.C_Str());
            if (aitexture)
            {
                // first, check if the texture is not compressed
                if (aitexture->mHeight != 0)
                {
                    bool isSupported = std::strncmp(aitexture->achFormatHint, "rgba8888", sizeof(aiTexture::achFormatHint)) == 0;
                    if (isSupported)
                    {
                        texture = LoadStandardTexture(rhiDevice->GetRenderDevice(), aitexture);
                    }
                }
                else
                {
                    auto  imageMemory = Rhi::ImageStorage::Load(std::bit_cast<std::byte*>(aitexture->pcData), aitexture->mWidth);
                    auto& image       = imageMemory.GetImage();
                    texture           = LoadTextureFromImage(rhiDevice->GetRenderDevice(), image, { aitexture->mFilename.C_Str(), aitexture->mFilename.length });
                }
            }
            else
            {
                Rhi::Image image;

                auto          filePath = std::filesystem::path(modelPath) / texturePath.C_Str();
                std::ifstream file(filePath, std::ios::binary);
                if (file)
                {
                    auto format = Rhi::ImageStorage::GetFormat(file);
                    if (format != Rhi::ImageFormat::Unknown)
                    {
                        image   = Rhi::ImageStorage::Decode(format, file);
                        texture = LoadTextureFromImage(rhiDevice->GetRenderDevice(), image.ConvertTo32Bits(), filePath.filename().string());
                    }
                }
            }
        }

        if (!texture)
        {
            auto commonRenderPass = rhiDevice->GetCommonRenderPass();
            texture               = commonRenderPass->GetDefaultTexture(placeholderType);
        }

        return texture;
    }

    //

    struct TextureNameAndAiType
    {
        const char*        Name = nullptr;
        aiTextureType      Type = aiTextureType_NONE;
        Rhi::CommonTexture Placeholder;
    };

    struct TextureNameAndResource
    {
        const char*   Name    = nullptr;
        Dg::ITexture* Texture = nullptr;
    };

    //

    template<typename Ty, typename PlTy>
    void ApplyMaterialConstant(
        aiMaterial*    srcMaterial,
        Rhi::Material* dstMaterial,
        const char*    key,
        int            type,
        int            index,
        const char*    name,
        const PlTy&    placeholder)
    {
        Ty val;
        if (srcMaterial->Get(key, type, index, val) == aiReturn_SUCCESS)
        {
            dstMaterial->WriteUserData(name, std::bit_cast<const std::byte*>(std::addressof(val)), sizeof(val));
        }
        else
        {
            dstMaterial->WriteUserData(name, std::bit_cast<const std::byte*>(std::addressof(placeholder)), sizeof(placeholder));
        }
    }

    //

    void AssImpModelImporter::CreateMaterials(
        MeshModel::CreateDesc& createDesc,
        Rhi::IRhiDevice*       rhiDevice) const
    {
        const aiScene* scene = m_Importer.GetScene();

        if (!scene->HasMaterials())
        {
            return;
        }

        using StdMat3DNames = Rhi::StandardMaterial3D::Names;
        constexpr std::array c_TextureTypes{
            TextureNameAndAiType{ StdMat3DNames::BaseColorMap, aiTextureType_DIFFUSE, Rhi::CommonTexture::DevTexture },
            TextureNameAndAiType{ StdMat3DNames::NormalMap, aiTextureType_NORMALS, Rhi::CommonTexture::White2D },
            TextureNameAndAiType{ StdMat3DNames::SpecularMap, aiTextureType_SPECULAR, Rhi::CommonTexture::White2D },
            TextureNameAndAiType{ StdMat3DNames::RoughnessMap, aiTextureType_DIFFUSE_ROUGHNESS, Rhi::CommonTexture::White2D },
            TextureNameAndAiType{ StdMat3DNames::AmbientMap, aiTextureType_AMBIENT, Rhi::CommonTexture::White2D },
            TextureNameAndAiType{ StdMat3DNames::EmissiveMap, aiTextureType_EMISSIVE, Rhi::CommonTexture::Black2D },
            TextureNameAndAiType{ StdMat3DNames::HeightMap, aiTextureType_HEIGHT, Rhi::CommonTexture::Black2D },
            TextureNameAndAiType{ StdMat3DNames::ShininessMap, aiTextureType_SHININESS, Rhi::CommonTexture::Black2D },
            TextureNameAndAiType{ StdMat3DNames::OpacityMap, aiTextureType_OPACITY, Rhi::CommonTexture::White2D },
            TextureNameAndAiType{ StdMat3DNames::DisplacementMap, aiTextureType_DISPLACEMENT, Rhi::CommonTexture::Black2D },
            TextureNameAndAiType{ StdMat3DNames::LightMap, aiTextureType_LIGHTMAP, Rhi::CommonTexture::Black2D },
            TextureNameAndAiType{ StdMat3DNames::MetallicMap, aiTextureType_METALNESS, Rhi::CommonTexture::Black2D },
        };
        static_assert(Rhi::StandardMaterial3D::CreateDesc::StdResources.size() == c_TextureTypes.size(), "Texture types mismatch for StandardMaterial3D");

        //

        auto getOrCreateTexture = [this,
                                   rhiDevice,
                                   scene,
                                   textureCache = std::map<String, Ptr<Dg::ITexture>>()](
                                      const aiString&    texturePath,
                                      Rhi::CommonTexture placeholder) mutable -> Dg::ITexture*
        {
            auto iter = textureCache.find(texturePath.C_Str());
            if (iter != textureCache.end())
            {
                return iter->second;
            }

            auto texture = LoadTexture(rhiDevice, scene, m_ModelRootPath, texturePath, placeholder);
            return textureCache.emplace(texturePath.C_Str(), texture).first->second;
        };

        //

        auto transformAiTexture = [rhiDevice,
                                   texturePath = aiString(),
                                   &getOrCreateTexture](
                                      aiMaterial*          aimaterial,
                                      TextureNameAndAiType desc) mutable -> TextureNameAndResource
        {
            aimaterial->GetTexture(desc.Type, 0, &texturePath);
            auto texture = getOrCreateTexture(texturePath, desc.Placeholder);
            return TextureNameAndResource{ desc.Name, texture };
        };

        //

        //

        createDesc.Materials.reserve(scene->mNumMaterials);
        createDesc.Materials.emplace_back(Rhi::Material::Create(rhiDevice->GetRenderDevice(), Rhi::StandardMaterial3D::CreateDesc()));

        auto tryAppendMaterial = [&](uint32_t index) -> Ptr<Rhi::Material>&
        {
            if (index)
            {
                createDesc.Materials.emplace_back(createDesc.Materials[0]->Instantiate());
            }
            return createDesc.Materials.back();
        };

        //

        for (uint32_t i = 0; i < scene->mNumMaterials; i++)
        {
            auto& material    = tryAppendMaterial(i);
            auto  materialSrb = material->GetBindings();

            aiMaterial* aimaterial = scene->mMaterials[i];

            aiString name;
            if (aimaterial->Get(AI_MATKEY_NAME, name) == aiReturn_SUCCESS)
            {
                material->SetName(StringView(name.C_Str(), name.length));
            }

            auto allTextures = c_TextureTypes |
                               std::views::transform([&](auto& desc)
                                                     { return transformAiTexture(aimaterial, desc); });
            for (auto desc : allTextures)
            {
                Rhi::BindAllInSrb(materialSrb, Dg::SHADER_TYPE_ALL_GRAPHICS, desc.Name, desc.Texture->GetDefaultView(Dg::TEXTURE_VIEW_SHADER_RESOURCE));
            }

            ApplyMaterialConstant<aiColor4D>(aimaterial, material, AI_MATKEY_COLOR_DIFFUSE, StdMat3DNames::BaseColorCst, Colors::c_White);
            ApplyMaterialConstant<aiColor3D>(aimaterial, material, AI_MATKEY_COLOR_AMBIENT, StdMat3DNames::AmbientCst, Colors::c_Black);
            ApplyMaterialConstant<aiColor3D>(aimaterial, material, AI_MATKEY_COLOR_SPECULAR, StdMat3DNames::SpecularCst, Colors::c_Black);
            ApplyMaterialConstant<aiColor3D>(aimaterial, material, AI_MATKEY_COLOR_EMISSIVE, StdMat3DNames::EmissiveCst, Colors::c_Black);
            ApplyMaterialConstant<float>(aimaterial, material, AI_MATKEY_OPACITY, StdMat3DNames::TransparencyCst, 1.0f);
        }
    }
} // namespace Ame::Ecs