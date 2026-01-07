/**
 * @file stringify_format.hpp
 * @brief 字符串化格式化
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-16
 * @details 提供 stringify 相关的文本格式化工具函数，用于生成标题、分隔符、列表以及容量大小等可读字符串。
 *          具体符号与单位展示通常由 StringifyConfig 及其子结构（如 StorageSymbol）控制。
 */
#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "danejoe/stringify/stringify_config.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @enum FormatPosition
     * @brief 格式化位置
     * @details 用于控制文本在指定宽度内的对齐方式。
     */
    enum class FormatPosition
    {
        /// @brief 左对齐
        LEFT,
        /// @brief 居中对齐
        CENTER,
        /// @brief 右对齐
        RIGHT,
        /// @brief 未知位置
        Unknown
    };
    /**
     * @brief 将格式化位置转换为字符串（调试用）
     * @param position 格式化位置
     * @return 对应的枚举字符串
     */
    std::string to_string(FormatPosition position);
    /**
     * @brief 从字符串转换为格式化位置
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的枚举值，未知时返回 FormatPosition::Unknown
     */
    template<>
    FormatPosition enum_cast<FormatPosition>(const std::string& enum_string);
    /**
     * @brief 格式化标题
     * @param title 标题
     * @param sign 分隔符
     * @param width 宽度
     * @param position 位置
     * @return 格式化后的标题
     * @details 在给定宽度内按 position 对齐 title，并使用 sign 填充剩余区域。
     */
    std::string format_title(
        const std::string& title,
        const std::string& sign = "=",
        std::size_t width = 50,
        FormatPosition position = FormatPosition::CENTER);
    /**
     * @brief 格式化分隔符
     * @param sign 分隔符
     * @param width 宽度
     * @return 格式化后的分隔符
     * @details 生成由 sign 重复组成的分隔线字符串。
     */
    std::string format_separator(
        const std::string& sign = "-",
        std::size_t width = 50);
    /**
     * @brief 格式化字符串列表
     * @param space_counter 空格数量
     * @param item_sign 项目符号
     * @param list 列表
     * @param is_add_index 是否添加索引
     * @param begin_index 起始索引
     * @return 格式化后的字符串列表
     * @details 将 list 按行格式化。
     *          每行可添加 space_counter 个前导空格与 item_sign；当 is_add_index 为 true 时附加从 begin_index 开始的序号。
     */
    std::string format_string_list(
        int space_counter,
        const std::string& item_sign,
        const std::vector<std::string>& list,
        bool is_add_index = false,
        int begin_index = 1);
    /**
     * @brief 获取存储单位符号
     * @param unit 存储单位
     * @return 存储单位符号
     * @details 单位符号通常由 StringifyConfig::storage_symbol 控制。
     */
    std::string get_storage_unit_symbol(StorageUnit unit);
    /**
     * @brief 格式化容量大小
     * @param size 容量大小(Bytes)
     * @param dest_unit 目标单位
     * @param precision 精度
     * @return 格式化后的容量大小
     * @details 将字节数换算为指定单位并按 precision 控制小数位数。
     */
    std::string format_capacity_size(
        uint64_t size,
        StorageUnit dest_unit,
        std::size_t precision = 0);
}
