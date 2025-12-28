#include "service/server_file_info_service.hpp"


std::vector<ServerFileInfo> ServerFileInfoService::get_all()
{
    return file_info_repository.get_all();
}
bool ServerFileInfoService::add(const ServerFileInfo& file_info)
{
    return file_info_repository.add(file_info);
}
std::optional<ServerFileInfo> ServerFileInfoService::get_by_id(int32_t file_id)
{
    return file_info_repository.get_by_id(file_id);
}
bool ServerFileInfoService::update(const ServerFileInfo& file_info)
{
    return file_info_repository.update(file_info);
}

bool ServerFileInfoService::remove(int32_t file_id)
{
    return file_info_repository.remove(file_id);
}

std::optional<ServerFileInfo> ServerFileInfoService::get_by_md5(const std::string& md5_code)
{
    return file_info_repository.get_by_md5(md5_code);
}

void ServerFileInfoService::init()
{
    if (is_init)
    {
        return;
    }
    file_info_repository.init();
    is_init = true;
}

int32_t ServerFileInfoService::count()
{
    return file_info_repository.count();
}