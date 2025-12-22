/**
 * @file url_resolver.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief URL解析器
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <string>
#include <cstdint>

#include "danejoe/network/url/url_info.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
    /**
     * @class UrlResolver
     * @brief URL解析器
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
}
