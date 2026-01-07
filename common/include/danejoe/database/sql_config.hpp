/**
 * @file sql_config.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库配置
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SqlConfig，用于描述数据库连接所需的基础配置项（名称、路径与认证信息等）。
 */
#pragma once

#include <string>

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @struct SqlConfig
     * @brief 数据库配置
     * @details 用于数据库驱动 connect() 的配置参数。
     *          - database_name：数据库名称
     *          - path：数据库文件路径/连接路径（具体含义由驱动实现决定）
     *          - user_name/user_password：认证信息（对不需要认证的后端可为空）
     */
    struct SqlConfig
    {
        /// @brief 数据库名称
        std::string database_name = "default";
        /// @brief 数据库路径
        std::string path = "./database/default.db";
        /// @brief 用户名
        std::string user_name;
        /// @brief 用户密码
        std::string user_password;
    };
}
