#pragma once

#include <Core/Ame.hpp>
#include <concurrencpp/concurrencpp.h>

namespace Ame
{
    namespace Co = concurrencpp;

    template<template<typename... ArgsTy> typename ContainerTy, typename Ty>
    [[nodiscard]] auto GeneratorToList(
        Co::generator<Ty> generator)
    {
        ContainerTy<Ty> container{};
        for (auto v : generator)
        {
            container.emplace(container.end(), std::move(v));
        }
        return container;
    }
} // namespace Ame
