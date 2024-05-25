#pragma once

#include <optional>
#include <Core/Ame.hpp>
#include <kangaru/kangaru.hpp>

namespace Ame
{
    class Container
    {
    public:
        /// <summary>
        /// Register a subsystem to the container.
        /// </summary>
        template<typename Ty, typename... ArgsTy>
        bool RegisterSubsystem(
            ArgsTy&&... args)
        {
            return m_Container.emplace<Ty>(std::forward<ArgsTy>(args)...);
        }

        /// <summary>
        /// This function return true if the container contains the subsystem Ty. Returns false otherwise.
        /// T nust be a single service.
        /// </summary>
        template<typename Ty>
        [[nodiscard]] bool HasSubsystem() const
        {
            return m_Container.contains<Ty>();
        }

        /// <summary>
        /// This function will return a reference to the subsystem Ty.
        /// </summary>
        template<typename Ty>
        kgr::service_type<Ty> GetSubsystem()
        {
            return m_Container.service<Ty>();
        }

        /// <summary>
        /// This function will return a reference to the subsystem Ty.
        /// </summary>
        template<typename Ty>
        [[nodiscard]] std::optional<Ref<std::remove_reference_t<kgr::service_type<Ty>>>> GetSubsystemOpt()
        {
            return HasSubsystem<Ty>() ? std::optional{ std::ref(m_Container.service<Ty>()) } : std::nullopt;
        }

    public:
        /// <summary>
        ///  This function merges a container with another.
        ///  The receiving container will prefer it's own instances in a case of conflicts.
        ///
        ///  This function consumes the container `other`
        /// </summary>
        void Merge(
            Container& Other)
        {
            m_Container.merge(Other.m_Container);
        }

        /// <summary>
        /// This function will add all services form the container sent as parameter into this one.
        /// Note that the lifetime of the container sent as parameter must be at least as long as this one.
        /// If the container you rebase from won't live long enough, consider using the merge function.
        ///
        /// It takes a predicate type as template argument.
        /// The default predicate is kgr::all.
        ///
        /// This version of the function takes a predicate that is default constructible.
        /// It will call rebase() with a predicate as parameter.
        /// </summary>
        void Rebase(
            const Container& other)
        {
            m_Container.rebase(other.m_Container);
        }

        /// <summary>
        /// This function will add all services form the container sent as parameter into this one.
        /// Note that the lifetime of the container sent as parameter must be at least as long as this one.
        /// If the container you rebase from won't live long enough, consider using the merge function.
        ///
        /// It takes a predicate type as argument to filter.
        /// </summary>
        template<typename FnTy>
        void Rebase(
            const Container& other,
            FnTy&&           predicate)
        {
            m_Container.rebase(other.m_Container, std::forward<FnTy>(predicate));
        }

    private:
        kgr::container m_Container;
    };
} // namespace Ame
