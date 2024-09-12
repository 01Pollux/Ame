#include <map>
#include <filesystem>

#include <EcsComponent/Renderables/3D/Model.hpp>
#include <Asset/Types/Ecs/Model.Assimp.hpp>

#include <assimp/postprocess.h>
#include <assimp/LogStream.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/DefaultIOStream.h>

#include <Log/Logger.hpp>

namespace Ame::Ecs
{
    struct AssimpLogStream : public Assimp::LogStream
    {
        static void InitializeOnce();

        virtual void write(const char* message) override
        {
            String messageStr(message);
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

            static std::map<String, Log::LogLevel> c_Severities{ { "Warn", Log::LogLevel::Warning },
                                                                 { "Info", Log::LogLevel::Info },
                                                                 { "Debug", Log::LogLevel::Debug } };

            auto severity = Log::LogLevel::Error;

            auto iter = c_Severities.find(type);
            if (iter != c_Severities.end()) [[likely]]
            {
                severity = iter->second;
            }

            if (auto logger = Log::ILogger::Get(); logger && logger->CanLog(severity))
            {
                logger->WriteMessage({ severity, messageStr });
            }
        }
    };

    void AssimpLogStream::InitializeOnce()
    {
        static AssimpLogStream s_AssimpLogStream;
        static std::once_flag  s_Initialized;

        std::call_once(s_Initialized,
                       []()
                       {
                           if (Assimp::DefaultLogger::isNullLogger()) [[unlikely]]
                           {
#if AME_DEBUG
                               Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
                               Assimp::DefaultLogger::get()->attachStream(
                                   &s_AssimpLogStream, Assimp::Logger::Debugging | Assimp::Logger::Info |
                                                           Assimp::Logger::Warn | Assimp::Logger::Err);
#else
                    Assimp::DefaultLogger::create("", Assimp::Logger::NORMAL);
                    Assimp::DefaultLogger::get()->attachStream(&s_AssimpLogStream, Assimp::Logger::Warn | Assimp::Logger::Err);
#endif
                           }
                       });
    }

    //

    static const uint32_t s_MeshImportFlags = aiProcess_CalcTangentSpace |
                                              // aiProcess_OptimizeGraph |
                                              aiProcess_GenNormals | aiProcess_Triangulate | aiProcess_SortByPType |
                                              aiProcess_MakeLeftHanded | aiProcess_FlipUVs |
                                              aiProcess_FlipWindingOrder | aiProcess_JoinIdenticalVertices |
                                              aiProcess_ValidateDataStructure | aiProcess_GlobalScale |
                                              aiProcess_OptimizeMeshes | aiProcess_RemoveRedundantMaterials;

    //

    AssImpModelImporter::AssImpModelImporter(const String& path) :
        m_ModelRootPath(std::filesystem::path(path).parent_path().string())
    {
        AssimpLogStream::InitializeOnce();

        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, true);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_LIGHTS, false);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_CAMERAS, false);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_ANIMATIONS, false);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_MATERIALS, true);
        m_Importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_READ_TEXTURES, true);

        m_Importer.ReadFile(path, s_MeshImportFlags);
    }

    //

    MeshModel::CreateDesc AssImpModelImporter::CreateModelDesc(Rhi::IRhiDevice* rhiDevice) const
    {
        MeshModel::CreateDesc createDesc;
        if (m_Importer.GetScene())
        {
            CreateBufferResources(createDesc, rhiDevice);
            CreateMaterials(createDesc, rhiDevice);
        }
        return createDesc;
    }

    Co::result<MeshModel::CreateDesc> AssImpModelImporter::CreateModelDescAsync(Rhi::IRhiDevice* rhiDevice) const
    {
        MeshModel::CreateDesc createDesc;

        if (m_Importer.GetScene())
        {
            auto bufferTask =
                Coroutine::Get().background_executor()->submit([&] { CreateBufferResources(createDesc, rhiDevice); });
            auto materialTask =
                Coroutine::Get().background_executor()->submit([&] { CreateMaterials(createDesc, rhiDevice); });

            co_await Co::when_all(
                Coroutine::Get().background_executor(), std::move(bufferTask), std::move(materialTask))
                .run();
        }

        co_return createDesc;
    }
} // namespace Ame::Ecs