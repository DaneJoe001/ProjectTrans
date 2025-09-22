#include "client/connect/message_handler.hpp"

UrlResolver::UrlInfo MessageHandler::add_account_info(const UrlResolver::UrlInfo& raw_info, const std::string& account, const std::string& password)
{
    UrlResolver::UrlInfo result_info = raw_info;
    if (raw_info.protocol == UrlResolver::UrlProtocol::DANEJOE)
    {
        result_info.path += "?account=" + account + "&password=" + password;
    }
    return result_info;
}