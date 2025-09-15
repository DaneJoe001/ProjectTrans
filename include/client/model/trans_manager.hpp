#pragma once

#include <unordered_map>

#include "client/model/trans_info.hpp"
#include "client/model/client_file_info.hpp"

class ConnectionManager;

class TransManager {
public:
    static TransManager* get_instance();
    /**
     * @brief 新建下载,向指定地址发送下载请求
     * @param request 下载请求
     * @note 当请求成功时服务端返回文件信息，将文件信息保存到数据库中
     */
    ClientFileInfo add_download(const DownloadRequest& request);
    /**
     * @brief 新建传输完成时添加传输信息
     */
    void add_trans(const ClientFileInfo& info);
private:
    TransManager();
    ~TransManager();
private:
    std::unordered_map<int, TransInfo> m_trans_map;
    ConnectionManager& m_connection_manager;
};