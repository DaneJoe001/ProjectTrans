#pragma once

/**
 * @file server_file_info_table_model.hpp
 * @brief 服务器文件信息表模型
 * @author DaneJoe001
 */

#include <QList>
#include <QModelIndex>
#include <QAbstractTableModel>
#include <cstdint>

#include "model/entity/server_file_entity.hpp"
#include "service/server_file_info_service.hpp"

 /**
  * @class ServerFileInfoTableModel
  * @brief 服务器文件信息表模型
  */
class ServerFileInfoTableModel : public QAbstractTableModel
{
public:
    /**
     * @brief 获取实例
     * @return 实例
     */
    static ServerFileInfoTableModel* get_instance();
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 返回模型中行的数量，用于确定视图中有多少行可以显示
     * @param parent 父索引
     * @return int32_t 行数
     */
    int32_t columnCount(const QModelIndex& parent = QModelIndex()) const override;
    /**
     * @brief 返回模型中列的数量，用于确定视图中有多少列可以显示
     * @param parent 父索引
     * @return int32_t 列数
     */
    int32_t rowCount(const QModelIndex& parent = QModelIndex()) const override;
    /**
     * @brief 获取指定索引处的数据
     * @param index 索引
     * @param role 角色
     */
    QVariant data(const QModelIndex& index, int32_t role = Qt::DisplayRole) const override;
    /**
     * @brief 设置指定索引处的数据
     * @param index 索引
     * @param value 数据
     * @param role 角色
     */
    bool setData(const QModelIndex& index, const QVariant& value, int32_t role = Qt::EditRole)override;
    /**
     * @brief 获取表头数据
     * @param section 索引
     * @param orientation 方向
     * @param role 角色
     */
    QVariant headerData(int32_t section, Qt::Orientation orientation, int32_t role = Qt::DisplayRole) const override;
    /**
     * @brief 添加文件信息
     * @param file_info 文件信息
     */
    void add(const ServerFileInfo& file_info);
    /**
     * @brief 删除文件信息
     * @param file_info 文件信息
     */
    void remove(const ServerFileInfo& file_info);
private:
    /**
     * @brief 构造函数
     * @param parent 父对象
     */
    explicit ServerFileInfoTableModel(QObject* parent = nullptr);
private:
    /// @brief 列数
    int32_t m_column_count = 5;
    /// @brief 文件信息列表
    QList<ServerFileInfo> m_file_info_list;
    /// @brief 文件信息服务
    ServerFileInfoService m_file_info_service;
};