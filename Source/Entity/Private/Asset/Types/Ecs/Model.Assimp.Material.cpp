#include <map>
#include <fstream>

#include <EcsComponent/Renderables/3D/Model.hpp>
#include <Asset/Types/Ecs/Model.Assimp.hpp>
#include <Image/ImageStorage.hpp>

#include <assimp/scene.h>

#include <Log/Wrapper.hpp>

namespace Ame::Ecs
{
    // Check aiTexture::achFormatHint
    [[nodiscard]] static Dg::TEXTURE_FORMAT AIDiscoverTextureFormat(
        const char* format)
    {
        constexpr std::array c_Types{
            std::pair{ "rgba8888", Dg::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM },
            std::pair{ "argb8888", Dg::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM },
            std::pair{ "rgba5650", Dg::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM },
            std::pair{ "rgba0010", Dg::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM },
        };
    }

    //

    [[nodiscard]] Ptr<Dg::ITexture> LoadDummyTexture(
        Dg::IRenderDevice* renderDevice,
        const char*        textureName)
    {
#ifndef AME_DIST
        auto name = std::format("Invalid Texture: {}", textureName);
#endif
        Dg::TextureDesc desc{
#ifndef AME_DIST
            name.c_str(),
#else
            nullptr,
#endif
            Dg::RESOURCE_DIM_TEX_2D,
            1,
            1,
            1,
            Dg::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM
        };

        constexpr std::array<uint8_t, 4> whitePixel{
            254, 254, 254, 255
        };

        Dg::TextureSubResData subresource{
            whitePixel.data(),
            sizeof(whitePixel)
        };
        Dg::TextureData textureData{ &subresource, 1 };

        Ptr<Dg::ITexture> texture;
        renderDevice->CreateTexture(desc, &textureData, &texture);

        return texture;
    }

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
        const aiTexture*   aitexture,
        const Rhi::Image&  image)
    {
        auto            size = image.GetSize();
        Dg::TextureDesc desc{
            aitexture->mFilename.C_Str(),
            Dg::RESOURCE_DIM_TEX_2D,
            size.x(),
            size.y(),
            1,
            Dg::TEXTURE_FORMAT::TEX_FORMAT_RGBA8_UNORM
        };

        auto            subresource = image.GetSubresource();
        Dg::TextureData textureData{ &subresource, 1 };

        Ptr<Dg::ITexture> texture;
        renderDevice->CreateTexture(desc, &textureData, &texture);

        return texture;
    }

    //

    [[nodiscard]] static Ptr<Dg::ITexture> LoadTexture(
        Dg::IRenderDevice* renderDevice,
        const aiScene*     scene,
        aiMaterial*        material,
        const aiString&    texturePath)
    {
        const aiTexture* aitexture = scene->GetEmbeddedTexture(texturePath.C_Str());
        if (aitexture)
        {
            // first, check if the texture is not compressed
            if (aitexture->mHeight != 0)
            {
                bool isSupported = std::strncmp(aitexture->achFormatHint, "rgba8888", sizeof(aiTexture::achFormatHint)) == 0;
                return isSupported ? LoadStandardTexture(renderDevice, aitexture)
                                   : LoadDummyTexture(renderDevice, aitexture->mFilename.C_Str());
            }
            else
            {
                auto  imageMemory = Rhi::ImageStorage::Load(std::bit_cast<std::byte*>(aitexture->pcData), aitexture->mWidth);
                auto& image       = imageMemory.GetImage();
                return LoadTextureFromImage(renderDevice, aitexture, image);
            }
        }
        else
        {
            Rhi::Image image;

            // load from file
            std::ifstream file(texturePath.C_Str(), std::ios::binary);
            if (file)
            {
                auto format = Rhi::ImageStorage::GetFormat(file);
                image       = Rhi::ImageStorage::Decode(format, file);
            }

            return image ? LoadTextureFromImage(renderDevice, aitexture, image)
                         : LoadDummyTexture(renderDevice, texturePath.C_Str());
        }
    }

    void AssImpModelImporter::CreateMaterials(
        MeshModel::CreateDesc& createDesc,
        Dg::IRenderDevice*     renderDevice) const
    {
        const aiScene* scene = m_Importer.GetScene();

        if (!scene->HasMaterials())
        {
            return;
        }

        constexpr std::array c_TextureTypes{
            std::pair{ "Diffuse", aiTextureType_DIFFUSE },
            std::pair{ "Specular", aiTextureType_SPECULAR },
            std::pair{ "Ambient", aiTextureType_AMBIENT },
            std::pair{ "Emissive", aiTextureType_EMISSIVE },
            std::pair{ "Height", aiTextureType_HEIGHT },
            std::pair{ "Normals", aiTextureType_NORMALS },
            std::pair{ "Shininess", aiTextureType_SHININESS },
            std::pair{ "Opacity", aiTextureType_OPACITY },
            std::pair{ "Displacement", aiTextureType_DISPLACEMENT },
            std::pair{ "Lightmap", aiTextureType_LIGHTMAP },
            std::pair{ "Reflection", aiTextureType_REFLECTION },
            std::pair{ "Diffuse", aiTextureType_BASE_COLOR },
            std::pair{ "Emissive", aiTextureType_EMISSION_COLOR },
            std::pair{ "Metallic", aiTextureType_METALNESS },
            std::pair{ "Roughness", aiTextureType_DIFFUSE_ROUGHNESS },
            std::pair{ "AmbientOcclusion", aiTextureType_AMBIENT_OCCLUSION }
        };

        std::map<String, Ptr<Dg::ITexture>> textureCache;

        auto loadTextureFromCache = [renderDevice, scene, &textureCache](aiMaterial* material, const aiString& texturePath)
        {
            auto iter = textureCache.find(texturePath.C_Str());
            if (iter != textureCache.end())
            {
                return iter->second;
            }

            auto texture = LoadTexture(renderDevice, scene, material, texturePath);
            return textureCache.emplace(texturePath.C_Str(), texture).first->second;
        };

        //

        aiString texturePath;
        for (uint32_t i = 0; i < scene->mNumMaterials; i++)
        {
            aiMaterial* material = scene->mMaterials[i];

            aiString name;
            material->Get(AI_MATKEY_NAME, name);

            for (auto& [textureName, textureType] : c_TextureTypes)
            {
                if (material->GetTextureCount(textureType) > 0)
                {
                    auto texture = loadTextureFromCache(material, texturePath);
                }
            }
        }
    }
} // namespace Ame::Ecs