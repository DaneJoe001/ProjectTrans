/**
 * @file sql_database.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SqlDatabase，用于封装数据库驱动（ISqlDriver）与连接配置（SqlConfig）。
 *          该类提供对底层驱动的简单包装，负责保存配置并触发连接。
 */
#pragma once

#include <memory>

#include "danejoe/database/sql_config.hpp"
#include "danejoe/database/i_sql_driver.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqlDatabase
     * @brief 数据库
     * @details 轻量级数据库对象，内部持有一个 ISqlDriver 实例以及对应 SqlConfig。
     *          - set_config() 用于设置连接参数
     *          - connect() 触发 driver->connect(config)
     * @note 线程安全性由具体实现决定；除非实现内部做了同步，否则应避免并发调用。
     */
    class SqlDatabase
    {
    public:
        /**
         * @brief 构造函数
         * @param driver 数据库驱动
         * @details driver 用于执行实际的连接、prepare/bind/execute 等数据库操作。
         */
        SqlDatabase(std::shared_ptr<ISqlDriver> driver);
        /**
         * @brief 设置数据库配置
         * @param config 数据库配置
         * @details 设置 connect() 时使用的配置参数。
         */
        void set_config(const SqlConfig& config);
        /**
         * @brief 连接数据库
         * @return true 连接成功
         * @return false 连接失败
         * @details 使用已设置的 m_config 调用底层驱动进行连接。
         */
        bool connect();
        /**
         * @brief 获取数据库驱动
         * @return 数据库驱动
         * @details 返回内部持有的驱动指针。
         */
        std::shared_ptr<ISqlDriver> get_driver();
    private:
        /// @brief 数据库配置
        SqlConfig m_config;
        /// @brief 数据库驱动
        std::shared_ptr<ISqlDriver> m_driver;
    };
    /// @brief 数据库指针(shared_ptr)
    using SqlDatabasePtr = std::shared_ptr<SqlDatabase>;

}
