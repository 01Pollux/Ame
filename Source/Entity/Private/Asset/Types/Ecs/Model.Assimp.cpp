#include <map>

#include <EcsComponent/Renderables/3D/Model.hpp>
#include <Asset/Types/Ecs/Model.Assimp.hpp>

#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/DefaultIOStream.h>

#include <Log/Wrapper.hpp>

namespace Ame::Ecs
{
    struct AssimpLogStream : public Assimp::LogStream
    {
        static void InitializeOnce();

        virtual void write(
            const char* Message) override
        {
            String messageStr(Message);
            String type;

            size_t offset = 0;

            // Text string is built like this: "Type, Message", split it up.
            size_t typeEnd = messageStr.find_first_of(", ");
            if (typeEnd != String::npos) [[likely]]
            {
                type   = messageStr.substr(0, typeEnd);
                offset = typeEnd + 2;

                // Remove the space after the comma.
                if (offset < messageStr.size() && messageStr[offset] == ' ') [[likely]]
                {
                    offset++;
                }
                messageStr = messageStr.substr(offset);
            }

            static std::map<String, Log::LogLevel> c_Severities{
                { "Warn", Log::LogLevel::Warning },
                { "Info", Log::LogLevel::Info },
                { "Debug", Log::LogLevel::Debug }
            };

            auto severity = Log::LogLevel::Error;

            auto iter = c_Severities.find(type);
            if (iter != c_Severities.end()) [[likely]]
            {
                severity = iter->second;
            }

            Log::Asset().LogMessage(severity, messageStr);
        }
    };

    void AssimpLogStream::InitializeOnce()
    {
        static AssimpLogStream s_AssimpLogStream;
        static std::once_flag  s_Initialized;

        std::call_once(
            s_Initialized,
            []()
            {
                if (Assimp::DefaultLogger::isNullLogger()) [[unlikely]]
                {
#if AME_DEBUG
                    Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
                    Assimp::DefaultLogger::get()->attachStream(&s_AssimpLogStream, Assimp::Logger::Debugging | Assimp::Logger::Info | Assimp::Logger::Warn | Assimp::Logger::Err);
#else
                    Assimp::DefaultLogger::create("", Assimp::Logger::NORMAL);
                    Assimp::DefaultLogger::get()->attachStream(&s_AssimpLogStream, Assimp::Logger::Warn | Assimp::Logger::Err);
#endif
                }
            });
    }

    //

    static const uint32_t s_MeshImportFlags =
        aiProcess_CalcTangentSpace |    // Create binormals/tangents just in case
        aiProcess_ConvertToLeftHanded | // Make sure we're left handed
        aiProcess_Triangulate |         // Make sure we're triangles
        aiProcess_SortByPType |         // Split meshes by primitive type
        aiProcess_GenNormals |          // Make sure we have legit normals
        aiProcess_GenUVCoords |         // Convert UVs if required
        aiProcess_OptimizeGraph |
        aiProcess_OptimizeMeshes | // Batch draws where possible
        aiProcess_JoinIdenticalVertices |
        aiProcess_LimitBoneWeights |      // If more than N (=4) bone weights, discard least influencing bones and renormalise sum to 1
        aiProcess_ValidateDataStructure | // Validation
        aiProcess_GlobalScale;            // e.g. convert cm to m for fbx import (and other formats where cm is native)

    //

    /// <summary>
    /// Convert an AssImp matrix to a Neon matrix.
    /// </summary>
    [[nodiscard]] static Math::Matrix4x4 FromAiMatrix(
        const aiMatrix4x4& matrix)
    {
        return Math::Matrix4x4(
            { matrix.a1, matrix.b1, matrix.c1, matrix.d1 },
            { matrix.a2, matrix.b2, matrix.c2, matrix.d2 },
            { matrix.a3, matrix.b3, matrix.c3, matrix.d3 },
            { matrix.a4, matrix.b4, matrix.c4, matrix.d4 });
    }

    /// <summary>
    /// Traverse an AssImp node and add it to the model as a mesh node.
    /// </summary>
    static void TraverseAISubMesh(
        aiNode*                     aiNode,
        MeshModel::SubMeshDataList& subMeshes,
        MeshModel::MeshNodeList&    rootNode,
        uint32_t                    parentIndex     = std::numeric_limits<uint32_t>::max(),
        const Math::Matrix4x4&      parentTransform = Math::Matrix4x4::Constants::Identity)
    {
        MeshNode node{
            .Parent    = parentIndex,
            .Transform = FromAiMatrix(aiNode->mTransformation),
            .Name      = String(aiNode->mName.data, aiNode->mName.length)
        };

        Math::Matrix4x4 finalTransform = parentTransform * node.Transform;
        node.SubMeshes.reserve(aiNode->mNumMeshes);

        for (uint32_t i = 0; i < aiNode->mNumMeshes; ++i)
        {
            uint32_t meshIndex             = aiNode->mMeshes[i];
            subMeshes[meshIndex].Transform = finalTransform;
            node.SubMeshes.push_back(meshIndex);
        }

        uint32_t nodeIndex = static_cast<uint32_t>(rootNode.size());
        rootNode.emplace_back(std::move(node));
        rootNode[nodeIndex].Children.reserve(aiNode->mNumChildren);

        for (uint32_t i = 0; i < aiNode->mNumChildren; i++)
        {
            uint32_t ChildIndex = static_cast<uint32_t>(rootNode.size());
            rootNode[nodeIndex].Children.push_back(ChildIndex);
            TraverseAISubMesh(aiNode->mChildren[i], subMeshes, rootNode, nodeIndex, finalTransform);
        }
    }

    //

    AssImpModelImporter::AssImpModelImporter(
        const String& path)
    {
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_LIGHTS, false);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_CAMERAS, false);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ANIMATIONS, false);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_MATERIALS, true);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_TEXTURES, true);

        m_Importer.ReadFile(path, s_MeshImportFlags);
        m_Importer.GetScene()->mNumMeshes;
    }

    //

    template<typename Ty>
    struct VectorAndOffset
    {
        std::vector<Ty> Buffer;
        uint32_t        Offset = 0;

        void Append(const Ty& data)
        {
            Buffer.emplace_back(data);
        }

        template<typename OTy>
        void Append(std::span<const OTy> data)
        {
            Buffer.insert(Buffer.end(), data.begin(), data.end());
        }

        [[nodiscard]] const void* Data() const
        {
            return Buffer.data();
        }

        [[nodiscard]] bool Empty() const
        {
            return Buffer.empty();
        }

        [[nodiscard]] size_t Size() const
        {
            return Buffer.size();
        }

        [[nodiscard]] size_t ElementSize() const
        {
            return sizeof(Ty);
        }

        [[nodiscard]] size_t ByteSize() const
        {
            return Size() * ElementSize();
        }

        void Sync()
        {
            Offset = static_cast<uint32_t>(Buffer.size());
        }
    };

    //

    MeshModel::CreateDesc AssImpModelImporter::CreateModelDesc(
        Dg::IRenderDevice* renderDevice) const
    {
        MeshModel::CreateDesc createDesc;

        const aiScene* scene = m_Importer.GetScene();
        if (!scene || !scene->HasMeshes())
        {
            Log::Asset().Warning("No meshes found in scene when importing model");
            return createDesc;
        }

        createDesc.MeshNodes.reserve(scene->mNumMeshes);
        createDesc.SubMeshes.reserve(scene->mNumMeshes);

        //

        VectorAndOffset<VI3_Position> positions;
        VectorAndOffset<VI3_Normal>   normals;
        VectorAndOffset<VI3_Tangent>  tangents;
        VectorAndOffset<VI_TexCoord>  texCoords;

        VectorAndOffset<uint32_t> indices32;
        VectorAndOffset<uint32_t> indices16;
        bool                      use16BitIndices = true;

        //

        auto reserveBuffer = [](auto& buffer, size_t count)
        {
            buffer.reserve(buffer.size() + count);
        };

        auto reserverIndexBuffer = [&](size_t count)
        {
            if (use16BitIndices)
            {
                // check for overflow
                if ((indices16.Buffer.size() + count) <= std::numeric_limits<uint16_t>::max())
                {
                    indices16.Buffer.reserve(indices16.Buffer.size() + count);
                    return;
                }

                use16BitIndices = false;
                // transfer the data to 32 bit buffer
                indices32.Buffer.insert(indices32.Buffer.end(), indices16.Buffer.begin(), indices16.Buffer.end());

                indices16.Buffer.clear();
                indices16.Buffer.shrink_to_fit();
            }
            indices32.Buffer.reserve(indices16.Buffer.size() + count);
        };

        //

        for (uint32_t i = 0; i < scene->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[i];

            reserveBuffer(positions.Buffer, mesh->mNumVertices);
            reserveBuffer(normals.Buffer, mesh->mNumVertices);
            if (mesh->HasTangentsAndBitangents())
            {
                reserveBuffer(tangents.Buffer, mesh->mNumVertices);
            }
            if (mesh->HasTextureCoords(0))
            {
                reserveBuffer(texCoords.Buffer, mesh->mNumVertices);
            }
            reserverIndexBuffer(mesh->mNumFaces * 3);

            //

            Geometry::AABBMinMax minMax;

            for (uint32_t j = 0; j < mesh->mNumVertices; j++)
            {
                Math::Vector3 position(mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z);

                minMax.Accumulate(position);

                positions.Append(position);
                normals.Append({ mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z });

                if (mesh->HasTangentsAndBitangents())
                {
                    tangents.Append({ mesh->mTangents[j].x, mesh->mTangents[j].y, mesh->mTangents[j].z });
                }

                if (mesh->HasTextureCoords(0))
                {
                    texCoords.Append({ mesh->mTextureCoords[0][j].x, mesh->mTextureCoords[0][j].y });
                }
            }

            for (uint32_t j = 0; j < mesh->mNumFaces; j++)
            {
                aiFace&                   face = mesh->mFaces[j];
                std::span<const uint32_t> indices{ face.mIndices, face.mNumIndices };

                if (use16BitIndices)
                {
                    indices16.Append(indices);
                }
                else
                {
                    indices32.Append(indices);
                }
            }

            uint32_t indexCount = uint32_t(indices32.Size()) - indices32.Offset;

            createDesc.SubMeshes.emplace_back(SubMeshData{
                .AABB           = minMax.ToAABB(),
                .PositionOffset = positions.Offset,
                .NormalOffset   = normals.Offset,
                .TangentOffset  = mesh->HasTangentsAndBitangents() ? tangents.Offset : c_InvalidIndex,
                .TexCoordOffset = mesh->HasTextureCoords(0) ? texCoords.Offset : c_InvalidIndex,
                .IndexOffset    = indices32.Offset,
                .IndexCount     = indexCount,
                .MaterialIndex  = mesh->mMaterialIndex });

            positions.Sync();
            normals.Sync();
            tangents.Sync();
            texCoords.Sync();
            indices32.Sync();

            Log::Asset().Info("Mesh {} has {} vertices and {} indices", i, mesh->mNumVertices, indexCount);
        }

        createDesc.MeshNodes.reserve(scene->mNumMeshes);
        TraverseAISubMesh(scene->mRootNode, createDesc.SubMeshes, createDesc.MeshNodes);

        //

        Dg::BufferDesc bufferDesc;
        bufferDesc.Usage = Dg::USAGE_IMMUTABLE;

        auto createBuffer = [&bufferDesc, renderDevice, scene](const auto& buffer, const char* name, Dg::BIND_FLAGS bindFlags)
        {
            Dg::IBuffer* result = nullptr;
            if (buffer.Empty())
            {
#ifndef AME_DIST
                auto combinedName = std::format("{}_{}", name, StringView(scene->mName.data, scene->mName.length));
                bufferDesc.Name   = combinedName.c_str();
#endif

                bufferDesc.Size              = buffer.ByteSize();
                bufferDesc.BindFlags         = bindFlags;
                bufferDesc.ElementByteStride = buffer.ElementSize();

                Dg::BufferData initData(buffer.Data(), buffer.ByteSize());
                renderDevice->CreateBuffer(bufferDesc, &initData, &result);
            }
            return result;
        };

        //

        createDesc.PositionBuffer = createBuffer(positions, "VI3_Position", Dg::BIND_VERTEX_BUFFER);
        createDesc.NormalBuffer   = createBuffer(normals, "VI3_Normal", Dg::BIND_VERTEX_BUFFER);
        createDesc.TangentBuffer  = createBuffer(tangents, "VI3_Tangent", Dg::BIND_VERTEX_BUFFER);
        createDesc.TexCoordBuffer = createBuffer(texCoords, "VI_TexCoord", Dg::BIND_VERTEX_BUFFER);

        if (use16BitIndices) [[unlikely]]
        {
            createDesc.IndexBuffer = createBuffer(indices32, "VI_Index", Dg::BIND_INDEX_BUFFER);
        }
        else
        {
            createDesc.IndexBuffer      = createBuffer(indices16, "VI_Index", Dg::BIND_INDEX_BUFFER);
            createDesc.SmallIndexBuffer = true;
        }

        return createDesc;
    }
} // namespace Ame::Ecs