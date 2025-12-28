#include <format>
#include <cctype>

#include "danejoe/network/url/url_resolver.hpp"
#include "danejoe/common/diagnostic/diagnostic_system.hpp"
#include "danejoe/stringify/stringify_to_string.hpp"

DaneJoe::UrlInfo DaneJoe::UrlResolver::parse(const std::string& url)
{
    /// @todo URL转义解码
    // 构建并初始化Url信息
    UrlInfo info;
    info.protocol = UrlProtocol::Unknown;
    info.host = "";
    info.port = 0;
    info.path = "/";
    // Url起始下标
    // http://127.0.0.1:8080/download?file_id=1
    std::size_t current_pos = 0;
    // 片段标识符位置
    auto url_end_pos = url.find("#");
    // 检查是否存在片段标识符，不存在则以长度为结束位置
    url_end_pos = (url_end_pos == std::string::npos) ? url.size() : url_end_pos;
    // 查找协议结束位置
    auto protocol_end_pos = url.find("://");
    // 找不到时返回空信息
    if (protocol_end_pos == std::string::npos)
    {
        ADD_DIAG_ERROR("network", "Parse URL failed: missing protocol separator '://': {}", url);
        return info;
    }
    // 获取协议字符串
    std::string protocol_str = url.substr(current_pos, protocol_end_pos - current_pos);
    // 转换为协议枚举
    info.protocol = to_protocol(protocol_str);
    // 更新当前位置
    // 127.0.0.1:8080/download?file_id=1
    current_pos = protocol_end_pos + 3;
    // 检查是否存在剩余参数
    if (current_pos > url_end_pos)
    {
        return info;
    }
    // 端口号标记位置
    /// @todo IPv6支持
    auto port_sign_pos = url.find(":", current_pos);
    // 域名结束位置
    auto host_port_end_pos = url.find("/", current_pos);

    // 当url不存在路径时，路径结束位置为url结束位置
    if (host_port_end_pos == std::string::npos)
    {
        host_port_end_pos = url_end_pos;
    }
    // 当端口号标记位置在域名结束位置之后时，视为不存在端口号
    if (port_sign_pos > host_port_end_pos)
    {
        port_sign_pos = std::string::npos;
    }
    if (port_sign_pos == std::string::npos)
    {
        // 当没有端口号时
        std::string host_str = url.substr(current_pos, host_port_end_pos - current_pos);
        info.host = to_ip(host_str);
        // 获取默认端口号
        info.port = default_port(info.protocol);
    }
    else
    {
        // 当有端口号时
        std::string host_str = url.substr(current_pos, port_sign_pos - current_pos);
        info.host = to_ip(host_str);
        // 更新当前位置
        // 8080/download?file_id=1
        current_pos = port_sign_pos + 1;
        // 检查是否存在剩余参数
        if (current_pos > url_end_pos)
        {
            return info;
        }
        // 获取端口
        std::string port_str = url.substr(current_pos, host_port_end_pos - current_pos);
        info.port = std::stoi(port_str);
    }
    // 更新当前位置
    // download?file_id=1
    current_pos = host_port_end_pos;
    // 检查是否存在剩余参数
    if (current_pos > url_end_pos)
    {
        return info;
    }
    // 查询参数位置
    auto query_pos = url.find("?", current_pos);

    // '#'所在位置或者字符串结尾位置
    std::size_t path_end_pos = url_end_pos;

    // 检查是否存在查询参数
    if (query_pos != std::string::npos)
    {
        // 查询参数标识符位置即路径结束位置
        path_end_pos = query_pos;
    }
    // 获取路径
    std::string path_str = path_end_pos == current_pos ? "/" : url.substr(current_pos, path_end_pos - current_pos);
    info.path = path_str;
    // 更新当前位置
    // file_id=1
    current_pos = path_end_pos + 1;
    // 检查是否存在剩余参数
    if (current_pos > url_end_pos)
    {
        return info;
    }
    while (true)
    {
        auto equal_pos = url.find("=", current_pos);
        if (equal_pos == std::string::npos)
        {
            return info;
        }
        // 获取查询参数键值对
        auto query_key = url.substr(current_pos, equal_pos - current_pos);
        // 更新当前位置
        current_pos = equal_pos + 1;
        // 检查是否存在查询参数值
        if (current_pos > url_end_pos)
        {
            return info;
        }
        auto and_pos = url.find("&", current_pos);
        auto query_value_end_pos = and_pos == std::string::npos ? url_end_pos : and_pos;
        auto query_value = url.substr(current_pos, query_value_end_pos - current_pos);
        // 插入查询参数
        info.query.insert({ query_key,query_value });
        // 更新当前位置
        current_pos = query_value_end_pos + 1;
        // 检查是否存在剩余参数
        if (current_pos > url_end_pos)
        {
            return info;
        }
    }
    return info;
}

std::string DaneJoe::UrlResolver::build(const UrlInfo& info)
{
    // 构建url
    std::string result = std::format("{}://{}:{}{}", to_string(info.protocol), info.host, info.port, info.path);
    if (info.query.size() > 0)
    {
        result += "?";
        for (auto& [key, value] : info.query)
        {
            result += std::format("{}={}&", key, value);
        }
        // 移除最后一个多余的&
        result.pop_back();
    }
    return result;
}

DaneJoe::UrlProtocol DaneJoe::UrlResolver::to_protocol(const std::string& protocol)
{
    // 由协议字符串转换为枚举
    std::string temp = protocol;
    for (auto& ch : temp)
    {
        ch = std::tolower(ch);
    }
    if (temp == "http")
    {
        return UrlProtocol::Http;
    }
    else if (temp == "https")
    {
        return UrlProtocol::Https;
    }
    else if (temp == "ftp")
    {
        return UrlProtocol::Ftp;
    }
    else if (temp == "danejoe")
    {
        return UrlProtocol::Danejoe;
    }
    else
    {
        return UrlProtocol::Unknown;
    }
}

std::string DaneJoe::UrlResolver::to_ip(const std::string& url)
{
    /// @todo parse IPv4 and more
    ADD_DIAG_WARN("network", "Resolve IP skipped: current IP parser is not implemented");
    ADD_DIAG_WARN("network", "Resolve IP skipped: return raw url: {}", url);
    return url;
}

uint16_t DaneJoe::UrlResolver::default_port(UrlProtocol protocol)
{
    // 获取普通协议默认的端口号
    switch (protocol)
    {
    case UrlProtocol::Http:
        return 80;
    case UrlProtocol::Https:
        return 443;
    case UrlProtocol::Ftp:
        return 21;
    case UrlProtocol::Danejoe:
        return 8080;
    default:
        return 0;
    }

}

std::string DaneJoe::UrlResolver::to_string(UrlProtocol protocol)
{
    // 从协议枚举转为对应字符串
    switch (protocol)
    {
    case UrlProtocol::Http:
        return "http";
    case UrlProtocol::Https:
        return "https";
    case UrlProtocol::Danejoe:
        return "danejoe";
    case UrlProtocol::Ftp:
        return "ftp";
    default:
        return "";
    }

}
