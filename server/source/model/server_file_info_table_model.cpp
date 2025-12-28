#include "model/server_file_info_table_model.hpp"

ServerFileInfoTableModel* ServerFileInfoTableModel::get_instance()
{
    // 定义静态实例
    static ServerFileInfoTableModel instance;
    return &instance;
}
ServerFileInfoTableModel::ServerFileInfoTableModel(QObject* parent)
{

}

void ServerFileInfoTableModel::init()
{
    // 初始化服务
    m_file_info_service.init();
    // 获取所有文件信息
    auto data = m_file_info_service.get_all();
    // 转移数据至类内qt容器
    m_file_info_list = QList<ServerFileInfo>(data.begin(), data.end());
}

int32_t ServerFileInfoTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_column_count;
}

int32_t ServerFileInfoTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_file_info_list.size();
}
QVariant ServerFileInfoTableModel::data(const QModelIndex& index, int32_t role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    int32_t row = index.row();
    int32_t column = index.column();
    if (role == Qt::DisplayRole)
    {
        switch (column)
        {
        case 0:
            return m_file_info_list[row].file_id;
        case 1:
            return QString::fromStdString(m_file_info_list[row].file_name);
        case 2:
            return QString::fromStdString(m_file_info_list[row].resource_path);
        case 3:
            return quint64(m_file_info_list[row].file_size);
        case 4:
            return QString::fromStdString(m_file_info_list[row].md5_code);
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool ServerFileInfoTableModel::setData(const QModelIndex& index, const QVariant& value, int32_t role)
{
    if (!index.isValid())
    {
        return false;
    }
    if (role == Qt::EditRole)
    {
        int32_t row = index.row();
        int32_t column = index.column();
        if (row < 0 || row >= m_file_info_list.size() || column < 0 || column >= 5)
        {
            return false;
        }
        switch (column)
        {
        case 0:
            m_file_info_list[row].file_id = value.toInt();
            break;
        case 1:
            m_file_info_list[row].file_name = value.toString().toStdString();
            break;
        case 2:
            m_file_info_list[row].resource_path = value.toString().toStdString();
            break;
        case 3:
            m_file_info_list[row].file_size = value.toUInt();
            break;
        case 4:
            m_file_info_list[row].md5_code = value.toString().toStdString();
            break;
        default:
            return false;
        }
        emit dataChanged(index, index, { role });
        return true;
    }
    return false;
}

QVariant ServerFileInfoTableModel::headerData(int32_t section, Qt::Orientation orientation, int32_t role) const
{
    /// @brief 判断是否为显示角色
    if (role == Qt::DisplayRole)
    {
        /// @brief 设置列标题
        if (orientation == Qt::Horizontal)
        {
            switch (section)
            {
            case 0:
                return QStringLiteral("ID");
            case 1:
                return QStringLiteral("文件名");
            case 2:
                return QStringLiteral("资源路径");
            case 3:
                return QStringLiteral("文件大小");
            case 4:
                return QStringLiteral("MD5校验");
            default:
                return QStringLiteral("未知");
            }
        }
        /// @brief 设置行标题
        else if (orientation == Qt::Vertical)
        {
            return QString::number(section + 1);
        }
    }
    return QVariant();
}

void ServerFileInfoTableModel::add(const ServerFileInfo& file_info)
{
    auto row_count = m_file_info_list.size();
    beginInsertRows(QModelIndex(), row_count, row_count);
    m_file_info_list.push_back(file_info);endInsertRows();
}

void ServerFileInfoTableModel::remove(const ServerFileInfo& file_info)
{
    int32_t row = m_file_info_list.indexOf(file_info);
    if (row >= 0)
    {
        beginRemoveRows(QModelIndex(), row, row);
        m_file_info_list.removeAt(row);
        endRemoveRows();
    }
}