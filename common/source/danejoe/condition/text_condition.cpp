#include "danejoe/condition/text_condition.hpp"
#include "danejoe/common/enum/enum_convert.hpp"
#include "danejoe/common/core/variable_util.hpp"

std::string DaneJoe::to_string(SignDirection direction)
{
    switch (direction)
    {
    case SignDirection::SmallToLarge:
        return ENUM_TO_STRING(SignDirection::SmallToLarge);
    case SignDirection::LargeToSmall:
        return ENUM_TO_STRING(SignDirection::LargeToSmall);
    case SignDirection::VarLeft:
        return ENUM_TO_STRING(SignDirection::VarLeft);
    case SignDirection::VarRight:
        return ENUM_TO_STRING(SignDirection::VarRight);
    case SignDirection::Unknown:
    default:
        return ENUM_TO_STRING(SignDirection::Unknown);
    }
}

template<>
DaneJoe::SignDirection DaneJoe::enum_cast<DaneJoe::SignDirection>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(SignDirection::SmallToLarge))
    {
        return SignDirection::SmallToLarge;
    }
    else if (enum_string == ENUM_TO_STRING(SignDirection::LargeToSmall))
    {
        return SignDirection::LargeToSmall;
    }
    else if (enum_string == ENUM_TO_STRING(SignDirection::VarLeft))
    {
        return SignDirection::VarLeft;
    }
    else if (enum_string == ENUM_TO_STRING(SignDirection::VarRight))
    {
        return SignDirection::VarRight;
    }
    return SignDirection::Unknown;
}

std::string DaneJoe::to_string(ConditionRelation relation)
{
    switch (relation)
    {
    case ConditionRelation::Or:
        return ENUM_TO_STRING(ConditionRelation::Or);
    case ConditionRelation::And:
        return ENUM_TO_STRING(ConditionRelation::And);
    case ConditionRelation::Xor:
        return ENUM_TO_STRING(ConditionRelation::Xor);
    case ConditionRelation::Not:
        return ENUM_TO_STRING(ConditionRelation::Not);
    case ConditionRelation::Unknown:
    default:
        return ENUM_TO_STRING(ConditionRelation::Unknown);
    }
}

template<>
DaneJoe::ConditionRelation DaneJoe::enum_cast<DaneJoe::ConditionRelation>(const std::string& enum_string)
{
    if (enum_string == ENUM_TO_STRING(ConditionRelation::Or))
    {
        return ConditionRelation::Or;
    }
    else if (enum_string == ENUM_TO_STRING(ConditionRelation::And))
    {
        return ConditionRelation::And;
    }
    else if (enum_string == ENUM_TO_STRING(ConditionRelation::Xor))
    {
        return ConditionRelation::Xor;
    }
    else if (enum_string == ENUM_TO_STRING(ConditionRelation::Not))
    {
        return ConditionRelation::Not;
    }
    return ConditionRelation::Unknown;
}

DaneJoe::TextCondition::TextCondition() {}

void DaneJoe::TextCondition::add_match_text(const WildcardText& wildcard_text)
{
    auto text = wildcard_text.get_text();
    m_match_texts[text].insert(wildcard_text);
}

bool DaneJoe::TextCondition::is_match(const std::string &text) const {
  DANEJOE_UNUSED(text)
    return false;
}

bool DaneJoe::TextCondition::is_match(
    const std::vector<std::string> &texts) const {
  DANEJOE_UNUSED(texts)
    return false;
}

std::vector<std::string> DaneJoe::TextCondition::get_matched_texts(
    const std::vector<std::string> &texts) const {
  DANEJOE_UNUSED(texts)
    if (is_empty())
    {
        return std::vector<std::string>{};
    }
    std::vector<std::string> result;
    return result;
}

std::vector<std::string> DaneJoe::TextCondition::get_unmatched_texts(
    const std::vector<std::string>& texts) const
{
    if (is_empty())
    {
        return texts;
    }
    std::vector<std::string> result;
    return result;
}

void DaneJoe::TextCondition::clear()
{

}

bool DaneJoe::TextCondition::is_empty() const
{
    return m_match_texts.empty();
}