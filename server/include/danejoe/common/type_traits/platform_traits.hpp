#pragma once

#include <type_traits>

#if defined(__linux__)
#define DANEJOE_PLATFORM_LINUX 1
#else
#define DANEJOE_PLATFORM_LINUX 0
#endif

#if defined(WIN32)
#define DANEJOE_PLATFORM_WINDOWS 1
#else
#define DANEJOE_PLATFORM_WINDOWS 0
#endif

namespace DaneJoe
{
    template<class T>
    struct is_posix_handle :std::false_type {};
#if DANEJOE_PLATFORM_LINUX==1
    template<>
    struct is_posix_handle<int> :std::true_type {};
#endif
    using error_code = int;
}