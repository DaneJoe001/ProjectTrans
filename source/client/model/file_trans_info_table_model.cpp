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
    //初始化块请求信息服务
    m_block_request_info_service.init();
    m_client_file_info_service.init();
    /// @todo 获取文件列表，根据文件id计算块
    /// 有个地方需要持有块列表，用来分配块请求任务
    auto file_list = m_client_file_info_service.get_all();
    for (auto& file_info : file_list)
    {
        TransInfo trans_info;
        trans_info.file_info = file_info;
        int64_t block_request_info_waiting_count = m_block_request_info_service.get_count_by_file_id_and_state(file_info.file_id, FileState::Waiting);
        int64_t block_request_info_completed_count = m_block_request_info_service.get_count_by_file_id_and_state(file_info.file_id, FileState::Completed);
        trans_info.current_count = block_request_info_completed_count;
        trans_info.total_count = block_request_info_completed_count + block_request_info_waiting_count;
        m_trans_info_list.push_back(trans_info);
    }
}

FileTransInfoTableModel* FileTransInfoTableModel::get_instance()
{
    // 创建文件传输表格模型实例
    static FileTransInfoTableModel instance;
    return &instance;
}
int32_t FileTransInfoTableModel::columnCount(const QModelIndex& parent) const
{
    // 存在父对象时不处理
    if (parent.isValid())
    {
        return 0;
    }
    // 返回固定列数
    return 10;
}

int32_t FileTransInfoTableModel::rowCount(const QModelIndex& parent) const
{
    // 存在父对象时不处理
    if (parent.isValid())
    {
        return 0;
    }
    // 返回固定列数
    return m_trans_info_list.size();
}

QVariant FileTransInfoTableModel::data(const QModelIndex& index, int32_t role) const
{
    // 先判断索引是否有效
    if (!index.isValid())
    {
        return QVariant();
    }
    // 判断是否是显示角色
    if (role == Qt::DisplayRole)
    {
        // 获取行和列
        int32_t row = index.row();
        int32_t column = index.column();
        // 根据列索引返回对应数据
        switch (column)
        {
        case 0:
            return m_trans_info_list[row].file_info.file_id;
        case 1:
            return QString::fromStdString(m_trans_info_list[row].file_info.saved_name);
        case 2:
            return QString::fromStdString(m_trans_info_list[row].file_info.source_path);
        case 3:
            return QString::fromStdString(m_trans_info_list[row].file_info.saved_path);
        case 4:
        {
            // 将枚举转为字符串
            switch (m_trans_info_list[row].file_info.operation)
            {
            case Operation::Download:
                return "Download";
            case Operation::Upload:
                return "Upload";
            case Operation::Unknown:
                return "Unknown";
            default:
                return QVariant();
            }
        }
        case 5:
        {
            // 将枚举转为字符串
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
            return QString::fromStdString(m_trans_info_list[row].file_info.md5_code);
        case 7:
            return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(m_trans_info_list[row].file_info.create_time));
        case 8:
            /// @todo 当完成时间<=创建时间，返回0
            if (m_trans_info_list[row].file_info.finished_time <= m_trans_info_list[row].file_info.create_time)
            {
                return "-";
            }
            else
            {
                return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(m_trans_info_list[row].file_info.finished_time));
            }
        case 9:
            if (m_trans_info_list[row].total_count == 0)
            {
                return "-";
            }
            return QString("%1%").arg(m_trans_info_list[row].current_count * 100 / m_trans_info_list[row].total_count);
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool FileTransInfoTableModel::setData(const QModelIndex& index, const QVariant& value, int32_t role)
{
    // 先判断索引是否有效
    if (!index.isValid())
    {
        return false;
    }
    // 判断是否是可编辑角色
    if (role == Qt::EditRole)
    {
        // 获取行和列
        int32_t row = index.row();
        int32_t column = index.column();
        // 判断行和列索引是否合法
        if (row < 0 || row >= m_trans_info_list.size() || column < 0 || column >= 11)
        {
            return false;
        }
        // 根据列索引返回对应数据
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
            // 由字符串转为枚举
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
            // 由字符串转为枚举
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
        // 发送数据改变信号
        emit dataChanged(index, index, { role });
        return true;
    }
    return false;
}
QVariant FileTransInfoTableModel::headerData(int32_t section, Qt::Orientation orientation, int32_t role) const
{
    // 判断是否为显示角色
    if (role == Qt::DisplayRole)
    {
        // 设置列标题
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
        // 设置行标题
        else if (orientation == Qt::Vertical)
        {
            return QString::number(section + 1);
        }
    }
    return QVariant();
}
void FileTransInfoTableModel::add(const TransInfo& trans_info)
{
    auto row_count = m_trans_info_list.size();
    m_trans_info_list.push_back(trans_info);
    endInsertRows();
}
void FileTransInfoTableModel::remove(const TransInfo& trans_info)
{

}

void FileTransInfoTableModel::add(const ClientFileInfo& file_info)
{
    auto row_count = m_trans_info_list.size();
    beginInsertRows(QModelIndex(), row_count, row_count);
    TransInfo trans_info;
    trans_info.file_info = file_info;
    int64_t block_request_info_waiting_count = m_block_request_info_service.get_count_by_file_id_and_state(file_info.file_id, FileState::Waiting);
    int64_t block_request_info_completed_count = m_block_request_info_service.get_count_by_file_id_and_state(file_info.file_id, FileState::Completed);
    trans_info.current_count = block_request_info_completed_count;
    trans_info.total_count = block_request_info_completed_count + block_request_info_waiting_count;
    m_trans_info_list.push_back(trans_info);
    endInsertRows();
}

void FileTransInfoTableModel::remove(const ClientFileInfo& file_info)
{

}

ClientFileInfo FileTransInfoTableModel::get_file_info(int32_t index)
{
    if (index >= 0 && index < m_trans_info_list.size())
    {
        return m_trans_info_list[index].file_info;
    }
    return ClientFileInfo();
}