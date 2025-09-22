#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "common/network/url_resolver.hpp"

/**
 * @brief 用于构建和解析消息
 */

class MessageHandler
{
public:
    static UrlResolver::UrlInfo add_account_info(const UrlResolver::UrlInfo& raw_info, const std::string& account, const std::string& password);
};