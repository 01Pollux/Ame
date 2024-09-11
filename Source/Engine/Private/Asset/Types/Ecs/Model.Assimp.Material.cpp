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

namespace Ame::Ecs
{
    [[nodiscard]] static Ptr<Dg::ITexture> LoadStandardTexture(Dg::IRenderDevice* renderDevice,
                                                               const aiTexture* aitexture, const char* textureName)
    {
        Dg::TextureDesc desc{ textureName,
                              Dg::RESOURCE_DIM_TEX_2D,
                              aitexture->mWidth,
                              aitexture->mHeight,
                              1,
                              Dg::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM };

        Dg::TextureSubResData subresource{ static_cast<const void*>(aitexture->pcData),
                                           aitexture->mWidth * aitexture->mHeight * sizeof(uint8_t[4]) };
        Dg::TextureData       textureData{ &subresource, 1 };

        Ptr<Dg::ITexture> texture;
        renderDevice->CreateTexture(desc, &textureData, &texture);

        return texture;
    }

    [[nodiscard]] static Ptr<Dg::ITexture> LoadTextureFromImage(Dg::IRenderDevice* renderDevice,
                                                                const Rhi::Image& image, const char* textureName)
    {
        auto size = image.GetSize();

        Dg::TextureDesc desc{ textureName, Dg::RESOURCE_DIM_TEX_2D, size.x(), size.y(), 1, Dg::TEX_FORMAT_RGBA8_UNORM };
        desc.BindFlags = Dg::BIND_SHADER_RESOURCE;

        Dg::TextureSubResData subresource[]{ image.GetSubresource() };
        Dg::TextureData       textureData{ subresource, 1 };

        Ptr<Dg::ITexture> texture;
        renderDevice->CreateTexture(desc, &textureData, &texture);

        return texture;
    }

    //

    [[nodiscard]] static Ptr<Dg::ITexture> LoadTexture(Rhi::IRhiDevice* rhiDevice, const aiScene* scene,
                                                       const String& modelPath, const aiString& texturePath,
                                                       const char* textureName, Rhi::CommonTexture placeholderType)
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
                    bool isSupported =
                        std::strncmp(aitexture->achFormatHint, "rgba8888", sizeof(aiTexture::achFormatHint)) == 0;
                    if (isSupported)
                    {
                        texture = LoadStandardTexture(rhiDevice->GetRenderDevice(), aitexture, textureName);
                    }
                }
                else
                {
                    auto imageMemory =
                        Rhi::ImageStorage::Load(std::bit_cast<std::byte*>(aitexture->pcData), aitexture->mWidth);
                    auto image = imageMemory.GetImage();

                    if (image)
                    {
                        image = image.ConvertTo32Bits();
                    }
                    if (image)
                    {
                        image.FlipVertical();
                        texture = LoadTextureFromImage(rhiDevice->GetRenderDevice(), image, textureName);
                    }
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
                        image = Rhi::ImageStorage::Decode(format, file);
                        if (image)
                        {
                            image = image.ConvertTo32Bits();
                        }
                        if (image)
                        {
                            image.FlipVertical();
                            texture = LoadTextureFromImage(rhiDevice->GetRenderDevice(), image, textureName);
                        }
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

    struct TextureLoadDesc
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
    void ApplyMaterialConstant(aiMaterial* srcMaterial, Rhi::Material* dstMaterial, const char* key, int type,
                               int index, const char* name, const PlTy& placeholder)
    {
        Ty val;
        if (srcMaterial->Get(key, type, index, val) == aiReturn_SUCCESS)
        {
            dstMaterial->WriteUserData(name, std::bit_cast<const std::byte*>(std::addressof(val)), sizeof(val));
        }
        else
        {
            dstMaterial->WriteUserData(
                name, std::bit_cast<const std::byte*>(std::addressof(placeholder)), sizeof(placeholder));
        }
    }

    //

    void AssImpModelImporter::CreateMaterials(MeshModel::CreateDesc& createDesc, Rhi::IRhiDevice* rhiDevice) const
    {
        const aiScene* scene = m_Importer.GetScene();
        if (!scene->HasMaterials())
        {
            return;
        }

        using StdMat3DNames = Rhi::StandardMaterial3D::Names;
        constexpr std::array c_TextureTypes{
            TextureLoadDesc{ StdMat3DNames::BaseColorMap, aiTextureType_BASE_COLOR, Rhi::CommonTexture::DevTexture },
            TextureLoadDesc{ StdMat3DNames::NormalMap, aiTextureType_NORMALS, Rhi::CommonTexture::Black2D },
            TextureLoadDesc{ StdMat3DNames::AOMap, aiTextureType_LIGHTMAP, Rhi::CommonTexture::White2D },
            TextureLoadDesc{ StdMat3DNames::EmissiveMap, aiTextureType_EMISSIVE, Rhi::CommonTexture::Black2D },
            TextureLoadDesc{ StdMat3DNames::SpecularMap, aiTextureType_SHININESS, Rhi::CommonTexture::Black2D },
            TextureLoadDesc{ StdMat3DNames::Roughness_MetallicMap, aiTextureType_DIFFUSE_ROUGHNESS,
                             Rhi::CommonTexture::Black2D },
            TextureLoadDesc{ StdMat3DNames::HeightMap, aiTextureType_HEIGHT, Rhi::CommonTexture::Black2D },
        };
        static_assert(Rhi::StandardMaterial3D::CreateDesc::StdResources.size() == c_TextureTypes.size(),
                      "Texture types mismatch for StandardMaterial3D");

        //

        auto getOrCreateTexture = [this, rhiDevice, scene, textureCache = std::map<String, Ptr<Dg::ITexture>>()](
                                      [[maybe_unused]] Rhi::Material* material, [[maybe_unused]] const StringView name,
                                      const aiString&    texturePath,
                                      Rhi::CommonTexture placeholder) mutable -> Dg::ITexture*
        {
            auto iter = textureCache.find(texturePath.C_Str());
            if (iter != textureCache.end())
            {
                return iter->second;
            }

#ifndef AME_DIST
            String      textureName    = std::format("{}_{}", material->GetName(), name);
            const char* textureNameStr = textureName.c_str();
#else
            const char* textureNameStr = nullptr;
#endif

            auto texture = LoadTexture(rhiDevice, scene, m_ModelRootPath, texturePath, textureNameStr, placeholder);
            return textureCache.emplace(texturePath.C_Str(), texture).first->second;
        };

        //

        auto transformAiTexture = [rhiDevice, texturePath = aiString(), &getOrCreateTexture](
                                      aiMaterial* aimaterial, Rhi::Material* material,
                                      TextureLoadDesc desc) mutable -> TextureNameAndResource
        {
            if (aimaterial->GetTextureCount(desc.Type))
            {
                aimaterial->GetTexture(desc.Type, 0, &texturePath);
            }
            auto texture = getOrCreateTexture(material, desc.Name, texturePath, desc.Placeholder);
            texturePath.Clear();
            return TextureNameAndResource{ desc.Name, texture };
        };

        //

        //

        createDesc.Materials.reserve(scene->mNumMaterials);
        createDesc.Materials.emplace_back(
            Rhi::Material::Create(rhiDevice->GetRenderDevice(), Rhi::StandardMaterial3D::CreateDesc()));

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

            auto allTextures =
                c_TextureTypes |
                std::views::transform([&](auto& desc) { return transformAiTexture(aimaterial, material, desc); });
            for (auto desc : allTextures)
            {
                Rhi::BindAllInSrb(materialSrb, Dg::SHADER_TYPE_ALL_GRAPHICS, desc.Name,
                                  desc.Texture->GetDefaultView(Dg::TEXTURE_VIEW_SHADER_RESOURCE));
            }

            ApplyMaterialConstant<aiColor4D>(
                aimaterial, material, AI_MATKEY_COLOR_DIFFUSE, StdMat3DNames::BaseColorCst, Colors::c_White);
            ApplyMaterialConstant<aiColor3D>(
                aimaterial, material, AI_MATKEY_COLOR_AMBIENT, StdMat3DNames::AmbientCst, Colors::c_Black);
            ApplyMaterialConstant<aiColor3D>(
                aimaterial, material, AI_MATKEY_COLOR_EMISSIVE, StdMat3DNames::EmissiveCst, Colors::c_Black);
            ApplyMaterialConstant<float>(aimaterial, material, AI_MATKEY_SHININESS, StdMat3DNames::SpecularCst, 0.f);
            ApplyMaterialConstant<float>(aimaterial, material, AI_MATKEY_OPACITY, StdMat3DNames::TransparencyCst, 1.0f);
        }
    }
} // namespace Ame::Ecs