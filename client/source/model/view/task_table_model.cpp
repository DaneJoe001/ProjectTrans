#include <QDateTime>
#include <QFileInfo>
#include <QDir>

#include <chrono>

#include <danejoe/logger/logger_manager.hpp>

#include "model/view/task_table_model.hpp"
#include "model/entity/task_entity.hpp"

TaskTableModel::TaskTableModel(
    TaskService& task_service,
    QObject* parent) :
    QAbstractTableModel(parent),
    m_task_service(task_service)
{}

TaskTableModel::~TaskTableModel() {}

void TaskTableModel::init()
{
    // 获取所有任务信息
    auto tasks = m_task_service.get_all();
    beginInsertRows(QModelIndex(), 0, tasks.size());
    m_task_list = QList<TaskEntity>(tasks.begin(), tasks.end());
    endInsertRows();
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
        if (task.task_id == m_task_list[row].task_id)
        {
            m_task_list[row] = task;
            break;
        }
    }
    // m_task_list[row].state = TaskState::Completed;
    // m_task_list[row].end_time = std::chrono::system_clock::now();
    emit dataChanged(index(row, 0), index(row, columnCount()));
}

int32_t TaskTableModel::columnCount(const QModelIndex& parent) const
{
    // 存在父对象时不处理
    if (parent.isValid())
    {
        return 0;
    }
    // 返回固定列数
    return 8;
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
        auto file_path_info = QFileInfo(QString::fromStdString(m_task_list[row].saved_path));
        // 根据列索引返回对应数据
        switch (column)
        {
            case 0:
                return static_cast<qint64>(m_task_list[row].task_id);
            case 1:
                return file_path_info.fileName();
            case 2:
                return file_path_info.dir().path();
            case 3:
                return QString::fromStdString(m_task_list[row].source_url);
            case 4:
            {
                // 将枚举转为字符串
                switch (m_task_list[row].operation)
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
                switch (m_task_list[row].state)
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
                return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(m_task_list[row].start_time));
            case 7:
                /// @todo 当完成时间<=创建时间，返回0
                if (m_task_list[row].end_time <= m_task_list[row].start_time)
                {
                    return "-";
                }
                else
                {
                    return QDateTime::fromSecsSinceEpoch(std::chrono::system_clock::to_time_t(m_task_list[row].end_time));
                }
            default:
                return QVariant();
        }
    }
    return QVariant();
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
                    return QStringLiteral("文件名");
                case 2:
                    return QStringLiteral("保存地址");
                case 3:
                    return QStringLiteral("源链接");
                case 4:
                    return QStringLiteral("操作");
                case 5:
                    return QStringLiteral("状态");
                case 6:
                    return QStringLiteral("创建时间");
                case 7:
                    return QStringLiteral("完成时间");
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
void TaskTableModel::add(const TaskEntity& task_view_model)
{
    auto row = 0;
    for (; row < m_task_list.size(); ++row)
    {
        if (m_task_list[row].task_id == task_view_model.task_id)
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
void TaskTableModel::remove(const TaskEntity& task_view_model)
{
    auto row = 0;
    for (; row < m_task_list.size(); ++row)
    {
        if (m_task_list[row].task_id == task_view_model.task_id)
        {
            beginRemoveRows(QModelIndex(), row, row);
            m_task_list.removeAt(row);
            endRemoveRows();
            break;
        }
    }
}

std::optional<TaskEntity> TaskTableModel::get_task_by_row(int64_t row)
{
    if (row < 0 || row >= m_task_list.size())
    {
        return std::nullopt;
    }
    return m_task_list[row];
}