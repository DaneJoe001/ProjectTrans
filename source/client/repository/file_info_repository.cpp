#include "client/repository/file_info_repository.hpp"
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
            saved_path TEXT NOT NULL,
            file_size INTEGER NOT NULL,
            operation INTEGER NOT NULL,
            state INTEGER NOT NULL,
            md5_code TEXT NOT NULL,
            create_time INTEGER NOT NULL,  -- 使用 INTEGER 存储时间戳
            finished_time INTEGER NOT NULL   -- 使用 INTEGER 存储时间戳
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
    m_database = DatabaseManager::get_instance().get_database("client_database");
}

std::vector<FileInfo> FileInfoRepository::get_all()
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return std::vector<FileInfo>();
    }
}
bool FileInfoRepository::add(const FileInfo& file_info)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
}
std::optional<FileInfo> FileInfoRepository::get_by_id(int file_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
}

bool FileInfoRepository::update(const FileInfo& file_info)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
}
bool FileInfoRepository::remove(int file_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
}