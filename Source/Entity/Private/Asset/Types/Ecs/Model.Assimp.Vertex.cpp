#include <EcsComponent/Renderables/3D/Model.hpp>
#include <Asset/Types/Ecs/Model.Assimp.hpp>
#include <assimp/scene.h>

#include <Log/Wrapper.hpp>

namespace Ame::Ecs
{
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
        Log::Asset().Assert(HasMeshes(), "No meshes found in scene when importing model");

        MeshModel::CreateDesc createDesc;

        CreateBufferResources(createDesc, renderDevice);
        CreateMaterials(createDesc, renderDevice);

        return createDesc;
    }

    //

    bool AssImpModelImporter::HasMeshes() const
    {
        return m_Importer.GetScene() && m_Importer.GetScene()->HasMeshes();
    }

    void AssImpModelImporter::CreateBufferResources(
        MeshModel::CreateDesc& createDesc,
        Dg::IRenderDevice*     renderDevice) const
    {
        const aiScene* scene = m_Importer.GetScene();

        createDesc.MeshNodes.reserve(scene->mNumMeshes);
        createDesc.SubMeshes.reserve(scene->mNumMeshes);

        //

        VectorAndOffset<VI3_Position> positions;
        VectorAndOffset<VI3_Normal>   normals;
        VectorAndOffset<VI3_Tangent>  tangents;
        VectorAndOffset<VI_TexCoord>  texCoords;

        //

        auto reserveBuffer = [](auto& buffer, size_t count)
        {
            buffer.reserve(buffer.size() + count);
        };

        size_t   totalVertices = 0;
        uint32_t maxIndex      = 0;
        for (aiMesh* mesh : std::span{ scene->mMeshes, scene->mNumMeshes })
        {
            totalVertices += mesh->mNumVertices;
            std::span faces(mesh->mFaces, mesh->mNumFaces);
            for (const aiFace& face : faces)
            {
                std::span indices(face.mIndices, face.mNumIndices);
                maxIndex = std::max(maxIndex, *std::ranges::max_element(indices));
            }
        }

        VectorAndOffset<uint32_t> indices32;
        VectorAndOffset<uint32_t> indices16;
        bool                      use16BitIndices = maxIndex <= std::numeric_limits<uint16_t>::max();

        reserveBuffer(positions.Buffer, totalVertices);
        reserveBuffer(normals.Buffer, totalVertices);
        reserveBuffer(tangents.Buffer, totalVertices);
        reserveBuffer(texCoords.Buffer, totalVertices);

        if (use16BitIndices)
        {
            indices16.Buffer.reserve(totalVertices * 3);
        }
        else
        {
            indices32.Buffer.reserve(totalVertices * 3);
        }

        //

        for (aiMesh* mesh : std::span{ scene->mMeshes, scene->mNumMeshes })
        {
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

            uint32_t indexCount = mesh->mNumFaces * 3;
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

            auto indexOffset = static_cast<uint32_t>(use16BitIndices ? indices16.Offset : indices32.Offset);

            createDesc.SubMeshes.emplace_back(SubMeshData{
                .AABB           = minMax.ToAABB(),
                .PositionOffset = positions.Offset,
                .NormalOffset   = normals.Offset,
                .TangentOffset  = mesh->HasTangentsAndBitangents() ? tangents.Offset : c_InvalidIndex,
                .TexCoordOffset = mesh->HasTextureCoords(0) ? texCoords.Offset : c_InvalidIndex,
                .IndexOffset    = indexOffset,
                .IndexCount     = indexCount,
                .MaterialIndex  = mesh->mMaterialIndex });

            positions.Sync();
            normals.Sync();
            tangents.Sync();
            texCoords.Sync();
            indices32.Sync();
            indices16.Sync();

            Log::Asset().Info("Mesh {} has {} vertices and {} indices", StringView(mesh->mName.C_Str(), mesh->mName.length), mesh->mNumVertices, indexCount);
        }

        createDesc.MeshNodes.reserve(scene->mNumMeshes);
        TraverseAISubMesh(scene->mRootNode, createDesc.SubMeshes, createDesc.MeshNodes);

        //

        Dg::BufferDesc bufferDesc;
        bufferDesc.Usage = Dg::USAGE_IMMUTABLE;

        auto createBuffer = [&bufferDesc, renderDevice, scene](const auto& buffer, const char* name, Dg::BIND_FLAGS bindFlags)
        {
            Dg::IBuffer* result = nullptr;
            if (!buffer.Empty())
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

        createDesc.SmallIndexBuffer = use16BitIndices;
        if (use16BitIndices) [[likely]]
        {
            createDesc.IndexBuffer = createBuffer(indices16, "VI_Index", Dg::BIND_INDEX_BUFFER);
        }
        else
        {
            createDesc.IndexBuffer = createBuffer(indices32, "VI_Index", Dg::BIND_INDEX_BUFFER);
        }
    }
} // namespace Ame::Ecs