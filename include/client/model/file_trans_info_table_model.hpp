#pragma once

/**
 * @file file_trans_info_table_model.hpp
 * @brief 文件传输信息TableModel
 * @author DaneJoe001
 */

#include <cstdint>

#include <QAbstractTableModel>
#include <QList>

#include "client/model/trans_info.hpp"
#include "client/service/block_request_info_service.hpp"
#include "client/service/client_file_info_service.hpp"

 /**
  * @class FileTransInfoTableModel
  * @brief 文件传输信息TableModel
  * @note 单例模式,继承自QAbstractTableModel
  */
class FileTransInfoTableModel : public QAbstractTableModel {
public:
    /**
     * @brief 获取实例
     * @return 实例
     */
    static FileTransInfoTableModel* get_instance();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 列数
     * @param parent 父索引
     * @return 列数
     */
    int32_t columnCount(const QModelIndex& parent = QModelIndex()) const override;
    /**
     * @brief 行数
     * @param parent 父索引
     * @return 行数
     */
    int32_t rowCount(const QModelIndex& parent = QModelIndex()) const override;
    /**
     * @brief 数据
     * @param index 索引
     * @param role 角色
     * @return 数据
     */
    QVariant data(const QModelIndex& index, int32_t role = Qt::DisplayRole) const override;
    /**
     * @brief 设置数据
     * @param index 索引
     * @param value 值
     * @param role 角色
     * @return 是否成功
     */
    bool setData(const QModelIndex& index, const QVariant& value, int32_t role = Qt::EditRole)override;
    /**
     * @brief 头部数据
     * @param section 节
     * @param orientation 方向
     * @param role 角色
     * @return 头部数据
     */
    QVariant headerData(int32_t section, Qt::Orientation orientation, int32_t role = Qt::DisplayRole) const override;
    /**
     * @brief 添加
     * @param trans_info 传输信息
     */
    void add(const TransInfo& trans_info);
    /**
     * @brief 添加
     * @param file_info 文件信息
     */
    void add(const ClientFileInfo& file_info);
    /**
     * @brief 删除
     * @param trans_info 传输信息
     */
    void remove(const TransInfo& trans_info);
    /**
     * @brief 删除
     * @param file_info 文件信息
     */
    void remove(const ClientFileInfo& file_info);

private:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    FileTransInfoTableModel(QObject* parent = nullptr);
    /**
     * @brief 析构函数
     */
    ~FileTransInfoTableModel();
private:
    /// @brief 传输信息列表
    QList<TransInfo> m_trans_info_list;
    /// @brief 块请求信息服务
    BlockRequestInfoService m_block_request_info_service;
    /// @brief 文件信息服务
    ClientFileInfoService m_client_file_info_service;
};