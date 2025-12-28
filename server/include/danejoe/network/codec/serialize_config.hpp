/**
 * @file serialize_config.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 序列化配置
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <cstdint>

/**
 * @namespace DaneJoe
 * @brief DaneJoe命名空间
 */
namespace DaneJoe
{
    /**
     * @struct SerializeConfig
     * @brief 序列化配置
     */
    struct SerializeConfig
    {
        /// @brief 最大字段值长度
        uint64_t max_field_value_length = 40 * 1024 * 1024;
        /// @brief 最大字段名称长度
        uint32_t max_field_name_length = 128;
        /// @brief 预分配大小
        uint32_t pre_allocated_size = 4 * 1024;
        /**
         * @brief 构造函数
         */
        SerializeConfig();
    };
}
