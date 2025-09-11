#pragma once

#include <string>
#include <cstdint>

class UrlResolver
{
public:
    enum class UrlProtocol
    {
        UNKNOWN,
        HTTP,
        HTTPS,
        /// @brief 个人测试用
        DANEJOE,
    };
    struct UrlInfo
    {
        UrlProtocol protocol;
        std::string ip;
        uint16_t port;
        std::string path;
    };
public:
    UrlResolver();
    ~UrlResolver();
    static UrlInfo parse(const std::string& url);
    static std::string build(const UrlInfo& info);
    static UrlProtocol to_protocol(const std::string& protocol);
    static std::string to_string(UrlProtocol protocol);
    static std::string to_ip(const std::string& url);
    static uint16_t default_port(UrlProtocol protocol);
};