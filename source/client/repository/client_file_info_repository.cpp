#include "client/repository/client_file_info_repository.hpp"
#include "common/database/database_manager.hpp"
#include "log/manage_logger.hpp"

ClientFileInfoRepository::ClientFileInfoRepository() {}
ClientFileInfoRepository::~ClientFileInfoRepository() {}
bool ClientFileInfoRepository::ensure_table_exists()
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
    if (m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database already initialized");
        return;
    }
    m_database = DatabaseManager::get_instance().get_database("client_database");
}

std::vector<ClientFileInfo> ClientFileInfoRepository::get_all()
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return std::vector<ClientFileInfo>();
    }
    auto data = m_database->query(R"(
        SELECT * FROM file_info;
    )");
    std::vector<ClientFileInfo> result(data.size());
    for (int i = 0; i < data.size(); i++)
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
bool ClientFileInfoRepository::add(const ClientFileInfo& file_info)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
    auto data = get_by_md5(file_info.md5_code);
    if (data != std::nullopt)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "File already exists");
        return true;
    }
    return m_database->execute(std::format(R"(
        INSERT INTO file_info (saved_name, source_path, saved_path, file_size, operation, state, md5_code, create_time, finished_time)
        VALUES ('{}', '{}', '{}', {}, {}, {}, '{}', {}, {});
    )", file_info.saved_name, file_info.source_path, file_info.saved_path, file_info.file_size, static_cast<int>(file_info.operation), static_cast<int>(file_info.state), file_info.md5_code, std::chrono::duration_cast<std::chrono::seconds>(file_info.create_time.time_since_epoch()).count(), std::chrono::duration_cast<std::chrono::seconds>(file_info.finished_time.time_since_epoch()).count()
    ));
}
std::optional<ClientFileInfo> ClientFileInfoRepository::get_by_id(int file_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    auto data = m_database->query(std::format(R"(
        SELECT * FROM file_info WHERE file_id = {};
    )", file_id));
    if (data.size() == 0)
    {
        return std::nullopt;
    }
    return ClientFileInfo{
        file_id,
        data[0][1],
        data[0][2],
        data[0][3],
        std::stoul(data[0][4]),
        static_cast<Operation>(std::stoi(data[0][5])),
        static_cast<FileState>(std::stoi(data[0][6])),
        data[0][7],
        std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[0][8]))),
        std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[0][9])))
    };
}

std::optional<ClientFileInfo> ClientFileInfoRepository::get_by_md5(const std::string& md5_code)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return std::nullopt;
    }
    auto data = m_database->query(std::format(R"(
        SELECT * FROM file_info WHERE md5_code = '{}';
    )", md5_code));
    if (data.size() == 0)
    {
        return std::nullopt;
    }
    return ClientFileInfo{
        std::stoi(data[0][0]),
        data[0][1],
        data[0][2],
        data[0][3],
        std::stoul(data[0][4]),
        static_cast<Operation>(std::stoi(data[0][5])),
        static_cast<FileState>(std::stoi(data[0][6])),
        data[0][7],
        std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[0][8]))),
        std::chrono::time_point<std::chrono::system_clock>(std::chrono::seconds(std::stoi(data[0][9])))
    };
}

bool ClientFileInfoRepository::update(const ClientFileInfo& file_info)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
    return m_database->execute(std::format(R"(
        UPDATE file_info SET saved_name = '{}', source_path = '{}', saved_path = '{}', file_size = {}, operation = {}, state = {}, md5_code = '{}', create_time = {}, finished_time = {}
        WHERE file_id = {};
    )", file_info.saved_name, file_info.source_path, file_info.saved_path, file_info.file_size, static_cast<int>(file_info.operation), static_cast<int>(file_info.state), file_info.md5_code, std::chrono::duration_cast<std::chrono::seconds>(file_info.create_time.time_since_epoch()).count(), std::chrono::duration_cast<std::chrono::seconds>(file_info.finished_time.time_since_epoch()).count(), file_info.file_id
    ));
}
bool ClientFileInfoRepository::remove(int file_id)
{
    if (!m_database)
    {
        DANEJOE_LOG_TRACE("default", "FileInfoRepository", "Database not initialized");
        return false;
    }
    return m_database->execute(std::format(R"(
        DELETE FROM file_info WHERE file_id = {};
    )", file_id));
}