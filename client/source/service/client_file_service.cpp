#include "danejoe/logger/logger_manager.hpp"

#include "service/client_file_service.hpp"

ClientFileService::ClientFileService() {}

ClientFileService::~ClientFileService() {}

void ClientFileService::init()
{
    m_client_file_repository.init();
}
std::vector<ClientFileEntity> ClientFileService::get_all()
{
    return m_client_file_repository.get_all();
}
bool ClientFileService::add(const ClientFileEntity& client_file_entity)
{
    if (client_file_entity.file_id < 0)
    {
        DANEJOE_LOG_WARN("default", "ClientFileService", "Failed to add client file entity: Filed ID is invalid!");
        return false;
    }
    return m_client_file_repository.add(client_file_entity);
}
std::optional<ClientFileEntity> ClientFileService::get_by_file_id(int32_t file_id)
{
    return m_client_file_repository.get_by_id(file_id);
}

bool ClientFileService::update(const ClientFileEntity& file_info)
{
    return m_client_file_repository.update(file_info);
}

bool ClientFileService::remove(int64_t file_id)
{
    return m_client_file_repository.remove(file_id);
}

std::optional<ClientFileEntity> ClientFileService::get_by_md5(const std::string& md5_code)
{
    return m_client_file_repository.get_by_md5(md5_code);
}