/**
 * @file business_runtime.hpp
 * @author DaneJoe001 (danejoe001.github)
 * @brief 业务运行时
 * @date 2026-01-06
 */

#pragma once

#include <atomic>
#include <memory>

#include "danejoe/network/runtime/reactor_mail_box.hpp"
#include "protocol/server_message_codec.hpp"
#include "service/server_file_info_service.hpp"

/**
 * @class BusinessRuntime
 * @brief 业务运行时
 */
class BusinessRuntime
{
public:
    /**
     * @brief 构造函数
     * @param reactor_mail_box 反应器邮箱
     */
    BusinessRuntime(std::shared_ptr<DaneJoe::ReactorMailBox> reactor_mail_box);
    /**
     * @brief 初始化
     */
    void init();
    /**
     * @brief 运行
     */
    void run();
    /**
     * @brief 停止
     */
    void stop();
    /**
     * @brief 处理连接请求数据
     * @param data 接收到的字节数组
     * @param connect_id 连接ID
     */
    void handle_request(
        const std::vector<uint8_t>& data,
        uint64_t connect_id);
    /**
     * @brief 处理未知请求
     */
    void handle_unknown_request();
    /**
     * @brief 处理下载请求
     * @param download_request 下载请求
     * @param request_id 请求ID
     * @param connect_id 连接ID
     */
    void handle_download_request(
        const DownloadRequestTransfer& download_request,
        int64_t request_id,
        uint64_t connect_id);
    /**
     * @brief 处理测试请求
     * @param test_request 测试请求
     * @param request 请求ID
     * @param connect_id 连接ID
     */
    void handle_test_request(
        const TestRequestTransfer& test_request,
        int64_t request,
        uint64_t connect_id);
    /**
     * @brief 处理块请求
     * @param block_request 块请求
     * @param request_id 请求ID
     * @param connect_id 连接ID
     */
    void handle_block_request(
        const BlockRequestTransfer& block_request,
        int64_t request_id,
        uint64_t connect_id);
private:
    /// @brief 是否正在运行
    std::atomic<bool> m_is_running = false;
    /// @brief 反应器邮箱
    std::shared_ptr<DaneJoe::ReactorMailBox> m_reactor_mail_box = nullptr;
    /// @brief 服务端消息编解码器
    ServerMessageCodec m_message_codec;
    /// @brief 服务器文件信息服务
    ServerFileInfoService m_file_info_service;
};