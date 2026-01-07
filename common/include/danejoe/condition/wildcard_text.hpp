/**
 * @file wildcard_text.hpp
 * @brief 通配符文本
 * @author DaneJoe001
 * @version 0.2.0
 * @date 2025-12-04
 * @details 定义 WildcardText，用于根据指定匹配模式对输入文本进行匹配判定。
 *          支持精确/部分/前缀/后缀匹配，以及正则匹配；并支持大小写敏感与长度约束等附加条件。
 */
#pragma once

#include <string>
#include <vector>
#include <optional>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @enum MatchPattern
     * @brief 匹配模式
     * @details 用于指定 WildcardText 的匹配策略。
     *          RegexMatch 的正则语法与实现相关，通常遵循 C++ 标准库 std::regex 规则。
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
     * @class WildcardText
     * @brief 通配符文本
     * @details 表达一条文本匹配规则：
     *          - m_text：模式文本
     *          - m_match_pattern：匹配模式
     *          - m_is_case_sensitive：是否区分大小写
     *          - m_min_length/m_max_length：对输入文本长度的约束（可选）
     *          - m_is_included_rule：是否为“包含/排除”规则标记（用于上层逻辑组合）
     */
    class WildcardText
    {
    public:
        /**
         * @brief 默认构造函数
         * @param is_included_rule 是否为包含规则
         */
        WildcardText(bool is_included_rule = true);
        /**
         * @brief 构造函数
         * @param text 文本
         * @param match_pattern 匹配模式
         * @param is_included_rule 是否为包含规则
         */
        WildcardText(
            const std::string& text,
            MatchPattern match_pattern,
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
         * @details 同时更新匹配文本与匹配模式。
         */
        void set_text(
            const std::string& text,
            MatchPattern match_pattern);
        /**
         * @brief 设置匹配模式
         * @param match_pattern 匹配模式
         */
        void set_matched_pattern(MatchPattern match_pattern);
        /**
         * @brief 设置匹配长度
         * @param min_length 最小长度
         * @param max_length 最大长度
         * @details 为输入文本设置长度约束；传入 std::nullopt 表示不限制。
         */
        void set_matched_length(
            std::optional<std::size_t>min_length,
            std::optional<std::size_t>max_length);
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
         * @details 当不区分大小写时，比较与匹配将以统一的大小写形式进行。
         */
        void set_case_sensitive(bool is_case_sensitive);
        /**
         * @brief 判断文本是否相等
         * @param text_1 文本1
         * @param text_2 文本2
         * @return true 相等
         * @return false 不相等
         */
        bool is_equal(
            const std::string& text_1,
            const std::string& text_2)const;
        /**
         * @brief 判断文本是否匹配
         * @param text 文本
         * @return true 匹配
         * @return false 不匹配
         * @details 综合匹配模式与附加约束（大小写/长度等）进行判定。
         */
        bool is_match(const std::string& text) const;
        /**
         * @brief 判断文本是否完全匹配
         * @param text 待匹配文本
         * @return true 完全匹配
         * @return false 不完全匹配
         * @details 与 is_match() 的区别由实现决定；通常用于表达“严格满足当前规则”。
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
         * @details 该标记用于上层逻辑组合（例如白名单/黑名单规则）。
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
         * @details 返回规则的字符串表示，用于调试与哈希等用途。
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
 * @brief std 命名空间
 */
namespace std
{
    /**
     * @brief 通配符文本哈希结构
     * @details 提供 WildcardText 的哈希支持，使其可作为 unordered_set/unordered_map 的键。
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
