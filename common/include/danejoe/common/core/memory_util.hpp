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
     * @return true 容量足够或已扩容至目标大小
     * @note 当 data.size() < size 时会调用 resize(size)。
     * @note 若扩容触发内存分配失败，将以异常形式抛出（函数不会返回 false）。
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
     * @brief 确保容器有足够的数据可读
     * @tparam T 容器类型
     * @param data 容器
     * @param size 需要读取的元素数量
     * @return true 数据长度足够
     * @return false 数据长度不足
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
