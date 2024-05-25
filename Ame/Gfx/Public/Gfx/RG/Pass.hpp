#pragma once

#include <functional>

#include <Gfx/RG/Core.hpp>
#include <Gfx/RG/Resolver.hpp>

#include <Gfx/RG/Resources/Names.hpp>
#include <Gfx/RG/ResourceStorage.hpp>

#include <Math/Common.hpp>

namespace Ame::Gfx::RG
{
    class Pass
    {
        friend class PassStorage;
        friend class DependencyLevel;

    public:
        using BuildFuncType   = std::move_only_function<void(Resolver&)>;
        using ExecuteFuncType = std::move_only_function<void(const ResourceStorage&, Rhi::CommandList*)>;

        Pass() = default;

        Pass(const Pass&)                = default;
        Pass& operator=(const Pass&)     = default;
        Pass(Pass&&) noexcept            = default;
        Pass& operator=(Pass&&) noexcept = default;

        virtual ~Pass() = default;

        /// <summary>
        /// Set the queue type of the pass
        /// </summary>
        Pass& SetFlags(
            PassFlags flags,
            bool      enable = true)
        {
            using namespace EnumBitOperators;
            m_Flags |= flags;
            return *this;
        }

        /// <summary>
        /// Get the name of the pass
        /// </summary>
        Pass& Name(
            StringView name)
        {
#ifndef AME_DIST
            m_Name = name;
#endif
            return *this;
        }

        /// <summary>
        /// Initializes a build callback
        /// </summary>
        Pass& Build(
            BuildFuncType&& buildFunc)
        {
            m_BuildFunc = std::move(buildFunc);
            return *this;
        }

        /// <summary>
        /// Initializes an execute callback
        /// </summary>
        Pass& Execute(
            ExecuteFuncType&& executeFunc)
        {
            m_ExecuteFunc = std::move(executeFunc);
            return *this;
        }

        /// <summary>
        /// Get the name of the pass
        /// </summary>
        [[nodiscard]] StringView GetName() const
        {
#ifndef AME_DIST
            return m_Name;
#else
            return "";
#endif
        }

        /// <summary>
        /// Get the flags of the pass
        /// </summary>
        [[nodiscard]] PassFlags GetFlags() const
        {
            return m_Flags;
        }

        /// <summary>
        /// Get the queue type of the pass
        /// </summary>
        [[nodiscard]] PassFlags GetQueueType() const
        {
            using namespace EnumBitOperators;
            return m_Flags & PassFlags::TypeMask;
        }

    private:
        /// <summary>
        /// Build render pass
        /// </summary>
        void DoBuild(
            Resolver& resolver)
        {
            if (m_BuildFunc) [[likely]]
            {
                m_BuildFunc(resolver);
            }
        }

        /// <summary>
        /// Execute render pass
        /// </summary>
        void DoExecute(
            const ResourceStorage& storage,
            Rhi::CommandList*      commandList)
        {
            if (m_ExecuteFunc) [[likely]]
            {
                m_ExecuteFunc(storage, commandList);
            }
        }

    private:
        BuildFuncType   m_BuildFunc;
        ExecuteFuncType m_ExecuteFunc;
        PassFlags       m_Flags = PassFlags::None;
#ifndef AME_DIST
        String m_Name = "Unnamed";
#endif
    };

    //

    template<typename Ty>
    class TypedPass : public Pass
    {
    public:
        using BuildFuncType   = std::move_only_function<void(Ty&, Resolver&)>;
        using ExecuteFuncType = std::move_only_function<void(const Ty&, const ResourceStorage&, nri::CommandBuffer*)>;

        /// <summary>
        /// Initializes a build callback
        /// </summary>
        TypedPass& Build(
            BuildFuncType&& buildFunc)
        {
            Pass::Build(
                std::bind_front(
                    std::move(buildFunc),
                    std::ref(m_Data)));
            return *this;
        }

        /// <summary>
        /// Initializes an execute callback
        /// </summary>
        TypedPass& Execute(
            ExecuteFuncType&& executeFunc)
        {
            Pass::Execute(
                std::bind_front(
                    std::move(executeFunc),
                    std::cref(m_Data)));
            return *this;
        }

    private:
        Ty m_Data;
    };

    template<>
    class TypedPass<void> : public Pass
    {
    public:
    };
} // namespace Ame::Gfx::RG