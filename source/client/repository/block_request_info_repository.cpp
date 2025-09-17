#include <format>

#include "client/repository/block_request_info_repository.hpp"
#include "common/database/database_manager.hpp"
#include "log/manage_logger.hpp"

BlockRequestInfoRepository::BlockRequestInfoRepository() {}

BlockRequestInfoRepository::~BlockRequestInfoRepository() {}

bool BlockRequestInfoRepository::ensure_table_exists()
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
    // 执行创建表
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
    // 当内部数据库已经初始化时直接返回
    if (m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database already initialized");
        return;
    }
    // 未初始化时从管理器中获取数据库
    m_database = DatabaseManager::get_instance().get_database("client_database");
}

std::vector<BlockRequestInfo> BlockRequestInfoRepository::get_all()
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return std::vector<BlockRequestInfo>();
    }
    // 查询所有数据
    auto data = m_database->query("SELECT * FROM block_request_info;");
    // 构建结果
    std::vector<BlockRequestInfo> result = std::vector<BlockRequestInfo>(data.size());
    // 遍历填充结果
    for (int32_t i = 0;i < data.size();i++)
    {
        result[i] = BlockRequestInfo(
            std::stoi(data[i][0]),
            std::stoi(data[i][1]),
            std::stoi(data[i][2]),
            std::stoi(data[i][3]),
            static_cast<Operation>(std::stoi(data[i][4])),
            static_cast<FileState>(std::stoi(data[i][5])),
            std::chrono::time_point<std::chrono::steady_clock>(std::chrono::seconds(std::stoi(data[i][6]))),
            std::chrono::time_point<std::chrono::steady_clock>(std::chrono::seconds(std::stoi(data[i][7])))
        );
    }
    return result;
}
bool BlockRequestInfoRepository::add(const BlockRequestInfo& block_info)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
    // 执行插入数据并返回是否成功
    return m_database->execute(std::format(
        "INSERT INTO block_request_info (block_id, file_id, offset, block_size, operation, state, start_time, end_time) VALUES ({}, {}, {}, {}, {}, {}, {}, {});",
        block_info.block_id,
        block_info.file_id,
        block_info.offset,
        block_info.block_size,
        static_cast<int>(block_info.operation),
        static_cast<int>(block_info.state),
        std::chrono::duration_cast<std::chrono::seconds>(block_info.start_time.time_since_epoch()).count(),
        std::chrono::duration_cast<std::chrono::seconds>(block_info.end_time.time_since_epoch()).count()
    ));
}
std::optional<BlockRequestInfo> BlockRequestInfoRepository::get_by_id(int32_t block_id)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    // 查询对应ID的数据
    auto data = m_database->query(std::format("SELECT * FROM block_request_info WHERE block_id = {};", block_id));
    // 找不到时返回空
    if (data.size() == 0)
    {
        return std::nullopt;
    }
    // 返回查找到的结果
    return BlockRequestInfo(
        std::stoi(data[0][0]),
        std::stoi(data[0][1]),
        std::stoi(data[0][2]),
        std::stoi(data[0][3]),
        static_cast<Operation>(std::stoi(data[0][4])),
        static_cast<FileState>(std::stoi(data[0][5])),
        std::chrono::time_point<std::chrono::steady_clock>(std::chrono::seconds(std::stoi(data[0][6]))),
        std::chrono::time_point<std::chrono::steady_clock>(std::chrono::seconds(std::stoi(data[0][7])))
    );
}

bool BlockRequestInfoRepository::update(const BlockRequestInfo& block_info)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
    // 执行更新数据并返回是否成功
    return m_database->execute(std::format(
        "UPDATE block_request_info SET file_id = {}, offset = {}, block_size = {}, operation = {}, state = {}, start_time = {}, end_time = {} WHERE block_id = {};",
        block_info.file_id,
        block_info.offset,
        block_info.block_size,
        static_cast<int>(block_info.operation),
        static_cast<int>(block_info.state),
        std::chrono::duration_cast<std::chrono::seconds>(block_info.start_time.time_since_epoch()).count(),
        std::chrono::duration_cast<std::chrono::seconds>(block_info.end_time.time_since_epoch()).count(),
        block_info.block_id
    ));
}
bool BlockRequestInfoRepository::remove(int32_t block_id)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "BlockRequestInfoRepository", "Database not initialized");
        return false;
    }
    // 执行删除记录并返回是否成功
    return m_database->execute(std::format("DELETE FROM block_request_info WHERE block_id = {};", block_id));
}

bool BlockRequestInfoRepository::is_init()const
{
    // 判断数据库是否初始化
    return m_database != nullptr;
}