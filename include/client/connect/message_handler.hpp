#pragma once

#include <string>
#include <vector>
#include <list>
#include <cstdint>

#include "common/network/url_resolver.hpp"
#include "client/model/client_file_info.hpp"
#include "client/model/block_request_info.hpp"

/**
 * @brief 用于构建和解析消息
 */

class MessageHandler
{
public:
    static UrlResolver::UrlInfo add_account_info(const UrlResolver::UrlInfo& raw_info, const std::string& account, const std::string& password);
    static ClientFileInfo parse_raw_file_info(const std::string& raw_data, Operation operation);
    static std::list<BlockRequestInfo> calculate_block_info(const ClientFileInfo& file_info, const BlockParamConfig& config);
    static std::string build_block_request(const BlockRequestInfo& block_request_info);
};