/**
 * @file text_condition.hpp
 * @brief 文本条件
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-04
 */
#pragma once

#include <unordered_set>
#include <unordered_map>

#include "danejoe/condition/i_condition.hpp"
#include "danejoe/condition/wildcard_text.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe
  */
namespace DaneJoe
{
    /**
     * @brief 文本条件
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
         */
        void add_match_text(const WildcardText& wildcard_text);
        /**
         * @brief 判断文本是否匹配
         * @param text 文本
         * @return true 匹配
         * @return false 不匹配
         */
        bool is_match(const std::string& text)const;
        /**
         * @brief 判断文本是否匹配
         * @param texts 文本列表
         * @return true 匹配
         * @return false 不匹配
         */
        bool is_match(const std::vector<std::string>& texts) const;
        /**
         * @brief 判断文本条件是否为空
         * @return true 为空
         * @return false 不为空
         */
        bool is_empty()const;
        /**
         * @brief 获取匹配的文本
         * @param texts 文本列表
         * @return std::vector<std::string> 匹配的文本
         */
        std::vector<std::string> get_matched_texts(const std::vector<std::string>& texts)const;
        /**
         * @brief 获取未匹配的文本
         * @param texts 文本列表
         * @return std::vector<std::string> 未匹配的文本
         */
        std::vector<std::string> get_unmatched_texts(const std::vector<std::string>& texts)const;
        /**
         * @brief 清空文本条件
         */
        void clear()override;
    private:
        /// @brief 通配符文本
        std::unordered_map<std::string, std::unordered_set<WildcardText>> m_match_texts;
    };
}
