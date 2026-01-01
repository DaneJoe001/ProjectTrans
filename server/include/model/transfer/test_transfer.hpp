#pragma once

#include <string>

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