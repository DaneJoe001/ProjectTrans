#pragma once

#include <QAbstractTableModel>
#include <QList>

#include "client/model/trans_info.hpp"
#include "client/service/block_request_info_service.hpp"

class FileTransInfoTableModel : public QAbstractTableModel {
public:
    static FileTransInfoTableModel* get_instance();
    void init();
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole)override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    void add(const TransInfo& trans_info);
    void remove(const TransInfo& trans_info);
private:
    FileTransInfoTableModel(QObject* parent = nullptr);
    ~FileTransInfoTableModel();
private:
    QList<TransInfo> m_trans_info_list;
    BlockRequestInfoService m_block_request_info_service;
};