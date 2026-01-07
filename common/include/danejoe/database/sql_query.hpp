/**
 * @file sql_query.hpp
 * @author DaneJoe (danejoe001.github)
 * @brief 查询
 * @version 0.2.0
 * @date 2025-12-17
 * @details 定义 SqlQuery，用于封装 SQL 字符串与数据库驱动，提供链式的 prepare/bind/execute 操作。
 *          典型使用流程为：prepare(sql) -> bind(...) -> execute_query()/execute_command() -> reset()/clear_bindings()。
 */
#pragma once

#include <memory>

#include "danejoe/database/i_sql_driver.hpp"
#include "danejoe/database/sql_database.hpp"

 /**
  * @namespace DaneJoe
  * @brief DaneJoe 命名空间
  */
namespace DaneJoe
{
    /**
     * @class SqlQuery
     * @brief 查询
     * @details 用于管理单条 SQL 语句的准备、参数绑定与执行。
     *          内部以弱引用保存驱动对象（m_driver），调用方需确保驱动在执行期间仍然存活。
     * @note 线程安全性由驱动实现决定；除非实现内部同步，否则不建议多线程并发操作同一 SqlQuery 实例。
     */
    class SqlQuery
    {
    public:
        /**
         * @brief 构造函数
         * @param database 数据库
         * @details 从 SqlDatabase 提取并持有其驱动的弱引用。
         */
        SqlQuery(std::shared_ptr<SqlDatabase> database);
        /**
         * @brief 构造函数
         * @param driver 数据库驱动
         * @details 以 driver 初始化内部驱动引用；driver 的生命周期需覆盖 SqlQuery 的使用期。
         */
        SqlQuery(std::shared_ptr<ISqlDriver> driver);
        /**
         * @brief 准备SQL语句
         * @param sql SQL语句
         * @return 查询
         * @details 保存 SQL 字符串并调用驱动 prepare() 进行预编译（若驱动支持）。
         */
        SqlQuery& prepare(const std::string& sql);
        /**
         * @brief 绑定SQL语句
         * @tparam T 数据类型
         * @param index 参数索引
         * @param value 数据
         * @return 查询
         * @details 将 value 转换为 SqlCell 并绑定到当前 SQL。
         * @note index 的起始（0/1）由具体驱动决定；T 需要可被 get_data_type<T>() 与 CellData 承载。
         */
        template<class T>
        SqlQuery& bind(int index, T value)
        {
            SqlCell cell;
            cell.type = get_data_type<T>();
            cell.data = value;
            return bind(index, cell);
        }
        /**
         * @brief 绑定SQL语句
         * @param index 参数索引
         * @param cell 数据单元
         * @return 查询
         * @details 将 cell 绑定到当前 SQL 的参数位。
         */
        SqlQuery& bind(int index, const SqlCell& cell);
        /**
         * @brief 清除SQL语句绑定
         * @details 清除当前 SQL 的所有绑定参数。
         */
        void clear_bindings();
        /**
         * @brief 重置SQL语句
         * @details 重置当前 SQL 的执行状态（例如游标/执行状态），不一定清除绑定。
         */
        void reset();
        /**
         * @brief 执行查询
         * @return 查询结果
         * @details 执行当前 SQL 并返回二维结果集：外层为行，内层为列（SqlCell）。
         */
        std::vector<std::vector<SqlCell>> execute_query();
        /**
         * @brief 执行命令
         * @return true 执行成功
         * @return false 执行失败
         * @details 执行当前 SQL 的非查询语句（不返回结果集）。
         */
        bool execute_command();
    private:
        /// @brief SQL语句
        std::string m_sql;
        /// @brief 数据库驱动
        std::weak_ptr<ISqlDriver> m_driver;
    };
    /// @brief 查询指针(shared_ptr)
    using SqlQueryPtr = std::shared_ptr<SqlQuery>;
}
