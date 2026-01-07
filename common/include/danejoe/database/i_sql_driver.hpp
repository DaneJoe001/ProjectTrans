/**
 * @file i_sql_driver.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 数据库驱动接口
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义数据库驱动抽象接口 ISqlDriver，用于屏蔽不同数据库后端的连接、预编译、绑定与执行细节。
 *          典型使用流程为：connect() -> prepare() -> bind() -> execute_*() -> reset()/clear_bindings() -> close()。
 */
#pragma once

#include <memory>

#include "danejoe/database/sql_config.hpp"
#include "danejoe/database/sql_cell.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class ISqlDriver
     * @brief 数据库驱动接口
     * @details 该接口以 SQL 字符串作为语句标识，提供最小化的“准备-绑定-执行”能力。
     * @note 线程安全性由具体实现决定；除非实现明确声明为线程安全，否则应避免多线程并发调用同一实例。
     */
    class ISqlDriver
    {
    public:
        /**
         * @brief 连接数据库
         * @param config 数据库配置
         * @return true 连接成功
         * @return false 连接失败
         * @details 成功后方可调用 prepare()/execute_*() 等方法。
         */
        virtual bool connect(const SqlConfig& config) = 0;
        /**
         * @brief 准备SQL语句
         * @param sql SQL语句
         * @return true 准备成功
         * @return false 准备失败
         * @details 用于创建/缓存预编译语句。若 prepare() 失败，则后续 bind/execute 对同一 sql 的行为未定义。
         */
        virtual bool prepare(const std::string& sql) = 0;
        /**
         * @brief 绑定SQL语句
         * @param sql SQL语句
         * @param index 参数索引
         * @param cell 数据单元
         * @return true 绑定成功
         * @return false 绑定失败
         * @details 将 cell 绑定到已 prepare 的 sql 参数位。
         * @note index 的起始（0/1）与支持的 SqlCell 类型由具体实现决定。
         */
        virtual bool bind(const std::string& sql, int index, const SqlCell& cell) = 0;
        /**
         * @brief 重置SQL语句
         * @param sql SQL语句
         * @details 重置指定 sql 对应的预编译语句状态（例如游标/执行状态）。
         */
        virtual void reset(const std::string& sql) = 0;
        /**
         * @brief 清除SQL语句绑定
         * @param sql SQL语句
         * @details 清除指定 sql 已绑定的参数值，使其恢复到未绑定状态。
         */
        virtual void clear_bindings(const std::string& sql) = 0;
        /**
         * @brief 执行查询
         * @param sql SQL语句
         * @return 查询结果
         * @details 返回二维结果集：
         *          - 外层 vector：行
         *          - 内层 vector：列（以 SqlCell 表示）
         */
        virtual std::vector<std::vector<SqlCell>> execute_query(const std::string& sql) = 0;
        /**
         * @brief 执行命令
         * @param sql SQL语句
         * @return true 执行成功
         * @return false 执行失败
         * @details 用于执行不返回结果集的语句（例如 INSERT/UPDATE/DDL）。
         */
        virtual bool execute_command(const std::string& sql) = 0;
        /**
         * @brief 关闭数据库
         * @details 释放连接与相关资源；close() 后除非重新 connect()，否则实例不可继续使用。
         */
        virtual void close() = 0;
    };
    /// @brief 数据库驱动指针(shared_ptr)
    using ISqlDriverPtr = std::shared_ptr<ISqlDriver>;
}
