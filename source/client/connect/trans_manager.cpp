#include "client/connect/connection_manager.hpp"
#include "client/connect/trans_manager.hpp"
#include "log/manage_logger.hpp"

TransManager* TransManager::get_instance()
{
    // 创建传输管理器实例
    static TransManager instance;
    // 返回实例指针
    return &instance;
}

TransManager::TransManager() :m_connection_manager(ConnectionManager::get_instance())
{

}

TransManager::~TransManager()
{

}

void TransManager::add_trans(const ClientFileInfo& info)
{

}