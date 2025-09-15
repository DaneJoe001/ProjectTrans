#include <QDateTime>
#include <chrono>

#include "client/model/file_trans_info_table_model.hpp"
#include "log/manage_logger.hpp"

FileTransInfoTableModel::FileTransInfoTableModel(QObject* parent) :QAbstractTableModel(parent)
{

}

FileTransInfoTableModel::~FileTransInfoTableModel()
{

}

void FileTransInfoTableModel::init()
{
    m_block_request_info_service.init();
    // 构建测试数据
    for (int i = 0; i < 5; ++i)
    {
        ClientFileInfo file_info;
        file_info.file_id = i + 1;
        file_info.saved_name = "file_" + std::to_string(i + 1) + ".txt";
        file_info.source_path = "/source/path/file_" + std::to_string(i + 1) + ".txt";
        file_info.saved_path = "/saved/path/file_" + std::to_string(i + 1) + ".txt";
        file_info.file_size = static_cast<std::size_t>(std::rand() % 10000 + 1); // 随机文件大小 1-10000 字节
        file_info.operation = (i % 2 == 0) ? Operation::Upload : Operation::Download;
        file_info.state = (i % 5 == 0) ? FileState::Waiting : (i % 5 == 1) ? FileState::InTransfer : (i % 5 == 2) ? FileState::Completed : FileState::Failed;
        file_info.md5_code = "md5_" + std::to_string(i + 1);
        file_info.create_time = std::chrono::system_clock::now() - std::chrono::seconds(std::rand() % 1000);
        file_info.finished_time = file_info.create_time + std::chrono::seconds(std::rand() % 500); // 随机完成时间

        TransInfo trans_info;
        trans_info.file_info = file_info;
        trans_info.current_count = static_cast<std::size_t>(std::rand() % 10);
        trans_info.total_count = 10; // 假设总计 10

        m_trans_info_list.append(trans_info);
    }
}

FileTransInfoTableModel* FileTransInfoTableModel::get_instance()
{
    static FileTransInfoTableModel instance;
    return &instance;
}
int FileTransInfoTableModel::columnCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return 10;
}

int FileTransInfoTableModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid())
    {
        return 0;
    }
    return m_trans_info_list.size();
}

QVariant FileTransInfoTableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    if (role == Qt::DisplayRole)
    {
        int row = index.row();
        int column = index.column();
        switch (column)
        {
        case 0:
            return m_trans_info_list[row].file_info.file_id;
        case 1:
            return QString::fromStdString(m_trans_info_list[row].file_info.saved_name);
        case 2:
            return QString::fromStdString(m_trans_info_list[row].file_info.source_path);
        case 3:
            return (qint64)m_trans_info_list[row].file_info.file_size;
        case 4:
        {
            switch (m_trans_info_list[row].file_info.operation)
            {
            case Operation::Download:
                return "Download";
            case Operation::Upload:
                return "Upload";
            default:
                return QVariant();
            }
        }
        case 5:
        {
            switch (m_trans_info_list[row].file_info.state)
            {
            case FileState::Waiting:
                return "Waiting";
            case FileState::InTransfer:
                return "InTransfer";
            case FileState::Completed:
                return "Completed";
            case FileState::Failed:
                return "Failed";
            default:
                return QVariant();
            }
        }

        case 6:
            return QString::fromStdString(m_trans_info_list[row].file_info.source_path);
        case 7:
            return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(m_trans_info_list[row].file_info.create_time));
        case 8:
            return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(m_trans_info_list[row].file_info.finished_time));
        case 9:
            return QString("%1%").arg(m_trans_info_list[row].current_count * 100 / m_trans_info_list[row].total_count);
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool FileTransInfoTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
    {
        return false;
    }
    if (role == Qt::EditRole)
    {
        int row = index.row();
        int column = index.column();
        if (row < 0 || row >= m_trans_info_list.size() || column < 0 || column >= 11)
        {
            return false;
        }
        switch (column)
        {
        case 0:
            m_trans_info_list[row].file_info.file_id = value.toInt();
            break;
        case 1:
            m_trans_info_list[row].file_info.saved_name = value.toString().toStdString();
            break;
        case 2:
            m_trans_info_list[row].file_info.source_path = value.toString().toStdString();
            break;
        case 3:
            m_trans_info_list[row].file_info.saved_path = value.toString().toStdString();
            break;
        case 4:
            m_trans_info_list[row].file_info.file_size = value.toLongLong();
            break;
        case 5:
            if (value.toString() == "下载")
            {
                m_trans_info_list[row].file_info.operation = Operation::Download;
                break;
            }
            else if (value.toString() == "上传")
            {
                m_trans_info_list[row].file_info.operation = Operation::Upload;
                break;
            }
            else
            {
                return false;
            }
            break;
        case 6:
            if (value.toString() == "Waiting")
            {
                m_trans_info_list[row].file_info.state = FileState::Waiting;
                break;
            }
            else if (value.toString() == "InTransfer")
            {
                m_trans_info_list[row].file_info.state = FileState::InTransfer;
                break;
            }
            else if (value.toString() == "Completed")
            {
                m_trans_info_list[row].file_info.state = FileState::Completed;
            }
            else if (value.toString() == "Failed")
            {
                m_trans_info_list[row].file_info.state = FileState::Failed;
            }
            else
                return false;
        case 7:
            m_trans_info_list[row].file_info.md5_code = value.toString().toStdString();
            break;
        case 8:
            m_trans_info_list[row].file_info.create_time = std::chrono::system_clock::from_time_t(value.toDateTime().toSecsSinceEpoch());
            break;
        case 9:
            m_trans_info_list[row].file_info.finished_time = std::chrono::system_clock::from_time_t(value.toDateTime().toSecsSinceEpoch());
        default:
            return false;
        }
        emit dataChanged(index, index, { role });
        return true;
    }
    return false;
}
QVariant FileTransInfoTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
                return QStringLiteral("保存文件名");
            case 2:
                return QStringLiteral("源地址");
            case 3:
                return QStringLiteral("保存路径");
            case 4:
                return QStringLiteral("操作");
            case 5:
                return QStringLiteral("状态");
            case 6:
                return QStringLiteral("MD5");
            case 7:
                return QStringLiteral("创建时间");
            case 8:
                return QStringLiteral("完成时间");
            case 9:
                return QStringLiteral("进度");
            default:
                return QVariant();
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
void FileTransInfoTableModel::add(const TransInfo& trans_info)
{

}
void FileTransInfoTableModel::remove(const TransInfo& trans_info)
{

}