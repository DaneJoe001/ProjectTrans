/**
 * @file sql_cell.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据单元
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义数据库数据单元 SqlCell 及其值类型 CellData。
 *          CellData 使用 std::variant 表达常见数据库字段值类型，其中 std::nullptr_t 用于表示 SQL NULL。
 *          SqlCell 携带字段类型（DataType）、实际数据以及列名信息，作为驱动与上层之间交换数据的统一载体。
 */
#pragma once

#include <variant>
#include <string>
#include <cstdint>
#include <vector>

#include "danejoe/common/core/data_type.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @typedef CellData
     * @brief 数据单元值类型
     * @details 用 std::variant 表达可被 SqlCell 承载的数据类型集合。
     *          - std::nullptr_t 表示 SQL NULL
     *          - std::vector<uint8_t> 通常用于表达二进制/BLOB 数据
     * @note SqlCell::type 与 CellData::data 的实际类型应保持一致；不一致时的解释由驱动/字符串化实现决定。
     */
    using CellData = ::std::variant<
        int8_t,
        uint8_t,
        int16_t,
        uint16_t,
        int32_t,
        uint32_t,
        int64_t,
        uint64_t,
        float,
        double,
        bool,
        ::std::string,
        ::std::vector<uint8_t>,
        ::std::nullptr_t
    >;
    /**
     * @struct SqlCell
     * @brief 数据单元
     * @details 用于在数据库驱动与业务逻辑之间传递单个字段值。
     *          - type：字段的数据类型枚举
     *          - data：字段值（可为 nullptr 表示 NULL）
     *          - column_name：字段/列名（可选，具体含义由调用方约定）
     */
    struct SqlCell
    {
        /// @brief 数据类型枚举
        DataType type;
        /// @brief 数据
        CellData data;
        /// @brief 列名
        std::string column_name;
    };
}
