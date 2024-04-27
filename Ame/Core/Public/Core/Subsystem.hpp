#pragma once

#include <Core/Ame.hpp>
#include <kangaru/service.hpp>
#include <kangaru/autowire.hpp>

namespace Ame
{
    /// <summary>
    /// This class is simply a list of definition the current service depends on to be constructed.
    /// </summary>
    template<typename... ArgsTy>
    using Dependency = kgr::dependency<ArgsTy...>;

    /// <summary>
    /// This is the default non-single service.
    ///
    /// It hold and return the service by value.
    /// </summary>
    template<typename Ty, typename... ArgsTy>
    using Subsystem = kgr::service<Ty, ArgsTy...>;

    /// <summary>
    /// This class is the default single service.
    ///
    /// It hold the service as value, and returns it by reference.
    /// </summary>
    template<typename Ty, typename... ArgsTy>
    using SingleSubsystem = kgr::single_service<Ty, ArgsTy...>;

    /// <summary>
    /// This class is a service definition for a single service managed by an external system.
    ///
    /// It hold the service as a reference to the instance, and returns it by reference.
    /// </summary>
    template<typename Ty>
    using ExternSubsystem = kgr::extern_service<Ty>;

    /// <summary>
    /// This class is the service definition for a non-single heap allocated service.
    ///
    /// It works for both case where you need a shared pointer non-single service,
    /// because they are implicitly constructible from a unique pointer.
    ///
    /// It will hold the service as a std::unique_ptr, and inject it as a std::unique_ptr
    /// </summary>
    template<typename Ty, typename... ArgsTy>
    using UniqueSubsystem = kgr::unique_service<Ty, ArgsTy...>;

    /// <summary>
    /// This class is a service definition when a single should be injected as a shared pointer.
    ///
    /// It will hold the service as a std::shared_ptr and inject it a s a std::shared_ptr
    /// </summary>
    template<typename Ty, typename... ArgsTy>
    using SharedSubsystem = kgr::shared_service<Ty, ArgsTy...>;

    /// <summary>
    /// This class is a service definition for a single service managed by an external system.
    ///
    /// It hold and injects the service as a shared pointer to the supplied instance.
    /// </summary>
    template<typename Ty>
    using ExternSharedSubsystem = kgr::extern_shared_service<Ty>;

    /// <summary>
    /// This class is a abstract service that a kgr::single_service can override.
    ///
    /// It cannot be constructed, but only overrided.
    /// </summary>
    template<typename Ty>
    using AbstractSubsystem = kgr::abstract_service<Ty>;

    /// <summary>
    /// This class is an abstract service that can be overrided by kgr::shared_service
    ///
    /// As it is abstract, a service that overrides it must be instanciated by the container before usage.
    /// </summary>
    template<typename Ty>
    using AbstractSharedSubsystem = kgr::abstract_shared_service<Ty>;

    /// <summary>
    /// This class is an abstract service that can be overrided by kgr::unique_service
    ///
    /// As it is abstract, a service that overrides it must be instanciated by the container before usage.
    /// </summary>
    template<typename Ty>
    struct AbstractUniqueSubsystem : kgr::abstract
    {
        auto forward() -> std::unique_ptr<Ty>;
    };

    //

    /// <summary>
    /// The following aliases are indirect maps for autowired services.
    ///
    /// kgr::autowire is also a autowire tag to use instead of dependencies
    /// </summary>
    using Autowire       = kgr::autowire;
    using AutowireSingle = kgr::autowire_single;
    using AutowireUnique = kgr::autowire_unique;
    using AutowireShared = kgr::autowire_shared;

    template<typename MapTy, std::size_t MaxDeps = kgr::detail::default_max_dependency>
    using MappedAutowire = kgr::mapped_autowire<MapTy, MaxDeps>;

    template<typename Ty, typename MapTy = kgr::map<>, std::size_t MaxDeps = kgr::detail::default_max_dependency>
    using AutoWiredSubsystem = kgr::autowire_service<Ty, MapTy, MaxDeps>;

    template<typename Ty, typename MapTy = kgr::map<>, std::size_t MaxDeps = kgr::detail::default_max_dependency>
    using AutoWiredSingleSubsystem = kgr::autowire_single_service<Ty, MapTy, MaxDeps>;

    template<typename Ty, typename MapTy = kgr::map<>, std::size_t MaxDeps = kgr::detail::default_max_dependency>
    using AutoWiredUniqueSubsystem = kgr::autowire_unique_service<Ty, MapTy, MaxDeps>;

    template<typename Ty, typename MapTy = kgr::map<>, std::size_t MaxDeps = kgr::detail::default_max_dependency>
    using AutoWiredSharedSubsystem = kgr::autowire_shared_service<Ty, MapTy, MaxDeps>;
} // namespace Ame