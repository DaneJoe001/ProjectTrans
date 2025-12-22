#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/logger/logger_manager.hpp>
#include <danejoe/database/sql_query.hpp>

#include "client/repository/block_request_info_repository.hpp"
#include "client/model/common.hpp"

std::vector<BlockRequestInfo>
BlockRequestInfoRepository::from_query_data(const std::vector<std::vector<DaneJoe::SqlCell>>& data)
{
    // 构建结果
    std::vector<BlockRequestInfo> result;
    for (int i = 0; i < data.size(); i++)
    {
        BlockRequestInfo block_info;
        for (int j = 0; j < data[i].size(); j++)
        {
            if (data[i][j].column_name == "block_id")
            {
                block_info.block_id = std::get<int64_t>(data[i][j].data);
            }
            else if (data[i][j].column_name == "file_id")
            {
                block_info.file_id = std::get<int64_t>(data[i][j].data);
            }
            else if (data[i][j].column_name == "offset")
            {
                block_info.offset = std::get<int64_t>(data[i][j].data);
            }
            else if (data[i][j].column_name == "block_size")
            {
                block_info.block_size = std::get<int64_t>(data[i][j].data);
            }
            else if (data[i][j].column_name == "operation")
            {
                block_info.operation = static_cast<Operation>(std::get<int64_t>(data[i][j].data));
            }
            else if (data[i][j].column_name == "state")
            {
                block_info.state = static_cast<FileState>(std::get<int64_t>(data[i][j].data));
            }
            else if (data[i][j].column_name == "start_time")
            {
                block_info.start_time = std::chrono::time_point<std::chrono::steady_clock>(std::chrono::seconds(std::get<int64_t>(data[i][j].data)));
            }
            else if (data[i][j].column_name == "end_time")
            {
                block_info.end_time = std::chrono::time_point<std::chrono::steady_clock>(std::chrono::seconds(std::get<int64_t>(data[i][j].data)));
            }
        }
        result.push_back(block_info);
    }
    return result;
}

BlockRequestInfoRepository::BlockRequestInfoRepository() {}

BlockRequestInfoRepository::~BlockRequestInfoRepository() {}

bool BlockRequestInfoRepository::ensure_table_exists()
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
    // 执行创建表
    m_query->prepare(R"(
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
    return m_query->execute_command();
}

void BlockRequestInfoRepository::init()
{
    // 当内部数据库已经初始化时直接返回
    if (m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database already initialized");
        return;
    }
    // 未初始化时从管理器中获取数据库
    m_database = DaneJoe::SqlDatabaseManager::get_instance().get_database(
        "client_database");
    m_query = std::make_shared<DaneJoe::SqlQuery>(m_database);
}

std::vector<BlockRequestInfo> BlockRequestInfoRepository::get_all()
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return std::vector<BlockRequestInfo>();
    }
    // 查询所有数据
    // auto data = m_database->query("SELECT * FROM block_request_info;");
    m_query->prepare("SELECT * FROM block_request_info;");
    m_query->reset();
    auto data = m_query->execute_query();
    return from_query_data(data);
}

std::vector<BlockRequestInfo> BlockRequestInfoRepository::get_by_file_id(int32_t file_id)
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return std::vector<BlockRequestInfo>();
    }
    // 查询所有数据
    m_query->prepare("SELECT * FROM block_request_info where file_id = ?;");
    m_query->reset();
    m_query->bind(1, file_id);
    auto data = m_query->execute_query();
    return from_query_data(data);
}

int64_t BlockRequestInfoRepository::get_count_by_file_id_and_state(int32_t file_id, FileState state)
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return 0;
    }
    m_query->prepare("SELECT COUNT(*) FROM block_request_info where file_id = ? and state = ?;");
    m_query->reset();
    m_query->bind(1, file_id);
    m_query->bind(2, static_cast<int>(state));
    auto data = m_query->execute_query();
    if (data.size() > 0)
    {
        return std::get<int64_t>(data[0][0].data);
    }
    return 0;
}

std::vector<BlockRequestInfo> BlockRequestInfoRepository::get_by_file_id_and_state(int32_t file_id, FileState state)
{
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return std::vector<BlockRequestInfo>();
    }
    // 查询所有数据
    m_query->prepare("SELECT * FROM block_request_info where file_id = ? and state = ?;");
    m_query->reset();
    m_query->bind(1, file_id);
    m_query->bind(2, static_cast<int>(state));
    auto data = m_query->execute_query();
    return from_query_data(data);
}

bool BlockRequestInfoRepository::add(const BlockRequestInfo& block_info)
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
    m_query->prepare("INSERT INTO block_request_info (file_id, offset, "
        "block_size, operation, state, start_time, end_time) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);");
    m_query->reset();
    m_query->bind(1, block_info.file_id);
    m_query->bind(2, block_info.offset);
    m_query->bind(3, block_info.block_size);
    m_query->bind(4, static_cast<int>(block_info.operation));
    m_query->bind(5, static_cast<int>(block_info.state));
    m_query->bind(6, std::chrono::duration_cast<std::chrono::seconds>(
        block_info.start_time.time_since_epoch())
        .count());
    m_query->bind(7, std::chrono::duration_cast<std::chrono::seconds>(
        block_info.end_time.time_since_epoch())
        .count());
    return m_query->execute_command();
}
std::optional<BlockRequestInfo> BlockRequestInfoRepository::get_by_id(int32_t block_id)
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    m_query->prepare("SELECT * FROM block_request_info where block_id = ?;");
    m_query->reset();
    m_query->bind(1, block_id);
    auto data = m_query->execute_query();
    if (data.size() == 0)
    {
        return std::nullopt;
    }
    return from_query_data(data)[0];
}

bool BlockRequestInfoRepository::update(const BlockRequestInfo& block_info)
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
    m_query->prepare("UPDATE block_request_info SET file_id = ?, offset = ?, "
        "block_size = ?, operation = ?, state = ?, start_time = "
        "?, end_time = ? WHERE block_id = ?;");
    m_query->reset();
    m_query->bind(1, block_info.file_id);
    m_query->bind(2, block_info.offset);
    m_query->bind(3, block_info.block_size);
    m_query->bind(4, static_cast<int>(block_info.operation));
    m_query->bind(5, static_cast<int>(block_info.state));
    m_query->bind(6, std::chrono::duration_cast<std::chrono::seconds>(
        block_info.start_time.time_since_epoch())
        .count());
    m_query->bind(7, std::chrono::duration_cast<std::chrono::seconds>(
        block_info.end_time.time_since_epoch())
        .count());
    m_query->bind(8, block_info.block_id);
    return m_query->execute_command();
}
bool BlockRequestInfoRepository::remove(int32_t block_id)
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
    // 执行删除记录并返回是否成功
    m_query->prepare("DELETE FROM block_request_info WHERE block_id = ?;");
    m_query->reset();
    m_query->bind(1, block_id);
    return m_query->execute_command();
}

bool BlockRequestInfoRepository::is_init()const
{
    // 判断数据库是否初始化
    return m_query != nullptr;
}