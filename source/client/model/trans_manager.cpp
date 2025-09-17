#include "common/network/url_resolver.hpp"
#include "client/connect/connection_manager.hpp"
#include "client/model/trans_manager.hpp"
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

ClientFileInfo TransManager::add_download(const DownloadRequest& request)
{
    // 创建文件信息
    ClientFileInfo info;
    // 解析URL
    auto url_info = UrlResolver::parse(request.file_url);
    // 根据不同的协议进行不同的处理
    if (url_info.protocol == UrlResolver::UrlProtocol::UNKNOWN)
    {
        return info;
    }
    else if (url_info.protocol == UrlResolver::UrlProtocol::DANEJOE)
    {
        // 设置源路径
        info.source_path = request.file_url;
        // 创建连接
        m_connection_manager.add_connection(url_info.ip, url_info.port);
        // 获取连接
        auto connection = m_connection_manager.get_connection_guard(url_info.ip, url_info.port);
        // 没有成功获取连接时直接返回信息
        if (!connection)
        {
            return info;
        }
        // 发送文件下载请求
        connection->send(std::vector<uint8_t>(url_info.path.begin(), url_info.path.end()));
        // 接收文件下载响应，包括文件详细信息
        /// @todo 考虑添加超时处理
        auto response = connection->recieve();
    }
}
