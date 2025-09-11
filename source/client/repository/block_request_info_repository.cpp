#include "client/repository/block_request_info_repository.hpp"
#include "common/database/database_manager.hpp"
#include "log/manage_logger.hpp"

BlockRequestInfoRepository::BlockRequestInfoRepository() {}

BlockRequestInfoRepository::~BlockRequestInfoRepository() {}

bool BlockRequestInfoRepository::ensure_table_exists()
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
    bool result = m_database->execute(R"(
        CREATE TABLE IF NOT EXISTS block_request_info (
            block_id INTEGER PRIMARY KEY,
            file_id INTEGER NOT NULL,
            offset INTEGER NOT NULL,
            block_size INTEGER NOT NULL,
            operation INTEGER NOT NULL,
            state INTEGER NOT NULL,
            start_time INTEGER NOT NULL,  -- 使用 INTEGER 存储时间戳
            end_time INTEGER NOT NULL      -- 使用 INTEGER 存储时间戳
        );
    )");
    return result;
}

void BlockRequestInfoRepository::init()
{
    if (m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database already initialized");
        return;
    }
    m_database = DatabaseManager::get_instance().get_database("client_database");
}

std::vector<BlockRequestInfo> BlockRequestInfoRepository::get_all()
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return std::vector<BlockRequestInfo>();
    }
}
bool BlockRequestInfoRepository::add(const BlockRequestInfo& block_info)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
}
std::optional<BlockRequestInfo> BlockRequestInfoRepository::get_by_id(int block_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return std::nullopt;
    }
}

bool BlockRequestInfoRepository::update(const BlockRequestInfo& block_info)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
}
bool BlockRequestInfoRepository::remove(int block_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
}