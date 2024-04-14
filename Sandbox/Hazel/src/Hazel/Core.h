#pragma once

#include <memory>

#ifdef HZ_BUILD_DLL
    #define HAZEL_API __declspec(dllexport)
#else
    #define HAZEL_API __declspec(dllimport)
#endif

namespace Hazel {
    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T>
    using Ref = std::shared_ptr<T>;
}