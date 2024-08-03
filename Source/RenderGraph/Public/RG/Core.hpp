#pragma once

#include <map>
#include <set>
#include <variant>

#include <Core/Ame.hpp>
#include <Core/Enum.hpp>
#include <Core/String.hpp>
#include <Math/Colors.hpp>

#include <Rhi/Core.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/TextureView.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/BufferView.h>

namespace Ame::Rhi
{
    class IRhiDevice;
} // namespace Ame::Rhi

namespace Ame::RG
{
    class Pass;
    class Context;
    class DependencyLevel;
    class PassStorage;
    class CoreResources;
    class ResourceStorage;
    class Graph;

    class ResourceId;
    class ResourceViewId;

    struct FrameResourceCPU;

    enum class PassFlags : uint8_t
    {
        None = 0,

        Copy     = 1 << 0,
        Compute  = 1 << 1,
        Graphics = 1 << 3 | Copy | Compute,
        TypeMask = Copy | Compute | Graphics,

        NoSetups = 1 << 4
    };

    enum class ERTClearType : uint8_t
    {
        Ignore,
        Color,
    };

    enum class EDSClearType : uint8_t
    {
        Ignore,
        Depth,
        Stencil,
        DepthStencil
    };

    enum class ResourceType
    {
        Buffer,
        Texture,
        Sampler
    };

    enum class TextureViewType
    {
        Texture2D,
        Texture2DArray,
        Texture3D,
    };

    //

    struct RtvCustomDesc
    {
        Math::Color4 ClearColor = Colors::c_Black;

        ERTClearType ClearType : 1 = ERTClearType::Ignore;

        /// <summary>
        /// Force clear color even if texture has no clear value
        /// If false, will use texture clear value
        /// </summary>
        bool ForceColor : 1 = false;
    };

    struct DsvCustomDesc
    {
        float   Depth   = 1.f;
        uint8_t Stencil = 0;

        EDSClearType ClearType    : 2 = EDSClearType::Ignore;
        bool         ForceDepth   : 1 = false;
        bool         ForceStencil : 1 = false;
    };

    struct RenderTargetViewDesc : Dg::TextureViewDesc, RtvCustomDesc
    {
    };

    struct DepthStencilViewDesc : Dg::TextureViewDesc, DsvCustomDesc
    {
    };
} // namespace Ame::RG