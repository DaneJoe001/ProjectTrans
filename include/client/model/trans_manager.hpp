#pragma once

/**
 * @file trans_manager.hpp
 * @brief 传输管理器
 * @author DaneJoe001
 */

#include <unordered_map>
#include <cstdint>

#include "client/model/trans_info.hpp"
#include "client/model/client_file_info.hpp"

class ConnectionManager;

/**
 * @class TransManager
 * @brief 传输管理器
 */
class TransManager {
public:
    /**
     * @brief 获取实例
     * @return 实例
     */
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
    /**
     * @brief 构造函数
     */
    TransManager();
    /**
     * @brief 析构函数
     */
    ~TransManager();
private:
    /// @brief 传输信息映射
    std::unordered_map<int32_t, TransInfo> m_trans_map;
    /// @brief 连接管理器
    ConnectionManager& m_connection_manager;
};