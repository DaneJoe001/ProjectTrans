/**
 * @file memory_util.hpp
 * @brief 内存管理工具
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-16
 */
#pragma once

#include <cstdint>
#include <vector>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @brief 确保容器有足够的容量
     * @tparam T 容器类型
     * @param data 容器
     * @param size 需要的容量
     * @return true 容量足够
     * @return false 容量不足
     */
    template<typename T>
    bool ensure_enough_capacity(std::vector<T>& data, uint32_t size)
    {
        if (data.size() >= size)
        {
            return true;
        }
        data.resize(size);
        return true;
    }
    /**
     * @brief 确保容器有足够的容量
     * @tparam T 容器类型
     * @param data 容器
     * @param size 需要的容量
     * @return true 容量足够
     * @return false 容量不足
     */
    template<typename T>
    bool ensure_enough_to_read(const std::vector<T>& data, uint32_t size)
    {
        if (data.size() >= size)
        {
            return true;
        }
        return false;
    }
}
