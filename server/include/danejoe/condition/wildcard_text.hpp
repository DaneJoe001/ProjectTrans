/**
 * @file wildcard_text.hpp
 * @brief 通配符文本
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-04
 */
#pragma once

#include <string>
#include <vector>
#include <optional>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe
  */
namespace DaneJoe
{
    /**
     * @enum MatchPattern
     * @brief 匹配模式
     */
    enum class MatchPattern
    {
        /// @brief 精确匹配
        ExactMatch,
        /// @brief 部分匹配
        PartialMatch,
        /// @brief 前缀匹配
        PrefixMatch,
        /// @brief 后缀匹配
        SuffixMatch,
        /// @brief 正则匹配
        RegexMatch,
        /// @brief 未知匹配模式
        Unknown,
    };
    /**
     * @brief 转换为字符串
     * @param match_pattern 匹配模式
     * @return std::string 字符串
     */
    std::string to_string(MatchPattern match_pattern);
    /**
     * @brief 通配符文本
     */
    class WildcardText
    {
    public:
        /**
         * @brief 默认构造函数
         */
        WildcardText(bool is_included_rule = true);
        /**
         * @brief 构造函数
         * @param text 文本
         * @param match_pattern 匹配模式
         */
        WildcardText(const std::string& text, MatchPattern match_pattern,
            bool is_included_rule = true);
        /**
         * @brief 重载等于运算符
         * @param other 其他对象
         * @return true 等于
         * @return false 不等于
         */
        bool operator==(const WildcardText& other) const;
        /**
         * @brief 设置文本
         * @param text 文本
         * @param match_pattern 匹配模式
         */
        void set_text(const std::string& text, MatchPattern match_pattern);
        /**
         * @brief 设置匹配模式
         */
        void set_matched_pattern(MatchPattern match_pattern);
        /**
         * @brief 设置匹配长度
         * @param min_length 最小长度
         * @param max_length 最大长度
         */
        void set_matched_length(
            std::optional<std::size_t>min_length, std::optional<std::size_t>max_length);
        /**
         * @brief 设置前部分内容长度
         * @param length 长度
         */
        void set_front_context_length(std::optional<std::size_t>length);
        /**
         * @brief 设置后部分内容长度
         * @param length 长度
         */
        void set_back_context_length(std::optional<std::size_t>length);
        /**
         * @brief 设置是否区分大小写
         * @param is_case_sensitive 是否区分大小写
         */
        void set_case_sensitive(bool is_case_sensitive);
        /**
         * @brief 判断文本是否相等
         * @param text_1 文本1
         * @param text_2 文本2
         * @return true 相等
         * @return false 不相等
         */
        bool is_equal(const std::string& text_1, const std::string& text_2)const;
        /**
         * @brief 判断文本是否匹配
         * @param text 文本
         * @return true 匹配
         * @return false 不匹配
         */
        bool is_match(const std::string& text) const;
        /**
         * @brief 获取匹配的文本
         * @param text 待匹配文本
         * @return true 匹配
         * @return false 不匹配
         */
        bool is_all_match(const std::string& text) const;
        /**
         * @brief 判断文本是否前缀匹配
         * @param text 文本
         * @return true 前缀匹配
         * @return false 不前缀匹配
         */
        bool is_prefix_match(const std::string& text)const;
        /**
         * @brief 判断文本是否后缀匹配
         * @param text 文本
         * @return true 后缀匹配
         * @return false 不后缀匹配
         */
        bool is_suffix_match(const std::string& text)const;
        /**
         * @brief 判断文本是否部分匹配
         * @param text 文本
         * @return true 部分匹配
         * @return false 不部分匹配
         */
        bool is_partial_match(const std::string& text)const;
        /**
         * @brief 判断文本是否精确匹配
         * @param text 文本
         * @return true 精确匹配
         * @return false 不精确匹配
         */
        bool is_exact_match(const std::string& text)const;
        /**
         * @brief 判断文本是否正则匹配
         * @param text 文本
         * @return true 正则匹配
         * @return false 不正则匹配
         */
        bool is_regex_match(const std::string& text)const;
        /**
         * @brief 判断文本是否长度匹配
         * @param text 文本
         * @return true 长度匹配
         * @return false 不长度匹配
         */
        bool is_length_match(const std::string& text) const;
        /**
         * @brief 获取是否包含规则
         * @return true 包含规则
         * @return false 不包含规则
         */
        bool is_included_rule()const;
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
         * @brief 转换为小写字符串
         * @param text 文本
         * @return std::string 小写字符串
         */
        std::string to_lower_string(const std::string& text)const;
        /**
         * @brief 转换为字符串
         * @return std::string 字符串
         */
        std::string to_string()const;
        /**
         * @brief 获取匹配的文本
         * @return std::string 匹配的文本
         */
        std::string get_text()const;
    private:
        /// @brief 是否为包含规则
        bool m_is_included_rule = true;
        /// @brief 是否区分大小写
        bool m_is_case_sensitive = true;
        /// @brief 匹配模式
        MatchPattern m_match_pattern = MatchPattern::ExactMatch;
        /// @brief 匹配文本
        std::string m_text;
        /// @brief 最大长度
        std::optional<std::size_t> m_max_length = std::nullopt;
        /// @brief 最小长度
        std::optional<std::size_t> m_min_length = std::nullopt;
        /// @brief 前部分内容长度
        std::optional<std::size_t> m_front_context_length = std::nullopt;
        /// @brief 后部分内容长度
        std::optional<std::size_t> m_back_context_length = std::nullopt;
    };
} // namespace DaneJoe

/**
 * @namespace std
 * @brief std
 */
namespace std
{
    /**
     * @brief 通配符文本哈希结构
     */
    template<>
    struct hash<DaneJoe::WildcardText>
    {
        /**
         * @brief 哈希函数
         * @param wildcard_text 通配符文本
         * @return std::size_t 哈希值
         */
        std::size_t operator()(const DaneJoe::WildcardText& wildcard_text) const
        {
            return std::hash<std::string>()(wildcard_text.to_string());
        }
    };
}
