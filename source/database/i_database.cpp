#include "database/i_database.hpp"
#include "log/manage_logger.hpp"


template<class T>
IStatement& IStatement::arg(const T& value)
{
    size_t pos = this->find("?");
    if (pos != std::string::npos)
    {
        this->replace(pos, 1, std::to_string(value));
    }
    return *this;
}

IDatabase::IDatabase(){}

void IDatabase::set_path(const std::string& path)
{
    if (path.empty())
    {
        DANEJOE_LOG_WARN("default","Database","path is empty");
    }
    m_config.path = path;
}

void IDatabase::set_user_name(const std::string& user_name)
{
    if (user_name.empty())
    {
        DANEJOE_LOG_WARN("default","Database","user_name is empty");
    }
    m_config.user_name = user_name;
}

void IDatabase::set_user_password(const std::string& user_password)
{
    if (user_password.empty())
    {
        DANEJOE_LOG_WARN("default","Database","user_password is empty");
    }
    m_config.user_password = user_password;
}

void IDatabase::set_database_name(const std::string& database_name)
{
    if (database_name.empty())
    {
        DANEJOE_LOG_WARN("default","Database","database_name is empty");
    }
    m_config.database_name = database_name;
}

void IDatabase::set_config(const DatabaseConfig& config)
{
    m_config = config;
}