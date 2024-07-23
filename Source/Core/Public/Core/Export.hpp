#pragma once

#pragma once

#include <boost/config.hpp>

// AME_DYNAMIC_LINKING: enable dynamic linking
// AME_EXPORT_API: export/import symbols

#if defined(AME_DYNAMIC_LINKING) && defined(BOOST_HAS_DECLSPEC)

#if defined(AME_EXPORT_API)
#define AME_API BOOST_SYMBOL_EXPORT
#else
#define AME_API BOOST_SYMBOL_IMPORT
#endif

#else

#define AME_API

#endif

// Your code here
