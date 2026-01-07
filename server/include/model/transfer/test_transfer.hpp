/**
 * @file test_transfer.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 测试传输模型
 * @date 2026-01-06
 */

#pragma once

#include <string>

/**
 * @struct TestRequestTransfer
 * @brief 测试请求传输模型
 */
struct TestRequestTransfer
{
    /// @brief 消息
    std::string message;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};

/**
 * @struct TestResponseTransfer
 * @brief 测试响应传输模型
 */
struct TestResponseTransfer
{
    /// @brief 消息
    std::string message;
    /**
     * @brief 转换为字符串
     * @return 字符串
     */
    std::string to_string() const;
};