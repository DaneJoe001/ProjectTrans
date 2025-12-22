/**
 * @file url_info.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief URL信息
 * @version 0.2.0
 * @date 2025-12-17
 */
#pragma once

#include <string>
#include <cstdint>
#include <unordered_map>

#include "danejoe/common/enum/enum_convert.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe命名空间
  */
namespace DaneJoe
{
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
     * @brief 将协议枚举转换为字符串（内部调试用）
     * @param protocol 协议枚举
     * @return 对应的枚举字符串
     */
    std::string to_string(UrlProtocol protocol);
    /**
     * @brief 从字符串转换为协议枚举
     * @param enum_string 由 ENUM_TO_STRING 生成的枚举字符串
     * @return 对应的协议枚举，未知时返回 UrlProtocol::Unknown
     */
    template<>
    UrlProtocol enum_cast<UrlProtocol>(const std::string& enum_string);
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
}
