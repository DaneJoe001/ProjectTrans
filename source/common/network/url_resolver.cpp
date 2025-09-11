#include <format>
#include <cctype>

#include "common/network/url_resolver.hpp"
#include "log/manage_logger.hpp"

UrlResolver::UrlInfo UrlResolver::parse(const std::string& url)
{
    UrlInfo info = { UrlProtocol::UNKNOWN, "", 0, "" };
    /// @example http://www.example.com:8080/path/to/resource
    auto pos = url.find("://");
    if (pos == std::string::npos)
    {
        DANEJOE_LOG_ERROR("default", "UrlResolver", "Invalid URL: {}", url);
        return info;
    }
    /// @brief 解析协议
    std::string protocol_str = url.substr(0, pos);
    info.protocol = to_protocol(protocol_str);
    /// @brief 解析ip地址
    auto port_sign_pos = url.find(":", pos + 3);
    auto ip_end_pos = url.find("/", pos + 3);
    if (ip_end_pos == std::string::npos)
    {
        ip_end_pos = url.size();
    }
    if (port_sign_pos == std::string::npos)
    {
        /// @brief 当没有端口号时
        info.ip = to_ip(url.substr(pos + 3, ip_end_pos - 3));
        info.port = default_port(info.protocol);
    }
    else
    {
        /// @brief 当有端口号时
        info.ip = to_ip(url.substr(pos + 3, port_sign_pos - protocol_str.size() - 3));
        info.port = std::stoi(url.substr(port_sign_pos + 1, ip_end_pos - port_sign_pos - 1));
    }
    info.path = url.substr(ip_end_pos);
    return info;
}

std::string UrlResolver::build(const UrlInfo& info)
{
    return std::format("{}://{}:{}{}", to_string(info.protocol), info.ip, info.port, info.path);
}

UrlResolver::UrlProtocol UrlResolver::to_protocol(const std::string& protocol)
{
    std::string temp = protocol;
    for (auto& ch : temp)
    {
        ch = std::tolower(ch);
    }
    if (temp == "http")
    {
        return UrlProtocol::HTTP;
    }
    else if (temp == "https")
    {
        return UrlProtocol::HTTPS;
    }
    else if (temp == "danejoe")
    {
        return UrlProtocol::DANEJOE;
    }
    else
    {
        return UrlProtocol::UNKNOWN;
    }
}

std::string UrlResolver::to_ip(const std::string& url)
{
    /// @todo parse IPv4 and more
    DANEJOE_LOG_WARN("default", "UrlResolver", "Current IP parser is not implemented!");
    DANEJOE_LOG_WARN("default", "UrlResolver", "Return raw url: {}!", url);
    return url;
}

uint16_t UrlResolver::default_port(UrlProtocol protocol)
{
    switch (protocol)
    {
    case UrlProtocol::HTTP:
        return 80;
    case UrlProtocol::HTTPS:
        return 443;
    case UrlProtocol::DANEJOE:
        return 8080;
    default:
        return 0;
    }

}

std::string UrlResolver::to_string(UrlProtocol protocol)
{
    switch (protocol)
    {
    case UrlProtocol::HTTP:
        return "http";
    case UrlProtocol::HTTPS:
        return "https";
    case UrlProtocol::DANEJOE:
        return "danejoe";
    default:
        return "";
    }

}