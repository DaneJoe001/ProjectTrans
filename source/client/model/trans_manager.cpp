#include "common/network/url_resolver.hpp"
#include "client/connect/connection_manager.hpp"
#include "client/model/trans_manager.hpp"
#include "log/manage_logger.hpp"

TransManager* TransManager::get_instance()
{
    static TransManager instance;
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
    ClientFileInfo info;
    auto url_info = UrlResolver::parse(request.file_url);
    if (url_info.protocol == UrlResolver::UrlProtocol::UNKNOWN)
    {
        return info;
    }
    else if (url_info.protocol == UrlResolver::UrlProtocol::DANEJOE)
    {
        info.source_path = request.file_url;
        m_connection_manager.add_connection(url_info.ip, url_info.port);
        auto connection = m_connection_manager.get_connection_guard(url_info.ip, url_info.port);
        if (!connection)
        {
            return info;
        }
        connection->send(std::vector<uint8_t>(url_info.path.begin(), url_info.path.end()));
        auto response = connection->recieve();
    }
}