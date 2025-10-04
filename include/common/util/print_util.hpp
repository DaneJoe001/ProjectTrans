#pragma once

#include <iostream>

#include <string>
#include <typeinfo>
#include <map>
#include <unordered_map>
#include <type_traits>
#include <cstdint>
#include <utility>
#include <iterator>

/// @brief 将变量并转为字符串
#define VARIABLE_NAME_TO_STRING(x) #x

namespace DaneJoe
{
    template <typename T, typename = void>
    struct has_std_to_string : std::false_type
    {
    };

    template <typename T>
    struct has_std_to_string<T, std::void_t<decltype(std::to_string(std::declval<T>()))>> : std::true_type
    {
    };

    template <typename T, typename = void>
    struct has_member_to_string : std::false_type
    {
    };

    template <typename T>
    struct has_member_to_string<T, std::void_t<decltype(std::declval<T>().to_string())>> : std::true_type
    {
    };

    template <typename T, typename = void>
    struct has_iterator : std::false_type
    {
    };

    template <typename T>
    struct has_iterator<T, std::void_t<
        decltype(std::begin(std::declval<T>())),
        decltype(std::end(std::declval<T>()))>> : std::true_type
    {
    };

    template<class T>
    std::string try_to_string(const T& obj)
    {
        std::string str;
        if constexpr (std::is_enum_v<T>)
        {
            using UnderlyingType = std::underlying_type_t<T>;
            str = typeid(obj).name() + std::string("<") + std::to_string(static_cast<UnderlyingType>(obj)) + std::string(">");
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            str = obj;
        }
        else if constexpr (std::is_same_v<T, char> || std::is_same_v<T, unsigned char>)
        {
            str = std::string(1, static_cast<char>(obj));
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            str = obj ? "true" : "false";
        }
        else if constexpr (has_member_to_string<T>::value)
        {
            str = obj.to_string();
        }
        else if  constexpr (has_std_to_string<T>::value)
        {
            str = std::to_string(obj);
        }
        else
        {
            str = typeid(obj).name() + std::string("<No to_string>");
        }
        return str;
    }

    template<class T>
    std::string to_string(const T& value)
    {
        std::string str;
        if constexpr (std::is_same_v<T, std::string>)
        {
            str = value;
        }
        else if constexpr (has_iterator<T>::value)
        {
            str += "[";
            for (auto begin = std::begin(value); begin != std::end(value); begin++)
            {
                str += DaneJoe::to_string(*begin);
                auto begin_copy = begin;
                if (++begin_copy != std::end(value))
                {
                    str += ", ";
                }
            }
            str += "]";
        }
        else
        {
            str = DaneJoe::try_to_string(value);
        }
        return str;
    }

    template<class T>
    std::string to_string(const T* array, std::size_t count = 1)
    {
        std::string str;
        if (array == nullptr || count == 0)
        {
            str = "[]";
        }
        else if (count == 1)
        {
            return DaneJoe::try_to_string(*array);
        }
        else
        {
            str += "[";
            for (int i = 0; i < count; i++)
            {
                str += DaneJoe::try_to_string(*(array + i));
                if (i < count - 1)
                {
                    str += ", ";
                }
            }
            str += "]";
        }
        return str;
    }

    template<class Key, class Value>
    std::string to_string(const std::pair<Key, Value>& pair)
    {
        std::string str = "{";
        str += DaneJoe::to_string(pair.first);
        str += ":";
        str += DaneJoe::to_string(pair.second);
        str += "}";
        return str;
    }

    template<class Key, class Value>
    std::string to_string(const std::map<Key, Value>& map)
    {
        std::string str = "[";
        std::size_t count = map.size();
        std::size_t index = 0;
        for (auto begin = map.begin(); begin != map.end(); begin++, index++)
        {
            str += to_string(*begin);
            if (index < count - 1)
            {
                str += ", ";
            }
        }
        str += "]";
        return str;
    }
    template<class Key, class Value>
    std::string to_string(const std::multimap<Key, Value>& map)
    {
        std::string str = "[";
        std::size_t count = map.size();
        std::size_t index = 0;
        for (auto begin = map.begin(); begin != map.end(); begin++, index++)
        {
            str += to_string(*begin);
            if (index < count - 1)
            {
                str += ", ";
            }
        }
        str += "]";
        return str;
    }
    template<class Key, class Value>
    std::string to_string(const std::unordered_map<Key, Value>& map)
    {
        std::string str = "[";
        std::size_t count = map.size();
        std::size_t index = 0;
        for (auto begin = map.begin(); begin != map.end(); begin++, index++)
        {
            str += to_string(*begin);
            if (index < count - 1)
            {
                str += ", ";
            }
        }
        str += "]";
        return str;
    }

}