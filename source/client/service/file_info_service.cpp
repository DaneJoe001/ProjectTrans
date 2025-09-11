#include "client/service/file_info_service.hpp"
std::vector<FileInfo> FileInfoService::get_all()
{
    return file_info_repository.get_all();
}
bool FileInfoService::add(const FileInfo& file_info)
{
    return file_info_repository.add(file_info);
}
std::optional<FileInfo> FileInfoService::get_by_id(int file_id)
{
    return file_info_repository.get_by_id(file_id);
}
bool FileInfoService::update(const FileInfo& file_info)
{
    return file_info_repository.update(file_info);
}

bool FileInfoService::remove(int file_id)
{
    return file_info_repository.remove(file_id);
}