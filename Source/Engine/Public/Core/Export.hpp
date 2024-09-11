#pragma once

#include <boost/config.hpp>

#ifdef _MSC_VER
#pragma warning(disable : 4251)
#pragma warning(disable : 4275)
#endif

#if !defined(AME_STATIC_LINKING)

#if defined(AME_ENGINE_EXPORT)
#define AME_ENGINE_API BOOST_SYMBOL_EXPORT
#else
#define AME_ENGINE_API BOOST_SYMBOL_IMPORT
#endif

#else

#define AME_ENGINE_API

#endif

// Your code here
