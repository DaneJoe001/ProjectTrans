#include "client/repository/client_file_info_repository.hpp"
#include "common/database/database_manager.hpp"
#include "log/manage_logger.hpp"

ClientFileInfoRepository::ClientFileInfoRepository() {}
ClientFileInfoRepository::~ClientFileInfoRepository() {}
bool ClientFileInfoRepository::ensure_table_exists()
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return false;
    }
    // 执行创建表
    bool result = m_database->execute(R"(
        CREATE TABLE IF NOT EXISTS file_info (
            file_id INTEGER PRIMARY KEY,
            saved_name TEXT NOT NULL,
            source_path TEXT NOT NULL,
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

void ClientFileInfoRepository::init()
{
    // 当内部数据库已经初始化时直接返回
    if (m_database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database already initialized");
        return;
    }
    // 未初始化时从管理器中获取数据库
    m_database = DatabaseManager::get_instance().get_database("client_database");
}

std::vector<ClientFileInfo> ClientFileInfoRepository::get_all()
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return std::vector<ClientFileInfo>();
    }
    // 查询所有数据
    auto data = m_database->query(R"(
        SELECT * FROM file_info;
    )");
    // 构建结果
    std::vector<ClientFileInfo> result(data.size());
    // 遍历填充结果
    for (int32_t i = 0; i < data.size(); i++)
    {
        result[i].file_id = std::stoi(data[i][0]);
        result[i].saved_name = data[i][1];
        result[i].source_path = data[i][2];
        result[i].saved_path = data[i][3];
        result[i].file_size = std::stoul(data[i][4]);
        result[i].operation = static_cast<Operation>(std::stoi(data[i][5]));
        result[i].state = static_cast<FileState>(std::stoi(data[i][6]));
        result[i].md5_code = data[i][7];
        result[i].create_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[i][8])));
        result[i].finished_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[i][9])));
    }
    return result;
}

std::optional<ClientFileInfo> ClientFileInfoRepository::get_by_saved_name_and_path(const std::string& saved_name, const std::string& path)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    auto statement = m_database->get_statement(std::format(R"(
        SELECT * FROM file_info WHERE saved_name = ? AND saved_path = ?;
    )"));
    auto data = statement->arg(saved_name).arg(path).query();
    if (data.size() == 0 || data[0].size() == 0)
    {
        return std::nullopt;
    }
    ClientFileInfo result;
    result.file_id = std::get<int64_t>(data[0][0]);
    result.saved_name = std::get<std::string>(data[0][1]);
    result.source_path = std::get<std::string>(data[0][2]);
    result.saved_path = std::get<std::string>(data[0][3]);
    result.file_size = (uint32_t)std::get<int64_t>(data[0][4]);
    result.operation = static_cast<Operation>(std::get<int64_t>(data[0][5]));
    result.state = static_cast<FileState>(std::get<int64_t>(data[0][6]));
    result.md5_code = std::get<std::string>(data[0][7]);
    result.create_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[0][8])));
    result.finished_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[0][9])));
    return result;
}

bool ClientFileInfoRepository::add(const ClientFileInfo& file_info)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return false;
    }
    // 根据md5_code判断库中是否已存在
    auto data = get_by_md5(file_info.md5_code);
    // 如果已存在则直接返回
    if (data != std::nullopt)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "File already exists");
        return true;
    }
    std::unique_ptr<IStatement> statement = m_database->get_statement(R"(
        INSERT INTO file_info (saved_name, source_path, saved_path, file_size, operation, state, md5_code, create_time, finished_time)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);
    )");
    DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "File info: {}", file_info.to_string());

    bool result = statement->arg(file_info.saved_name).arg(file_info.source_path).arg(file_info.saved_path).arg(file_info.file_size).arg(static_cast<int>(file_info.operation)).arg(static_cast<int>(file_info.state)).arg(file_info.md5_code).arg(std::chrono::duration_cast<std::chrono::seconds>(file_info.create_time.time_since_epoch()).count()).arg(std::chrono::duration_cast<std::chrono::seconds>(file_info.finished_time.time_since_epoch()).count()).execute();

    // std::string state_ment = std::format(R"(
    //     INSERT INTO file_info (saved_name, source_path, saved_path, file_size, operation, state, md5_code, create_time, finished_time)
    //     VALUES ('{}', '{}', '{}', {}, {}, {}, '{}', {}, {});
    // )", file_info.saved_name, file_info.source_path, file_info.saved_path, file_info.file_size, static_cast<int>(file_info.operation), static_cast<int>(file_info.state), file_info.md5_code, std::chrono::duration_cast<std::chrono::seconds>(file_info.create_time.time_since_epoch()).count(), std::chrono::duration_cast<std::chrono::seconds>(file_info.finished_time.time_since_epoch()).count()
    // );
    // // 执行插入数据并返回是否成功
    // return m_database->execute(state_ment);
    return result;
}
std::optional<ClientFileInfo> ClientFileInfoRepository::get_by_id(int32_t file_id)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    // 查询对应ID的数据
    auto data = m_database->query(std::format(R"(
        SELECT * FROM file_info WHERE file_id = {};
    )", file_id));
    // 找不到时返回空
    if (data.size() == 0)
    {
        return std::nullopt;
    }
    // 返回查找到的结果
    return ClientFileInfo{
        file_id,
        data[0][1],
        data[0][2],
        data[0][3],
        (uint32_t)std::stoul(data[0][4]),
        static_cast<Operation>(std::stoi(data[0][5])),
        static_cast<FileState>(std::stoi(data[0][6])),
        data[0][7],
        std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[0][8]))),
        std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[0][9])))
    };
}

std::optional<ClientFileInfo> ClientFileInfoRepository::get_by_md5(const std::string& md5_code)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    // 根据md5码查询数据库
    auto data = m_database->query(std::format(R"(
        SELECT * FROM file_info WHERE md5_code = '{}';
    )", md5_code));
    // 找不到时返回空
    if (data.size() == 0)
    {
        return std::nullopt;
    }
    // 返回查找到的结果
    return ClientFileInfo{
        std::stoi(data[0][0]),
        data[0][1],
        data[0][2],
        data[0][3],
        (uint32_t)std::stoul(data[0][4]),
        static_cast<Operation>(std::stoi(data[0][5])),
        static_cast<FileState>(std::stoi(data[0][6])),
        data[0][7],
        std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[0][8]))),
        std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[0][9])))
    };
}

bool ClientFileInfoRepository::update(const ClientFileInfo& file_info)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return false;
    }
    // 执行更新数据并返回是否成功
    return m_database->execute(std::format(R"(
        UPDATE file_info SET saved_name = '{}', source_path = '{}', saved_path = '{}', file_size = {}, operation = {}, state = {}, md5_code = '{}', create_time = {}, finished_time = {}
        WHERE file_id = {};
    )", file_info.saved_name, file_info.source_path, file_info.saved_path, file_info.file_size, static_cast<int>(file_info.operation), static_cast<int>(file_info.state), file_info.md5_code, std::chrono::duration_cast<std::chrono::seconds>(file_info.create_time.time_since_epoch()).count(), std::chrono::duration_cast<std::chrono::seconds>(file_info.finished_time.time_since_epoch()).count(), file_info.file_id
    ));
}
bool ClientFileInfoRepository::remove(int32_t file_id)
{
    // 判断数据库是否初始化
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return false;
    }
    // 执行删除记录并返回是否成功
    return m_database->execute(std::format(R"(
        DELETE FROM file_info WHERE file_id = {};
    )", file_id));
}

bool ClientFileInfoRepository::is_init()const
{
    // 判断数据库是否初始化
    return m_database != nullptr;
}