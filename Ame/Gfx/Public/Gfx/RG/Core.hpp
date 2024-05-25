#pragma once

#include <Core/Ame.hpp>
#include <Core/String.hpp>
#include <Math/Colors.hpp>

#include <Rhi/Core.hpp>
#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Resource/View.hpp>
#include <Rhi/CommandList/CommandList.hpp>

#include <variant>
#include <optional>
#include <set>

namespace Ame::Gfx::RG
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

        Transfer = 1 << 0,
        Compute  = 1 << 1,
        Graphics = Transfer | Compute,
        TypeMask = Transfer | Compute | Graphics,

        OneShot  = 1 << 3,
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
        float Depth = 1.f;

        uint8_t Stencil = 0;

        EDSClearType ClearType : 2 = EDSClearType::Ignore;
    };

    struct RenderTargetViewDesc : Rhi::TextureViewDesc, RtvCustomDesc
    {
    };

    struct DepthStencilViewDesc : Rhi::TextureViewDesc, DsvCustomDesc
    {
    };

    //

    using ResourceDesc = std::variant<
        Rhi::BufferDesc,
        Rhi::TextureDesc>;

    using ResourceViewDesc = std::variant<
        Rhi::BufferViewDesc,
        Rhi::TextureViewDesc,
        RenderTargetViewDesc,
        DepthStencilViewDesc>;
} // namespace Ame::Gfx::RG
