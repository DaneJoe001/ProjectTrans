/**
 * @file platform_traits.hpp
 * @brief 平台相关类型特征
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2026-01-06
 * @details 提供与平台相关的编译期宏与类型萃取工具。
 *          当前包含 Linux/Windows 平台宏，以及用于识别 POSIX 句柄类型的 trait。
 */
#pragma once

#include <type_traits>

 /**
  * @def DANEJOE_PLATFORM_LINUX
  * @brief Linux 平台标志
  * @details 当编译目标为 Linux 时该宏为 1，否则为 0。
  */
#if defined(__linux__)
#define DANEJOE_PLATFORM_LINUX 1
#else
#define DANEJOE_PLATFORM_LINUX 0
#endif

  /**
   * @def DANEJOE_PLATFORM_WINDOWS
   * @brief Windows 平台标志
   * @details 当编译目标为 Windows 时该宏为 1，否则为 0。
   */
#if defined(WIN32)
#define DANEJOE_PLATFORM_WINDOWS 1
#else
#define DANEJOE_PLATFORM_WINDOWS 0
#endif

   /**
    * @namespace DaneJoe
    * @brief DaneJoe 命名空间
    */
namespace DaneJoe
{
    /**
     * @brief 判断类型是否为 POSIX 句柄
     * @tparam T 待检测类型
     * @note 默认实现为 false，需要在对应平台上为具体句柄类型提供特化。
     */
    template<class T>
    struct is_posix_handle :std::false_type {};
#if DANEJOE_PLATFORM_LINUX==1
    /// @brief Linux 下 int（文件描述符）视为 POSIX 句柄
    template<>
    struct is_posix_handle<int> :std::true_type {};
#endif
    /// @brief 错误码类型别名（POSIX 下通常为 errno）
    using error_code = int;
}