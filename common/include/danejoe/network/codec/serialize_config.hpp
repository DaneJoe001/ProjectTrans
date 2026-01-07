/**
 * @file serialize_config.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 序列化配置
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SerializeConfig，用于约束序列化/反序列化过程中的边界条件（字段长度等）以及构建阶段的预分配策略。
 */
#pragma once

#include <cstdint>

/**
 * @namespace DaneJoe
 * @brief DaneJoe 命名空间
 */
namespace DaneJoe
{
    /**
     * @struct SerializeConfig
     * @brief 序列化配置
     * @details 用于控制序列化编码器的行为：
     *          - max_field_value_length/max_field_name_length：字段长度上限（用于边界检查）
     *          - pre_allocated_size：构建序列化字节流时的预分配容量
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
