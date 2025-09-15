#pragma once

/**
 * @file url_resolver.hpp
 * @brief URL解析器
 * @author DaneJoe001
 */

#include <string>
#include <cstdint>

 /**
  * @class UrlResolver
  * @brief URL解析器
  */
class UrlResolver
{
public:
    /**
     * @enum UrlProtocol
     * @brief URL协议
     */
    enum class UrlProtocol
    {
        /// @brief 未知协议
        UNKNOWN,
        /// @brief HTTP协议
        HTTP,
        /// @brief HTTPS协议
        HTTPS,
        /// @brief 个人测试用
        DANEJOE,
    };
    /**
     * @struct UrlInfo
     * @brief URL信息
     */
    struct UrlInfo
    {
        /// @brief URL协议
        UrlProtocol protocol;
        /// @brief IP地址
        std::string ip;
        /// @brief 端口
        uint16_t port;
        /// @brief 路径
        std::string path;
    };
public:
    /**
     * @brief 构造函数
     */
    UrlResolver();
    /**
     * @brief 析构函数
     */
    ~UrlResolver();
    /**
     * @brief 解析URL
     * @param url URL
     * @return URL信息
     */
    static UrlInfo parse(const std::string& url);
    /**
     * @brief 构建URL
     * @param info URL信息
     * @return URL
     */
    static std::string build(const UrlInfo& info);
    /**
     * @brief 从 URL 提取 IP
     * @param url URL链接
     * @return IP地址
     */
    static std::string to_ip(const std::string& url);
    /**
     * @brief 获取默认端口
     * @param protocol URL协议
     * @return 默认端口
     */
    static uint16_t default_port(UrlProtocol protocol);
    /**
     * @brief 获取协议字符串
     * @param protocol 协议
     * @return 字符串
     */
    static std::string to_string(UrlProtocol protocol);
    /**
     * @brief 字符串转换协议枚举
     * @param protocol 协议字符串
     * @return 协议枚举
     */
    static UrlProtocol to_protocol(const std::string& protocol);
};