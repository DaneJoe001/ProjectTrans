#include "common/database/i_database.hpp"
#include "log/manage_logger.hpp"


template<class T>
IStatement& IStatement::arg(const T& value)
{
    /// @todo 使用INT等替代?进行严格的类型审查
    size_t pos = this->find("?");
    if (pos != std::string::npos)
    {
        this->replace(pos, 1, std::to_string(value));
    }
    return *this;
}

IDatabase::IDatabase() {}

void IDatabase::set_path(const std::string& path)
{
    // 判断目标路径是否为空，空则不修改当前路径
    if (path.empty())
    {
        DANEJOE_LOG_WARN("default", "IDatabase", "path is empty");
        return;
    }
    m_config.path = path;
}

void IDatabase::set_user_name(const std::string& user_name)
{
    // 对于可能存在的用户名为空的情况，不修改当前用户名
    if (user_name.empty())
    {
        DANEJOE_LOG_WARN("default", "IDatabase", "user_name is empty");
        return;
    }
    m_config.user_name = user_name;
}

void IDatabase::set_user_password(const std::string& user_password)
{
    // 对于可能存在的密码为空只提出警告
    if (user_password.empty())
    {
        DANEJOE_LOG_WARN("default", "IDatabase", "user_password is empty");
    }
    m_config.user_password = user_password;
}

void IDatabase::set_database_name(const std::string& database_name)
{
    // 对于可能存在的数据库名为空只提出警告
    if (database_name.empty())
    {
        DANEJOE_LOG_WARN("default", "IDatabase", "database_name is empty");
    }
    m_config.database_name = database_name;
}

void IDatabase::set_config(const DatabaseConfig& config)
{
    // 更新配置
    m_config = config;
}