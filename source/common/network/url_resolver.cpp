#include <format>
#include <cctype>

#include "common/network/url_resolver.hpp"
#include "log/manage_logger.hpp"

UrlResolver::UrlInfo UrlResolver::parse(const std::string& url)
{
    // 构建并初始化Url信息
    UrlInfo info = { UrlProtocol::UNKNOWN, "", 0, "" };
    // 查找协议分割
    auto pos = url.find("://");
    // 找不到时返回空信息
    if (pos == std::string::npos)
    {
        DANEJOE_LOG_ERROR("default", "UrlResolver", "Invalid URL: {}", url);
        return info;
    }
    // 获取协议字符串
    std::string protocol_str = url.substr(0, pos);
    info.protocol = to_protocol(protocol_str);
    // 端口号标记位置
    auto port_sign_pos = url.find(":", pos + 3);
    // 域名结束位置
    auto ip_end_pos = url.find("/", pos + 3);
    // 当域名结束位置为空时，则将域名结束位置设置为URL长度
    if (ip_end_pos == std::string::npos)
    {
        ip_end_pos = url.size();
    }
    if (port_sign_pos == std::string::npos)
    {
        // 当没有端口号时
        info.ip = to_ip(url.substr(pos + 3, ip_end_pos - 3));
        // 获取默认端口号
        info.port = default_port(info.protocol);
    }
    else
    {
        // 当有端口号时
        info.ip = to_ip(url.substr(pos + 3, port_sign_pos - protocol_str.size() - 3));
        // 获取端口
        info.port = std::stoi(url.substr(port_sign_pos + 1, ip_end_pos - port_sign_pos - 1));
    }
    // 获取路径
    info.path = url.substr(ip_end_pos);
    return info;
}

std::string UrlResolver::build(const UrlInfo& info)
{
    // 构建url
    return std::format("{}://{}:{}{}", to_string(info.protocol), info.ip, info.port, info.path);
}

UrlResolver::UrlProtocol UrlResolver::to_protocol(const std::string& protocol)
{
    // 由协议字符串转换为枚举
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
    // 获取普通协议默认的端口号
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
    // 从协议枚举转为对应字符串
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