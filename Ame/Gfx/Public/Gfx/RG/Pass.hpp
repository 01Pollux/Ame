#pragma once

#include <functional>

#include <boost/compressed_pair.hpp>

#include <Gfx/RG/Core.hpp>
#include <Gfx/RG/Resolver.hpp>

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
            PassFlags Flags,
            bool      Enable = true)
        {
            using namespace EnumBitOperators;
            m_Flags |= Flags;
            return *this;
        }

        /// <summary>
        /// Get the name of the pass
        /// </summary>
        Pass& Name(
            StringView Name)
        {
#ifndef AME_DIST
            m_Name = Name;
#endif
            return *this;
        }

        /// <summary>
        /// Initializes a build callback
        /// </summary>
        Pass& Build(
            BuildFuncType&& BuildFunc)
        {
            m_BuildFunc = std::move(BuildFunc);
            return *this;
        }

        /// <summary>
        /// Initializes an execute callback
        /// </summary>
        Pass& Execute(
            ExecuteFuncType&& ExecuteFunc)
        {
            m_ExecuteFunc = std::move(ExecuteFunc);
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
            Resolver& RgResolver)
        {
            if (m_BuildFunc) [[likely]]
            {
                m_BuildFunc(RgResolver);
            }
        }

        /// <summary>
        /// Execute render pass
        /// </summary>
        void DoExecute(
            const ResourceStorage& RgStorage,
            Rhi::CommandList*      CommandList)
        {
            if (m_ExecuteFunc) [[likely]]
            {
                m_ExecuteFunc(RgStorage, CommandList);
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
            BuildFuncType&& BuildFunc)
        {
            Pass::Build(
                std::bind_front(
                    std::move(BuildFunc),
                    std::ref(m_Data)));
            return *this;
        }

        /// <summary>
        /// Initializes an execute callback
        /// </summary>
        TypedPass& Execute(
            ExecuteFuncType&& ExecuteFunc)
        {
            Pass::Execute(
                std::bind_front(
                    std::move(ExecuteFunc),
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