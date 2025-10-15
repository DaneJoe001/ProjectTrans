#pragma once

/**
 * @file url_resolver.hpp
 * @brief URL解析器
 * @author DaneJoe001
 */

#include <string>
#include <cstdint>
#include <unordered_map>

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
        Unknown,
        /// @brief HTTP协议
        Http,
        /// @brief HTTPS协议
        Https,
        /// @brief FTP协议
        Ftp,
        /// @brief 个人测试用
        Danejoe,
    };
    /**
     * @struct UrlInfo
     * @brief URL信息
     */
    struct UrlInfo
    {
        /// @brief URL协议
        UrlProtocol protocol;
        /// @brief 主机地址
        std::string host;
        /// @brief 端口
        uint16_t port;
        /// @brief 路径
        std::string path;
        /// @brief 查询参数
        std::unordered_multimap<std::string, std::string> query;
        /**
         * @brief 将结构体字符串化，便于调试输出
         * @return 返回字符串化结果
         */
        std::string to_string() const;
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