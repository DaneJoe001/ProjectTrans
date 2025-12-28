#include <QDateTime>

#include <chrono>

#include <danejoe/logger/logger_manager.hpp>

#include "model/view/task_table_model.hpp"
#include "model/entity/task_entity.hpp"

TaskTableModel::TaskTableModel(QObject* parent) :QAbstractTableModel(parent),
m_task_service(TaskViewService::get_instance())
{}

TaskTableModel::~TaskTableModel() {}

void TaskTableModel::init()
{
    // 获取所有任务信息
    auto tasks = m_task_service.get_all();
    m_task_list = QList<TaskViewModel>(tasks.begin(), tasks.end());
}

void TaskTableModel::update(int64_t task_id)
{
    auto task_opt = m_task_service.get_by_task_id(task_id);
    if (!task_opt.has_value())
    {
        DANEJOE_LOG_ERROR("default", "TaskTableModel", "Task not found: {}", task_id);
        return;
    }
    auto task = task_opt.value();
    int row = 0;
    for (;row < m_task_list.size(); row++)
    {
        if (task.task_entity.task_id == m_task_list[row].task_entity.task_id)
        {
            m_task_list[row] = task;
            break;
        }
    }
    emit dataChanged(index(row, 0), index(row, columnCount()));
}

TaskTableModel* TaskTableModel::get_instance()
{
    // 创建文件传输表格模型实例
    static TaskTableModel instance;
    return &instance;
}
int32_t TaskTableModel::columnCount(const QModelIndex& parent) const
{
    // 存在父对象时不处理
    if (parent.isValid())
    {
        return 0;
    }
    // 返回固定列数
    return 10;
}

int32_t TaskTableModel::rowCount(const QModelIndex& parent) const
{
    // 存在父对象时不处理
    if (parent.isValid())
    {
        return 0;
    }
    // 返回固定列数
    return m_task_list.size();
}

QVariant TaskTableModel::data(const QModelIndex& index, int32_t role) const
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
            return static_cast<qint64>(m_task_list[row].task_entity.task_id);
        case 1:
            return QString::fromStdString(m_task_list[row].file_entity.saved_name);
        case 2:
            return QString::fromStdString(m_task_list[row].file_entity.source_path);
        case 3:
            return QString::fromStdString(m_task_list[row].file_entity.saved_path);
        case 4:
        {
            // 将枚举转为字符串
            switch (m_task_list[row].task_entity.operation)
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
            switch (m_task_list[row].task_entity.state)
            {
            case TaskState::Waiting:
                return "Waiting";
            case TaskState::InTransfer:
                return "InTransfer";
            case TaskState::Completed:
                return "Completed";
            case TaskState::Failed:
                return "Failed";
            default:
                return QVariant();
            }
        }

        case 6:
            return QString::fromStdString(m_task_list[row].file_entity.md5_code);
        case 7:
            return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(m_task_list[row].task_entity.start_time));
        case 8:
            /// @todo 当完成时间<=创建时间，返回0
            if (m_task_list[row].task_entity.end_time <= m_task_list[row].task_entity.start_time)
            {
                return "-";
            }
            else
            {
                return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(m_task_list[row].task_entity.end_time));
            }
        case 9:
            if (m_task_list[row].total_blocks == 0)
            {
                return "-";
            }
            return QString("%1%").arg(m_task_list[row].completed_blocks * 100 / m_task_list[row].total_blocks);
        default:
            return QVariant();
        }
    }
    return QVariant();
}

bool TaskTableModel::setData(const QModelIndex& index, const QVariant& value, int32_t role)
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
        if (row < 0 || row >= m_task_list.size() || column < 0 || column >= 11)
        {
            return false;
        }
        // 根据列索引返回对应数据
        switch (column)
        {
        case 0:
            m_task_list[row].file_entity.file_id = value.toInt();
            break;
        case 1:
            m_task_list[row].file_entity.saved_name = value.toString().toStdString();
            break;
        case 2:
            m_task_list[row].file_entity.source_path = value.toString().toStdString();
            break;
        case 3:
            m_task_list[row].file_entity.saved_path = value.toString().toStdString();
            break;
        case 4:
            m_task_list[row].file_entity.file_size = value.toLongLong();
            break;
        case 5:
            // 由字符串转为枚举
            if (value.toString() == "下载")
            {
                m_task_list[row].task_entity.operation = Operation::Download;
                break;
            }
            else if (value.toString() == "上传")
            {
                m_task_list[row].task_entity.operation = Operation::Upload;
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
                m_task_list[row].task_entity.state = TaskState::Waiting;
                break;
            }
            else if (value.toString() == "InTransfer")
            {
                m_task_list[row].task_entity.state = TaskState::InTransfer;
                break;
            }
            else if (value.toString() == "Completed")
            {
                m_task_list[row].task_entity.state = TaskState::Completed;
            }
            else if (value.toString() == "Failed")
            {
                m_task_list[row].task_entity.state = TaskState::Failed;
            }
            else
                return false;
        case 7:
            m_task_list[row].file_entity.md5_code = value.toString().toStdString();
            break;
        case 8:
            m_task_list[row].task_entity.start_time = std::chrono::system_clock::from_time_t(value.toDateTime().toSecsSinceEpoch());
            break;
        case 9:
            m_task_list[row].task_entity.end_time = std::chrono::system_clock::from_time_t(value.toDateTime().toSecsSinceEpoch());
        default:
            return false;
        }
        // 发送数据改变信号
        emit dataChanged(index, index, { role });
        return true;
    }
    return false;
}
QVariant TaskTableModel::headerData(int32_t section, Qt::Orientation orientation, int32_t role) const
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
void TaskTableModel::add(const TaskViewModel& task_view_model)
{
    auto row = 0;
    for (; row < m_task_list.size(); ++row)
    {
        if (m_task_list[row].task_entity.task_id == task_view_model.task_entity.task_id)
        {
            break;
        }
    }
    if (row == m_task_list.size())
    {
        beginInsertRows(QModelIndex(), row, row);
        m_task_list.push_back(task_view_model);
        endInsertRows();
    }
    else
    {
        m_task_list[row] = task_view_model;
        emit dataChanged(index(row, 0), index(row, columnCount()));
    }
}
void TaskTableModel::remove(const TaskViewModel& task_view_model)
{
    auto row = 0;
    for (; row < m_task_list.size(); ++row)
    {
        if (m_task_list[row].task_entity.task_id == task_view_model.task_entity.task_id)
        {
            beginRemoveRows(QModelIndex(), row, row);
            m_task_list.removeAt(row);
            endRemoveRows();
            break;
        }
    }
}