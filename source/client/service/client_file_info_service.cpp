#include "client/service/clent_file_info_service.hpp"
std::vector<ClientFileInfo> ClientFileInfoService::get_all()
{
    return file_info_repository.get_all();
}
bool ClientFileInfoService::add(const ClientFileInfo& file_info)
{
    return file_info_repository.add(file_info);
}
std::optional<ClientFileInfo> ClientFileInfoService::get_by_id(int32_t file_id)
{
    return file_info_repository.get_by_id(file_id);
}
bool ClientFileInfoService::update(const ClientFileInfo& file_info)
{
    return file_info_repository.update(file_info);
}

bool ClientFileInfoService::remove(int32_t file_id)
{
    return file_info_repository.remove(file_id);
}

std::optional<ClientFileInfo> ClientFileInfoService::get_by_md5(const std::string& md5_code)
{
    return file_info_repository.get_by_md5(md5_code);
}