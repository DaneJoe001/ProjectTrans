/**
 * @file std_type_traits.hpp
 * @brief std类型特征工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-16
 * @details 提供对常见 std 类型（chrono、string_view、optional、variant、tuple 等）的编译期类型特征（type traits）。
 */
#pragma once

#include <chrono>
#include <variant>
#include <string_view>
#include <type_traits>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 判断类型是否为 std::chrono::duration
     * @tparam T 待判断的类型
     * @note 若为 std::chrono::duration<Rep, Period> 形式，则 is_chrono_duration<T>::value 为 true，否则为 false。
     */
    template<class T>
    struct is_chrono_duration :std::false_type {};
    /**
     * @brief std::chrono::duration 的特化版本
     * @tparam Rep 表示计数类型（如 int、long long 等）
     * @tparam Period 表示计时周期类型（如 std::ratio<1>、std::milli 等）
     * @note 当 T 为 std::chrono::duration<Rep, Period> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class Rep, class Period>
    struct is_chrono_duration<std::chrono::duration<Rep, Period>> :std::true_type {};
    /**
     * @brief 判断类型是否为 std::chrono::time_point
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 若为 std::chrono::time_point<Clock, TimeType> 形式，则 is_chrono_time_point<T>::value 为 true，否则为 false。
     */
    template<class T>
    struct is_chrono_time_point :std::false_type {};
    /**
     * @brief std::chrono::time_point 的特化版本
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 当 T 为 std::chrono::time_point<std::chrono::system_clock, TimeType> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class T>
    struct is_chrono_time_point<std::chrono::time_point<std::chrono::system_clock, T>> : std::true_type {};
    /**
     * @brief 判断类型是否为 std::chrono::duration<T, std::micro>
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 若为 std::chrono::duration<T, std::micro> 形式，则 is_chrono_microseconds<T>::value 为 true，否则为 false。
     */
    template<typename T>
    struct is_chrono_microseconds :std::false_type {};
    /**
     * @brief 判断类型是否为 std::chrono::duration<T, std::micro>
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 当 T 为 std::chrono::duration<T, std::micro> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class T>
    struct is_chrono_microseconds<std::chrono::duration<T, std::micro>> :std::true_type {};
    /**
     * @brief 判断类型是否为 std::chrono::duration<T, std::milli>
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 若为 std::chrono::duration<T, std::milli> 形式，则 is_chrono_milliseconds<T>::value 为 true，否则为 false。
     */
    template<typename T>
    struct is_chrono_milliseconds :std::false_type {};
    /**
     * @brief 判断类型是否为 std::chrono::duration<T, std::milli>
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 当 T 为 std::chrono::duration<T, std::milli> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class T>
    struct is_chrono_milliseconds<std::chrono::duration<T, std::milli>> :std::true_type {};
    /**
     * @brief 判断类型是否为 std::chrono::duration<T, std::nano>
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 若为 std::chrono::duration<T, std::nano> 形式，则 is_chrono_nanoseconds<T>::value 为 true，否则为 false。
     */
    template<typename T>
    struct is_chrono_nanoseconds :std::false_type {};
    /**
     * @brief 判断类型是否为 std::chrono::duration<T, std::nano>
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 当 T 为 std::chrono::duration<T, std::nano> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class T>
    struct is_chrono_nanoseconds<std::chrono::duration<T, std::nano>> :std::true_type {};
    /**
     * @brief 判断类型是否为 std::chrono::duration<T, std::ratio<1>>
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 若为 std::chrono::duration<T, std::ratio<1>> 形式，则 is_chrono_seconds<T>::value 为 true，否则为 false。
     */
    template<typename T>
    struct is_chrono_seconds :std::false_type {};
    /**
     * @brief 判断类型是否为 std::chrono::duration<T, std::ratio<1>>
     * @tparam T 表示时间类型（如 int、long long 等）
     * @note 当 T 为 std::chrono::duration<T, std::ratio<1>> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class T>
    struct is_chrono_seconds<std::chrono::duration<T, std::ratio<1>>> :std::true_type {};
    /**
     * @brief 判断类型是否为 std::basic_string_view<CharT, Traits>
     * @tparam T 表示字符串类型（如 std::string_view、std::wstring_view 等）
     * @note 若为 std::basic_string_view<CharT, Traits> 形式，则 is_std_string_view<T>::value 为 true，否则为 false。
     */
    template<class T>
    struct is_std_string_view :std::false_type {};
    /**
     * @brief std::basic_string_view 的特化版本
     * @tparam CharT 表示字符类型（如 char、wchar_t 等）
     * @tparam Traits 表示字符类型特征（如 std::char_traits<char>、std::char_traits<wchar_t> 等）
     * @note 当 T 为 std::basic_string_view<CharT, Traits> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class CharT, class Traits>
    struct is_std_string_view<std::basic_string_view<CharT, Traits>> : std::true_type {};
    /**
     * @brief 判断类型是否为 const char*
     * @tparam T 表示指针类型（如 const char*、char* 等）
     * @note 若为 const char* 形式，则 is_c_string<T>::value 为 true，否则为 false。
     */
    template<class T>
    struct is_c_string :std::bool_constant<
        std::is_pointer_v<std::remove_reference_t<T>>&&
        std::is_same_v<std::remove_cv_t<std::remove_pointer_t<std::remove_reference_t<T>>>, char>> {};
    /**
     * @brief 判断类型是否为 C 数组类型的特化版本
     * @tparam T 元素类型
     * @note 该模板的基础版本默认返回 false
     */
    template<class T>
    struct is_c_array :std::false_type {};
    /**
     * @brief 判断类型是否为 C 数组类型
     * @tparam T 元素类型
     * @tparam N 数组大小
     * @note 当类型为 T[N] 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class T, std::size_t N>
    struct is_c_array<T[N]> :std::true_type {};
    /**
     * @brief 判断类型是否为 std::pair<Key, Value>
     * @tparam Key 表示键类型（如 int、std::string 等）
     * @tparam Value 表示值类型（如 int、std::string 等）
     * @note 若为 std::pair<Key, Value> 形式，则 is_std_pair<T>::value 为 true，否则为 false。
     */
    template<class T>
    struct is_std_pair :std::false_type {};
    /**
     * @brief std::pair<Key, Value> 的特化版本
     * @tparam Key 表示键类型（如 int、std::string 等）
     * @tparam Value 表示值类型（如 int、std::string 等）
     * @note 当 T 为 std::pair<Key, Value> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class Key, class Value>
    struct is_std_pair<std::pair<Key, Value>> :std::true_type {};
    /**
     * @brief 判断类型是否为 std::optional<T>
     * @tparam T 待判断的类型
     * @note 若为 std::optional<U> 形式，则 is_std_optional<T>::value 为 true，否则为 false。
     */
    template<class T>
    struct is_std_optional :std::false_type {};
    /**
     * @brief std::optional<T> 的特化版本
     * @tparam T 表示值类型（如 int、std::string 等）
     * @note 当 T 为 std::optional<T> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class T>
    struct is_std_optional<std::optional<T>> :std::true_type {};
    /**
     * @brief 判断类型是否为 std::variant<Args...>
     * @tparam T 表示变体类型（如 int、std::string 等）
     * @note 若为 std::variant<Args...> 形式，则 is_std_variant<T>::value 为 true，否则为 false。
     */
    template<class T>
    struct is_std_variant :std::false_type {};
    /**
     * @brief std::variant<Args...> 的特化版本
     * @tparam Args... 表示元组类型（如 int、std::string 等）
     * @note 当 T 为 std::variant<Args...> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class ...Args>
    struct is_std_variant<std::variant<Args...>> :std::true_type {};
    /**
     * @brief 判断类型是否为 std::tuple<Args...>
     * @tparam Args... 表示元组类型（如 int、std::string 等）
     * @note 若为 std::tuple<Args...> 形式，则 is_std_tuple<T>::value 为 true，否则为 false。
     */
    template<class T>
    struct is_std_tuple :std::false_type {};
    /**
     * @brief std::tuple<Args...> 的特化版本
     * @tparam Args... 表示元组类型（如 int、std::string 等）
     * @note 当 T 为 std::tuple<Args...> 时匹配该特化，此时 Trait<T>::value 为 true。
     */
    template<class ...Args>
    struct is_std_tuple<std::tuple<Args...>> :std::true_type {};
}
