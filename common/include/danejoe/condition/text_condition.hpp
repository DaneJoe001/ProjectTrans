/**
 * @file text_condition.hpp
 * @brief 文本条件
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-04
 * @details 定义 TextCondition，用于对输入文本进行基于 WildcardText 的匹配判定。
 *          TextCondition 可注册多个匹配规则，并提供批量匹配、筛选匹配/未匹配文本等辅助能力。
 */
#pragma once

#include <unordered_set>
#include <unordered_map>

#include "danejoe/condition/i_condition.hpp"
#include "danejoe/condition/wildcard_text.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class TextCondition
     * @brief 文本条件
     * @details 基于 WildcardText 的文本匹配条件集合。
     *          - add_match_text() 用于注册一个通配符匹配规则
     *          - is_match() 用于判断文本（或文本列表）是否满足当前条件
     * @note 空条件（未注册任何规则）时的匹配行为取决于实现；通常可视为“不限制/全部匹配”或“全部不匹配”。
     */
    class TextCondition : public ICondition
    {
    public:
        /**
         * @brief 默认构造函数
         */
        TextCondition();
        /**
         * @brief 添加匹配文本
         * @param wildcard_text 通配符文本
         * @details 将 wildcard_text 作为一条匹配规则加入条件集合。
         */
        void add_match_text(const WildcardText& wildcard_text);
        /**
         * @brief 判断文本是否匹配
         * @param text 文本
         * @return true 匹配
         * @return false 不匹配
         * @details 对单个文本进行匹配判定。
         */
        bool is_match(const std::string& text)const;
        /**
         * @brief 判断文本是否匹配
         * @param texts 文本列表
         * @return true 匹配
         * @return false 不匹配
         * @details 对一组文本进行匹配判定。
         */
        bool is_match(const std::vector<std::string>& texts) const;
        /**
         * @brief 判断文本条件是否为空
         * @return true 为空
         * @return false 不为空
         * @details 当未添加任何匹配规则时返回 true。
         */
        bool is_empty()const;
        /**
         * @brief 获取匹配的文本
         * @param texts 文本列表
         * @return std::vector<std::string> 匹配的文本
         * @details 从 texts 中筛选出满足当前条件的元素。
         */
        std::vector<std::string> get_matched_texts(const std::vector<std::string>& texts)const;
        /**
         * @brief 获取未匹配的文本
         * @param texts 文本列表
         * @return std::vector<std::string> 未匹配的文本
         * @details 从 texts 中筛选出不满足当前条件的元素。
         */
        std::vector<std::string> get_unmatched_texts(const std::vector<std::string>& texts)const;
        /**
         * @brief 清空文本条件
         * @details 移除所有已注册的匹配规则。
         */
        void clear()override;
    private:
        /// @brief 通配符文本
        std::unordered_map<std::string, std::unordered_set<WildcardText>> m_match_texts;
    };
}
