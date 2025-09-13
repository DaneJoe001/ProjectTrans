#pragma once

#include <QList>
#include <QModelIndex>
#include <QAbstractTableModel>

#include "server/model/server_file_info.hpp"
#include "server/service/server_file_info_service.hpp"

class ServerFileInfoTableModel : public QAbstractTableModel {
public:
    static ServerFileInfoTableModel* get_instance();
    void init();
    /**
     * @brief 返回模型中行的数量，用于确定视图中有多少行可以显示
     * @param parent 父索引
     * @return int 行数
     */
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    /**
     * @brief 返回模型中列的数量，用于确定视图中有多少列可以显示
     * @param parent 父索引
     * @return int 列数
     */
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    /**
     * @brief 获取指定索引处的数据
     * @param index 索引
     * @param role 角色
     */
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    /**
     * @brief 设置指定索引处的数据
     * @param index 索引
     * @param value 数据
     * @param role 角色
     */
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole)override;
    /**
     * @brief 获取表头数据
     * @param section 索引
     * @param orientation 方向
     * @param role 角色
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void add(const ServerFileInfo& file_info);
    void remove(const ServerFileInfo& file_info);
private:
    explicit ServerFileInfoTableModel(QObject* parent = nullptr);
private:
    int m_column_count = 5;
    QList<ServerFileInfo> m_file_info_list;
    ServerFileInfoService m_file_info_service;
};