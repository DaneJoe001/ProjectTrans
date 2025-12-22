#include "client/repository/client_file_info_repository.hpp"
#include <danejoe/database/sql_database_manager.hpp>
#include <danejoe/logger/logger_manager.hpp>

ClientFileInfoRepository::ClientFileInfoRepository() {}
ClientFileInfoRepository::~ClientFileInfoRepository() {}
bool ClientFileInfoRepository::ensure_table_exists()
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return false;
    }
    // 执行创建表
    m_query->prepare(R"(
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
    return m_query->execute_command();
}

void ClientFileInfoRepository::init()
{
    // 当内部数据库已经初始化时直接返回
    if (m_query)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database already initialized");
        return;
    }
    // 未初始化时从管理器中获取数据库
    m_database = DaneJoe::SqlDatabaseManager::get_instance().get_database("client_database");
    m_query = std::make_shared<DaneJoe::SqlQuery>(m_database);
}

std::vector<ClientFileInfo> ClientFileInfoRepository::get_all()
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return std::vector<ClientFileInfo>();
    }
    // 查询所有数据
    m_query->prepare("SELECT * FROM file_info;");
    m_query->reset();
    auto data = m_query->execute_query();
    // 构建结果
    std::vector<ClientFileInfo> result;
    result.reserve(data.size());
    for (size_t i = 0; i < data.size(); i++)
    {
        ClientFileInfo file_info;
        for (size_t j = 0; j < data[i].size(); j++)
        {
            if (data[i][j].column_name == "file_id")
            {
                file_info.file_id = std::get<int64_t>(data[i][j].data);
            }
            else if (data[i][j].column_name == "saved_name")
            {
                file_info.saved_name = std::get<std::string>(data[i][j].data);
            }
            else if (data[i][j].column_name == "source_path")
            {
                file_info.source_path = std::get<std::string>(data[i][j].data);
            }
            else if (data[i][j].column_name == "saved_path")
            {
                file_info.saved_path = std::get<std::string>(data[i][j].data);
            }
            else if (data[i][j].column_name == "file_size")
            {
                file_info.file_size = std::get<int64_t>(data[i][j].data);
            }
            else if (data[i][j].column_name == "operation")
            {
                file_info.operation = static_cast<Operation>(std::get<int64_t>(data[i][j].data));
            }
            else if (data[i][j].column_name == "state")
            {
                file_info.state = static_cast<FileState>(std::get<int64_t>(data[i][j].data));
            }
            else if (data[i][j].column_name == "md5_code")
            {
                file_info.md5_code = std::get<std::string>(data[i][j].data);
            }
            else if (data[i][j].column_name == "create_time")
            {
                file_info.create_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[i][j].data)));
            }
            else if (data[i][j].column_name == "finished_time")
            {
                file_info.finished_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[i][j].data)));
            }
        }
        result.push_back(file_info);
    }
    return result;
}

std::optional<ClientFileInfo> ClientFileInfoRepository::get_by_saved_name_and_path(const std::string& saved_name, const std::string& path)
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    m_query->prepare("SELECT * FROM file_info WHERE saved_name = ? AND saved_path = ?;");
    m_query->reset();
    m_query->bind(1, saved_name);
    m_query->bind(2, path);
    auto data = m_query->execute_query();
    if (data.empty())
    {
        return std::nullopt;
    }
    ClientFileInfo file_info;
    for (size_t j = 0; j < data[0].size(); j++)
    {
        if (data[0][j].column_name == "file_id")
        {
            file_info.file_id = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "saved_name")
        {
            file_info.saved_name = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "source_path")
        {
            file_info.source_path = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "saved_path")
        {
            file_info.saved_path = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "file_size")
        {
            file_info.file_size = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "operation")
        {
            file_info.operation = static_cast<Operation>(std::get<int64_t>(data[0][j].data));
        }
        else if (data[0][j].column_name == "state")
        {
            file_info.state = static_cast<FileState>(std::get<int64_t>(data[0][j].data));
        }
        else if (data[0][j].column_name == "md5_code")
        {
            file_info.md5_code = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "create_time")
        {
            file_info.create_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[0][j].data)));
        }
        else if (data[0][j].column_name == "finished_time")
        {
            file_info.finished_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[0][j].data)));
        }
    }
    return file_info;
}

bool ClientFileInfoRepository::add(const ClientFileInfo& file_info)
{
    // 判断数据库是否初始化
    if (!m_query)
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
    m_query->prepare("INSERT INTO file_info (file_id, saved_name, source_path, saved_path, file_size, operation, state, md5_code, create_time, finished_time) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
    m_query->reset();
    m_query->bind(1, file_info.file_id);
    m_query->bind(2, file_info.saved_name);
    m_query->bind(3, file_info.source_path);
    m_query->bind(4, file_info.saved_path);
    m_query->bind(5, file_info.file_size);
    m_query->bind(6, static_cast<int>(file_info.operation));
    m_query->bind(7, static_cast<int>(file_info.state));
    m_query->bind(8, file_info.md5_code);
    m_query->bind(9, std::chrono::duration_cast<std::chrono::seconds>(file_info.create_time.time_since_epoch()).count());
    m_query->bind(10, std::chrono::duration_cast<std::chrono::seconds>(file_info.finished_time.time_since_epoch()).count());
    bool result = m_query->execute_command();

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
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    // 查询对应ID的数据
    m_query->prepare("SELECT * FROM file_info WHERE file_id = ?;");
    m_query->reset();
    m_query->bind(1, file_id);
    auto data = m_query->execute_query();
    if (data.empty())
    {
        return std::nullopt;
    }
    ClientFileInfo file_info;
    for (size_t j = 0; j < data[0].size(); j++)
    {
        if (data[0][j].column_name == "file_id")
        {
            file_info.file_id = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "saved_name")
        {
            file_info.saved_name = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "source_path")
        {
            file_info.source_path = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "saved_path")
        {
            file_info.saved_path = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "file_size")
        {
            file_info.file_size = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "operation")
        {
            file_info.operation = static_cast<Operation>(std::get<int64_t>(data[0][j].data));
        }
        else if (data[0][j].column_name == "state")
        {
            file_info.state = static_cast<FileState>(std::get<int64_t>(data[0][j].data));
        }
        else if (data[0][j].column_name == "md5_code")
        {
            file_info.md5_code = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "create_time")
        {
            file_info.create_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[0][j].data)));
        }
        else if (data[0][j].column_name == "finished_time")
        {
            file_info.finished_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[0][j].data)));
        }
    }
    return file_info;
}

std::optional<ClientFileInfo> ClientFileInfoRepository::get_by_md5(const std::string& md5_code)
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    // 根据md5码查询数据库
    m_query->prepare("SELECT * FROM file_info WHERE md5_code = ?;");
    m_query->reset();
    m_query->bind(1, md5_code);
    auto data = m_query->execute_query();
    if (data.empty())
    {
        return std::nullopt;
    }
    ClientFileInfo file_info;
    for (size_t j = 0; j < data[0].size(); j++)
    {
        if (data[0][j].column_name == "file_id")
        {
            file_info.file_id = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "saved_name")
        {
            file_info.saved_name = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "source_path")
        {
            file_info.source_path = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "saved_path")
        {
            file_info.saved_path = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "file_size")
        {
            file_info.file_size = std::get<int64_t>(data[0][j].data);
        }
        else if (data[0][j].column_name == "operation")
        {
            file_info.operation = static_cast<Operation>(std::get<int64_t>(data[0][j].data));
        }
        else if (data[0][j].column_name == "state")
        {
            file_info.state = static_cast<FileState>(std::get<int64_t>(data[0][j].data));
        }
        else if (data[0][j].column_name == "md5_code")
        {
            file_info.md5_code = std::get<std::string>(data[0][j].data);
        }
        else if (data[0][j].column_name == "create_time")
        {
            file_info.create_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[0][j].data)));
        }
        else if (data[0][j].column_name == "finished_time")
        {
            file_info.finished_time = std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::get<int64_t>(data[0][j].data)));
        }
    }
    return file_info;
}

bool ClientFileInfoRepository::update(const ClientFileInfo& file_info)
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return false;
    }
    // 执行更新数据并返回是否成功
    m_query->prepare("UPDATE file_info SET saved_name = ?, source_path = ?, saved_path = ?, file_size = ?, operation = ?, state = ?, md5_code = ?, create_time = ?, finished_time = ? WHERE file_id = ?;");
    m_query->reset();
    m_query->bind(1, file_info.saved_name);
    m_query->bind(2, file_info.source_path);
    m_query->bind(3, file_info.saved_path);
    m_query->bind(4, file_info.file_size);
    m_query->bind(5, static_cast<int>(file_info.operation));
    m_query->bind(6, static_cast<int>(file_info.state));
    m_query->bind(7, file_info.md5_code);
    m_query->bind(8, std::chrono::duration_cast<std::chrono::seconds>(file_info.create_time.time_since_epoch()).count());
    m_query->bind(9, std::chrono::duration_cast<std::chrono::seconds>(file_info.finished_time.time_since_epoch()).count());
    m_query->bind(10, file_info.file_id);
    return m_query->execute_command();
}
bool ClientFileInfoRepository::remove(int32_t file_id)
{
    // 判断数据库是否初始化
    if (!m_query)
    {
        DANEJOE_LOG_TRACE("default", "ClientFileInfoRepository", "Database not initialized");
        return false;
    }
    // 执行删除记录并返回是否成功
    m_query->prepare("DELETE FROM file_info WHERE file_id = ?;");
    m_query->reset();
    m_query->bind(1, file_id);
    return m_query->execute_command();
}

bool ClientFileInfoRepository::is_init()const
{
    return m_query != nullptr;
}