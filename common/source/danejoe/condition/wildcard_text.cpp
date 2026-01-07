#include <sstream>

#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/common/core/variable_util.hpp"
#include "danejoe/condition/wildcard_text.hpp"
#include "danejoe/common/enum/enum_convert.hpp"

std::string DaneJoe::to_string(MatchPattern match_pattern)
{
    switch (match_pattern)
    {
    case MatchPattern::ExactMatch:
        return ENUM_TO_STRING(MatchPattern::ExactMatch);
    case MatchPattern::PartialMatch:
        return ENUM_TO_STRING(MatchPattern::PartialMatch);
    case MatchPattern::PrefixMatch:
        return ENUM_TO_STRING(MatchPattern::PrefixMatch);
    case MatchPattern::SuffixMatch:
        return ENUM_TO_STRING(MatchPattern::SuffixMatch);
    case MatchPattern::RegexMatch:
        return ENUM_TO_STRING(MatchPattern::RegexMatch);
    case MatchPattern::Unknown:
    default:
        return ENUM_TO_STRING(MatchPattern::Unknown);
    }
}

template<>
DaneJoe::MatchPattern DaneJoe::enum_cast<DaneJoe::MatchPattern>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(MatchPattern::ExactMatch))
    {
        return MatchPattern::ExactMatch;
    }
    else if (enum_string == ENUM_TO_STRING(MatchPattern::PartialMatch))
    {
        return MatchPattern::PartialMatch;
    }
    else if (enum_string == ENUM_TO_STRING(MatchPattern::PrefixMatch))
    {
        return MatchPattern::PrefixMatch;
    }
    else if (enum_string == ENUM_TO_STRING(MatchPattern::SuffixMatch))
    {
        return MatchPattern::SuffixMatch;
    }
    else if (enum_string == ENUM_TO_STRING(MatchPattern::RegexMatch))
    {
        return MatchPattern::RegexMatch;
    }
    return MatchPattern::Unknown;
}

DaneJoe::WildcardText::WildcardText(bool is_included_rule) : m_is_included_rule(is_included_rule) {}

DaneJoe::WildcardText::WildcardText(const std::string& text, MatchPattern match_pattern, bool is_included_rule) : m_is_included_rule(is_included_rule)
{
    set_text(text, match_pattern);
}

bool DaneJoe::WildcardText::operator==(const WildcardText& other)const
{
    return m_text == other.m_text && m_match_pattern == other.m_match_pattern &&
        m_min_length == other.m_min_length &&
        m_max_length == other.m_max_length &&
        m_front_context_length == other.m_front_context_length &&
        m_back_context_length == other.m_back_context_length &&
        m_is_case_sensitive == other.m_is_case_sensitive &&
        m_is_included_rule == other.m_is_included_rule;

}

void DaneJoe::WildcardText::set_text(const std::string& text, MatchPattern match_pattern)
{
    m_text = text;
    m_match_pattern = match_pattern;
}
void DaneJoe::WildcardText::set_matched_pattern(MatchPattern match_pattern)
{
    m_match_pattern = match_pattern;
}
void DaneJoe::WildcardText::set_matched_length(
    std::optional<std::size_t>min_length, std::optional<std::size_t>max_length)
{
    m_min_length = min_length;
    m_max_length = max_length;
}
void DaneJoe::WildcardText::set_front_context_length(std::optional<std::size_t>length)
{
    m_front_context_length = length;
}
void DaneJoe::WildcardText::set_back_context_length(std::optional<std::size_t>length)
{
    m_back_context_length = length;
}
void DaneJoe::WildcardText::set_case_sensitive(bool is_case_sensitive)
{
    m_is_case_sensitive = is_case_sensitive;
}
bool DaneJoe::WildcardText::is_equal(const std::string& text_1, const std::string& text_2)const
{
    if (text_1.size() != text_2.size())
    {
        return false;
    }
    for (std::size_t i = 0; i < text_1.size(); ++i)
    {
        char c1 = m_is_case_sensitive ? text_1[i] : std::tolower(text_1[i]);
        char c2 = m_is_case_sensitive ? text_2[i] : std::tolower(text_2[i]);
        if (c1 != c2)
        {
            return false;
        }
    }
    return true;
}
bool DaneJoe::WildcardText::is_match(const std::string& text) const
{
    if (m_is_included_rule)
    {
        return is_all_match(text);
    }
    else
    {
        return !is_all_match(text);
    }
}
bool DaneJoe::WildcardText::is_all_match(const std::string& text) const
{
    bool is_length_matched = is_length_match(text);
    if (!is_length_matched)
    {
        return false;
    }
    switch (m_match_pattern)
    {
    case MatchPattern::ExactMatch:
        return is_exact_match(text);
    case MatchPattern::PartialMatch:
        return is_partial_match(text);
    case MatchPattern::PrefixMatch:
        return is_prefix_match(text);
    case MatchPattern::SuffixMatch:
        return is_suffix_match(text);
    case MatchPattern::RegexMatch:
        return is_regex_match(text);
    default:
        return false;
    }
    return false;
}
bool DaneJoe::WildcardText::is_prefix_match(const std::string& text)const
{
    if (m_back_context_length.has_value())
    {
        if (text.size() != m_text.size() + m_back_context_length.value())
        {
            return false;
        }
    }
    std::string prefix_text = text.substr(0, m_text.size());
    return is_equal(prefix_text, m_text);
}
bool DaneJoe::WildcardText::is_suffix_match(const std::string& text)const
{
    if (m_front_context_length.has_value())
    {
        if (text.size() != m_text.size() + m_front_context_length.value())
        {
            return false;
        }
    }
    std::string suffix_text = text.substr(text.size() - m_text.size());
    return is_equal(suffix_text, m_text);
}
bool DaneJoe::WildcardText::is_partial_match(const std::string& text)const
{
    std::string text_copy = m_is_case_sensitive ? text : to_lower_string(text);
    std::string m_text_copy = m_is_case_sensitive ? m_text : to_lower_string(m_text);
    return text_copy.find(m_text_copy) != std::string::npos;
}
bool DaneJoe::WildcardText::is_exact_match(const std::string& text)const
{
    return is_equal(text, m_text);
}
bool DaneJoe::WildcardText::is_regex_match(const std::string& text)const
{
    DANEJOE_UNUSED(text)
    ADD_DIAG_WARN("condition", "Regex match skipped: not supported");
    return false;
}
bool DaneJoe::WildcardText::is_length_match(const std::string& text)const
{
    if (m_min_length.has_value())
    {
        if (text.size() < m_min_length.value())
        {
            return false;
        }
    }
    if (m_max_length.has_value())
    {
        if (text.size() > m_max_length.value())
        {
            return false;
        }
    }
    return text.size() >= m_text.size();
}
bool DaneJoe::WildcardText::is_included_rule() const
{
    return m_is_included_rule;
}
std::vector<std::string> DaneJoe::WildcardText::get_matched_texts(
    const std::vector<std::string>& texts) const
{
    std::vector<std::string> matched_texts;
    for (const auto& text : texts)
    {
        if (is_match(text))
        {
            matched_texts.push_back(text);
        }
    }
    return matched_texts;
}
std::vector<std::string> DaneJoe::WildcardText::get_unmatched_texts(
    const std::vector<std::string>& texts) const
{
    std::vector<std::string> unmatched_texts;
    for (const auto& text : texts)
    {
        if (!is_match(text))
        {
            unmatched_texts.push_back(text);
        }
    }
    return unmatched_texts;
}

std::string DaneJoe::WildcardText::to_lower_string(const std::string& text)const
{
    std::string result = text;
    for (auto& c : result)
    {
        c = std::tolower(c);
    }
    return result;
}

std::string DaneJoe::WildcardText::to_string()const
{
    std::stringstream ss;
    ss << "WildcardText:{ ";
    ss << "is_included_rule:" << (m_is_included_rule ? "true" : "false") << ", ";
    ss << "text:" << m_text << ", ";
    ss << "match_pattern:" << DaneJoe::to_string(m_match_pattern) << ", ";
    ss << "min_length:" << (m_min_length.has_value() ? std::to_string(m_min_length.value()) : "null") << ", ";
    ss << "max_length:" << (m_max_length.has_value() ? std::to_string(m_max_length.value()) : "null") << ", ";
    ss << "front_context_length:" << (m_front_context_length.has_value() ? std::to_string(m_front_context_length.value()) : "null") << ", ";
    ss << "back_context_length:" << (m_back_context_length.has_value() ? std::to_string(m_back_context_length.value()) : "null") << " ";
    ss << "}";
    return ss.str();
}

std::string DaneJoe::WildcardText::get_text()const
{
    return m_text;
}
