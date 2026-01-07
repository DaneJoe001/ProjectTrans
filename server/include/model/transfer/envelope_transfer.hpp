/**
 * @file envelope_transfer.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 信封传输模型
 * @date 2026-01-06
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

/**
 * @enum ContentType
 * @brief 内容类型
 */
enum class ContentType :uint8_t
{
    /// @brief application/json
    Json,
    /// @brief danejoe serialized
    DaneJoe,
    /// @brief 未知类型
    Unknown,
};

/**
 * @enum ResponseStatus
 * @brief 响应状态
 */
enum class ResponseStatus :uint16_t
{
    ///< 未知状态
    Unknown = 0,
    /// @brief 请求成功
    Ok = 200,
    /// @brief 请求成功并创建了资源
    Created = 201,
    /// @brief 请求已接受，但未处理完成
    Accepted = 202,
    /// @brief 请求成功，但没有内容返回
    NoContent = 204,
    /// @brief 错误的请求
    BadRequest = 400,
    /// @brief 未授权
    Unauthorized = 401,
    /// @brief 禁止访问
    Forbidden = 403,
    /// @brief 找不到资源
    NotFound = 404,
    /// @brief 方法不被允许
    MethodNotAllowed = 405,
    /// @brief 请求冲突
    Conflict = 409,
    /// @brief 服务器内部错误
    InternalServerError = 500,
    /// @brief 未实现的功能
    NotImplemented = 501,
    /// @brief 错误的网关
    BadGateway = 502,
    /// @brief 服务不可用
    ServiceUnavailable = 503
};

/**
 * @brief 将内容类型转换为字符串
 * @param content_type 内容类型
 * @return 字符串
 */
std::string to_string(ContentType content_type);

/**
 * @brief 将响应状态转换为字符串
 * @param status 响应状态
 * @return 字符串
 */
std::string to_string(ResponseStatus status);

/**
 * @struct EnvelopeRequestTransfer
 * @brief 信封请求传输模型
 */
struct EnvelopeRequestTransfer
{
    /// @brief 协议版本
    uint16_t version;
    /// @brief 请求ID
    uint64_t request_id;
    /// @brief 请求类型
    uint8_t request_type;
    /// @brief 请求路径
    std::string path;
    /// @brief 内容类型
    ContentType content_type;
    /// @brief 请求体
    std::vector<uint8_t> body;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};

/**
 * @struct EnvelopeResponseTransfer
 * @brief 信封响应传输模型
 */
struct EnvelopeResponseTransfer
{
    /// @brief 协议版本
    uint16_t version;
    /// @brief 请求ID
    uint64_t request_id;
    /// @brief 响应状态
    ResponseStatus status;
    /// @brief 内容类型
    ContentType content_type;
    /// @brief 响应消息体
    std::vector<uint8_t> body;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};