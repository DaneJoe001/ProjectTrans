#pragma once

/**
 * @file i_database.hpp
 * @brief 数据库接口
 * @author DaneJoe001
 */

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include <variant>

 /// @todo 使用format
 /// @todo 考虑是否添加文件名后缀
 /// @note sqlite应该需要以.db结束,mysql等可能不需要
 /// @todo 在配置项中实现对数据库的类型选择(如mysql和sqlite)

 /// 设计一个arg函数拓展，填充位置只要查询当前位置最小的地方即可，不对，不能用arg的方式，还是得用多参的方式传递
 /// 考虑使用枚举定义INT,STR等数据库类型，在查询语句中使用select INT,STR等来指定参数类型，然后根据参数类型进行转换。
 /// 通过参数对类型进行安全验证，防止注入攻击。

 /**
  * @brief Statement 类用于表示数据库查询语句。
  * 该类继承自 std::string，并提供了参数化查询的功能。
  * 可以通过 arg 方法添加参数到查询语句中。
  * @note 该类是一个抽象基类，必须实现 replace 方法来替换占位符。
  * @todo 后面再考虑完善实现以适应不同的数据库
  */
class IStatement
{
public:
    using DataType = std::variant<std::nullptr_t, int32_t, int64_t, double, std::string, std::vector<uint8_t>>;
public:
    template<class T>
    IStatement& arg(T value)
    {
        bind(to_data_type(value));
        return *this;
    }
    static DataType to_data_type(const std::nullptr_t&) { return nullptr; }
    static DataType to_data_type(const char* s) { return std::string{ s ? s : "" }; }
    static DataType to_data_type(const std::string& s) { return s; }
    static DataType to_data_type(std::string_view s) { return std::string{ s }; }
    template <class I, std::enable_if_t<std::is_integral_v<I> && !std::is_same_v<I, bool>, int> = 0>
    static DataType to_data_type(I v) { return static_cast<int64_t>(v); }

    template <class F, std::enable_if_t<std::is_floating_point_v<F>, int> = 0>
    static DataType to_data_type(F v) { return static_cast<double>(v); }
    static DataType to_data_type(const std::vector<uint8_t>& b) { return b; }
    virtual void bind(const DataType& value) = 0;
    virtual ~IStatement() = default;
    virtual bool execute() = 0;
    virtual std::vector<std::vector<DataType>> query() = 0;
};

/**
 * @class BaseDatabase 数据库抽象类
 * @brief 数据库抽象类，定义了数据库操作的接口。
 */
class IDatabase
{
public:
    /**
     * @struct DatabaseConfig 数据库配置结构体
     */
    struct DatabaseConfig
    {
        /// @brief 数据库名称。
        std::string database_name;
        /// @brief 数据库路径。
        std::string path;
        /// @brief 用户名。
        std::string user_name;
        /// @brief 用户密码。
        std::string user_password;
    };
public:
    /**
     * @brief 构造函数
     */
    IDatabase();
    /**
     * @brief 连接到数据库。
     * @note 该方法必须被实现。
     */
    virtual bool connect() = 0;
    /**
     * @brief 执行操作。
     * @DataType statement 查询语句。
     */
    virtual bool execute(const std::string& statement) = 0;
    /**
     * @brief 查询操作。
     * @DataType statement 查询语句。
     */
    virtual std::vector<std::vector<std::string>> query(const std::string& statement) = 0;
    /**
     * @brief 获取错误信息。
     */
    virtual std::string error_message() = 0;
    /**
     * @brief 获取错误码。
     */
    virtual std::string error_code() = 0;
    /**
     * @brief 设置数据库路径。
     * @DataType path 数据库路径。
     */
    void set_path(const std::string& path);
    /**
     * @brief 设置用户名。
     * @DataType user_name 用户名。
     */
    void set_user_name(const std::string& user_name);
    /**
     * @brief 设置密码。
     * @DataType user_password 密码。
     */
    void set_user_password(const std::string& user_password);
    /**
     * @brief 设置数据库名称。
     * @DataType database_name 数据库名称。
     */
    void set_database_name(const std::string& database_name);
    /**
     * @brief 设置数据库配置。
     */
    void set_config(const DatabaseConfig& config);
    virtual std::unique_ptr<IStatement> get_statement(const std::string statement) = 0;
protected:
    /// @brief 数据库配置
    DatabaseConfig m_config;
    /// @brief 错误信息
    std::string m_error_message;
    /// @brief 错误码
    std::string m_error_code;
};

/**
 * @class IDatabaseCreator
 * @brief 数据库创建器
 */
class IDatabaseCreator
{
public:
    /**
     * @brief 创建数据库
     * @return 数据库
     */
    virtual std::shared_ptr<IDatabase> create_database() = 0;
};