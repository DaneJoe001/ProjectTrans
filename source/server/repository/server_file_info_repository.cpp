#include "server/repository/server_file_info_repository.hpp"
#include "common/database/database_manager.hpp"
#include "log/manage_logger.hpp"

ServerFileInfoRepository::ServerFileInfoRepository() {}
ServerFileInfoRepository::~ServerFileInfoRepository() {}

bool ServerFileInfoRepository::ensure_table_exists()
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return false;
    }
    bool result = m_database->execute(R"(
        CREATE TABLE IF NOT EXISTS file_info (
            file_id INTEGER PRIMARY KEY,
            file_name TEXT NOT NULL,
            resource_path TEXT NOT NULL,
            file_size INTEGER NOT NULL,
            md5_code TEXT NOT NULL
        );
    )");
    return result;
}

void ServerFileInfoRepository::init()
{
    if (m_database)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database already initialized");
        return;
    }
    m_database = DatabaseManager::get_instance().get_database("server_database");
}

std::vector<ServerFileInfo> ServerFileInfoRepository::get_all()
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return std::vector<ServerFileInfo>();
    }
    auto data = m_database->query(R"(
        SELECT * FROM file_info;
    )");
    std::vector<ServerFileInfo> result(data.size());
    for (std::size_t i = 0; i < data.size(); ++i)
    {
        result[i].file_id = std::stoi(data[i][0]);
        result[i].file_name = data[i][1];
        result[i].resource_path = data[i][2];
        result[i].file_size = std::stoul(data[i][3]);
        result[i].md5_code = data[i][4];
    }
    return result;
}
bool ServerFileInfoRepository::add(const ServerFileInfo& file_info)
{
    if (!m_database)
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
    return m_database->execute(std::format(R"(
        INSERT INTO file_info (file_name, resource_path, file_size, md5_code)
        VALUES ('{}', '{}', {}, '{}');
    )", file_info.file_name, file_info.resource_path, file_info.file_size, file_info.md5_code));
}
std::optional<ServerFileInfo> ServerFileInfoRepository::get_by_id(int32_t file_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    auto data = m_database->query(std::format(R"(
        SELECT * FROM file_info WHERE file_id = {};
    )", file_id));
    if (data.size() == 0)
    {
        return std::nullopt;
    }
    return ServerFileInfo{
        .file_id = std::stoi(data[0][0]),
        .file_name = data[0][1],
        .resource_path = data[0][2],
        .file_size = (uint32_t)std::stoul(data[0][3]),
        .md5_code = data[0][4]
    };
}

bool ServerFileInfoRepository::update(const ServerFileInfo& file_info)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return false;
    }
    return m_database->execute(std::format(R"(
        UPDATE file_info SET file_name = '{}', resource_path = '{}', file_size = {}, md5_code = '{}' WHERE file_id = {};
    )", file_info.file_name, file_info.resource_path, file_info.file_size, file_info.md5_code, file_info.file_id));
}
bool ServerFileInfoRepository::remove(int32_t file_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return false;
    }
    return m_database->execute(std::format(R"(
        DELETE FROM file_info WHERE file_id = {};
    )", file_id));
}

std::optional<ServerFileInfo>  ServerFileInfoRepository::get_by_md5(const std::string& md5_code)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    auto data = m_database->query(std::format(R"(
        SELECT * FROM file_info WHERE md5_code = '{}';
    )", md5_code));
    if (data.size() == 0)
    {
        return std::nullopt;
    }
    return ServerFileInfo{
        std::stoi(data[0][0]),
        data[0][1],
        data[0][2],
        (uint32_t)std::stoul(data[0][3]),
        data[0][4]
    };
}

int32_t ServerFileInfoRepository::count()
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ServerFileInfoRepository", "Database not initialized");
        return -1;
    }
    auto data = m_database->query("SELECT COUNT(*) FROM file_info");
    return std::stoi(data[0][0]);
}