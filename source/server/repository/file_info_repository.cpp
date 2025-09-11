#include "server/repository/file_info_repository.hpp"
#include "common/database/database_manager.hpp"
#include "log/manage_logger.hpp"

FileInfoRepository::FileInfoRepository() {}
FileInfoRepository::~FileInfoRepository() {}

bool FileInfoRepository::ensure_table_exists()
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
    bool result = m_database->execute(R"(
        CREATE TABLE IF NOT EXISTS file_info (
            file_id INTEGER PRIMARY KEY,
            saved_name TEXT NOT NULL,
            resource_path TEXT NOT NULL,
            file_size INTEGER NOT NULL,
            md5_code TEXT NOT NULL
        );
    )");
    return result;
}

void FileInfoRepository::init()
{
    if (m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database already initialized");
        return;
    }
    m_database = DatabaseManager::get_instance().get_database("server_database");
}

std::vector<FileInfo> FileInfoRepository::get_all()
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return std::vector<FileInfo>();
    }
}
bool FileInfoRepository::add(const FileInfo& block_info)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
}
std::optional<FileInfo> FileInfoRepository::get_by_id(int block_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
}

bool FileInfoRepository::update(const FileInfo& block_info)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
}
bool FileInfoRepository::remove(int block_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
}