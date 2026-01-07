/**
 * @file sql_condition.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库条件信息
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SqlConditionItem，用于描述 SQL 构建/执行过程中所需的单个条件项信息。
 *          条件项包含列信息、条件对象（ICondition）以及可选的排序方向等元数据。
 */
#pragma once

#include <memory>
#include <optional>

#include "danejoe/condition/i_condition.hpp"
#include "danejoe/database/sql_schema.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{

    /**
     * @struct SqlConditionItem
     * @brief 数据库条件信息
     * @details 用于表达单个列的过滤/排序条件。
     *          - is_set：标记该条件项是否生效（未设置时可被构建器忽略）
     *          - column_info：目标列信息
     *          - condition：条件对象（表达比较/范围/文本等语义，具体由实现决定）
     *          - is_desc_order：可选的排序方向（true 表示降序，false 表示升序，std::nullopt 表示未指定）
     */
    struct SqlConditionItem
    {
        /// @brief 是否设置
        bool is_set = false;
        /// @brief 列信息
        SqlColumnItem column_info;
        /// @brief 条件
        std::shared_ptr<ICondition> condition;
        /// @brief 是否降序
        std::optional<bool> is_desc_order;
    };
}
