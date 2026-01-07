/**
  * @file test_transfer.hpp
  * @brief 测试请求/响应传输模型
  * @author DaneJoe001
  * @date 2026-01-06
  */
#pragma once

#include <string>

/**
 * @struct TestRequestTransfer
 * @brief 测试请求传输对象
 * @details 用于进行连通性/功能测试的请求消息。
 */
struct TestRequestTransfer
{
    /// @brief 消息
    std::string message;
    /**
     * @brief 转换为字符串
     * @return 字符串
     * @details 将测试请求传输对象的各字段转换为字符串描述。
     */
    std::string to_string() const;
};

/**
 * @struct TestResponseTransfer
 * @brief 测试响应传输对象
 * @details 服务端对测试请求的响应消息。
 */
struct TestResponseTransfer
{
    /// @brief 消息
    std::string message;
    /**
     * @brief 转换为字符串
     * @return 字符串
     * @details 将测试响应传输对象的各字段转换为字符串描述。
     */
    std::string to_string() const;
};