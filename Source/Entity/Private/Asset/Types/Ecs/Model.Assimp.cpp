#include <map>

#include <EcsComponent/Renderables/3D/Model.hpp>
#include <Asset/Types/Ecs/Model.Assimp.hpp>

#include <assimp/postprocess.h>
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

    AssImpModelImporter::AssImpModelImporter(
        const String& path)
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
} // namespace Ame::Ecs