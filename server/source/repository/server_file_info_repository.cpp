#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/database/sql_database_manager.hpp>

#include "repository/server_file_info_repository.hpp"

ServerFileInfoRepository::ServerFileInfoRepository() {}
ServerFileInfoRepository::~ServerFileInfoRepository() {}

bool ServerFileInfoRepository::ensure_table_exists()
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return false;
    }
    m_query->prepare(R"(
        CREATE TABLE IF NOT EXISTS file_info (
            file_id INTEGER PRIMARY KEY,
            file_name TEXT NOT NULL,
            resource_path TEXT NOT NULL,
            file_size INTEGER NOT NULL,
            md5_code TEXT NOT NULL
        );
    )");
    return m_query->execute_command();
}

void ServerFileInfoRepository::init()
{
    if (m_query)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database already initialized");
        return;
    }
    m_database = DaneJoe::SqlDatabaseManager::get_instance().get_database("server_database");
    m_query = std::make_shared<DaneJoe::SqlQuery>(m_database);
}

std::vector<ServerFileInfo> ServerFileInfoRepository::get_all()
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return std::vector<ServerFileInfo>();
    }
    m_query->prepare("SELECT * FROM file_info;");
    m_query->reset();
    auto data = m_query->execute_query();
    std::vector<ServerFileInfo> result;
    result.reserve(data.size());
    for (size_t i = 0; i < data.size(); i++)
    {
        ServerFileInfo file_info;
        for (size_t j = 0; j < data[i].size(); j++)
        {
            if (data[i][j].column_name == "file_id")
            {
                file_info.file_id = std::get<int64_t>(data[i][j].data);
            }
            else if (data[i][j].column_name == "file_name")
            {
                file_info.file_name = std::get<std::string>(data[i][j].data);
            }
            else if (data[i][j].column_name == "resource_path")
            {
                file_info.resource_path = std::get<std::string>(data[i][j].data);
            }
            else if (data[i][j].column_name == "file_size")
            {
                file_info.file_size = std::get<int64_t>(data[i][j].data);
            }
            else if (data[i][j].column_name == "md5_code")
            {
                file_info.md5_code = std::get<std::string>(data[i][j].data);
            }
        }
        result.push_back(file_info);
    }
    return result;
}
bool ServerFileInfoRepository::add(const ServerFileInfo& file_info)
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return false;
    }
    auto data = get_by_md5(file_info.md5_code);
    if (data != std::nullopt)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "File already exists");
        return false;
    }
    m_query->prepare("INSERT INTO file_info (file_name, resource_path, file_size, md5_code) VALUES (?, ?, ?, ?);");
    m_query->reset();
    m_query->bind(1, file_info.file_name);
    m_query->bind(2, file_info.resource_path);
    m_query->bind(3, file_info.file_size);
    m_query->bind(4, file_info.md5_code);
    return m_query->execute_command();
}
std::optional<ServerFileInfo> ServerFileInfoRepository::get_by_id(int32_t file_id)
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    DANEJOE_LOG_DEBUG("default", "ServerFileInfoRepository", "Get file info by id: {}", file_id);
    m_query->prepare("SELECT * FROM file_info WHERE file_id = ?;");
    m_query->reset();
    m_query->bind(1, file_id);
    auto data = m_query->execute_query();
    if (data.empty())
    {
        return std::nullopt;
    }
    ServerFileInfo file_info;
    for (size_t j = 0; j < data[0].size(); j++)
    {
        if (data[0][j].column_name == "file_id")
        {
            file_info.file_id = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "file_name")
        {
            file_info.file_name = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "resource_path")
        {
            file_info.resource_path = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "file_size")
        {
            file_info.file_size = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "md5_code")
        {
            file_info.md5_code = std::get<std::string>(data[0][j].data);
        }
    }
    return file_info;
}

bool ServerFileInfoRepository::update(const ServerFileInfo& file_info)
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return false;
    }
    m_query->prepare("UPDATE file_info SET file_name = ?, resource_path = ?, file_size = ?, md5_code = ? WHERE file_id = ?;");
    m_query->reset();
    m_query->bind(1, file_info.file_name);
    m_query->bind(2, file_info.resource_path);
    m_query->bind(3, file_info.file_size);
    m_query->bind(4, file_info.md5_code);
    m_query->bind(5, file_info.file_id);
    return m_query->execute_command();
}
bool ServerFileInfoRepository::remove(int32_t file_id)
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return false;
    }
    m_query->prepare("DELETE FROM file_info WHERE file_id = ?;");
    m_query->reset();
    m_query->bind(1, file_id);
    return m_query->execute_command();
}

std::optional<ServerFileInfo>  ServerFileInfoRepository::get_by_md5(const std::string& md5_code)
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    m_query->prepare("SELECT * FROM file_info WHERE md5_code = ?;");
    m_query->reset();
    m_query->bind(1, md5_code);
    auto data = m_query->execute_query();
    if (data.empty())
    {
        return std::nullopt;
    }
    ServerFileInfo file_info;
    for (size_t j = 0; j < data[0].size(); j++)
    {
        if (data[0][j].column_name == "file_id")
        {
            file_info.file_id = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "file_name")
        {
            file_info.file_name = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "resource_path")
        {
            file_info.resource_path = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "file_size")
        {
            file_info.file_size = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "md5_code")
        {
            file_info.md5_code = std::get<std::string>(data[0][j].data);
        }
    }
    return file_info;
}

int32_t ServerFileInfoRepository::count()
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return -1;
    }
    m_query->prepare("SELECT COUNT(*) FROM file_info");
    m_query->reset();
    auto data = m_query->execute_query();
    if (data.empty() || data[0].empty())
    {
        return -1;
    }
    return std::get<int64_t>(data[0][0].data);
}

bool ServerFileInfoRepository::is_init()const
{
    return m_query != nullptr;
}