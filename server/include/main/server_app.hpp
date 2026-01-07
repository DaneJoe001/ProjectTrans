/**
 * @file server_app.hpp
 * @brief 服务器应用程序主类头文件
 * @author DaneJoe001 (danejoe001.github)
 * @date 2026-01-06
 */
#pragma once

#include <memory>
#include <thread>

#include <QEvent>
#include <QObject>

#include "danejoe/network/runtime/reactor_mail_box.hpp"

#include "runtime/business_runtime.hpp"
#include "runtime/network_runtime.hpp"

class QKeyEvent;

class ServerThread;
class ServerMainWindow;

/**
 * @class ServerApp
 * @brief 服务器应用程序主类
 * @details 负责初始化与管理服务器端运行时（网络线程/业务线程）以及主窗口的显示逻辑。
 */
class ServerApp : public QObject {
  Q_OBJECT
public:
  /**
   * @brief 构造函数
   * @param parent 父对象
   */
  ServerApp(QObject *parent = nullptr);
  /**
   * @brief 析构函数
   */
  ~ServerApp();
  /**
   * @brief 初始化服务器应用
   * @details 初始化日志/数据库，并启动网络与业务运行时。
   */
  void init();
  /**
   * @brief 停止服务器应用
   * @details 请求停止网络与业务运行时，并回收相关线程资源。
   */
  void stop();
  /**
   * @brief 事件过滤器
   * @param watched 被监视的对象
   * @param event 事件
   * @return 是否处理了事件
   * @details 用于拦截并处理应用级事件。
   */
  bool eventFilter(QObject *watched, QEvent *event) override;
  /**
   * @brief 显示主窗口
   * @details 将主窗口置为可见。
   */
  void show_main_window();
  /**
   * @brief 隐藏主窗口
   * @details 将主窗口置为隐藏。
   */
  void hide_main_window();

private:
  /**
   * @brief 初始化日志器
   * @details 配置日志输出并准备日志文件。
   */
  void init_logger();
  /**
   * @brief 清理日志文件
   * @details 清理历史日志文件或归档数据。
   */
  void clear_log();
  /**
   * @brief 初始化数据库
   * @details 初始化数据库连接与必要的表结构。
   */
  void init_database();
  /**
   * @brief 清理数据库
   * @details 清理数据库中的临时数据或执行重置操作。
   */
  void clear_database();

private:
  /// @brief 是否已初始化
  bool m_is_init = false;
  /// @brief 主窗口是否可见
  bool m_is_main_window_visible = false;

  /// @brief 业务线程
  std::thread m_business_thread;
  /// @brief 网络线程
  std::thread m_network_thread;
  /// @brief 网络运行时
  std::shared_ptr<NetworkRuntime> m_network_runtime = nullptr;
  /// @brief 业务运行时
  std::shared_ptr<BusinessRuntime> m_bussiness_runtime = nullptr;
  /// @brief 反应器邮箱
  std::shared_ptr<DaneJoe::ReactorMailBox> m_reactor_mail_box = nullptr;

  /// @brief 服务器主窗口
  ServerMainWindow *m_server_main_window = nullptr;
};