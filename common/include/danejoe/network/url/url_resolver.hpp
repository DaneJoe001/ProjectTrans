/**
 * @file url_resolver.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief URL解析器
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 UrlResolver，用于解析 URL 字符串为 UrlInfo，并支持将 UrlInfo 反向构建为 URL 字符串。
 *          同时提供协议/端口相关的辅助转换接口。
 */
#pragma once

#include <string>
#include <cstdint>

#include "danejoe/network/url/url_info.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class UrlResolver
     * @brief URL解析器
     * @details 提供一组静态方法用于 URL 的解析与构建：
     *          - parse()：将 URL 字符串解析为 UrlInfo
     *          - build()：将 UrlInfo 构建为 URL 字符串
     *          - default_port()：根据协议获取默认端口
     *          - to_protocol()/to_string()：协议字符串与枚举间转换
     */
    class UrlResolver
    {

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
         * @details 解析 URL 的协议、主机、端口、路径与查询参数。
         */
        static UrlInfo parse(const std::string& url);
        /**
         * @brief 构建URL
         * @param info URL信息
         * @return URL
         * @details 根据 UrlInfo 构造 URL 字符串。
         */
        static std::string build(const UrlInfo& info);
        /**
         * @brief 从 URL 提取 IP
         * @param url URL链接
         * @return IP地址
         * @details 该函数用于从 URL 中提取主机部分（通常为 IP 或域名字符串）。
         */
        static std::string to_ip(const std::string& url);
        /**
         * @brief 获取默认端口
         * @param protocol URL协议
         * @return 默认端口
         * @details 若协议未知则返回 0。
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
         * @details 未知时返回 UrlProtocol::Unknown。
         */
        static UrlProtocol to_protocol(const std::string& protocol);
    };
}
