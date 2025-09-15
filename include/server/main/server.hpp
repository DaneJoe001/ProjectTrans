#pragma once

/**
 * @file server.hpp
 * @brief 服务器
 * @author DaneJoe001
 */

/**
 * @brief 清除日志
 */
void clear_log();

/**
 * @brief 初始化数据库
 */
void init_database();

/**
 * @brief 清除数据库
 */
void clear_database();

/**
 * @brief 运行服务器
 */
void run_server();

/**
 * @brief 停止服务器
 * @param signal 信号
 */
void stop_server_handler(int32_t signal);

/**
 * @brief 打开窗口
 * @param signal 信号
 */
void open_window_handler(int32_t signal);