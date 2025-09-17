#include "client/service/clent_file_info_service.hpp"
#include "log/manage_logger.hpp"
std::vector<ClientFileInfo> ClientFileInfoService::get_all()
{
    // 当未初始化时返回空
    if (!file_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "ClientFileInfoService", "Failed: file_info_repository is not init");
        return std::vector<ClientFileInfo>();
    }
    return file_info_repository.get_all();
}
bool ClientFileInfoService::add(const ClientFileInfo& file_info)
{
    // 当未初始化时返回错误
    if (!file_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "ClientFileInfoService", "Failed: file_info_repository is not init");
        return false;
    }
    return file_info_repository.add(file_info);
}
std::optional<ClientFileInfo> ClientFileInfoService::get_by_id(int32_t file_id)
{
    // 当未初始化时返回空
    if (!file_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "ClientFileInfoService", "Failed: file_info_repository is not init");
        return std::nullopt;
    }
    return file_info_repository.get_by_id(file_id);
}
bool ClientFileInfoService::update(const ClientFileInfo& file_info)
{
    // 当未初始化时返回错误
    if (!file_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "ClientFileInfoService", "Failed: file_info_repository is not init");
        return false;
    }
    return file_info_repository.update(file_info);
}

bool ClientFileInfoService::remove(int32_t file_id)
{
    // 当未初始化时返回错误
    if (!file_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "ClientFileInfoService", "Failed: file_info_repository is not init");
        return false;
    }
    return file_info_repository.remove(file_id);
}

std::optional<ClientFileInfo> ClientFileInfoService::get_by_md5(const std::string& md5_code)
{
    // 当未初始化时返回空
    if (!file_info_repository.is_init())
    {
        DANEJOE_LOG_ERROR("default", "ClientFileInfoService", "Failed: file_info_repository is not init");
        return std::nullopt;
    }
    return file_info_repository.get_by_md5(md5_code);
}